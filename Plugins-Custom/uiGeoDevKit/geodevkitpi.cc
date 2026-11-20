/*+
________________________________________________________________________

 uiGeoDevKit —— 地质解释定制开发工具包 · 插件入口（P0 骨架）

 作用：
   1. 在「实用工具」菜单注入一个子菜单，作为后续 P1/P2/P3 所有定制
      向导的统一挂载点；
   2. 提供一个「工区与运行环境自检」对话框，用于确认插件确实被
      od_main 加载、并且当前挂载的是预期的工区数据。

 开发约定：
   - 本文件只负责插件生命周期与菜单，不写业务逻辑；具体功能请新建
     .cc/.h 并显式登记到本目录 CMakeLists.txt 的 OD_MODULE_SOURCES /
     OD_MODULE_INCFILES。
   - 所有面向用户的字符串必须走 tr()，中文翻译放在
     geodevkit_zh_CN.ts，构建时由 lrelease 生成 .qm。
   - 翻译包名为 "geodevkit"，与 .qm 文件名前缀保持一致。

 许可：基于 OpendTect (GPLv3+) 的衍生插件，按 GPLv3+ 交付完整源码。
________________________________________________________________________
-*/

// 必须最先包含自动生成的模块头：mExternC() 展开需要其中的
// Export_uiGeoDevKit / Extern_uiGeoDevKit 导出宏，同时它会带出
// 依赖模块（uiODMain 及其下层）的头文件。
#include "uigeodevkitmod.h"

#include "uigeodevkitinfo.h"
#include "uigeodevkitintersect.h"
#include "uigeodevkitexport.h"
#include "uigeodevkitcontour.h"
#include "uigeodevkitt2d.h"

#include "callback.h"
#include "odplugin.h"
#include "ptrman.h"
#include "uimenu.h"
#include "uimsg.h"
#include "uiodmain.h"
#include "uiodmenumgr.h"
#include "uistring.h"
#include "uifiledlg.h"
#include "uigraphicsscene.h"
#include "uigraphicsviewbase.h"
#include "uigraphicsview.h"
#include "uiflatviewer.h"
#include "uiflatviewwin.h"
#include "uiodviewer2d.h"
#include "uiodviewer2dmgr.h"
#include "oddirs.h"
#include "bufstringset.h"
#include "survgeom.h"
#include "posgeomid.h"


mDefODPluginInfo(uiGeoDevKit)
{
    mDefineStaticLocalObject( PluginInfo, retpi, (
	"Geological Interpretation Toolkit",
	"OpendTect Custom Development",
	"Zhou Project Dev Team",
	"0.1.0",
	"P0 plugin skeleton for the customized geological interpretation "
	"system.\n"
	"Provides an environment self-check dialog and the common menu "
	"anchor for the P1/P2/P3 wizards." ))
    return &retpi;
}


/* uiPluginInitMgr 会在主窗口菜单重建、工区切换等时机回调。
   注意：菜单对象由 uiMenuMgr 接管所有权，这里不要持有 PtrMan，
   也不要在成员里保存可能失效的裸指针。 */
class uiGeoDevKitMgr : public uiPluginInitMgr
{ mTextTranslationClass(uiGeoDevKitMgr, "geodevkit")
public:
			uiGeoDevKitMgr();

private:

    void		dTectMenuChanged() override;

    void		showInfoDlg(CallBacker*);
    void		showIntersectDlg(CallBacker*);
    void		showExportDlg(CallBacker*);
    void		showContourDlg(CallBacker*);
    void		showT2DDlg(CallBacker*);
    void		exportSection(CallBacker*);
    void		showAbout(CallBacker*);
};


uiGeoDevKitMgr::uiGeoDevKitMgr()
    : uiPluginInitMgr()
{
    init();
}


