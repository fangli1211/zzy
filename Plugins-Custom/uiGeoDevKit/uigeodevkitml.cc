#include "uigeodevkitml.h"

#include "uibutton.h"
#include "bufstringset.h"
#include "uicombobox.h"
#include "uigeninput.h"
#include "uilabel.h"
#include "uilistbox.h"
#include "uimsg.h"
#include "uitabstack.h"
#include "uitable.h"

#include "survinfo.h"


// 统一提示：明确这是 UI 壳，不做任何计算。uiString 由对话框成员用自身
// tr() 生成，保证翻译上下文正确。
static void showStubMessage( const uiString& msg )
{
    uiMSG().message( msg );
}


//=================== 控制中心 ==========================================

uiGeoDevKitMLCenterDlg::uiGeoDevKitMLCenterDlg( uiParent* p )
    : uiDialog(p,uiDialog::Setup(
	tr("Machine Learning Control Center (UI Shell)"),
	tr("Geological interpretation - self-developed UI placeholder"),
	mNoHelpKey))
{
    // Project 行
    projectfld_ = new uiGenInput( this, tr("Project"), StringInpSpec() );
    projectfld_->setText( GetSurveyName() );
    projectfld_->setReadOnly();

    // 左侧五个类别（截图里的 Wells / Seismic / Seismic+Wells / 预训练 /
    // 神经网络）。用单选列表复刻，不提供真实功能。
    uiListBox::Setup catsu( OD::ChooseOnlyOne, tr("Input source") );
    catlist_ = new uiListBox( this, catsu, "ML categories" );
    catlist_->addItem( tr("Wells") );
    catlist_->addItem( tr("Seismic") );
    catlist_->addItem( tr("Seismic + Wells") );
    catlist_->addItem( tr("Pre-trained Models") );
    catlist_->addItem( tr("Neural Networks") );
    catlist_->setCurrentItem( 1 );   // Seismic，与截图一致
    catlist_->attach( leftBorder, 5 );
    catlist_->attach( alignedBelow, projectfld_ );

    // 右侧 3D Seismic 功能树（截图里的四项）。用单选列表复刻。
    uiListBox::Setup tasksu( OD::ChooseOnlyOne, tr("3D Seismic") );
    tasklist_ = new uiListBox( this, tasksu, "ML tasks" );
    tasklist_->addItem( tr("Horizon-based Seismic Facies") );
    tasklist_->addItem( tr("Seismic Classification (Supervised)") );
    tasklist_->addItem( tr("Seismic Image Segmentation") );
    tasklist_->addItem( tr("Seismic Image Regression") );
    tasklist_->setCurrentItem( 2 );   // Segmentation，与截图一致
    tasklist_->setNrLines( 5 );
    tasklist_->attach( rightOf, catlist_ );

    uiPushButton* gobut = new uiPushButton( this, tr("Go ..."),
			    mCB(this,uiGeoDevKitMLCenterDlg,goCB), true );
    gobut->attach( hCentered );

    statuslbl_ = new uiLabel( this, tr(
	"Using self-developed UI shell - Python backend not connected") );
    statuslbl_->attach( alignedBelow, gobut );
}


void uiGeoDevKitMLCenterDlg::categorySelCB( CallBacker* )
{
    // 类别切换在占位壳里不改变右侧任务列表，保留接口供 P3 接后端时使用。
}


void uiGeoDevKitMLCenterDlg::goCB( CallBacker* )
{
    const int taskidx = tasklist_ ? tasklist_->currentItem() : 2;
    // Segmentation(2) 与 Regression(3) 进三步向导；前两项给说明。
    if ( taskidx<2 )
    {
	showStubMessage( tr(
	    "%1 is listed in the requirement screenshot but is out of this "
	    "shell's scope. Only Seismic Image Segmentation and Regression "
	    "are previewed here.").arg( tasklist_->getText() ) );
	return;
    }

    uiGeoDevKitMLWizDlg dlg( this, taskidx==2 );
    dlg.go();
}


//=================== 三步向导 ==========================================

uiGeoDevKitMLWizDlg::uiGeoDevKitMLWizDlg( uiParent* p, bool segmentation )
    : uiDialog(p,uiDialog::Setup(
	segmentation
	    ? tr("Seismic Image Segmentation - 3D")
	    : tr("Seismic Image Regression - 3D"),
	tr("Extract / Train / Apply - UI shell, algorithm not connected"),
	mNoHelpKey))
    , segmentation_( segmentation )
{
    wizardtabs_ = new uiTabStack( this, "ML workflow tabs" );

    createExtractPage( wizardtabs_ );
    createTrainPage( wizardtabs_ );
    createApplyPage( wizardtabs_ );
}


