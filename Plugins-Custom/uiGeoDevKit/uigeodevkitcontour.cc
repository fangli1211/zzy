/*+
________________________________________________________________________

 等值线成图导出实现

 坐标映射与官方 uiContourTreeItemContourGenerator::addDisplayCoord 一致：
   IsoContourTracer 的输出顶点 vrtx.x_/vrtx.y_ 是「测量坐标含义」的
   Inline / Crossline 值（由 setSampling 指定），用
   SI().binID2Coord().transform(Coord(x,y)) 转成真实 X/Y；Z 即等值线值。
________________________________________________________________________
-*/

#include "uigeodevkitcontour.h"

#include "uigeodevkitmod.h"
#include "geodevkitdxf.h"

#include "binidsurface.h"
#include "ctxtioobj.h"
#include "emfault3d.h"
#include "emhorizon3d.h"
#include "emmanager.h"
#include "emobject.h"
#include "emsurfacetr.h"
#include "ioman.h"
#include "isocontourtracer.h"
#include "manobjectset.h"
#include "od_istream.h"
#include "od_ostream.h"
#include "oddirs.h"
#include "ptrman.h"
#include "refcount.h"
#include "survinfo.h"

#include "uibutton.h"
#include "uifiledlg.h"
#include "uigeninput.h"
#include "uiioobjsel.h"
#include "uimsg.h"
#include "uitextedit.h"


uiGeoDevKitContourDlg::uiGeoDevKitContourDlg( uiParent* p )
    : uiDialog( p, Setup(tr("Contour Map Export"),
			 tr("Trace contours on a 3D horizon and export to DXF"),
			 mNoHelpKey) )
{
    horsel_ = new uiIOObjSel( this, mIOObjContext(EMHorizon3D),
			      tr("3D Horizon") );
    fltsel_ = new uiIOObjSel( this, mIOObjContext(EMFault3D),
			      tr("Fault overlay (optional)") );
    fltsel_->attach( alignedBelow, horsel_ );

    startfld_ = new uiGenInput( this, tr("First contour"),
				 FloatInpSpec(0.f) );
    startfld_->attach( alignedBelow, fltsel_ );
    stepfld_ = new uiGenInput( this, tr("Contour interval"),
				 FloatInpSpec(100.f) );
    stepfld_->attach( rightOf, startfld_ );
    stopfld_ = new uiGenInput( this, tr("Last contour"),
				 FloatInpSpec(2000.f) );
    stopfld_->attach( rightOf, stepfld_ );

    uiPushButton* computebut = new uiPushButton( this,
		tr("Trace contours"),
		mCB(this,uiGeoDevKitContourDlg,computeCB), true );
    computebut->attach( alignedBelow, startfld_ );

    outnamefld_ = new uiGenInput( this, tr("Output base name"),
				  StringInpSpec("contours") );
    outnamefld_->attach( rightOf, computebut );

    uiPushButton* savebut = new uiPushButton( this, tr("Export DXF..."),
		mCB(this,uiGeoDevKitContourDlg,saveCB), true );
    savebut->attach( alignedBelow, computebut );

    resultfld_ = new uiTextEdit( this, "Contour result", true );
    resultfld_->attach( alignedBelow, savebut );
}


