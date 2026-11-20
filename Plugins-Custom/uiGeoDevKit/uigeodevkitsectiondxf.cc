/*+
________________________________________________________________________

 剖面矢量 DXF 导出向导实现

 剖面类型（sectypefld_）：
   0 = 3D Inline（固定 inl，crl 变化）
   1 = 3D Xline （固定 crl，inl 变化）
   2 = 2D 测线   （Survey::GM() 中已加载的 2D geometry）

 对每种类型先构造"采样道序列" —— 每道 = (BinID, 累计距离 X[米], Coord)：
   - 3D：沿 crl/inl 的 StepInterval 逐道；X 由相邻 BinID 的平面坐标
         horDistTo 累加。
   - 2D：Geometry2D::data().positions() 每道 Line2DPos；X 由 coord_ 累加。

 然后逐个层位/断层对象在这批道上采样 Z：
   - 层位：EM::Horizon3D::getZ(BinID)，mIsUdf 过滤。
   - 断层：FaultStickSurface 逐棍节点，按 BinID 与剖面法向的贴近程度
         选点（3D：固定 inl 用 |bid.inl-inl|<容差；2D：用该 2D 测线上
         是否存在该 binID 的道），投影到 X 轴后连折线。

 输出：GeoDevDXFWriter，每对象一图层，坐标 (X, 0, Z)。
________________________________________________________________________
-*/

#include "uigeodevkitsectiondxf.h"

#include "uigeodevkitmod.h"

#include <cstdlib>

#include "bufstring.h"
#include "callback.h"
#include "coord.h"
#include "emfault3d.h"
#include "emhorizon3d.h"
#include "emmanager.h"
#include "emobject.h"
#include "executor.h"
#include "ioman.h"
#include "multiid.h"
#include "notify.h"
#include "oddirs.h"
#include "ptrman.h"
#include "ranges.h"
#include "refcount.h"
#include "survgeom.h"
#include "survgeom2d.h"
#include "survinfo.h"
#include "trckeysampling.h"
#include "typeset.h"

#include "geodevkitdxf.h"
#include "uibutton.h"
#include "uicombobox.h"
#include "uifiledlg.h"
#include "uigeninput.h"
#include "uimsg.h"
#include "uimultisurfaceread.h"
#include "uitextedit.h"


// 采样道：平面位置、BinID、沿剖面累计距离
struct SecTrace
{
    BinID	bid_;
    Coord	crd_;
    double	dist_ = 0.0;

    bool operator==( const SecTrace& o ) const
    { return bid_==o.bid_ && crd_==o.crd_ && dist_==o.dist_; }
};


uiGeoDevKitSectionDXFDlg::uiGeoDevKitSectionDXFDlg( uiParent* p )
    : uiDialog( p, Setup(tr("Export Section as Vector DXF"),
			 tr("Sample horizons and faults along an inline, "
			    "xline or 2D survey line and write layered DXF"),
			 mNoHelpKey))
{
    sectypefld_ = new uiComboBox( this, "Section type" );
    sectypefld_->addItem( tr("3D Inline") );
    sectypefld_->addItem( tr("3D Crossline") );
    sectypefld_->addItem( tr("2D survey line") );
    sectypefld_->setCurrentItem( 0 );

    inlnrfld_ = new uiGenInput( this, tr("Inline number"),
				 IntInpSpec(SI().inlRange().center()) );
    inlnrfld_->attach( alignedBelow, sectypefld_ );

    crlnrfld_ = new uiGenInput( this, tr("Crossline number"),
				 IntInpSpec(SI().crlRange().center()) );
    crlnrfld_->attach( alignedBelow, inlnrfld_ );

    line2dfld_ = new uiComboBox( this, "2D line" );
    Survey::GM().getList( line2dnms_, line2dids_, true );
    for ( int idx=0; idx<line2dnms_.size(); idx++ )
	line2dfld_->addItem( line2dnms_.get(idx).buf() );
    if ( !line2dnms_.isEmpty() )
	line2dfld_->setCurrentItem( 0 );
    line2dfld_->attach( alignedBelow, crlnrfld_ );

    horsel_ = new uiMultiSurfaceRead( this,
			EM::Horizon3D::typeStr(), &SI().zDomainInfo() );
    horsel_->attach( alignedBelow, line2dfld_ );

    fltsel_ = new uiMultiSurfaceRead( this,
			EM::Fault3D::typeStr(), &SI().zDomainInfo() );
    fltsel_->attach( alignedBelow, horsel_ );

    outnamefld_ = new uiGenInput( this, tr("Output file base name"),
				  StringInpSpec("section") );
    outnamefld_->attach( alignedBelow, fltsel_ );

    exportbut_ = new uiPushButton( this, tr("Export section DXF"),
			mCB(this,uiGeoDevKitSectionDXFDlg,exportCB), true );
    exportbut_->attach( alignedBelow, outnamefld_ );

    logfld_ = new uiTextEdit( this, "Export log", true );
    logfld_->attach( alignedBelow, exportbut_ );

    mAttachCB( sectypefld_->selectionChanged,
	       uiGeoDevKitSectionDXFDlg::sectionTypeCB );
    sectionTypeCB( nullptr );
}


