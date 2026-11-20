/*+
________________________________________________________________________

 工区与运行环境自检对话框（P0 骨架示例）

 说明：这是插件内部使用的类，无需 mExpClass 导出宏。新增头文件必须
       登记到 CMakeLists.txt 的 OD_MODULE_INCFILES，否则不会参与编译，
       也不会被 OpendTect 的换行/关键字 lint 检查覆盖。
________________________________________________________________________
-*/

#ifndef uigeodevkitinfo_h
#define uigeodevkitinfo_h

#include "uidialog.h"
#include "uistring.h"

class uiTextEdit;


class uiGeoDevKitInfoDlg : public uiDialog
{ mTextTranslationClass(uiGeoDevKitInfoDlg, "geodevkit")
public:
			uiGeoDevKitInfoDlg(uiParent*);

private:

    void		fillReport();

    uiTextEdit*		reportfld_ = nullptr;
};

#endif