void uiGeoDevKitContourDlg::computeCB( CallBacker* )
{
    const IOObj* horioobj = horsel_->ioobj( true );
    if ( !horioobj )
    {
	uiMSG().error( tr("Please select a 3D horizon.") );
	return;
    }
    RefMan<EM::EMObject> emobj =
	EM::EMM().loadIfNotFullyLoaded( horioobj->key() );
    mDynamicCastGet( EM::Horizon3D*, hor, emobj.ptr() )
    if ( !hor )
    {
	uiMSG().error( tr("The selected object is not a 3D horizon.") );
	return;
    }
    Geometry::BinIDSurface* surf = hor->geometry().geometryElement();
    Array2D<float>* field = surf ? surf->getArray() : nullptr;
    if ( !field )
    {
	uiMSG().error( tr("The horizon has no usable grid.") );
	return;
    }

    const float start = startfld_->getFValue();
    const float step = stepfld_->getFValue();
    const float stop = stopfld_->getFValue();
    if ( mIsUdf(start) || mIsUdf(step) || mIsUdf(stop) || step<=0.f ||
	 stop<start )
    {
	uiMSG().error( tr("Invalid contour range: check the first contour, "
			  "interval and last contour.") );
	return;
    }
    // 防止误输入导致上万次全网格扫描。
    const int nrlevels = (int)((stop-start)/step) + 1;
    if ( nrlevels<1 || nrlevels>500 )
    {
	uiMSG().error( tr("The number of contour levels must be between 1 "
			  "and 500. Adjust the interval.") );
	return;
    }

    IsoContourTracer tracer( *field );
    tracer.setSampling( surf->rowRange(), surf->colRange() );

    GeoDevDXFWriter dxf;
    dxf.addLayer( "CONTOURS", 3 );
    dxf.addLayer( "FAULT_OVERLAY", 1 );

    BufferString txt;
    int nrcnt = 0;
    int nrpts = 0;
    for ( int lvl=0; lvl<nrlevels; lvl++ )
    {
	const float z = start + lvl*step;
	ManagedObjectSet<ODPolygon<float> > contours;
	if ( !tracer.getContours( contours, z, false ) )
	    continue;

	BufferString layernm( "CONTOUR_" );
	layernm.add( z );
	dxf.addLayer( layernm.buf(), 3 );
	for ( int ic=0; ic<contours.size(); ic++ )
	{
	    const ODPolygon<float>& poly = *contours[ic];
	    if ( poly.size()<2 )
		continue;
	    TypeSet<Coord3> line;
	    for ( int iv=0; iv<poly.size(); iv++ )
	    {
		const Geom::Point2D<float> v = poly.getVertex( iv );
		const Coord xy = SI().binID2Coord().transform(
						Coord(v.x_,v.y_) );
		line.add( Coord3(xy.x_,xy.y_,z) );
	    }
	    dxf.addPolyline( line, poly.isClosed(), layernm.buf() );
	    nrpts += line.size();
	}
	nrcnt += contours.size();
    }

    // 断层叠加：把每条断层棍投影成平面折线，帮助解释人员核对等值线
    // 在断层附近是否连续；Z 用断层棍自身的 Z，便于在 CAD 里分层观察。
    const IOObj* fltioobj = fltsel_->ioobj( true );
    int nrsticks = 0;
    if ( fltioobj )
    {
	RefMan<EM::EMObject> fltemobj =
	    EM::EMM().loadIfNotFullyLoaded( fltioobj->key() );
	mDynamicCastGet( EM::Fault3D*, flt, fltemobj.ptr() )
	if ( flt && flt->geometry().geometryElement() )
	{
	    Geometry::FaultStickSurface* fss =
		flt->geometry().geometryElement();
	    for ( int stick=0; stick<fss->nrSticks(); stick++ )
	    {
		TypeSet<Coord3> line;
		for ( int knot=0; knot<fss->nrKnots(stick); knot++ )
		    line.add( fss->getKnot(RowCol(stick,knot)) );
		if ( line.size()>1 )
		{
		    dxf.addPolyline( line, false, "FAULT_OVERLAY" );
		    nrsticks++;
		}
	    }
	}
    }

    lastdxf_.setEmpty();
    // 结果暂存在内存，由 saveCB 写出；这里同官方做法先把内容序列化到
    // BufferString，避免对话框关闭后对象析构导致指针失效。
    BufferString errmsg;
    // 生成一个临时内存占位：真正写出在 saveCB，这里用 nrEntities 判断。
    if ( dxf.nrEntities()<1 )
    {
	uiMSG().message( tr("No contour was found in the given range.") );
	return;
    }

    // 保存 DXF 文本：直接借用 write() 逻辑写到一个临时文件再由 saveCB
    // 复制，比在内存里重复实现 DXF 序列化更稳妥。
    const BufferString tmpfnm( GetProcFileName("geodevkit_contour_tmp.dxf") );
    if ( !dxf.write( tmpfnm.buf(), errmsg ) )
    {
	uiMSG().error( toUiString(errmsg.buf()) );
	return;
    }
    od_istream istr( tmpfnm.buf() );
    if ( istr.isOK() )
    {
	BufferString line;
	while ( istr.getLine(line) )
	    lastdxf_.add( line.buf() ).add( "\n" );
    }

    txt.add( tr("Horizon: %1").arg(horioobj->name()) ).add( "\n" );
    txt.add( tr("Contour levels: %1").arg(nrlevels) ).add( "\n" );
    txt.add( tr("Contour polylines: %1").arg(nrcnt) ).add( "\n" );
    txt.add( tr("Contour vertices: %1").arg(nrpts) ).add( "\n" );
    txt.add( tr("Fault sticks overlaid: %1").arg(nrsticks) ).add( "\n" );
    txt.add( tr("DXF entities: %1").arg(dxf.nrEntities()) ).add( "\n" );
    resultfld_->setText( txt.buf() );
    lastcontours_ = nrcnt;
}


void uiGeoDevKitContourDlg::saveCB( CallBacker* )
{
    if ( lastdxf_.isEmpty() )
    {
	uiMSG().error( tr("Please trace contours first.") );
	return;
    }
    BufferString deffnm( outnamefld_->text() );
    deffnm.add( ".dxf" );
    uiFileDialog dlg( this, false, GetProcFileName(deffnm.buf()), "*.dxf",
		      tr("Export contours as DXF") );
    if ( !dlg.go() )
	return;
    od_ostream strm( dlg.fileName() );
    if ( !strm.isOK() )
    {
	uiMSG().error( tr("Cannot open the output file for writing.") );
	return;
    }
    strm << lastdxf_.buf();
    strm.close();
    if ( strm.isOK() )
	uiMSG().message( tr("Contours exported successfully.") );
    else
	uiMSG().error( tr("Failed to write the output file.") );
}
