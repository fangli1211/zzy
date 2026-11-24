#ifndef uigeodevkitml_h
#define uigeodevkitml_h
/*+
________________________________________________________________________

 Machine Learning 界面（P3 · UI Shell，按需求文档 03_ml 截图复刻，不接算法）

 背景：
   需求文档 03_ml.docx 的截图来自 dGB 商业插件 Machine Learning Control
   Center。开源构建里 Analysis 菜单由 uiodmenumgr.cc::fillAnalMenu() 硬编码，
   根本没有该入口；dGB_ML_* 只是外部可执行程序的注册名单，对应的 exe 在本机
   全部缺失。本套界面按截图把控件布局完整做出来，便于甲方确认交互流程。

 边界（重要）：
   - 所有 Extract / Train / Apply / Go 按钮只弹「算法后端未接入」提示，
     不读写数据、不训练、不推理；
   - 列表里的条目是截图中的示例条目，不是从工区真实读取的对象；
   - 真正算法等 P3 决策 D1（自研 PyTorch vs 购买 dGB 授权）与 D4
     （标注样本）确认后再接。本壳不承担任何数值职责，不改内核。
________________________________________________________________________
-*/

#include "uidialog.h"

class uiButtonGroup;
class uiComboBox;
class uiGenInput;
class uiLabel;
class uiListBox;
class uiPushButton;
class uiTabStack;
class uiTable;


/* 控制中心：对应需求截图 image2（Machine Learning Control Center） */
class uiGeoDevKitMLCenterDlg : public uiDialog
{ mTextTranslationClass(uiGeoDevKitMLCenterDlg,"geodevkit")
public:
			uiGeoDevKitMLCenterDlg(uiParent*);

protected:
    void		goCB(CallBacker*);
    void		categorySelCB(CallBacker*);

    uiGenInput*		projectfld_	= nullptr;
    uiListBox*		catlist_	= nullptr;
    uiListBox*		tasklist_	= nullptr;
    uiLabel*		statuslbl_	= nullptr;
};


/* 三步向导：对应需求截图 image3/4/5（Extract / Train / Apply） */
class uiGeoDevKitMLWizDlg : public uiDialog
{ mTextTranslationClass(uiGeoDevKitMLWizDlg,"geodevkit")
public:
			uiGeoDevKitMLWizDlg(uiParent*,bool segmentation);

protected:
    bool		acceptOK(CallBacker*) override;
    void		stubCB(CallBacker*);

private:
    void		createExtractPage(uiTabStack*);
    void		createTrainPage(uiTabStack*);
    void		createApplyPage(uiTabStack*);

    bool		segmentation_	= true;
    uiTabStack*		wizardtabs_	= nullptr;
};


#endif