void uiGeoDevKitMgr::dTectMenuChanged()
{
    // 后续 P1/P2/P3 的向导动作统一加进这个子菜单，避免污染主菜单。
    uiMenu* mnu = new uiMenu( tr("Geological Interpretation Toolkit") );
    mnu->insertAction( new uiAction(
		tr("Survey and Environment Check..."),
		mCB(this,uiGeoDevKitMgr,showInfoDlg) ) );
    mnu->insertAction( new uiAction(
		tr("Fault-Horizon Intersection..."),
		mCB(this,uiGeoDevKitMgr,showIntersectDlg) ) );
    mnu->insertAction( new uiAction(
		tr("Interpretation Export Center..."),
		mCB(this,uiGeoDevKitMgr,showExportDlg) ) );
    mnu->insertAction( new uiAction(
		tr("Contour Map Export..."),
		mCB(this,uiGeoDevKitMgr,showContourDlg) ) );
    mnu->insertAction( new uiAction(
		tr("Time-Depth Conversion Wizard..."),
		mCB(this,uiGeoDevKitMgr,showT2DDlg) ) );
    mnu->insertAction( new uiAction(
		tr("Export 2D Section Image..."),
		mCB(this,uiGeoDevKitMgr,exportSection) ) );
    mnu->insertAction( new uiAction(
		tr("About This Toolkit..."),
		mCB(this,uiGeoDevKitMgr,showAbout) ) );

    // addMenu 会把 uiMenu 的所有权交给 utilMnu()
    appl().menuMgr().utilMnu()->addMenu( mnu );
}


void uiGeoDevKitMgr::showInfoDlg( CallBacker* )
{
    uiGeoDevKitInfoDlg dlg( &appl() );
    dlg.go();
}


void uiGeoDevKitMgr::showIntersectDlg( CallBacker* )
{
    uiGeoDevKitIntersectDlg dlg( &appl() );
    dlg.go();
}


void uiGeoDevKitMgr::showExportDlg( CallBacker* )
{
    uiGeoDevKitExportDlg dlg( &appl() );
    dlg.go();
}


void uiGeoDevKitMgr::showContourDlg( CallBacker* )
{
    uiGeoDevKitContourDlg dlg( &appl() );
    dlg.go();
}


void uiGeoDevKitMgr::showT2DDlg( CallBacker* )
{
    uiGeoDevKitT2DDlg dlg( &appl() );
    dlg.go();
}


void uiGeoDevKitMgr::exportSection( CallBacker* )
{
    // 导出当前打开的 2D 剖面视图为 PNG。OpendTect 把层位/断层的叠加显示
    // 都渲染进 rgbCanvas().scene()，因此 saveAsImage 得到的图就是所见即所得
    // 的剖面，无需自己重画数据。
    if ( !appl().viewer2DMgrAvailable() ||
	 appl().viewer2DMgr().nr2DViewers()<1 )
    {
	uiMSG().message( tr("No 2D section viewer is open. Open a seismic "
			    "line in a 2D window first.") );
	return;
    }

    uiODViewer2D* vwr = nullptr;
    BufferStringSet lnm;
    TypeSet<Pos::GeomID> geomids;
    Survey::GM().getList( lnm, geomids, true );
    for ( int idx=0; idx<geomids.size(); idx++ )
    {
	vwr = appl().viewer2DMgr().find2DViewer( geomids[idx] );
	if ( vwr )
	    break;
    }
    if ( !vwr || !vwr->viewwin() || !vwr->viewwin()->nrViewers() )
    {
	uiMSG().message( tr("The open section has no renderable view.") );
	return;
    }

    BufferString deffnm( "section_" );
    deffnm.add( GetSurveyName() ).add( ".png" );
    uiFileDialog dlg( &appl(), false, GetProcFileName(deffnm.buf()),
		      "*.png", tr("Export 2D section as PNG") );
    if ( !dlg.go() )
	return;

    uiFlatViewer& flatvwr = vwr->viewwin()->viewer( 0 );
    uiGraphicsScene& scene = flatvwr.rgbCanvas().scene();
    scene.saveAsImage( dlg.fileName(), 1200, 800, 150 );
    uiMSG().message( tr("Section image exported successfully.") );
}


void uiGeoDevKitMgr::showAbout( CallBacker* )
{
    uiMSG().message( tr(
	"Geological Interpretation Toolkit 0.1.0 (P0 skeleton). "
	"External PLUGIN_DIR plugin for OpendTect v8.1.0. "
	"Planned modules: DWT volume (P1), interpretation wizards and DXF "
	"export (P2), U-Net bridge (P3)." ) );
}


mDefODInitPlugin(uiGeoDevKit)
{
    mDefineStaticLocalObject( PtrMan<uiGeoDevKitMgr>, theinst_,
				= new uiGeoDevKitMgr() );
    if ( !theinst_ )
	return "Cannot instantiate the uiGeoDevKit plugin";

    return nullptr; // nullptr 表示加载成功
}
