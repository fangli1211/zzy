/*+
________________________________________________________________________

 时深转换向导实现

 预检 -> 中文步骤说明 -> 一键打开官方对话框。
 对话框对象通过 PtrMan 持有成员指针，跨多次调用复用，与官方
 uiEMPartServer::processTime2Depth 的做法一致。
________________________________________________________________________
-*/

#include "uigeodevkitt2d.h"

#include "uigeodevkitmod.h"

#include "emposid.h"
#include "uitime2depthdlg.h"

#include "ptrman.h"
#include "uibutton.h"
#include "uimsg.h"
#include "uitextedit.h"
#include "uistringset.h"


uiGeoDevKitT2DDlg::uiGeoDevKitT2DDlg( uiParent* p )
    : uiDialog( p, Setup(tr("Time-Depth Conversion Wizard"),
			 tr("Guided time-to-depth conversion for horizons "
			    "and faults"),
			 mNoHelpKey) )
    , parent_( p )
{
    uiTextEdit* intro = new uiTextEdit( this, "T2D guide", true );
    intro->setText( tr(
	"Conversion steps (OpendTect native engine):\n"
	"1. Select the horizon or fault to convert.\n"
	"2. Choose a velocity cube or a time-depth table.\n"
	"3. Check units and datum (ms / m, reference level).\n"
	"4. Preview the transformed Z range.\n"
	"5. Run the batch conversion; a new object is saved.\n"
	"\n"
	"Click 'Check readiness' to verify the current survey supports the "
	"conversion, then 'Open conversion dialog' to start.") );

    uiPushButton* checkbut = new uiPushButton( this, tr("Check readiness"),
		mCB(this,uiGeoDevKitT2DDlg,checkCB), true );
    checkbut->attach( alignedBelow, intro );
    uiPushButton* launchbut = new uiPushButton( this,
		tr("Open conversion dialog"),
		mCB(this,uiGeoDevKitT2DDlg,launchCB), true );
    launchbut->attach( rightOf, checkbut );
}


void uiGeoDevKitT2DDlg::checkCB( CallBacker* )
{
    const uiRetVal ret = EM::uiTime2DepthDlg::canTransform(
						EM::ObjectType::Hor3D );
    if ( !ret.isOK() )
    {
	uiMSG().message( ret.messages().cat() );
	return;
    }
    uiMSG().message( tr("The current survey is ready for time-depth "
			"conversion.") );
}


void uiGeoDevKitT2DDlg::launchCB( CallBacker* )
{
    const uiRetVal ret = EM::uiTime2DepthDlg::canTransform(
						EM::ObjectType::Hor3D );
    if ( !ret.isOK() )
    {
	uiMSG().message( ret.messages().cat() );
	return;
    }
    // 非模态，与官方 uiEMPartServer 一致：用静态对象持有，避免对话框
    // 一闪而过；对话框自己负责生命周期，这里不 delete。
    mDefineStaticLocalObject( PtrMan<EM::uiTime2DepthDlg>, t2ddlg,
	= new EM::uiTime2DepthDlg(parent_,EM::ObjectType::Hor3D) );
    if ( t2ddlg )
	t2ddlg->show();
}
