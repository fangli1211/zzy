#ifndef uigeodevkitintersect_h
#define uigeodevkitintersect_h

/*+
________________________________________________________________________

 断层—层位交线提取对话框（P2 第一个落地功能）

 设计要点：
   1. 复用 OpendTect 已有的 Geometry::FaultBinIDSurfaceIntersector，
      不重新实现交线算法；本类只负责对象选择、参数输入、结果显示
      与 ASCII 导出。
   2. 断层由 EM::Fault3D 的 geometryElement() 得到 FaultStickSurface，
      再包一层 ExplFaultStickSurface；层位由 EM::Horizon3D 得到
      BinIDSurface。二者都不能为空。
   3. 字符串全部走 tr()，翻译包名 "geodevkit"，与插件其它文件一致。
   4. 这是插件内部对话框，无需 mExpClass 导出宏；新文件必须显式
      登记到本目录 CMakeLists.txt 的 OD_MODULE_SOURCES/INCFILES。
________________________________________________________________________
-*/

#include "uidialog.h"
#include "uistring.h"

#include "bufstring.h"

class uiGenInput;
class uiIOObjSel;
class uiPushButton;
class uiTextEdit;


class uiGeoDevKitIntersectDlg : public uiDialog
{ mTextTranslationClass(uiGeoDevKitIntersectDlg, "geodevkit")
public:
			uiGeoDevKitIntersectDlg(uiParent*);

protected:
    // 计算交线并刷新结果文本框；失败时给出中文错误提示。
    void		computeCB(CallBacker*);
    // 把最近一次计算结果导出为 X/Y/Z 三列 ASCII 文件。
    void		saveCB(CallBacker*);

    uiIOObjSel*		horsel_ = nullptr;
    uiIOObjSel*		fltsel_ = nullptr;
    uiGenInput*		horshiftfld_ = nullptr;
    uiPushButton*	savebut_ = nullptr;
    uiTextEdit*		resultfld_ = nullptr;

    // 缓存最近一次结果，供导出按钮使用；为空表示尚未成功计算。
    BufferString	lastresult_;
};

#endif
