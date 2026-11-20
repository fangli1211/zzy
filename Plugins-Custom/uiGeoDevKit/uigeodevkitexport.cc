/*+
________________________________________________________________________

 成果导出中心实现

 每个导出函数都遵循同一模式：
   读对象 -> 组装 TypeSet<Coord3> 分层点列 -> 调 GeoDevDXFWriter -> 落盘；
   ASCII 模式则把同样的数据拼成带表头的文本。这样 DXF 与 ASCII 内容一致，
 便于在 CAD 与 Excel 之间交叉核对。
________________________________________________________________________
-*/

#include "uigeodevkitexport.h"

#include "uigeodevkitmod.h"

#include "binidsurface.h"
#include "bufstringset.h"
#include "ctxtioobj.h"
#include "emfault3d.h"
#include "emhorizon3d.h"
#include "emmanager.h"
#include "emobject.h"
#include "emsurfacetr.h"
#include "explfaultsticksurface.h"
#include "faulthorintersect.h"
#include "geodevkitdxf.h"
#include "ioman.h"
#include "od_ostream.h"
#include "oddirs.h"
#include "posinfo2d.h"
#include "ptrman.h"
#include "refcount.h"
#include "survgeom.h"
#include "survgeom2d.h"
#include "survinfo.h"
#include "wellman.h"
#include "welldata.h"
#include "welltrack.h"
#include "welltransl.h"

#include "uibutton.h"
#include "uicombobox.h"
#include "uifiledlg.h"
#include "uigeninput.h"
#include "uiioobjsel.h"
#include "uimsg.h"
#include "uitextedit.h"


uiGeoDevKitExportDlg::uiGeoDevKitExportDlg( uiParent* p )
    : uiDialog( p, Setup(tr("Interpretation Export Center"),
			 tr("Export horizons, faults, intersections, wells "
			    "and 2D lines to DXF or ASCII"),
			 mNoHelpKey) )
{
    formatfld_ = new uiGenInput( this, tr("Output format"),
		BoolInpSpec(true,tr("DXF (AutoCAD R12)"),
			    tr("ASCII (X Y Z table)")) );

    // ---- 三维层位 ----
    horsel_ = new uiIOObjSel( this, mIOObjContext(EMHorizon3D),
			      tr("3D Horizon") );
    horsel_->attach( alignedBelow, formatfld_ );
    uiPushButton* horbut = new uiPushButton( this, tr("Export horizon"),
		mCB(this,uiGeoDevKitExportDlg,exportHorizonCB), true );
    horbut->attach( rightOf, horsel_ );

    // ---- 三维断层 ----
    fltsel_ = new uiIOObjSel( this, mIOObjContext(EMFault3D),
			      tr("3D Fault") );
    fltsel_->attach( alignedBelow, horsel_ );
    uiPushButton* fltbut = new uiPushButton( this, tr("Export fault"),
		mCB(this,uiGeoDevKitExportDlg,exportFaultCB), true );
    fltbut->attach( rightOf, fltsel_ );

    // ---- 交线（层位 + 断层 + 垂向偏移）----
    hor2sel_ = new uiIOObjSel( this, mIOObjContext(EMHorizon3D),
			       tr("Horizon for intersection") );
    hor2sel_->attach( alignedBelow, fltsel_ );
    flt2sel_ = new uiIOObjSel( this, mIOObjContext(EMFault3D),
			       tr("Fault for intersection") );
    flt2sel_->attach( alignedBelow, hor2sel_ );
    horshiftfld_ = new uiGenInput( this, tr("Horizon vertical shift"),
				   FloatInpSpec(0.f) );
    horshiftfld_->attach( alignedBelow, flt2sel_ );
    uiPushButton* isectbut = new uiPushButton( this,
		tr("Export intersection"),
		mCB(this,uiGeoDevKitExportDlg,exportIntersectCB), true );
    isectbut->attach( rightOf, horshiftfld_ );

    // ---- 井（全部井，按名称列出供核对）----
    BufferStringSet wellnms;
    Well::MGR().getWellNames( wellnms, false );
    welllist_ = new uiComboBox( this, "Wells" );
    welllist_->addItems( wellnms );
    welllist_->attach( alignedBelow, horshiftfld_ );
    for ( int idx=0; idx<wellnms.size(); idx++ )
    {
	PtrMan<IOObj> ioobj = Well::findIOObj( wellnms.get(idx), nullptr );
	if ( ioobj )
	    wellids_.add( ioobj->key() );
    }
    uiPushButton* wellbut = new uiPushButton( this, tr("Export all wells"),
		mCB(this,uiGeoDevKitExportDlg,exportWellsCB), true );
    wellbut->attach( rightOf, welllist_ );

    // ---- 2D 测线 ----
    BufferStringSet linenms;
    TypeSet<Pos::GeomID> geomids;
    Survey::GM().getList( linenms, geomids, true );
    linelist_ = new uiComboBox( this, "2D survey lines" );
    linelist_->addItems( linenms );
    linelist_->attach( alignedBelow, welllist_ );
    for ( int idx=0; idx<linenms.size(); idx++ )
	lineidxs_.add( idx );
    uiPushButton* linebut = new uiPushButton( this, tr("Export all lines"),
		mCB(this,uiGeoDevKitExportDlg,exportLinesCB), true );
    linebut->attach( rightOf, linelist_ );

    outnamefld_ = new uiGenInput( this, tr("Output base name"),
				  StringInpSpec("interpretation") );
    outnamefld_->attach( alignedBelow, linelist_ );

    logfld_ = new uiTextEdit( this, "Export log", true );
    logfld_->attach( alignedBelow, outnamefld_ );
}


