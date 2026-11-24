/*+
________________________________________________________________________

 主窗口内「标签页视窗」实现 —— 只操作主窗口 QMdiArea，不建任何新窗口。

 需求边界：新建的视窗必须在原主体窗口内部，是真正的 OpendTect 3D
 视窗；只要视窗和标签栏，不要工具栏 / 图片 / 底图 / 状态栏等其它 UI；
 原有的平铺视窗保留，可随时切回。

 实现：
   1. 用 uiODSceneMgr::tile() + addScene(true) 走官方新建场景的安全路径
      （官方菜单 mAddSceneMnuItm 同样要求 addScene 之前先 tile），新建的
      3D 视窗是主窗口 QMdiArea 的原生 QMdiSubWindow。
   2. 把 QMdiArea 设为 TabbedView，让多个视窗以"单个大视窗"的方式呈现；
      uiMdiArea::tile() 的 setGeometry 会把子窗口拉回普通状态，所以切换后
      要保证至少一个子窗口处于最大化，Qt 才会重建标签布局。
   3. Qt 自带的 QTabBar 在本机构建里不显示（viewMode 报 TABBED、bar 对象
      存在，但界面上看不到标签），因此显式创建一条 QTabBar 放在 MDI 区
      顶部，标签对应每个 QMdiSubWindow，点击即 setActiveSubWindow 切换。
      这是唯一的自定义控件，其余全是 OpendTect 原生视窗。
   4. 低频守护（1 秒）同步标签数量/选中态，并修正 OpendTect 后续 tile()
      造成的平铺回退。点「平铺视窗（原有）」时守护停止、标签栏隐藏，
      恢复原生平铺，不会覆盖用户的选择。
   5. 全程不 reparent 任何 QMdiSubWindow，不碰 uiODSceneMgr 的所有权。
   6. OD_GEODEVKIT_TABDEBUG=1 时写 assets/tabdebug.log，仅供开发定位，
      默认关闭，不属于用户可见功能。

 许可：基于 OpendTect (GPLv3+) 的衍生插件，按 GPLv3+ 交付完整源码。
________________________________________________________________________
-*/

#include "uigeodevkitmdi.h"

#include <QApplication>
#include <QByteArray>
#include <QDateTime>
#include <QFile>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPointer>
#include <QString>
#include <QTabBar>
#include <QTabWidget>
#include <QTextStream>
#include <QTimer>
#include <QWidget>

#include "uiobj.h"
#include "uiodmain.h"
#include "uiodscenemgr.h"


