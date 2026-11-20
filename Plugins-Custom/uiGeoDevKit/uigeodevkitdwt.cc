/*+
________________________________________________________________________

 三维地震体 DWT 分解向导实现（P1）

 关键设计：
   - 一个输出分量 = 一个 SeisSingleTraceProc = 一遍输入扫描。
   - 逐道处理在 proctobedone_ 回调中完成；回调类 DWTProc 持有
     GeoDevDWT1D 引擎，按分量编号重建 cA/cD 或写属性标量。
   - uiTaskRunner 负责进度条与取消；每个分量跑完后检查用户是否取消。
   - 多基对比在正式分解前单独抽样若干道，结果写 ASCII 报告。
________________________________________________________________________
-*/

#include "uigeodevkitdwt.h"

#include "uigeodevkitmod.h"

#include "bufstringset.h"
#include "callback.h"
#include "ctxtioobj.h"
#include "datainpspec.h"
#include "file.h"
#include "filepath.h"
#include "ioman.h"
#include "ioobj.h"
#include "multiid.h"
#include "od_ostream.h"
#include "oddirs.h"
#include "ptrman.h"
#include "seiscbvs.h"
#include "seisioobjinfo.h"
#include "seisread.h"
#include "seissingtrcproc.h"
#include "seistrc.h"
#include "seistrctr.h"
#include "uibutton.h"
#include "uicombobox.h"
#include "uigeninput.h"
#include "uiioobjsel.h"
#include "uilabel.h"
#include "uimsg.h"
#include "uitaskrunner.h"
#include "uitextedit.h"


// 分量编码：>=0 为近似/细节级号（-1=cA, 1..L=cD_j）；负值区间为属性
enum CompKind { kApprox=-1, kApproxEnergy=-2, kDetailEnergy=-3,
		kRelDetail=-4, kEntropy=-5, kDetailBase=1 };


static bool isDetailComp( int c )	{ return c>=kDetailBase; }


// 每道 DWT 处理回调：holder 持有引擎，proc 是执行器，component 指定输出
class DWTProc : public CallBacker
{
public:
    DWTProc( GeoDevDWT1D::WaveletID wt,int level,int component )
	: engine_(wt), level_(level), component_(component) {}

    void setProc( SeisSingleTraceProc* p ) { proc_ = p; }

    void procCB( CallBacker* )
    {
	if ( !proc_ ) return;
	SeisTrc& trc = proc_->getTrace();
	const int n = trc.size();
	if ( n<2 ) return;	// 空道原样保留

	TypeSet<float> sig; sig.setSize( n );
	for ( int idx=0; idx<n; idx++ )
	    sig[idx] = trc.get( idx, 0 );

	if ( !engine_.decompose(sig.arr(),n) ) return;

	TypeSet<float> out; out.setSize( n );
	bool ok = false;
	if ( component_==kApprox )
	    ok = engine_.reconstructApprox( level_, out.arr(), n );
	else if ( isDetailComp(component_) )
	    ok = engine_.reconstructDetail( component_, out.arr(), n );
	else
	{
	    double val = 0;
	    if ( component_==kApproxEnergy ) val = engine_.approxEnergy(level_);
	    else if ( component_==kDetailEnergy ) val = engine_.detailEnergy(level_);
	    else if ( component_==kRelDetail ) val = engine_.relativeDetailEnergy(level_);
	    else if ( component_==kEntropy ) val = engine_.entropy();
	    for ( int idx=0; idx<n; idx++ ) out[idx] = (float)val;
	    ok = true;
	}
	if ( !ok ) return;

	for ( int idx=0; idx<n; idx++ )
	    trc.set( idx, out[idx], 0 );
    }

private:
    GeoDevDWT1D	engine_;
    int		level_;
    int		component_;
    SeisSingleTraceProc* proc_ = nullptr;
};


static bool createOutputObj( const char* nm, MultiID& outid,
			     BufferString& errmsg )
{
    IOObjContext ctxt( mIOObjContext(SeisTrc) );
    ctxt.forread_ = false;
    ctxt.deftransl_ = CBVSSeisTrcTranslator::translKey();
    CtxtIOObj ctio( ctxt );
    ctio.setName( nm );
    IOM().getEntry( ctio );
    if ( !ctio.ioobj_ )
    {
	errmsg = "Cannot create output volume entry in the data base";
	return false;
    }
    IOM().commitChanges( *ctio.ioobj_ );
    outid = ctio.ioobj_->key();
    return true;
}


