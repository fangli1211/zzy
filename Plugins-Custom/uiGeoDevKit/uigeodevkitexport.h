#ifndef uigeodevkitexport_h
#define uigeodevkitexport_h

/*+
________________________________________________________________________

 成果导出中心（P2 · ioDXF 模块 UI）

 把工区里已有的解释成果统一导出为 ASCII DXF R12 或纯 ASCII 表格：
   - 三维层位：每个网格节点输出为 POINT，并把每条 Inline 连成 POLYLINE；
   - 三维断层：逐条断层棍输出为 POLYLINE；
   - 断层—层位交线：复用交线对话框的同一算法，输出为 POLYLINE；
   - 井：井口位置 + 井轨迹（测深 MD 作为 Z）；
   - 2D 测线：平面折线（Z=0）。

 设计要点：
   1. 对象选择用 uiIOObjSel；所有几何读取都走 EM::EMM() 的公开接口，
      不碰内核私有结构。
   2. 真正的 DXF 格式化全部交给 GeoDevDXFWriter；本类只做「对象 -> 点列」。
   3. 字符串全部走 tr()，翻译包名 "geodevkit"。
   4. 插件内部类，无需 mExpClass；新文件已登记到 CMakeLists.txt。
________________________________________________________________________
-*/

#include "uidialog.h"
#include "uistring.h"

#include "bufstring.h"
#include "multiid.h"
#include "positionlist.h"
#include "typeset.h"

class Coord3;
class uiComboBox;
class uiGenInput;
class uiIOObjSel;
class uiPushButton;
class uiTextEdit;

class GeoDevDXFWriter;


class uiGeoDevKitExportDlg : public uiDialog
{ mTextTranslationClass(uiGeoDevKitExportDlg, "geodevkit")
public:
			uiGeoDevKitExportDlg(uiParent*);

protected:

    // 各成果的导出回调
    void		exportHorizonCB(CallBacker*);
    void		exportFaultCB(CallBacker*);
    void		exportIntersectCB(CallBacker*);
    void		exportWellsCB(CallBacker*);
    void		exportLinesCB(CallBacker*);

    // 工具函数
    bool		writeOutput(const GeoDevDXFWriter&,const char* deffnm,
				    const uiString& title);
    void		writeAscii(const char* deffnm,const uiString& title,
				   const BufferString& content);
    void		log(const uiString&);

    uiIOObjSel*		horsel_ = nullptr;
    uiIOObjSel*		fltsel_ = nullptr;
    uiIOObjSel*		hor2sel_ = nullptr;
    uiIOObjSel*		flt2sel_ = nullptr;
    uiGenInput*		horshiftfld_ = nullptr;
    uiGenInput*		outnamefld_ = nullptr;
    uiGenInput*		formatfld_ = nullptr;
    uiComboBox*		welllist_ = nullptr;
    uiComboBox*		linelist_ = nullptr;
    uiTextEdit*		logfld_ = nullptr;

    TypeSet<MultiID>	wellids_;
    TypeSet<int>	lineidxs_;
};

#endif