// ------------------- Extract 页 ---------------------------------------
void uiGeoDevKitMLWizDlg::createExtractPage( uiTabStack* wizard )
{
    uiGroup* page = new uiGroup( wizard->tabGroup(), "Extract page" );

    // 顶部工具栏占位：四个小按钮复截图的 新建/打开/保存/另存 图标区
    uiPushButton* newbut = new uiPushButton( page, tr("New"),
			mCB(this,uiGeoDevKitMLWizDlg,stubCB), false );
    uiPushButton* openbut = new uiPushButton( page, tr("Open ..."),
			mCB(this,uiGeoDevKitMLWizDlg,stubCB), false );
    openbut->attach( rightOf, newbut );
    uiPushButton* savebut = new uiPushButton( page, tr("Save"),
			mCB(this,uiGeoDevKitMLWizDlg,stubCB), false );
    savebut->attach( rightOf, openbut );

    // Extract 页内三个子 Tab：Input Data / Target / Sampling
    uiTabStack* subtabs = new uiTabStack( page, "Extract sub tabs" );
    subtabs->attach( alignedBelow, newbut );

    // ---- Input Data：带勾选框的数据列表 + Filter ----
    uiGroup* inpage = new uiGroup( subtabs->tabGroup(), "Input Data page" );
    uiGenInput* filterfld = new uiGenInput( inpage, tr("Filter"),
					    StringInpSpec() );
    uiListBox::Setup insu( OD::ChooseZeroOrMore, tr("Seismic Data") );
    uiListBox* inlist = new uiListBox( inpage, insu, "Seismic data list" );
    inlist->setMultiChoice( true );
    inlist->addItem( tr("KAIYUAN_Inline_290_D4_corr") );
    inlist->addItem( tr("Prestack_seismic") );
    inlist->addItem( tr("Seismic") );
    inlist->addItem( tr("{Prestack_seismic}") );
    inlist->attach( alignedBelow, filterfld );
    inlist->setNrLines( 12 );
    uiPushButton* importbut = new uiPushButton( inpage, tr("Select ..."),
			mCB(this,uiGeoDevKitMLWizDlg,stubCB), false );
    importbut->attach( alignedBelow, inlist );
    subtabs->addTab( inpage, tr("Input Data") );

    // ---- Target：标签掩码列表，注释与截图一致 ----
    uiGroup* tgtpage = new uiGroup( subtabs->tabGroup(), "Target page" );
    uiListBox::Setup tgtsu( OD::ChooseZeroOrMore, tr("Target Data") );
    uiListBox* tgtlist = new uiListBox( tgtpage, tgtsu, "Target data list" );
    tgtlist->setMultiChoice( true );
    tgtlist->addItem( tr("Label mask / horizon attribute") );
    tgtlist->setNrLines( 12 );
    uiLabel* tgtnote = new uiLabel( tgtpage, tr(
	"Import labels and keep sample size one-to-one with the seismic data.") );
    tgtnote->attach( alignedBelow, tgtlist );
    uiPushButton* tgtimp = new uiPushButton( tgtpage, tr("Select ..."),
			mCB(this,uiGeoDevKitMLWizDlg,stubCB), false );
    tgtimp->attach( alignedBelow, tgtnote );
    subtabs->addTab( tgtpage, tr("Target") );

    // ---- Sampling：截图 image4 的全部参数 ----
    uiGroup* smppage = new uiGroup( subtabs->tabGroup(), "Sampling page" );
    uiGenInput* modeltype = new uiGenInput( smppage, tr("Extract for"),
			BoolInpSpec(false, tr("3D model"), tr("2D model")) );
    uiGenInput* dimnr = new uiGenInput( smppage, tr("Image dimensions (nr)"),
					IntInpSpec(32) );
    dimnr->attach( alignedBelow, modeltype );
    uiGenInput* dimz = new uiGenInput( smppage, tr("Image dimensions (z)"),
					IntInpSpec(32) );
    dimz->attach( rightOf, dimnr );
    uiGenInput* sym = new uiGenInput( smppage, tr("Symmetric"),
					BoolInpSpec(false) );
    sym->attach( rightOf, dimz );

    // 范围表：Selection / Range / # images
    uiTable::Setup tsu( 2, 3 );
    tsu.rowdesc( "Selection" ).coldesc( "Range" );
    uiTable* tbl = new uiTable( smppage, tsu, "Sampling ranges" );
    uiStringSet colnames;
    colnames.add( tr("Selection") ).add( tr("Range") ).add( tr("# images") );
    tbl->setColumnLabels( colnames );
    tbl->setColumnWidth( 0, 100 );
    tbl->setColumnWidth( 1, 360 );
    tbl->setColumnWidth( 2, 200 );
    tbl->setText( RowCol(0,0), tr("Selection 1") );
    tbl->setText( RowCol(0,1), tr("426-426, 700-1200, 400-400") );
    tbl->setText( RowCol(0,2), tr("15 x (inl: 1 crl: 32 z: 32)") );
    tbl->attach( alignedBelow, dimnr );

    uiGenInput* along = new uiGenInput( smppage, tr("Extract along"),
			BoolInpSpec(true, tr("In-line"), tr("Cross-line")) );
    along->attach( alignedBelow, tbl );

    // 三行 range/step/overlap/shift/#images，与截图字段一一对应
    uiGenInput* inlrange = new uiGenInput( smppage, tr("In-line range"),
			StringInpSpec("426 - 426") );
    inlrange->attach( alignedBelow, along );
    uiGenInput* crlrange = new uiGenInput( smppage, tr("Cross-line range"),
			StringInpSpec("700 - 1200") );
    crlrange->attach( alignedBelow, inlrange );
    uiGenInput* timerange = new uiGenInput( smppage, tr("Time range (ms)"),
			StringInpSpec("400 - 448") );
    timerange->attach( alignedBelow, crlrange );
    uiGenInput* step = new uiGenInput( smppage, tr("Step / Overlap (%) / Shift"),
			StringInpSpec("2 / 50 / 2") );
    step->attach( alignedBelow, timerange );
    uiGenInput* cnt = new uiGenInput( smppage, tr("Total number of images"),
			StringInpSpec("15") );
    cnt->attach( alignedBelow, step );
    uiGenInput* sz = new uiGenInput( smppage, tr("Expected dataset size"),
			StringInpSpec("120.00 kB") );
    sz->attach( alignedBelow, cnt );
    subtabs->addTab( smppage, tr("Sampling") );

    // 底部输出行 + Extract
    uiGenInput* outfld = new uiGenInput( page,
			tr("Output Deep Learning Example Data"),
			StringInpSpec("exampledata") );
    outfld->attach( alignedBelow, subtabs );
    uiPushButton* extbut = new uiPushButton( page, tr("Extract"),
			mCB(this,uiGeoDevKitMLWizDlg,stubCB), false );
    extbut->attach( rightOf, outfld );

    uiLabel* foot = new uiLabel( page, tr(
	"UI shell only - no algorithm is connected; no data is written.") );
    foot->attach( alignedBelow, outfld );

    wizard->addTab( page, tr("Extract") );
}


