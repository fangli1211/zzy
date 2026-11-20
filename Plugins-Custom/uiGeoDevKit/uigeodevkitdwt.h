#ifndef uigeodevkitdwt_h
#define uigeodevkitdwt_h
/*+
________________________________________________________________________

 三维地震体 DWT 分解向导（P1 核心功能）

 流程：
   选输入 3D 体 -> 选小波基(23 种) -> 选分解级数 -> 选输出分量
   (cA_J / cD_1..cD_J) -> 选小波域属性分量 -> 逐道变换 ->
   输出一组同网格 CBVS 体 + 多小波基对比报告。

 实现要点：
   - 用 SeisSingleTraceProc 逐道读输入，在 proctobedone_ 回调里对
     work trace 做 DWT；输出体用 CtxtIOObj + CBVS translator 创建。
   - 因一个 SeisSingleTraceProc 只写一个输出体，本向导为每个选中输出
     分量建一个 processor，一次任务串行跑完所有分量（每个分量扫一遍输入）。
     这样内存占用与体大小无关，1 GB 级体不会爆内存。
   - 属性分量（近似/细节/相对细节能量、小波熵）以每道一个标量写到
     同网格体：把标量广播到该道全部样点，便于在 3D 视图里显示。

 边界方式：内核 DWT 固定为 circular（周期延拓），不可配置 —— UI 上
 明确标注，不做假的开关。
________________________________________________________________________
-*/

#include "uidialog.h"
#include "geodevkitdwt.h"

class uiComboBox;
class uiGenInput;
class uiIOObjSel;
class uiLabel;
class uiPushButton;
class uiTextEdit;
class IOObj;
class MultiID;


class uiGeoDevKitDWTDlg : public uiDialog
{ mTextTranslationClass(uiGeoDevKitDWTDlg,"geodevkit")
public:
			uiGeoDevKitDWTDlg(uiParent*);
			~uiGeoDevKitDWTDlg();

protected:
    uiIOObjSel*		inpsel_		= nullptr;
    uiComboBox*		wavefld_	= nullptr;
    uiGenInput*		levelfld_	= nullptr;
    uiGenInput*		compapprox_	= nullptr;
    uiGenInput*		compdetail_	= nullptr;
    uiGenInput*		attrfld_	= nullptr;
    uiGenInput*		cmpfld_		= nullptr;
    uiGenInput*		cmpsamplesfld_	= nullptr;
    uiGenInput*		outpfx_		= nullptr;
    uiTextEdit*		logfld_		= nullptr;
    uiPushButton*	runbut_		= nullptr;
    uiLabel*		infolbl_	= nullptr;

    BufferStringSet	wnames_;
    MultiID		inpkey_;

    void		waveSelCB(CallBacker*);
    void		runCB(CallBacker*);

    void		log(const uiString&);
    bool		createOutput(const IOObj& inpobj,const char* outnm,
				     MultiID&) const;
};

#endif