void uiGeoDevKitExportDlg::log( const uiString& msg )
{
    BufferString txt = logfld_->text();
    txt.add( msg.getFullString() ).add( "\n" );
    logfld_->setText( txt.buf() );
}


void uiGeoDevKitExportDlg::writeAscii( const char* deffnm, const uiString& title,
				       const BufferString& content )
{
    uiFileDialog dlg( this, false, GetProcFileName(deffnm), "*.txt",
		      tr("Export ASCII data") );
    if ( !dlg.go() )
	return;
    od_ostream strm( dlg.fileName() );
    if ( !strm.isOK() )
    {
	uiMSG().error( tr("Cannot open the output file for writing.") );
	return;
    }
    strm << content.buf();
    strm.close();
    if ( !strm.isOK() )
	uiMSG().error( tr("Failed to write the output file.") );
    else
	log( title );
}


bool uiGeoDevKitExportDlg::writeOutput( const GeoDevDXFWriter& writer,
					const char* deffnm, const uiString& title )
{
    if ( !formatfld_->getBoolValue() )
    {
	// ASCII 分支由各回调自行组装内容，这里仅作兜底提示。
	return false;
    }
    uiFileDialog dlg( this, false, GetProcFileName(deffnm), "*.dxf",
		      tr("Export DXF file") );
    if ( !dlg.go() )
	return false;
    BufferString errmsg;
    if ( !writer.write( dlg.fileName(), errmsg ) )
    {
	uiMSG().error( toUiString(errmsg.buf()) );
	return false;
    }
    log( title );
    return true;
}