// ------------------- Train 页 -----------------------------------------
void uiGeoDevKitMLWizDlg::createTrainPage( uiTabStack* wizard )
{
    uiGroup* page = new uiGroup( wizard->tabGroup(), "Train page" );

    uiGenInput* exfld = new uiGenInput( page,
			tr("Input Deep Learning Example Data"),
			StringInpSpec("exampledata") );
    uiGenInput* trtype = new uiGenInput( page, tr("Training Type"),
			BoolInpSpec(true, tr("New"), tr("Other"), true) );
    trtype->attach( alignedBelow, exfld );
    uiGenInput* modefld = new uiGenInput( page, tr("Training mode"),
			BoolInpSpec(false, tr("Resume"), tr("Transfer"), true) );
    modefld->attach( rightOf, trtype );
    uiGenInput* moutfld = new uiGenInput( page,
			tr("Output Deep Learning Model"),
			StringInpSpec("model") );
    moutfld->attach( alignedBelow, trtype );

    // Training / Parameters / Advanced 子 Tab
    uiTabStack* subtabs = new uiTabStack( page, "Train sub tabs" );
    subtabs->attach( alignedBelow, moutfld );

    uiGroup* trpage = new uiGroup( subtabs->tabGroup(), "Training page" );
    uiComboBox* typefld = new uiComboBox( trpage, "Network type" );
    typefld->addItem( segmentation_
		? tr("dGB UNet Segmentation") : tr("dGB UNet Regression") );
    uiGenInput* valid = new uiGenInput( trpage, tr("Validation Split"),
			StringInpSpec("0.20") );
    valid->attach( alignedBelow, typefld );
    uiGenInput* batch = new uiGenInput( trpage, tr("Batch Size"),
			IntInpSpec(8) );
    batch->attach( alignedBelow, valid );
    uiGenInput* epoch = new uiGenInput( trpage, tr("Epochs"),
			IntInpSpec(15) );
    epoch->attach( alignedBelow, batch );
    uiGenInput* early = new uiGenInput( trpage, tr("Early Stopping"),
			IntInpSpec(15) );
    early->attach( alignedBelow, epoch );
    uiGenInput* lr = new uiGenInput( trpage,
			tr("Initial Learning Rate (1e)"), IntInpSpec(-4) );
    lr->attach( alignedBelow, early );
    uiGenInput* drop = new uiGenInput( trpage, tr("Epoch drop (%)"),
			StringInpSpec("33.33") );
    drop->attach( alignedBelow, lr );
    uiGenInput* seed = new uiGenInput( trpage, tr("Random seed"),
			IntInpSpec(42) );
    seed->attach( alignedBelow, drop );
    uiGenInput* dec = new uiGenInput( trpage, tr("Decimate Input"),
			BoolInpSpec(false) );
    dec->attach( alignedBelow, seed );
    subtabs->addTab( trpage, tr("Training") );

    uiGroup* parpage = new uiGroup( subtabs->tabGroup(), "Parameters page" );
    uiLabel* parnote = new uiLabel( parpage, tr(
	"Advanced network parameters will appear here when the algorithm "
	"backend is connected in P3.") );
    subtabs->addTab( parpage, tr("Parameters") );

    uiGroup* advpage = new uiGroup( subtabs->tabGroup(), "Advanced page" );
    uiLabel* advnote = new uiLabel( advpage, tr(
	"Advanced options are reserved for the P3 algorithm backend.") );
    subtabs->addTab( advpage, tr("Advanced") );

    uiPushButton* resetbut = new uiPushButton( page, tr("Reset"),
			mCB(this,uiGeoDevKitMLWizDlg,stubCB), false );
    resetbut->attach( alignedBelow, subtabs );
    uiPushButton* backbut = new uiPushButton( page, tr("Back"),
			mCB(this,uiGeoDevKitMLWizDlg,stubCB), false );
    backbut->attach( rightOf, resetbut );
    uiPushButton* trainbut = new uiPushButton( page, tr("Train"),
			mCB(this,uiGeoDevKitMLWizDlg,stubCB), false );
    trainbut->attach( rightOf, backbut );

    wizard->addTab( page, tr("Train") );
}


