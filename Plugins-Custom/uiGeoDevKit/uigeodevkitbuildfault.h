#ifndef uigeodevkitbuildfault_h
#define uigeodevkitbuildfault_h

/*+
________________________________________________________________________

 断层棍 → 断层面生成向导（P2 收尾功能 1）

 作用：
   选择一个或多个 EM::FaultStickSet（断层棍集合），把它们整理、合并，
   通过内核 EM::FSStoFault3DConverter 转换为一个 EM::Fault3D 断层面，
   并用标准的 saver() 持久化到当前工区。对应合同「断层绘制与断层面
   生成」中"多剖面拾取同一断层后一键整理、转换、保存"的要求。

 关键实现要点（来自源码核查）：
   - 多选 FSS 用 uiMultiSurfaceRead（uiEarthModel 导出类），不要用
     uiIOObjSel（后者在本版本是单选，没有 multi()）。
   - 合并多个 FSS 到一个临时 FaultStickSet，再一次性 convert；因为
     FSStoFault3DConverter::convert() 开头会清空目标 Fault3D。
   - 合并用 FaultGeometry::selectAllSticks + copySelectedSticksTo。
   - 保存走 EMObject::saver()->execute()，不存在 writeObjectWithKey/save。
________________________________________________________________________
-*/

#include "uidialog.h"
#include "uistring.h"
#include "bufstring.h"

class uiGenInput;
class uiMultiSurfaceRead;
class uiTextEdit;
class uiPushButton;

namespace EM { class FaultStickSet; }


class uiGeoDevKitBuildFaultDlg : public uiDialog
{ mTextTranslationClass(uiGeoDevKitBuildFaultDlg,"geodevkit")
public:
			uiGeoDevKitBuildFaultDlg(uiParent*);
			~uiGeoDevKitBuildFaultDlg();

protected:
    void		buildCB(CallBacker*);
    bool		acceptOK(CallBacker*) override	{ return true; }

private:
    uiMultiSurfaceRead*	fsssel_;
    uiGenInput*		outnamefld_;
    uiPushButton*	buildbut_;
    uiTextEdit*		logfld_;

    BufferString	lastsummary_;
};


#endif