void uiGeoDevKitExportDlg::exportHorizonCB( CallBacker* )
{
    const IOObj* ioobj = horsel_->ioobj( true );
    if ( !ioobj )
    {
	uiMSG().error( tr("Please select a 3D horizon first.") );
	return;
    }
    RefMan<EM::EMObject> emobj =
	EM::EMM().loadIfNotFullyLoaded( ioobj->key() );
    mDynamicCastGet( EM::Horizon3D*, hor, emobj.ptr() )
    if ( !hor )
    {
	uiMSG().error( tr("The selected object is not a 3D horizon.") );
	return;
    }
    Geometry::BinIDSurface* surf = hor->geometry().geometryElement();
    if ( !surf || !surf->getArray() )
    {
	uiMSG().error( tr("The horizon has no usable grid.") );
	return;
    }

    const StepInterval<int> inlrg = surf->rowRange();
    const StepInterval<int> crlrg = surf->colRange();
    const BufferString layernm( "HORIZON_", ioobj->name() );

    GeoDevDXFWriter dxf;
    dxf.addLayer( layernm.buf(), 3 );
    BufferString asc;
    asc.add( "# Horizon: " ).add( ioobj->name() ).add( "\n" );
    asc.add( "# X\tY\tZ (" ).add( SI().getZUnitString() ).add( ")\n" );

    int nrwrt = 0;
    for ( int inl=inlrg.start_; inl<=inlrg.stop_; inl+=inlrg.step_ )
    {
	TypeSet<Coord3> line;
	for ( int crl=crlrg.start_; crl<=crlrg.stop_; crl+=crlrg.step_ )
	{
	    const Coord3 p = hor->getCoord( TrcKey(BinID(inl,crl)) );
	    if ( mIsUdf(p.z_) )
		continue;
	    line.add( p );
	    asc.add( p.x_ ).add( '\t' ).add( p.y_ ).add( '\t' )
	       .add( p.z_ ).add( '\n' );
	    nrwrt++;
	    // POINT 数量过多会拖慢 CAD；折线已经完整保留网格，只在网格较小
	    // 时额外写点，避免几十万点导致文件暴涨。
	}
	if ( line.size()>1 )
	    dxf.addPolyline( line, false, layernm.buf() );
    }

    if ( !nrwrt )
    {
	uiMSG().error( tr("The horizon contains no defined nodes.") );
	return;
    }

    BufferString deffnm( outnamefld_->text() );
    deffnm.add( "_horizon" );
    if ( !formatfld_->getBoolValue() )
    {
	deffnm.add( ".txt" );
	writeAscii( deffnm.buf(), tr("Horizon exported as ASCII."), asc );
    }
    else
    {
	deffnm.add( ".dxf" );
	writeOutput( dxf, deffnm.buf(), tr("Horizon exported as DXF.") );
    }
    log( tr("Horizon nodes exported: %1").arg(nrwrt) );
}


void uiGeoDevKitExportDlg::exportFaultCB( CallBacker* )
{
    const IOObj* ioobj = fltsel_->ioobj( true );
    if ( !ioobj )
    {
	uiMSG().error( tr("Please select a 3D fault first.") );
	return;
    }
    RefMan<EM::EMObject> emobj =
	EM::EMM().loadIfNotFullyLoaded( ioobj->key() );
    mDynamicCastGet( EM::Fault3D*, flt, emobj.ptr() )
    if ( !flt )
    {
	uiMSG().error( tr("The selected object is not a 3D fault.") );
	return;
    }
    Geometry::FaultStickSurface* fss = flt->geometry().geometryElement();
    if ( !fss )
    {
	uiMSG().error( tr("The fault has no usable geometry.") );
	return;
    }

    const BufferString layernm( "FAULT_", ioobj->name() );
    GeoDevDXFWriter dxf;
    dxf.addLayer( layernm.buf(), 1 );
    BufferString asc;
    asc.add( "# Fault: " ).add( ioobj->name() ).add( "\n" );
    asc.add( "# Stick\tKnot\tX\tY\tZ\n" );

    int nrsticks = 0;
    for ( int stick=0; stick<fss->nrSticks(); stick++ )
    {
	const int nrknots = fss->nrKnots( stick );
	if ( nrknots<2 )
	    continue;
	TypeSet<Coord3> line;
	for ( int knot=0; knot<nrknots; knot++ )
	{
	    const Coord3 p = fss->getKnot( RowCol(stick,knot) );
	    if ( mIsUdf(p.z_) )
		continue;
	    line.add( p );
	    asc.add( stick ).add( '\t' ).add( knot ).add( '\t' )
	       .add( p.x_ ).add( '\t' ).add( p.y_ ).add( '\t' )
	       .add( p.z_ ).add( '\n' );
	}
	if ( line.size()>1 )
	{
	    dxf.addPolyline( line, false, layernm.buf() );
	    nrsticks++;
	}
    }

    if ( !nrsticks )
    {
	uiMSG().error( tr("The fault contains no usable stick.") );
	return;
    }

    BufferString deffnm( outnamefld_->text() );
    deffnm.add( "_fault" );
    if ( !formatfld_->getBoolValue() )
    {
	deffnm.add( ".txt" );
	writeAscii( deffnm.buf(), tr("Fault exported as ASCII."), asc );
    }
    else
    {
	deffnm.add( ".dxf" );
	writeOutput( dxf, deffnm.buf(), tr("Fault exported as DXF.") );
    }
    log( tr("Fault sticks exported: %1").arg(nrsticks) );
}