uiGeoDevKitSectionDXFDlg::~uiGeoDevKitSectionDXFDlg()
{
    detachAllNotifiers();
}


void uiGeoDevKitSectionDXFDlg::sectionTypeCB( CallBacker* )
{
    const int st = sectypefld_->currentItem();
    inlnrfld_->display( st==0 );
    crlnrfld_->display( st==1 );
    line2dfld_->display( st==2 );
}


void uiGeoDevKitSectionDXFDlg::exportCB( CallBacker* )
{
    const int st = sectypefld_->currentItem();

    TypeSet<SecTrace> traces;
    BufferString sectdesc;

    if ( st==0 || st==1 )
    {
	const bool isinl = st==0;
	const int linenr = isinl ? inlnrfld_->getIntValue()
				 : crlnrfld_->getIntValue();
	const StepInterval<int> rg( isinl ? SI().crlRange()
					  : SI().inlRange() );
	if ( rg.start_>rg.stop_ )
	{
	    uiMSG().error( tr("The survey has no traces in that direction.") );
	    return;
	}
	double acc = 0.0;
	Coord prev;
	bool haveprev = false;
	for ( int t=rg.start_; t<=rg.stop_; t+=rg.step_>0 ? rg.step_ : 1 )
	{
	    SecTrace tr;
	    tr.bid_ = isinl ? BinID(linenr,t) : BinID(t,linenr);
	    tr.crd_ = SI().transform( tr.bid_ );
	    if ( haveprev )
		acc += tr.crd_.horDistTo( prev );
	    tr.dist_ = acc;
	    prev = tr.crd_;
	    haveprev = true;
	    traces += tr;
	}
	sectdesc.add( isinl ? tr("Inline ") : tr("Crossline ") )
		.add( linenr );
    }
    else
    {
	if ( line2dids_.isEmpty() )
	{
	    uiMSG().error( tr("No 2D survey line is available. "
			      "Load a 2D line first.") );
	    return;
	}
	const int selidx = line2dfld_->currentItem();
	if ( selidx<0 || selidx>=line2dids_.size() )
	{
	    uiMSG().error( tr("Please select a valid 2D survey line.") );
	    return;
	}
	const Pos::GeomID geomid = line2dids_[selidx];
	const Survey::Geometry* geom = Survey::GM().getGeometry( geomid );
	const Survey::Geometry2D* g2d = geom ? geom->as2D() : nullptr;
	if ( !g2d || g2d->data().positions().isEmpty() )
	{
	    uiMSG().error( tr("The selected line has no trace positions.") );
	    return;
	}
	const TypeSet<PosInfo::Line2DPos>& posns = g2d->data().positions();
	double acc = 0.0;
	Coord prev;
	bool haveprev = false;
	for ( int idx=0; idx<posns.size(); idx++ )
	{
	    SecTrace tr;
	    tr.crd_ = posns[idx].coord_;
	    tr.bid_ = SI().transform( tr.crd_ );
	    if ( haveprev )
		acc += tr.crd_.horDistTo( prev );
	    tr.dist_ = acc;
	    prev = tr.crd_;
	    haveprev = true;
	    traces += tr;
	}
	sectdesc.add( tr("2D line ") ).add( line2dnms_.get(selidx).buf() );
    }

    if ( traces.isEmpty() )
    {
	uiMSG().error( tr("No traces could be found on this section.") );
	return;
    }

    TypeSet<MultiID> horms, fltmids;
    horsel_->getSurfaceIds( horms );
    fltsel_->getSurfaceIds( fltmids );
    if ( horms.isEmpty() && fltmids.isEmpty() )
    {
	uiMSG().error( tr("Please select at least one horizon or fault.") );
	return;
    }

    GeoDevDXFWriter dxf;
    BufferString logtxt;
    logtxt.add( tr("Section: %1").arg(sectdesc.buf()) ).add( "\n" )
	  .add( tr("Traces sampled: %1").arg(traces.size()) ).add( "\n" );

    int nrlines = 0;

    // ---- 层位：逐道 getZ(BinID)，空节点断开折线 ----
    for ( int hidx=0; hidx<horms.size(); hidx++ )
    {
	RefMan<EM::EMObject> obj = EM::EMM().loadIfNotFullyLoaded( horms[hidx] );
	mDynamicCastGet( EM::Horizon3D*, hor, obj.ptr() )
	if ( !hor )
	    continue;

	const BufferString layernm( "HORIZON_", obj->name() );
	dxf.addLayer( layernm.buf(), 3 ); // 绿色

	TypeSet<Coord3> seg;
	for ( int t=0; t<traces.size(); t++ )
	{
	    const float z = hor->getZ( traces[t].bid_ );
	    if ( mIsUdf(z) )
	    {
		if ( seg.size()>1 )
		    { dxf.addPolyline( seg, false, layernm.buf() ); nrlines++; }
		seg.erase();
		continue;
	    }
	    seg += Coord3( traces[t].dist_, 0.0, z );
	}
	if ( seg.size()>1 )
	    { dxf.addPolyline( seg, false, layernm.buf() ); nrlines++; }

	logtxt.add( tr("Horizon polylines: %1 -> %2")
		      .arg(obj->name()).arg(layernm.buf()) ).add( "\n" );
    }

    // ---- 断层：逐棍节点，按是否在剖面上投影到 X 轴 ----
    // 3D inline 剖面：取 |bid.inl - 固定inl| <= 半个 inl 步长的节点；
    // 3D xline 剖面：对 crl 同理；2D 测线：只要该节点 BinID 与某采样道
    // BinID 相同即采纳（2D 道的 BinID 由测线坐标换算而来）。
    const int linenr = st==0 ? inlnrfld_->getIntValue()
			     : (st==1 ? crlnrfld_->getIntValue() : 0);
    const int inltol = st==0 ? (SI().inlRange().step_>0
				 ? SI().inlRange().step_ : 1) / 2 + 1 : 0;
    const int crltol = st==1 ? (SI().crlRange().step_>0
				 ? SI().crlRange().step_ : 1) / 2 + 1 : 0;

    for ( int fidx=0; fidx<fltmids.size(); fidx++ )
    {
	RefMan<EM::EMObject> obj = EM::EMM().loadIfNotFullyLoaded( fltmids[fidx] );
	mDynamicCastGet( EM::Fault3D*, flt, obj.ptr() )
	if ( !flt || !flt->geometry().geometryElement() )
	    continue;
	Geometry::FaultStickSurface* fss = flt->geometry().geometryElement();

	const BufferString layernm( "FAULT_", obj->name() );
	dxf.addLayer( layernm.buf(), 1 ); // 红色

	int nrsticks = 0;
	for ( int stick=0; stick<fss->nrSticks(); stick++ )
	{
	    const int nrknots = fss->nrKnots( stick );
	    if ( nrknots<1 )
		continue;

	    TypeSet<Coord3> pts;
	    for ( int knot=0; knot<nrknots; knot++ )
	    {
		const Coord3 p = fss->getKnot( RowCol(stick,knot) );
		if ( mIsUdf(p.z_) )
		    continue;
		const BinID bid = SI().transform( Coord(p.x_,p.y_) );
		bool onsection = false;
		if ( st==0 )
		    onsection = abs(bid.inl()-linenr) <= inltol;
		else if ( st==1 )
		    onsection = abs(bid.crl()-linenr) <= crltol;
		else
		{
		    for ( int t=0; t<traces.size(); t++ )
			if ( traces[t].bid_==bid )
			    { onsection = true; break; }
		}
		if ( !onsection )
		    continue;

		// 用节点自身平面坐标投影到剖面 X：沿剖面方向的累计距离
		// 近似为它到第一个采样道的平面距离（剖面为直线）。
		double dist = 0.0;
		if ( !traces.isEmpty() )
		    dist = traces[0].crd_.horDistTo( Coord(p.x_,p.y_) );
		pts += Coord3( dist, 0.0, p.z_ );
	    }
	    if ( pts.size()>1 )
	    {
		dxf.addPolyline( pts, false, layernm.buf() );
		nrsticks++;
	    }
	}
	nrlines += nrsticks;
	logtxt.add( tr("Fault sticks on section: %1 -> %2")
		      .arg(obj->name()).arg(nrsticks) ).add( "\n" );
    }

    if ( !nrlines )
    {
	uiMSG().error( tr("No horizon or fault data was found on this "
			  "section. Check the line number / selected objects.") );
	return;
    }

    BufferString base( outnamefld_->text() );
    if ( base.isEmpty() )
	base = "section";
    BufferString deffnm = base;
    deffnm.add( "_" ).add( sectdesc.buf() ).add( ".dxf" );
    deffnm.replace( ' ', '_' );
    uiFileDialog dlg( this, false, GetProcFileName(deffnm.buf()),
		      "*.dxf", tr("Export section as DXF") );
    if ( !dlg.go() )
	return;

    BufferString errmsg;
    if ( !dxf.write( dlg.fileName(), errmsg ) )
    {
	uiMSG().error( toUiString(errmsg.buf()) );
	return;
    }

    logtxt.add( "\n" ).add( tr("DXF entities written: %1")
				 .arg(dxf.nrEntities()) ).add( "\n" )
	   .add( tr("Saved to: %1").arg(dlg.fileName()) ).add( "\n" )
	   .add( tr("Note: X = distance along section [m], "
		    "Z = horizon/fault value; fault sticks show picked "
		    "knots only.") ).add( "\n" );
    logfld_->setText( logtxt.buf() );
    uiMSG().message( tr("Section DXF exported successfully.") );
}
