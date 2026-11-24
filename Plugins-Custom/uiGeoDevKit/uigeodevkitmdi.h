#pragma once
/*+
________________________________________________________________________

 主窗口内「标签页视窗」—— 极简实现

 需求边界（用户 2026-11-24 确认）：
   - 新建的视窗必须位于**原主体窗口内部**（主窗口 QMdiArea 的原生
     MDI 子窗口），不是独立的顶层窗口；
   - 只要视窗，不要工具栏 / 图片 / 底图 / 画面镜像 / 状态栏等任何
     自定义 UI；
   - 原来的平铺视窗保留，可以随时切回。

 实现：
   - 直接取主窗口的 QMdiArea，用 setViewMode(QMdiArea::TabbedView)
     让 Qt 生成原生标签栏，每个标签就是一个真正的 OpendTect 3D 视窗；
   - 用 uiODSceneMgr::tile() + addScene(true) 走官方新建场景的安全路径
     （官方菜单 mAddSceneMnuItm 同样要求 addScene 之前先 tile）；
   - 不创建任何 QWidget / QMainWindow / QTabWidget，不接管任何视窗
     所有权，不改动 uiODSceneMgr 内部逻辑。

 许可：基于 OpendTect (GPLv3+) 的衍生插件，按 GPLv3+ 交付完整源码。
________________________________________________________________________
-*/

namespace GeoDevKitTabWin
{

// 新建一个 3D 视窗（主窗口内部），并进入标签页视图。
void		newView();

// 标签页视图 / 平铺视图切换。false 时恢复原有平铺布局。
void		setTabbed( bool yn );
bool		isTabbed();
void		tileViews();

// 可选开发开关：OD_GEODEVKIT_TABBED=1 时启动后自动新建一个标签视窗。
// 默认关闭，插件加载时不对主窗口做任何改动。
void		maybeAutoStart();

} // namespace GeoDevKitTabWin