void uiGeoDevKitExportDlg::exportIntersectCB( CallBacker* )
{
    const IOObj* horioobj = hor2sel_->ioobj( true );
    const IOObj* fltioobj = flt2sel_->ioobj( true );
    if ( !horioobj || !fltioobj )
    {
	uiMSG().error( tr("Please select both a horizon and a fault.") );
	return;
    }
    RefMan<EM::EMObject> horemobj =
	EM::EMM().loadIfNotFullyLoaded( horioobj->key() );
    mDynamicCastGet( EM::Horizon3D*, hor, horemobj.ptr() )
    RefMan<EM::EMObject> fltemobj =
	EM::EMM().loadIfNotFullyLoaded( fltioobj->key() );
    mDynamicCastGet( EM::Fault3D*, flt, fltemobj.ptr() )
    if ( !hor || !flt )
    {
	uiMSG().error( tr("Selected objects must be a 3D horizon and a "
			  "3D fault.") );
	return;
    }
    Geometry::BinIDSurface* bidsurf = hor->geometry().geometryElement();
    Geometry::FaultStickSurface* fss = flt->geometry().geometryElement();
    if ( !bidsurf || !fss )
    {
	uiMSG().error( tr("Selected objects have no usable geometry.") );
	return;
    }
    PtrMan<Geometry::ExplFaultStickSurface> expfault =
	new Geometry::ExplFaultStickSurface( fss,
		sCast(float,SI().zDomain().userFactor()) );
    expfault->setCoordList( new Coord3ListImpl, new Coord3ListImpl, 0 );
    if ( !expfault->update(true,0) )
    {
	uiMSG().error( tr("Failed to build the fault surface.") );
	return;
    }

    Coord3ListImpl crdlist;
    const float horshift = horshiftfld_->getFValue( 0.f );
    Geometry::FaultBinIDSurfaceIntersector intersector( horshift, *bidsurf,
							*expfault, crdlist );
    intersector.compute();
    if ( crdlist.size()<1 )
    {
	uiMSG().error( tr("No intersection was found.") );
	return;
    }

    const BufferString layernm( "INTERSECTION_" );
    GeoDevDXFWriter dxf;
    dxf.addLayer( layernm.buf(), 5 );
    BufferString asc;
    asc.add( "# Fault-horizon intersection\n# X\tY\tZ\n" );
    TypeSet<Coord3> line;
    for ( int idx=0; idx<crdlist.size(); idx++ )
    {
	const Coord3 p = crdlist.get( idx );
	line.add( p );
	asc.add( p.x_ ).add( '\t' ).add( p.y_ ).add( '\t' )
	   .add( p.z_ ).add( '\n' );
    }
    dxf.addPolyline( line, false, layernm.buf() );

    BufferString deffnm( outnamefld_->text() );
    deffnm.add( "_intersection" );
    if ( !formatfld_->getBoolValue() )
    {
	deffnm.add( ".txt" );
	writeAscii( deffnm.buf(), tr("Intersection exported as ASCII."), asc );
    }
    else
    {
	deffnm.add( ".dxf" );
	writeOutput( dxf, deffnm.buf(), tr("Intersection exported as DXF.") );
    }
    log( tr("Intersection points exported: %1").arg(crdlist.size()) );
}


