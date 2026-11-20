/*+
________________________________________________________________________

 工区与运行环境自检对话框实现（P0 骨架示例）

 这里刻意只使用 OpendTect 稳定的公开 API（oddirs.h / survinfo.h），
 不碰 IOM()、Seis 数据遍历等后续阶段才需要的接口，保证骨架在
 任何工区（甚至空工区）下都能安全打开。
________________________________________________________________________
-*/

#include "uigeodevkitinfo.h"

#include "bufstring.h"
#include "oddirs.h"
#include "survinfo.h"
#include "uitextedit.h"


uiGeoDevKitInfoDlg::uiGeoDevKitInfoDlg( uiParent* p )
    : uiDialog(p,Setup(tr("Survey and Environment Check"),
		       tr("Current survey and runtime information"),
		       mNoHelpKey))
{
    // 第三个参数为只读；这份报告只用于核对环境，不提供编辑。
    reportfld_ = new uiTextEdit( this, "Environment report", true );
    fillReport();
}


void uiGeoDevKitInfoDlg::fillReport()
{
    /* 报告文本是给用户看的成果信息，插件界面固定为中文交付；
       需要可翻译的界面控件文字一律走 tr()，不要混用。 */
    BufferString report;
    report.add( "OpendTect 地质解释定制开发工具包 · 工区与运行环境自检\n" )
	  .add( "========================================================\n" )
	  .add( "插件版本   : 0.1.0 (P0 插件骨架)\n" )
	  .add( "工区名称   : " ).add( GetSurveyName() ).add( "\n" )
	  .add( "数据根目录 : " ).add( GetBaseDataDir() ).add( "\n" )
	  .add( "当前工区目录: " ).add( GetDataDir() ).add( "\n" )
	  .add( "\n" )
	  .add( "[后续模块挂载点]（当前为占位，将在各期实现）\n" )
	  .add( "  P1  uiDWTVolume   三维体小波分解 / SEG-Y 输出 / 属性优选\n" )
	  .add( "  P2  uiGeoWizard   人工解释 7 向导 / DXF 成果导出\n" )
	  .add( "  P3  uiMLBridge    U-Net 分割与回归（依赖 D1 决策）\n" )
	  .add( "  P4  框架增强 / 正演反演（建议单独立项）\n" )
	  .add( "\n" )
	  .add( "回归基准数据: F3 Demo 2023 完整工区（7.57 GiB）\n" );

    reportfld_->setText( report.buf() );
}
