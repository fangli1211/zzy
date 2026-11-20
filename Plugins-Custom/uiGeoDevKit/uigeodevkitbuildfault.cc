/*+
________________________________________________________________________

 断层棍 → 断层面生成向导实现

 计算链路（与生产代码 uiodfaulttoolman.cc:1053-1127 一致）：
   1) 把用户选中的多个 FSS 全部合并进一个临时 FaultStickSet：
        源 FSS.geometry().selectAllSticks(true)
        源 FSS.geometry().copySelectedSticksTo(tmpfss.geometry(), false)
   2) 用 EM::Fault3D::create(name) 建目标断层面（内存中）；
   3) EM::FSStoFault3DConverter(setup, *tmpfss, *outfault).convert(false)；
   4) outfault->setFullyLoaded(true) 后 PtrMan<Task> t=outfault->saver();
      t->execute() 持久化到工区。
________________________________________________________________________
-*/

#include "uigeodevkitbuildfault.h"

#include "uigeodevkitmod.h"

#include "bufstring.h"
#include "emfault.h"
#include "emfault3d.h"
#include "emfaultstickset.h"
#include "emfsstofault3d.h"
#include "emmanager.h"
#include "emobject.h"
#include "emsurfacetr.h"
#include "executor.h"
#include "multiid.h"
#include "ptrman.h"
#include "refcount.h"
#include "survinfo.h"
#include "typeset.h"

#include "uigeninput.h"
#include "uimsg.h"
#include "uimultisurfaceread.h"
#include "uitextedit.h"
#include "uibutton.h"


uiGeoDevKitBuildFaultDlg::uiGeoDevKitBuildFaultDlg( uiParent* p )
    : uiDialog( p, Setup(tr("Build Fault Surface from Sticks"),
			 tr("Merge fault sticks picked on multiple sections "
			    "into one 3D fault surface"),
			 mNoHelpKey))
{
    fsssel_ = new uiMultiSurfaceRead( this,
			EM::FaultStickSet::typeStr(), &SI().zDomainInfo() );

    outnamefld_ = new uiGenInput( this, tr("Output fault name"),
				  StringInpSpec("Merged_Fault") );
    outnamefld_->attach( alignedBelow, fsssel_ );

    buildbut_ = new uiPushButton( this, tr("Build fault surface"),
			mCB(this,uiGeoDevKitBuildFaultDlg,buildCB), true );
    buildbut_->attach( alignedBelow, outnamefld_ );

    logfld_ = new uiTextEdit( this, "Build log", true );
    logfld_->attach( alignedBelow, buildbut_ );
}


uiGeoDevKitBuildFaultDlg::~uiGeoDevKitBuildFaultDlg()
{
}


void uiGeoDevKitBuildFaultDlg::buildCB( CallBacker* )
{
    TypeSet<MultiID> mids;
    fsssel_->getSurfaceIds( mids );
    if ( mids.isEmpty() )
    {
	uiMSG().error( tr("Please select at least one fault stick set.") );
	return;
    }

    const BufferString outnm( outnamefld_->text() );
    if ( outnm.isEmpty() )
    {
	uiMSG().error( tr("Please enter an output fault name.") );
	return;
    }

    // 临时 FSS：作为多个源 FSS 棍的汇总容器。
    RefMan<EM::EMObject> tmpemobj = EM::FaultStickSet::create( EM::EMM() );
    mDynamicCastGet( EM::FaultStickSet*, tmpfss, tmpemobj.ptr() )
    if ( !tmpfss )
    {
	uiMSG().error( tr("Cannot create a temporary fault stick container.") );
	return;
    }

    int nrsticks = 0;
    BufferStringSet srcnms;
    for ( int idx=0; idx<mids.size(); idx++ )
    {
	RefMan<EM::EMObject> srcobj =
	    EM::EMM().loadIfNotFullyLoaded( mids[idx] );
	mDynamicCastGet( EM::FaultStickSet*, srcfss, srcobj.ptr() )
	if ( !srcfss )
	    continue;

	srcfss->geometry().selectAllSticks( true );
	const int beforeselected = srcfss->geometry().nrSelectedSticks();
	srcfss->geometry().copySelectedSticksTo( tmpfss->geometry(), false );
	srcnms.add( srcobj->name() );
	nrsticks += beforeselected;
    }

    if ( nrsticks < 2 )
    {
	uiMSG().error( tr("The selected stick sets contain fewer than two "
			  "sticks in total. At least two sticks are required "
			  "to build a fault surface.") );
	EM::EMM().removeObject( tmpfss );
	return;
    }

    // 目标断层面。create(name) 内部走 EMM().createObject()，会建立 IOObj
    // 目录项（MultiID），因此 saver() 有落盘目标。
    RefMan<EM::EMObject> outemobj = EM::Fault3D::create( outnm.buf() );
    mDynamicCastGet( EM::Fault3D*, outfault, outemobj.ptr() )
    if ( !outfault )
    {
	uiMSG().error( tr("Cannot create the output fault object.") );
	EM::EMM().removeObject( tmpfss );
	return;
    }
    outfault->setZDomain( SI().zDomainInfo() );

    EM::FSStoFault3DConverter::Setup setup;
    setup.pickplanedir_ = EM::FSStoFault3DConverter::Setup::Auto;
    setup.sortsticks_ = true;
    setup.addtohistory_ = false;
    EM::FSStoFault3DConverter conv( setup, *tmpfss, *outfault );
    if ( !conv.convert( false ) )
    {
	uiMSG().error( tr("The kernel failed to convert sticks into a fault "
			  "surface. Check the picked sticks and try again.") );
	EM::EMM().removeObject( outfault );
	EM::EMM().removeObject( tmpfss );
	return;
    }

    outfault->setFullyLoaded( true );
    PtrMan<Task> saver = outfault->saver();
    bool saved = false;
    if ( saver )
	saved = saver->execute();
    if ( !saved )
    {
	uiMSG().error( tr("The fault surface was built but could not be saved "
			  "to the survey. Check disk permissions.") );
	EM::EMM().removeObject( tmpfss );
	return;
    }

    lastsummary_.setEmpty();
    lastsummary_.add( tr("Fault surface built and saved.") ).add( "\n" )
	       .add( tr("Output name: %1").arg(outnm.buf()) ).add( "\n" )
	       .add( tr("Source stick sets: %1").arg(srcnms.size()) )
	       .add( "\n" )
	       .add( tr("Total sticks merged: %1").arg(nrsticks) )
	       .add( "\n\n" );
    for ( int idx=0; idx<srcnms.size(); idx++ )
	lastsummary_.add( "  - " ).add( srcnms.get(idx) ).add( "\n" );

    logfld_->setText( lastsummary_.buf() );
    buildbut_->setSensitive( false );

    // 临时容器不再需要；输出对象已 saver() 保存，保留在 EMM 中供显示。
    EM::EMM().removeObject( tmpfss );
}