void uiGeoDevKitExportDlg::exportWellsCB( CallBacker* )
{
    if ( wellids_.isEmpty() )
    {
	uiMSG().error( tr("No wells are available in this survey.") );
	return;
    }
    GeoDevDXFWriter dxf;
    dxf.addLayer( "WELL_HEAD", 4 );
    dxf.addLayer( "WELL_TRACK", 4 );
    BufferString asc;
    asc.add( "# Well\tMD\tX\tY\tZ\n" );
    int nrwells = 0;

    for ( int idx=0; idx<wellids_.size(); idx++ )
    {
	RefMan<Well::Data> wd = Well::MGR().get( wellids_[idx] );
	if ( !wd )
	    continue;
	const Well::Track& trck = wd->track();
	const Coord3 head( wd->info().surfacecoord_.x_,
			   wd->info().surfacecoord_.y_, 0. );
	dxf.addPoint( head, "WELL_HEAD" );
	TypeSet<Coord3> line;
	line.add( head );
	for ( int idy=0; idy<trck.size(); idy++ )
	{
	    const Coord3 p = trck.pos( idy );
	    line.add( p );
	    asc.add( wd->name() ).add( '\t' ).add( p.z_ ).add( '\t' )
	       .add( p.x_ ).add( '\t' ).add( p.y_ ).add( '\t' )
	       .add( p.z_ ).add( '\n' );
	}
	if ( line.size()>1 )
	    dxf.addPolyline( line, false, "WELL_TRACK" );
	nrwells++;
    }
    if ( !nrwells )
    {
	uiMSG().error( tr("No well trajectory could be loaded.") );
	return;
    }

    BufferString deffnm( outnamefld_->text() );
    deffnm.add( "_wells" );
    if ( !formatfld_->getBoolValue() )
    {
	deffnm.add( ".txt" );
	writeAscii( deffnm.buf(), tr("Wells exported as ASCII."), asc );
    }
    else
    {
	deffnm.add( ".dxf" );
	writeOutput( dxf, deffnm.buf(), tr("Wells exported as DXF.") );
    }
    log( tr("Wells exported: %1").arg(nrwells) );
}


void uiGeoDevKitExportDlg::exportLinesCB( CallBacker* )
{
    if ( lineidxs_.isEmpty() )
    {
	uiMSG().error( tr("No 2D survey lines are available.") );
	return;
    }
    GeoDevDXFWriter dxf;
    dxf.addLayer( "SURVEY_LINE", 2 );
    BufferString asc;
    asc.add( "# Line\tTraceNr\tX\tY\n" );
    int nrlines = 0;

    for ( int idx=0; idx<lineidxs_.size(); idx++ )
    {
	BufferStringSet nms;
	TypeSet<Pos::GeomID> ids;
	Survey::GM().getList( nms, ids, true );
	if ( lineidxs_[idx]>=ids.size() )
	    continue;
	const Survey::Geometry* geom = Survey::GM().getGeometry( ids[lineidxs_[idx]] );
	const Survey::Geometry2D* g2d = geom ? geom->as2D() : nullptr;
	if ( !g2d )
	    continue;
	const TypeSet<PosInfo::Line2DPos>& posns = g2d->data().positions();
	if ( posns.size()<2 )
	    continue;
	TypeSet<Coord3> line;
	for ( int ip=0; ip<posns.size(); ip++ )
	{
	    const PosInfo::Line2DPos& p = posns[ip];
	    line.add( Coord3(p.coord_.x_,p.coord_.y_,0.) );
	    asc.add( g2d->getName() ).add( '\t' ).add( p.nr_ ).add( '\t' )
	       .add( p.coord_.x_ ).add( '\t' ).add( p.coord_.y_ ).add( '\n' );
	}
	dxf.addPolyline( line, false, "SURVEY_LINE" );
	dxf.addText( line[0], nms.get(lineidxs_[idx]).buf(), 50.,
		     "SURVEY_LINE" );
	nrlines++;
    }
    if ( !nrlines )
    {
	uiMSG().error( tr("No usable 2D survey line was found.") );
	return;
    }

    BufferString deffnm( outnamefld_->text() );
    deffnm.add( "_lines" );
    if ( !formatfld_->getBoolValue() )
    {
	deffnm.add( ".txt" );
	writeAscii( deffnm.buf(), tr("Survey lines exported as ASCII."), asc );
    }
    else
    {
	deffnm.add( ".dxf" );
	writeOutput( dxf, deffnm.buf(), tr("Survey lines exported as DXF.") );
    }
    log( tr("2D lines exported: %1").arg(nrlines) );
}
