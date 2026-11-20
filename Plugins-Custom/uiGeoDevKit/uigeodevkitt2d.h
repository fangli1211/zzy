#ifndef uigeodevkitt2d_h
#define uigeodevkitt2d_h

/*+
________________________________________________________________________

 时深转换向导（P2 · 第 4 项）

 设计取舍：
   OpendTect 已经提供完整的、已导出的 EM::uiTime2DepthDlg，内部包含
   「选层位/断层 -> 选速度体或时深表 -> 单位与基准校验 -> 预览 -> 批量执行」
   全部环节，并且可执行器走 EM::SurfaceT2DTransformer。二次开发不应重复
   实现这套又要校验单位、又要处理 2D/3D 差异的逻辑。

 因此本向导的定位是：
   1. 用中文把 5 个步骤讲清楚，降低现场解释人员的使用门槛；
   2. 在打开官方对话框前先做 canTransform() 预检，避免让用户面对一个
      灰色不可用的对话框；
   3. 一键把当前工区切到官方对话框，并把转换入口统一收拢到本工具包菜单。

 这是「封装 + 汉化引导」，不是重复造轮子，符合计划里"封装
 uitime2depthdlg.cc / timedepthconv.cc"的要求。
________________________________________________________________________
-*/

#include "uidialog.h"
#include "uistring.h"


class uiGeoDevKitT2DDlg : public uiDialog
{ mTextTranslationClass(uiGeoDevKitT2DDlg, "geodevkit")
public:
			uiGeoDevKitT2DDlg(uiParent*);

protected:
    void		checkCB(CallBacker*);
    void		launchCB(CallBacker*);

    uiParent*		parent_ = nullptr;
};

#endif