// ------------------- Apply 页 -----------------------------------------
void uiGeoDevKitMLWizDlg::createApplyPage( uiTabStack* wizard )
{
    uiGroup* page = new uiGroup( wizard->tabGroup(), "Apply page" );

    uiGenInput* exfld = new uiGenInput( page,
			tr("Input Deep Learning Example Data"),
			StringInpSpec("exampledata") );
    uiGenInput* mdl = new uiGenInput( page,
			tr("Input Deep Learning Model"),
			StringInpSpec("model") );
    mdl->attach( alignedBelow, exfld );
    uiGenInput* inv = new uiGenInput( page, tr("Input seismic volume"),
			StringInpSpec() );
    inv->attach( alignedBelow, mdl );
    uiGenInput* outfld = new uiGenInput( page, tr("Output volume"),
			StringInpSpec( segmentation_
			    ? "segmentation_result" : "regression_result" ) );
    outfld->attach( alignedBelow, inv );
    uiLabel* note = new uiLabel( page, segmentation_
		? tr("Segmentation outputs a 0/1 label volume.")
		: tr("Regression outputs a continuous prediction volume.") );
    note->attach( alignedBelow, outfld );
    uiPushButton* applybut = new uiPushButton( page, tr("Apply"),
			mCB(this,uiGeoDevKitMLWizDlg,stubCB), false );
    applybut->attach( alignedBelow, note );

    wizard->addTab( page, tr("Apply") );
}


bool uiGeoDevKitMLWizDlg::acceptOK( CallBacker* )
{
    return true;
}


void uiGeoDevKitMLWizDlg::stubCB( CallBacker* )
{
    showStubMessage( tr(
	"This is a UI shell rebuilt from the requirement screenshots. "
	"No data is read or written, no training or inference runs.\n\n"
	"The real algorithm backend will be added in P3 after the client "
	"confirms:\n"
	"  D1 - self-developed PyTorch U-Net vs. dGB commercial license;\n"
	"  D4 - labelled training samples and their format.") );
}
