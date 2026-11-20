#ifndef uigeodevkitcontour_h
#define uigeodevkitcontour_h

/*+
________________________________________________________________________

 等值线成图导出向导（P2 · 第 5 项）

 在三维层位上按给定等值距计算等值线（复用 Algo::IsoContourTracer），
 把每条等值线作为 DXF 的 POLYLINE 输出到独立图层 CONTOUR_<level>，
 并可选叠加断层边界（断层棍折线）作为参照。

 关于「断层内不画等高线」：
   IsoContourTracer 提供 selectPolyROI() 可按多边形裁剪；本向导预留该入口，
   具体多边形来自甲方提供的断层约束面（D6/D8 待确认项）。当前版本先输出
   完整等值线 + 断层参照线，避免在没有确认约束规范前擅自裁剪数据。
________________________________________________________________________
-*/

#include "uidialog.h"
#include "uistring.h"


class uiGenInput;
class uiIOObjSel;
class uiTextEdit;


class uiGeoDevKitContourDlg : public uiDialog
{ mTextTranslationClass(uiGeoDevKitContourDlg, "geodevkit")
public:
			uiGeoDevKitContourDlg(uiParent*);

protected:
    void		computeCB(CallBacker*);
    void		saveCB(CallBacker*);

    uiIOObjSel*		horsel_ = nullptr;
    uiIOObjSel*		fltsel_ = nullptr;
    uiGenInput*		startfld_ = nullptr;
    uiGenInput*		stepfld_ = nullptr;
    uiGenInput*		stopfld_ = nullptr;
    uiGenInput*		outnamefld_ = nullptr;
    uiTextEdit*		resultfld_ = nullptr;

    BufferString	lastdxf_;
    int			lastcontours_ = 0;
};

#endif