uiGeoDevKitDWTDlg::uiGeoDevKitDWTDlg( uiParent* p )
    : uiDialog(p,uiDialog::Setup(tr("3D Volume DWT Decomposition"),
	 tr("Wavelet multi-resolution decomposition with same-grid output volumes"),
	 mNoHelpKey))
{
    GeoDevDWT1D::getWaveletNames( wnames_ );

    inpsel_ = new uiIOObjSel( this, mIOObjContext(SeisTrc),
			      tr("Input 3D seismic volume") );

    wavefld_ = new uiComboBox( this, "Wavelet basis" );
    wavefld_->addItems( wnames_ );
    wavefld_->setCurrentItem( GeoDevDWT1D::idxFromWavelet(
				  WaveletTransform::Daubechies4) );
    wavefld_->attach( alignedBelow, inpsel_ );

    levelfld_ = new uiGenInput( this, tr("Decomposition levels (1..L)"),
				IntInpSpec(3) );
    levelfld_->attach( alignedBelow, wavefld_ );

    compapprox_ = new uiGenInput( this, tr("Output approximation cA_J"),
				  BoolInpSpec(true) );
    compapprox_->attach( alignedBelow, levelfld_ );

    compdetail_ = new uiGenInput( this, tr("Output details cD_1..cD_J"),
				  BoolInpSpec(true) );
    compdetail_->attach( alignedBelow, compapprox_ );

    attrfld_ = new uiGenInput( this,
		tr("Output attributes (approx/detail energy, relative, entropy)"),
		BoolInpSpec(false) );
    attrfld_->attach( alignedBelow, compdetail_ );

    cmpfld_ = new uiGenInput( this, tr("Multi-wavelet comparison report"),
			      BoolInpSpec(true) );
    cmpfld_->attach( alignedBelow, attrfld_ );

    cmpsamplesfld_ = new uiGenInput( this,
		tr("Traces sampled for comparison"), IntInpSpec(50) );
    cmpsamplesfld_->attach( alignedBelow, cmpfld_ );

    outpfx_ = new uiGenInput( this, tr("Output name prefix"),
			      StringInpSpec("DWT") );
    outpfx_->attach( alignedBelow, cmpsamplesfld_ );

    infolbl_ = new uiLabel( this, tr(
	"Boundary mode: circular (kernel fixed, not configurable). "
	"Input signal length is zero-padded to a power of two internally; "
	"all output volumes keep the original grid.") );
    infolbl_->attach( alignedBelow, outpfx_ );

    runbut_ = new uiPushButton( this, tr("&Run decomposition"),
				mCB(this,uiGeoDevKitDWTDlg,runCB), true );
    runbut_->attach( alignedBelow, infolbl_ );

    logfld_ = new uiTextEdit( this, "DWT run log", true );
    logfld_->setPrefHeightInChar( 10 );
    logfld_->attach( alignedBelow, runbut_ );
}


uiGeoDevKitDWTDlg::~uiGeoDevKitDWTDlg()
{
}


void uiGeoDevKitDWTDlg::log( const uiString& msg )
{
    BufferString txt = logfld_->text();
    txt.add( msg.getFullString() ).add( "\n" );
    logfld_->setText( txt.buf() );
}


bool uiGeoDevKitDWTDlg::createOutput( const IOObj&, const char* outnm,
				      MultiID& outid ) const
{
    BufferString errmsg;
    return ::createOutputObj( outnm, outid, errmsg );
}


