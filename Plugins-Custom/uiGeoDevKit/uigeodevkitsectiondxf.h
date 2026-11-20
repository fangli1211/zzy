#ifndef uigeodevkitsectiondxf_h
#define uigeodevkitsectiondxf_h

/*+
________________________________________________________________________

 剖面矢量 DXF 导出向导（P2 收尾功能 2）

 作用：
   选一条剖面（3D Inline / 3D Xline / 已加载的 2D 测线），采样若干 3D
   层位与断层面在该剖面上的 Z 值，输出真正的二维剖面线 DXF —— X 轴为
   沿剖面的累计距离（米），Y=0，Z 为层位/断层的时间或深度值；每个对象
   独占一个图层，便于在 CAD 中按层开关与着色。

 采样实现（源码核查后采用的稳定路径）：
   - 层位：Horizon3D::getZ(BinID)，逐道取 Z，mIsUdf 过滤空节点。
   - 断层：逐根 FaultStickSurface 棍读取节点（getKnot(RowCol)），把其
     BinID 落在剖面容差范围内的节点按 X 排序后连成折线。地震解释中棍
     通常就拾取在 inline/xline 面上，因此该法对 3D 剖面可靠；局限是只画
     棍上已有节点，不补棍段与剖面的插值交点（首版可接受，已在日志说明）。
   - X 轴累计距离：3D 用相邻 BinID 的 Coord.horDistTo()，2D 用
     Line2DPos.coord_ 累加（Line2DData::distBetween 亦可）。
   - DXF 写出直接复用插件内 GeoDevDXFWriter（R12，LINE/POLYLINE/LAYER）。
________________________________________________________________________
-*/

#include "uidialog.h"
#include "uistring.h"
#include "posgeomid.h"
#include "typeset.h"

class uiComboBox;
class uiGenInput;
class uiIOObjSel;
class uiMultiSurfaceRead;
class uiPushButton;
class uiTextEdit;


class uiGeoDevKitSectionDXFDlg : public uiDialog
{ mTextTranslationClass(uiGeoDevKitSectionDXFDlg,"geodevkit")
public:
			uiGeoDevKitSectionDXFDlg(uiParent*);
			~uiGeoDevKitSectionDXFDlg();

protected:
    void		sectionTypeCB(CallBacker*);
    void		exportCB(CallBacker*);
    bool		acceptOK(CallBacker*) override	{ return true; }

private:
    uiComboBox*		sectypefld_;
    uiGenInput*		inlnrfld_;
    uiGenInput*		crlnrfld_;
    uiComboBox*		line2dfld_;
    uiMultiSurfaceRead*	horsel_;
    uiMultiSurfaceRead*	fltsel_;
    uiGenInput*		outnamefld_;
    uiPushButton*	exportbut_;
    uiTextEdit*		logfld_;

    // 2D 测线名 -> GeomID
    BufferStringSet		line2dnms_;
    TypeSet<Pos::GeomID>	line2dids_;
};


#endif