namespace GeoDevKitTabWin
{

static bool		s_want_tabbed = false;
static bool		s_autostart_done = false;
static QTimer*		s_guardtimer = nullptr;
static QPointer<QTabBar> s_tabbar;
static bool		s_syncing = false;


static void dbg( const QString& msg )
{
    if ( qgetenv("OD_GEODEVKIT_TABDEBUG") != "1" )
	return;

    QFile f( "D:/OpendTect-Docs/assets/tabdebug.log" );
    if ( !f.open(QIODevice::Append|QIODevice::Text) )
	return;
    QTextStream ts( &f );
    ts << QDateTime::currentDateTime().toString("HH:mm:ss.zzz")
       << " " << msg << "\n";
}


/* 取得主窗口里承载 3D 场景的那个 QMdiArea。
   首选：从当前 3D 视窗沿 parent 链找到 QMdiSubWindow，再用它的
   mdiArea() 反查 —— 最准确的定位方式。兜底：遍历主窗口及顶层窗口，
   取子窗口数量最多的 QMdiArea。 */
static QMdiArea* mainMdiArea()
{
    if ( ODMainWin() )
    {
	uiODSceneMgr& scenemgr = ODMainWin()->sceneMgr();
	ui3DViewer* vwr = scenemgr.get3DViewer( scenemgr.getActiveSceneID() );
	QWidget* w = vwr ? vwr->qwidget() : nullptr;
	int nup = 0;
	while ( w && nup < 12 )
	{
	    QMdiSubWindow* sub = qobject_cast<QMdiSubWindow*>( w );
	    if ( sub && sub->mdiArea() )
		return sub->mdiArea();
	    w = w->parentWidget();
	    nup++;
	}
    }

    QList<QMdiArea*> areas;
    QWidget* root = ODMainWin() ? ODMainWin()->qWidget() : nullptr;
    if ( root )
    {
	areas.append( root->findChildren<QMdiArea*>() );
	QMdiArea* self = qobject_cast<QMdiArea*>( root );
	if ( self )
	    areas.append( self );
    }

    const QWidgetList tops = QApplication::topLevelWidgets();
    for ( int idx=0; idx<tops.size(); idx++ )
    {
	if ( tops[idx] == root )
	    continue;
	areas.append( tops[idx]->findChildren<QMdiArea*>() );
	QMdiArea* self = qobject_cast<QMdiArea*>( tops[idx] );
	if ( self )
	    areas.append( self );
    }

    QMdiArea* best = nullptr;
    int bestnr = -1;
    for ( int idx=0; idx<areas.size(); idx++ )
    {
	QMdiArea* area = areas[idx];
	if ( !area )
	    continue;
	const int nr = area->subWindowList().size();
	if ( nr > bestnr )
	{
	    bestnr = nr;
	    best = area;
	}
    }

    return best;
}


/* 在主窗口 MDI 区顶部维护一条可见的标签栏。标签数量与标题跟随
   QMdiArea 的原生子窗口，点击标签即切换活动视窗。 */
static void syncTabBar( QMdiArea* mdi )
{
    if ( !mdi || s_syncing )
	return;

    s_syncing = true;
    const QList<QMdiSubWindow*> subs =
			mdi->subWindowList( QMdiArea::CreationOrder );

    QTabBar* bar = s_tabbar.data();
    if ( !bar || bar->parentWidget() != mdi )
    {
	delete s_tabbar.data();
	bar = new QTabBar( mdi );
	bar->setObjectName( "GeoDevKitViewTabBar" );
	bar->setExpanding( false );
	bar->setMovable( false );
	bar->setTabsClosable( false );
	bar->setDrawBase( true );
	bar->setFixedHeight( 28 );
	bar->setStyleSheet(
	    "QTabBar { background: #e4e4e4; }"
	    "QTabBar::tab { background: #f2f2f2; color: #222222;"
	    "  padding: 4px 18px; border: 1px solid #a8a8a8;"
	    "  border-bottom: none; }"
	    "QTabBar::tab:selected { background: #2d6fb5; color: #ffffff; }" );
	s_tabbar = bar;

	QObject::connect( bar, &QTabBar::currentChanged, []( int idx )
	{
	    if ( s_syncing )
		return;
	    QMdiArea* m = mainMdiArea();
	    if ( !m )
		return;
	    const QList<QMdiSubWindow*> ss =
			m->subWindowList( QMdiArea::CreationOrder );
	    if ( idx>=0 && idx<ss.size() && ss[idx] )
	    {
		m->setActiveSubWindow( ss[idx] );
		ss[idx]->raise();
	    }
	    if ( s_tabbar )
		s_tabbar->raise();
	} );
    }

    // 同步标签数量与文字
    if ( bar->count() != subs.size() )
    {
	const bool oldblk = bar->blockSignals( true );
	while ( bar->count() > subs.size() )
	    bar->removeTab( bar->count()-1 );
	while ( bar->count() < subs.size() )
	{
	    const int i = bar->count();
	    bar->addTab( QString::fromUtf8("视图 %1").arg(i+1) );
	}
	bar->blockSignals( oldblk );
    }

    // 同步选中态（不触发 currentChanged，避免递归）
    QMdiSubWindow* act = mdi->activeSubWindow();
    const int cur = act ? subs.indexOf( act ) : -1;
    if ( cur >= 0 && bar->currentIndex() != cur )
    {
	const bool oldblk = bar->blockSignals( true );
	bar->setCurrentIndex( cur );
	bar->blockSignals( oldblk );
    }

    bar->setGeometry( 0, 0, mdi->width(), 28 );
    bar->raise();
    bar->show();
    s_syncing = false;
}


static void applyTabbedLook( QMdiArea* mdi )
{
    if ( !mdi )
	return;

    mdi->setViewMode( QMdiArea::TabbedView );
    mdi->setTabsClosable( true );
    mdi->setTabsMovable( true );
    mdi->setDocumentMode( true );
    mdi->setTabShape( QTabWidget::Rounded );

    // tile() 会把子窗口全部 showNormal 并 setGeometry。TabbedView 下只要
    // 有一个子窗口回到最大化，Qt 就会重建标签布局。其余标签由 Qt 自己
    // 管理，不要逐个 showMaximized（会反复触发 OpendTect 场景回调）。
    const QList<QMdiSubWindow*> subs = mdi->subWindowList();
    bool anymax = false;
    for ( int idx=0; idx<subs.size(); idx++ )
	if ( subs[idx] && subs[idx]->isMaximized() )
	{
	    anymax = true;
	    break;
	}

    if ( !anymax && !subs.isEmpty() && subs[0] )
	subs[0]->showMaximized();

    syncTabBar( mdi );
}


/* 低频守护：同步标签栏，并在状态确实异常时修正。
   异常判据：视图模式不是 TabbedView / 显式标签栏不可见 /
   有子窗口但一个都没最大化（被 tile() 打回平铺）。 */
static void ensureTabState()
{
    if ( !s_want_tabbed )
	return;

    QMdiArea* mdi = mainMdiArea();
    if ( !mdi )
	return;

    const QList<QMdiSubWindow*> subs = mdi->subWindowList();
    int nmax = 0;
    for ( int idx=0; idx<subs.size(); idx++ )
	if ( subs[idx] && subs[idx]->isMaximized() )
	    nmax++;

    const bool needfix = mdi->viewMode() != QMdiArea::TabbedView
		      || !s_tabbar || !s_tabbar->isVisible()
		      || ( !subs.isEmpty() && nmax==0 );

    if ( needfix )
    {
	dbg( QString("FIX subs=%1 nmax=%2").arg(subs.size()).arg(nmax) );
	applyTabbedLook( mdi );
    }
    else
	syncTabBar( mdi ); // 只同步数量/选中态，保持标签栏置顶
}


static void startGuard()
{
    if ( !s_guardtimer )
    {
	s_guardtimer = new QTimer( qApp );
	s_guardtimer->setInterval( 1000 );
	QObject::connect( s_guardtimer, &QTimer::timeout, []()
	{
	    ensureTabState();
	} );
    }

    if ( !s_guardtimer->isActive() )
	s_guardtimer->start();
}


static void stopGuard()
{
    if ( s_guardtimer )
	s_guardtimer->stop();
}


bool isTabbed()
{
    QMdiArea* mdi = mainMdiArea();
    return mdi && mdi->viewMode() == QMdiArea::TabbedView;
}


void setTabbed( bool yn )
{
    s_want_tabbed = yn;
    QMdiArea* mdi = mainMdiArea();
    dbg( QString("setTabbed(%1)").arg(yn?1:0) );
    if ( !mdi )
	return;

    if ( yn )
    {
	applyTabbedLook( mdi );
	startGuard();
    }
    else
    {
	stopGuard();
	if ( s_tabbar )
	    s_tabbar->hide();
	mdi->setViewMode( QMdiArea::SubWindowView );
	// 恢复 OpendTect 原有的平铺布局
	if ( ODMainWin() )
	    ODMainWin()->sceneMgr().tile();
    }

    mdi->update();
}


void tileViews()
{
    setTabbed( false );
}


void newView()
{
    QMdiArea* mdi = mainMdiArea();
    dbg( QString("newView begin subs=%1")
	    .arg(mdi ? mdi->subWindowList().size() : -1) );
    if ( !mdi || !ODMainWin() )
	return;

    // 先暂停守护，保证 tile() 在 SubWindowView 下生效
    // （官方注释：addScene 之前不 tile 会崩溃）。
    stopGuard();
    if ( mdi->viewMode() == QMdiArea::TabbedView )
	mdi->setViewMode( QMdiArea::SubWindowView );

    ODMainWin()->sceneMgr().tile();
    ODMainWin()->sceneMgr().addScene( true );

    // 新建出来的 3D 视窗是主窗口 QMdiArea 的原生子窗口；切到 TabbedView
    // 之后每个视窗就是一个标签，点标签即切换。
    s_want_tabbed = true;
    applyTabbedLook( mdi );
    startGuard();

    // 等官方 tileTimerCB 跑完，再把新视窗设为当前标签
    QTimer::singleShot( 500, []()
    {
	QMdiArea* m = mainMdiArea();
	if ( !m )
	    return;
	if ( s_want_tabbed )
	    applyTabbedLook( m );
	const QList<QMdiSubWindow*> subs =
			m->subWindowList( QMdiArea::CreationOrder );
	if ( !subs.isEmpty() )
	    m->setActiveSubWindow( subs.last() );
	syncTabBar( m );
    } );
}


void maybeAutoStart()
{
    // 仅开发/演示用：默认关闭，不改变任何默认行为；只执行一次。
    if ( s_autostart_done )
	return;
    s_autostart_done = true;

    if ( qgetenv("OD_GEODEVKIT_TABBED") != "1" )
	return;

    dbg( QString("maybeAutoStart scheduled") );
    QTimer::singleShot( 2500, []()
    {
	dbg( QString("maybeAutoStart firing") );
	newView();
    } );
}


} // namespace GeoDevKitTabWin