void uiGeoDevKitDWTDlg::runCB( CallBacker* )
{
    const MultiID mid = inpsel_->key();
    if ( mid.isUdf() )
    {
	uiMSG().error( tr("Please select an input 3D seismic volume.") );
	return;
    }
    PtrMan<IOObj> inpobj = IOM().get( mid );
    if ( !inpobj )
    {
	uiMSG().error( tr("Cannot open the selected input volume.") );
	return;
    }
    SeisIOObjInfo sii( *inpobj );
    if ( !sii.isOK() || sii.is2D() )
    {
	uiMSG().error( tr("The selected object is not a 3D seismic volume.") );
	return;
    }

    const int level = levelfld_->getIntValue();
    const int maxlev = GeoDevDWT1D::maxLevel(1024); // 仅作上限参考，实际逐道检查
    if ( level<1 || level>12 )
    {
	uiMSG().error( tr("Decomposition level must be between 1 and 12.") );
	return;
    }
    const GeoDevDWT1D::WaveletID wt =
	GeoDevDWT1D::waveletFromIdx( wavefld_->currentItem() );

    // 收集输出分量列表
    TypeSet<int> comps;
    if ( compapprox_->getBoolValue() ) comps += (int)kApprox;
    if ( compdetail_->getBoolValue() )
	for ( int lvl=1; lvl<=level; lvl++ ) comps += lvl;
    if ( attrfld_->getBoolValue() )
    {
	comps += (int)kApproxEnergy;
	comps += (int)kDetailEnergy;
	comps += (int)kRelDetail;
	comps += (int)kEntropy;
    }
    if ( comps.isEmpty() )
    {
	uiMSG().error( tr("Select at least one output component.") );
	return;
    }
    (void)maxlev;

    const BufferString pfx = outpfx_->text();
    log( tr("Starting DWT decomposition...") );

    // 多基对比（抽样）
    if ( cmpfld_->getBoolValue() )
    {
	GeoDevDWTCompare cmp( level );
	const int nsamp = cmpsamplesfld_->getIntValue();
	Seis::GeomType gt = Seis::Vol;
	SeisTrcReader rdr( mid, gt );
	if ( rdr.prepareWork() )
	{
	    SeisTrc trc; int cnt=0;
	    while ( cnt<nsamp && rdr.get(trc) )
	    {
		const int n = trc.size();
		if ( n<2 ) continue;
		TypeSet<float> sig; sig.setSize( n );
		for ( int idx=0; idx<n; idx++ ) sig[idx] = trc.get( idx, 0 );
		cmp.addSample( sig.arr(), n );
		cnt++;
	    }
	}
	cmp.addWavelet( wt );
	// 常用对比基：db4 / sym4 / coif3（若与所选重复也无妨）
	cmp.addWavelet( WaveletTransform::Daubechies4 );
	cmp.addWavelet( WaveletTransform::Symmlet4 );
	cmp.addWavelet( WaveletTransform::Coiflet3 );
	cmp.compute();

	BufferString rptnm = pfx; rptnm.add( "_wavelet_report.txt" );
	FilePath fp( GetDataDir(), rptnm.buf() );
	BufferString rptfnm = fp.fullPath();
	od_ostream strm( rptfnm );
	if ( strm.isOK() )
	{
	    cmp.writeReport( strm );
	    log( tr("Comparison report: %1").arg(rptfnm.buf()) );
	}
	else
	    log( tr("Failed to write comparison report.") );
    }

    // 逐分量处理
    bool allok = true;
    int donecnt = 0;
    for ( int ci=0; ci<comps.size(); ci++ )
    {
	const int comp = comps[ci];
	BufferString outnm = pfx;
	if ( comp==kApprox ) outnm.add( "_cA" ).add( level );
	else if ( isDetailComp(comp) ) outnm.add( "_cD" ).add( comp );
	else if ( comp==kApproxEnergy ) outnm.add( "_ApproxEnergy" );
	else if ( comp==kDetailEnergy ) outnm.add( "_DetailEnergy" );
	else if ( comp==kRelDetail ) outnm.add( "_RelDetailEnergy" );
	else if ( comp==kEntropy ) outnm.add( "_Entropy" );

	MultiID outid;
	BufferString errmsg;
	if ( !::createOutputObj(outnm.buf(),outid,errmsg) )
	{
	    log( toUiString(errmsg.buf()) );
	    allok = false;
	    break;
	}
	PtrMan<IOObj> outobj = IOM().get( outid );
	if ( !outobj ) { allok=false; break; }

	DWTProc dwtproc( wt, level, comp );
	SeisSingleTraceProc stp( *inpobj, *outobj, "GeoDevKit DWT" );
	dwtproc.setProc( &stp );
	stp.proctobedone_.notify( mCB(&dwtproc,DWTProc,procCB) );

	uiTaskRunner runner( this );
	if ( !runner.execute(stp) || !stp.isOK() )
	{
	    log( tr("Failed on component: %1").arg(outnm.buf()) );
	    allok = false;
	    break;
	}
	stp.proctobedone_.remove( mCB(&dwtproc,DWTProc,procCB) );
	donecnt++;
	log( tr("Completed: %1").arg(outnm.buf()) );
    }

    if ( allok )
	uiMSG().message( tr("DWT decomposition completed. %1 volumes written.")
			 .arg(donecnt) );
    else
	uiMSG().warning( tr("DWT decomposition finished with errors. "
			    "See the run log.") );
}
