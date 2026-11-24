<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="zh_CN">
<context>
    <name>uiGeoDevKitMgr</name>
    <message>
        <source>Geological Interpretation Toolkit</source>
        <translation>地质解释定制工具包</translation>
    </message>
    <message>
        <source>Survey and Environment Check...</source>
        <translation>工区与运行环境自检...</translation>
    </message>
    <message>
        <source>Fault-Horizon Intersection...</source>
        <translation>断层—层位交线提取...</translation>
    </message>
    <message>
        <source>Interpretation Export Center...</source>
        <translation>解释成果导出中心...</translation>
    </message>
    <message>
        <source>Contour Map Export...</source>
        <translation>等值线成图导出...</translation>
    </message>
    <message>
        <source>Time-Depth Conversion Wizard...</source>
        <translation>时深转换向导...</translation>
    </message>
    <message>
        <source>Export 2D Section Image...</source>
        <translation>导出二维剖面图片...</translation>
    </message>
    <message>
        <source>Build Fault Surface from Sticks...</source>
        <translation>断层棍生成断层面...</translation>
    </message>
    <message>
        <source>Export Section as Vector DXF...</source>
        <translation>剖面导出为矢量 DXF...</translation>
    </message>
    <message>
        <source>About This Toolkit...</source>
        <translation>关于本工具包...</translation>
    </message>
    <message>
        <source>Geological Interpretation Toolkit 0.1.0 (P0 skeleton). External PLUGIN_DIR plugin for OpendTect v8.1.0. Planned modules: DWT volume (P1), interpretation wizards and DXF export (P2), U-Net bridge (P3).</source>
        <translation>地质解释定制工具包 0.1.0（P0 插件骨架）。以外部 PLUGIN_DIR 插件形式挂载到 OpendTect v8.1.0。规划模块：三维体小波分解（P1）、人工解释向导与 DXF 成果导出（P2）、U-Net 桥接（P3）。</translation>
    </message>
    <message>
        <source>No 2D section viewer is open. Open a seismic line in a 2D window first.</source>
        <translation>当前没有打开的二维剖面视图。请先在二维窗口中打开一条地震测线。</translation>
    </message>
    <message>
        <source>The open section has no renderable view.</source>
        <translation>当前剖面没有可导出的视图。</translation>
    </message>
    <message>
        <source>Export 2D section as PNG</source>
        <translation>将二维剖面导出为 PNG</translation>
    </message>
    <message>
        <source>Section image exported successfully.</source>
        <translation>剖面图片导出成功。</translation>
    </message>
    <message>
        <source>3D Volume DWT Decomposition...</source>
        <translation>三维地震体小波分解...</translation>
    </message>
    <message>
        <source>Machine Learning</source>
        <translation>机器学习</translation>
    </message>
    <message>
        <source>Machine Learning Control Center (UI Shell)...</source>
        <translation>机器学习控制中心（界面占位）...</translation>
    </message>
    <message>
        <source>Tabbed Views (Custom)</source>
        <translation>标签页视窗（定制）</translation>
    </message>
    <message>
        <source>New Tabbed View...</source>
        <translation>新建标签视窗...</translation>
    </message>
    <message>
        <source>Tabbed View</source>
        <translation>标签页视图</translation>
    </message>
    <message>
        <source>Tile Views (Classic)</source>
        <translation>平铺视窗（原有）</translation>
    </message>
</context>
<context>
    <name>uiGeoDevKitInfoDlg</name>
    <message>
        <source>Survey and Environment Check</source>
        <translation>工区与运行环境自检</translation>
    </message>
    <message>
        <source>Current survey and runtime information</source>
        <translation>当前工区与运行环境信息</translation>
    </message>
</context>
<context>
    <name>uiGeoDevKitIntersectDlg</name>
    <message>
        <source>Fault-Horizon Intersection</source>
        <translation>断层—层位交线提取</translation>
    </message>
    <message>
        <source>Extract the intersection line between a 3D fault and a 3D horizon</source>
        <translation>提取三维断层与三维层位的交线</translation>
    </message>
    <message>
        <source>3D Horizon</source>
        <translation>三维层位</translation>
    </message>
    <message>
        <source>Fault</source>
        <translation>断层</translation>
    </message>
    <message>
        <source>Horizon vertical shift</source>
        <translation>层位垂向偏移</translation>
    </message>
    <message>
        <source>Compute intersection</source>
        <translation>计算交线</translation>
    </message>
    <message>
        <source>Export ASCII...</source>
        <translation>导出 ASCII...</translation>
    </message>
    <message>
        <source>Please select both a 3D horizon and a fault.</source>
        <translation>请同时选择三维层位与断层。</translation>
    </message>
    <message>
        <source>The selected horizon is not a 3D horizon.</source>
        <translation>所选层位不是三维层位。</translation>
    </message>
    <message>
        <source>The selected horizon has no usable geometry.</source>
        <translation>所选层位没有可用的几何数据。</translation>
    </message>
    <message>
        <source>The selected object is not a 3D fault.</source>
        <translation>所选对象不是三维断层。</translation>
    </message>
    <message>
        <source>The selected fault has no usable geometry.</source>
        <translation>所选断层没有可用的几何数据。</translation>
    </message>
    <message>
        <source>Failed to build the fault surface.</source>
        <translation>构建断层面失败。</translation>
    </message>
    <message>
        <source>No intersection was found. The fault and horizon may not overlap.</source>
        <translation>未找到交线，断层与层位可能没有空间重叠。</translation>
    </message>
    <message>
        <source>Intersection points: %1</source>
        <translation>交线点数：%1</translation>
    </message>
    <message>
        <source>Horizon: %1    Fault: %2</source>
        <translation>层位：%1    断层：%2</translation>
    </message>
    <message>
        <source>Please compute an intersection first.</source>
        <translation>请先计算交线。</translation>
    </message>
    <message>
        <source>Export intersection as ASCII</source>
        <translation>将交线导出为 ASCII</translation>
    </message>
    <message>
        <source>Cannot open the output file for writing.</source>
        <translation>无法打开输出文件写入。</translation>
    </message>
    <message>
        <source>Failed to write the output file.</source>
        <translation>写入输出文件失败。</translation>
    </message>
    <message>
        <source>Intersection exported successfully.</source>
        <translation>交线导出成功。</translation>
    </message>
</context>
<context>
    <name>uiGeoDevKitExportDlg</name>
    <message>
        <source>Interpretation Export Center</source>
        <translation>解释成果导出中心</translation>
    </message>
    <message>
        <source>Export horizons, faults, intersections, wells and 2D lines to DXF or ASCII</source>
        <translation>将层位、断层、交线、井与二维测线导出为 DXF 或 ASCII</translation>
    </message>
    <message>
        <source>Output format</source>
        <translation>输出格式</translation>
    </message>
    <message>
        <source>DXF (AutoCAD R12)</source>
        <translation>DXF（AutoCAD R12）</translation>
    </message>
    <message>
        <source>ASCII (X Y Z table)</source>
        <translation>ASCII（X Y Z 表）</translation>
    </message>
    <message>
        <source>Export horizon</source>
        <translation>导出层位</translation>
    </message>
    <message>
        <source>Export fault</source>
        <translation>导出断层</translation>
    </message>
    <message>
        <source>Horizon for intersection</source>
        <translation>用于求交的层位</translation>
    </message>
    <message>
        <source>Fault for intersection</source>
        <translation>用于求交的断层</translation>
    </message>
    <message>
        <source>Export intersection</source>
        <translation>导出交线</translation>
    </message>
    <message>
        <source>Wells</source>
        <translation>井</translation>
    </message>
    <message>
        <source>Export all wells</source>
        <translation>导出全部井</translation>
    </message>
    <message>
        <source>2D survey lines</source>
        <translation>二维测线</translation>
    </message>
    <message>
        <source>Export all lines</source>
        <translation>导出全部测线</translation>
    </message>
    <message>
        <source>Output base name</source>
        <translation>输出文件名前缀</translation>
    </message>
    <message>
        <source>Please select a 3D horizon first.</source>
        <translation>请先选择一个三维层位。</translation>
    </message>
    <message>
        <source>Please select a 3D fault first.</source>
        <translation>请先选择一个三维断层。</translation>
    </message>
    <message>
        <source>Please select both a horizon and a fault.</source>
        <translation>请同时选择层位与断层。</translation>
    </message>
    <message>
        <source>Selected objects must be a 3D horizon and a 3D fault.</source>
        <translation>所选对象必须是三维层位与三维断层。</translation>
    </message>
    <message>
        <source>The selected object is not a 3D horizon.</source>
        <translation>所选对象不是三维层位。</translation>
    </message>
    <message>
        <source>The selected object is not a 3D fault.</source>
        <translation>所选对象不是三维断层。</translation>
    </message>
    <message>
        <source>The horizon has no usable grid.</source>
        <translation>该层位没有可用网格。</translation>
    </message>
    <message>
        <source>The horizon contains no defined nodes.</source>
        <translation>该层位没有已定义的节点。</translation>
    </message>
    <message>
        <source>The fault has no usable geometry.</source>
        <translation>该断层没有可用的几何数据。</translation>
    </message>
    <message>
        <source>The fault contains no usable stick.</source>
        <translation>该断层没有可用的断层棍。</translation>
    </message>
    <message>
        <source>Selected objects have no usable geometry.</source>
        <translation>所选对象没有可用的几何数据。</translation>
    </message>
    <message>
        <source>Failed to build the fault surface.</source>
        <translation>构建断层面失败。</translation>
    </message>
    <message>
        <source>No intersection was found.</source>
        <translation>未找到交线。</translation>
    </message>
    <message>
        <source>No wells are available in this survey.</source>
        <translation>本工区没有可用的井。</translation>
    </message>
    <message>
        <source>No well trajectory could be loaded.</source>
        <translation>无法加载任何井轨迹。</translation>
    </message>
    <message>
        <source>No 2D survey lines are available.</source>
        <translation>没有可用的二维测线。</translation>
    </message>
    <message>
        <source>No usable 2D survey line was found.</source>
        <translation>没有找到可用的二维测线。</translation>
    </message>
    <message>
        <source>Export DXF file</source>
        <translation>导出 DXF 文件</translation>
    </message>
    <message>
        <source>Export ASCII data</source>
        <translation>导出 ASCII 数据</translation>
    </message>
    <message>
        <source>Cannot open the output file for writing.</source>
        <translation>无法打开输出文件写入。</translation>
    </message>
    <message>
        <source>Failed to write the output file.</source>
        <translation>写入输出文件失败。</translation>
    </message>
    <message>
        <source>Horizon exported as ASCII.</source>
        <translation>层位已导出为 ASCII。</translation>
    </message>
    <message>
        <source>Horizon exported as DXF.</source>
        <translation>层位已导出为 DXF。</translation>
    </message>
    <message>
        <source>Fault exported as ASCII.</source>
        <translation>断层已导出为 ASCII。</translation>
    </message>
    <message>
        <source>Fault exported as DXF.</source>
        <translation>断层已导出为 DXF。</translation>
    </message>
    <message>
        <source>Intersection exported as ASCII.</source>
        <translation>交线已导出为 ASCII。</translation>
    </message>
    <message>
        <source>Intersection exported as DXF.</source>
        <translation>交线已导出为 DXF。</translation>
    </message>
    <message>
        <source>Wells exported as ASCII.</source>
        <translation>井数据已导出为 ASCII。</translation>
    </message>
    <message>
        <source>Wells exported as DXF.</source>
        <translation>井数据已导出为 DXF。</translation>
    </message>
    <message>
        <source>Survey lines exported as ASCII.</source>
        <translation>测线已导出为 ASCII。</translation>
    </message>
    <message>
        <source>Survey lines exported as DXF.</source>
        <translation>测线已导出为 DXF。</translation>
    </message>
    <message>
        <source>Horizon nodes exported: %1</source>
        <translation>已导出层位节点：%1</translation>
    </message>
    <message>
        <source>Fault sticks exported: %1</source>
        <translation>已导出断层棍：%1</translation>
    </message>
    <message>
        <source>Intersection points exported: %1</source>
        <translation>已导出交线点：%1</translation>
    </message>
    <message>
        <source>Wells exported: %1</source>
        <translation>已导出井：%1</translation>
    </message>
    <message>
        <source>2D lines exported: %1</source>
        <translation>已导出二维测线：%1</translation>
    </message>
</context>
<context>
    <name>uiGeoDevKitContourDlg</name>
    <message>
        <source>Contour Map Export</source>
        <translation>等值线成图导出</translation>
    </message>
    <message>
        <source>Trace contours on a 3D horizon and export to DXF</source>
        <translation>在三维层位上追踪等值线并导出为 DXF</translation>
    </message>
    <message>
        <source>Fault overlay (optional)</source>
        <translation>断层叠加（可选）</translation>
    </message>
    <message>
        <source>First contour</source>
        <translation>首条等值线</translation>
    </message>
    <message>
        <source>Contour interval</source>
        <translation>等值距</translation>
    </message>
    <message>
        <source>Last contour</source>
        <translation>末条等值线</translation>
    </message>
    <message>
        <source>Trace contours</source>
        <translation>追踪等值线</translation>
    </message>
    <message>
        <source>Export DXF...</source>
        <translation>导出 DXF...</translation>
    </message>
    <message>
        <source>Please select a 3D horizon.</source>
        <translation>请选择一个三维层位。</translation>
    </message>
    <message>
        <source>The horizon has no usable grid.</source>
        <translation>该层位没有可用网格。</translation>
    </message>
    <message>
        <source>Invalid contour range: check the first contour, interval and last contour.</source>
        <translation>等值线范围无效：请检查首条等值线、等值距与末条等值线。</translation>
    </message>
    <message>
        <source>The number of contour levels must be between 1 and 500. Adjust the interval.</source>
        <translation>等值线级数必须在 1 到 500 之间，请调整等值距。</translation>
    </message>
    <message>
        <source>No contour was found in the given range.</source>
        <translation>在给定范围内未找到等值线。</translation>
    </message>
    <message>
        <source>Horizon: %1</source>
        <translation>层位：%1</translation>
    </message>
    <message>
        <source>Contour levels: %1</source>
        <translation>等值线级数：%1</translation>
    </message>
    <message>
        <source>Contour polylines: %1</source>
        <translation>等值线折线：%1</translation>
    </message>
    <message>
        <source>Contour vertices: %1</source>
        <translation>等值线顶点：%1</translation>
    </message>
    <message>
        <source>Fault sticks overlaid: %1</source>
        <translation>叠加断层棍：%1</translation>
    </message>
    <message>
        <source>DXF entities: %1</source>
        <translation>DXF 实体数：%1</translation>
    </message>
    <message>
        <source>Please trace contours first.</source>
        <translation>请先追踪等值线。</translation>
    </message>
    <message>
        <source>Export contours as DXF</source>
        <translation>将等值线导出为 DXF</translation>
    </message>
    <message>
        <source>Contours exported successfully.</source>
        <translation>等值线导出成功。</translation>
    </message>
</context>
<context>
    <name>uiGeoDevKitT2DDlg</name>
    <message>
        <source>Time-Depth Conversion Wizard</source>
        <translation>时深转换向导</translation>
    </message>
    <message>
        <source>Guided time-to-depth conversion for horizons and faults</source>
        <translation>面向层位与断层的引导式时深转换</translation>
    </message>
    <message>
        <source>Check readiness</source>
        <translation>检查转换条件</translation>
    </message>
    <message>
        <source>Open conversion dialog</source>
        <translation>打开转换对话框</translation>
    </message>
    <message>
        <source>The current survey is ready for time-depth conversion.</source>
        <translation>当前工区已具备时深转换条件。</translation>
    </message>
</context>
<context>
    <name>uiGeoDevKitBuildFaultDlg</name>
    <message>
        <source>Build Fault Surface from Sticks</source>
        <translation>断层棍生成断层面</translation>
    </message>
    <message>
        <source>Merge fault sticks picked on multiple sections into one 3D fault surface</source>
        <translation>将多条剖面上拾取的断层棍合并为一个三维断层面</translation>
    </message>
    <message>
        <source>Output fault name</source>
        <translation>输出断层名称</translation>
    </message>
    <message>
        <source>Build fault surface</source>
        <translation>生成断层面</translation>
    </message>
    <message>
        <source>Please select at least one fault stick set.</source>
        <translation>请至少选择一个断层棍集合。</translation>
    </message>
    <message>
        <source>Please enter an output fault name.</source>
        <translation>请输入输出断层名称。</translation>
    </message>
    <message>
        <source>Cannot create a temporary fault stick container.</source>
        <translation>无法创建临时断层棍容器。</translation>
    </message>
    <message>
        <source>The selected stick sets contain fewer than two sticks in total. At least two sticks are required to build a fault surface.</source>
        <translation>所选断层棍集合的棍总数不足两根。生成断层面至少需要两根断层棍。</translation>
    </message>
    <message>
        <source>Cannot create the output fault object.</source>
        <translation>无法创建输出断层对象。</translation>
    </message>
    <message>
        <source>The kernel failed to convert sticks into a fault surface. Check the picked sticks and try again.</source>
        <translation>内核未能将断层棍转换为断层面。请检查拾取的断层棍后重试。</translation>
    </message>
    <message>
        <source>The fault surface was built but could not be saved to the survey. Check disk permissions.</source>
        <translation>断层面已生成，但无法保存到工区。请检查磁盘写入权限。</translation>
    </message>
    <message>
        <source>Fault surface built and saved.</source>
        <translation>断层面已生成并保存。</translation>
    </message>
    <message>
        <source>Output name: %1</source>
        <translation>输出名称：%1</translation>
    </message>
    <message>
        <source>Source stick sets: %1</source>
        <translation>源断层棍集合数：%1</translation>
    </message>
    <message>
        <source>Total sticks merged: %1</source>
        <translation>合并断层棍总数：%1</translation>
    </message>
</context>
<context>
    <name>uiGeoDevKitSectionDXFDlg</name>
    <message>
        <source>Export Section as Vector DXF</source>
        <translation>剖面导出为矢量 DXF</translation>
    </message>
    <message>
        <source>Sample horizons and faults along an inline, xline or 2D survey line and write layered DXF</source>
        <translation>沿 Inline、Xline 或二维测线采样层位与断层，输出分层 DXF</translation>
    </message>
    <message>
        <source>Section type</source>
        <translation>剖面类型</translation>
    </message>
    <message>
        <source>3D Inline</source>
        <translation>三维 Inline（主测线）</translation>
    </message>
    <message>
        <source>3D Crossline</source>
        <translation>三维 Crossline（联络测线）</translation>
    </message>
    <message>
        <source>2D survey line</source>
        <translation>二维测线</translation>
    </message>
    <message>
        <source>2D line</source>
        <translation>二维测线</translation>
    </message>
    <message>
        <source>Inline number</source>
        <translation>Inline 号</translation>
    </message>
    <message>
        <source>Crossline number</source>
        <translation>Crossline 号</translation>
    </message>
    <message>
        <source>Output file base name</source>
        <translation>输出文件名前缀</translation>
    </message>
    <message>
        <source>Export section DXF</source>
        <translation>导出剖面 DXF</translation>
    </message>
    <message>
        <source>The survey has no traces in that direction.</source>
        <translation>当前工区在该方向上没有可用道。</translation>
    </message>
    <message>
        <source>No 2D survey line is available. Load a 2D line first.</source>
        <translation>当前没有可用的二维测线。请先加载二维测线。</translation>
    </message>
    <message>
        <source>Please select a valid 2D survey line.</source>
        <translation>请选择有效的二维测线。</translation>
    </message>
    <message>
        <source>The selected line has no trace positions.</source>
        <translation>所选测线没有道位置信息。</translation>
    </message>
    <message>
        <source>No traces could be found on this section.</source>
        <translation>在该剖面上未找到任何道。</translation>
    </message>
    <message>
        <source>Please select at least one horizon or fault.</source>
        <translation>请至少选择一个层位或断层。</translation>
    </message>
    <message>
        <source>Section: %1</source>
        <translation>剖面：%1</translation>
    </message>
    <message>
        <source>Traces sampled: %1</source>
        <translation>采样道数：%1</translation>
    </message>
    <message>
        <source>Horizon polylines: %1 -&gt; %2</source>
        <translation>层位折线：%1 → %2</translation>
    </message>
    <message>
        <source>Fault sticks on section: %1 -&gt; %2</source>
        <translation>剖面上断层棍：%1 → %2</translation>
    </message>
    <message>
        <source>No horizon or fault data was found on this section. Check the line number / selected objects.</source>
        <translation>在该剖面上未找到层位或断层数据。请检查测线编号与所选对象。</translation>
    </message>
    <message>
        <source>Export section as DXF</source>
        <translation>将剖面导出为 DXF</translation>
    </message>
    <message>
        <source>DXF entities written: %1</source>
        <translation>已写入 DXF 实体：%1</translation>
    </message>
    <message>
        <source>Saved to: %1</source>
        <translation>保存位置：%1</translation>
    </message>
    <message>
        <source>Note: X = distance along section [m], Z = horizon/fault value; fault sticks show picked knots only.</source>
        <translation>说明：X = 沿剖面距离（米），Z = 层位/断层值；断层仅显示已拾取的棍节点。</translation>
    </message>
    <message>
        <source>Section DXF exported successfully.</source>
        <translation>剖面 DXF 导出成功。</translation>
    </message>
</context>
<context>
    <name>uiGeoDevKitDWTDlg</name>
    <message>
        <source>3D Volume DWT Decomposition</source>
        <translation>三维地震体小波分解</translation>
    </message>
    <message>
        <source>Wavelet multi-resolution decomposition with same-grid output volumes</source>
        <translation>小波多分辨率分解，输出与输入同网格的分量体</translation>
    </message>
    <message>
        <source>Input 3D seismic volume</source>
        <translation>输入三维地震体</translation>
    </message>
    <message>
        <source>Wavelet basis</source>
        <translation>小波基</translation>
    </message>
    <message>
        <source>Decomposition levels (1..L)</source>
        <translation>分解级数（1..L）</translation>
    </message>
    <message>
        <source>Output approximation cA_J</source>
        <translation>输出近似分量 cA_J</translation>
    </message>
    <message>
        <source>Output details cD_1..cD_J</source>
        <translation>输出细节分量 cD_1..cD_J</translation>
    </message>
    <message>
        <source>Output attributes (approx/detail energy, relative, entropy)</source>
        <translation>输出小波域属性（近似/细节能量、相对细节能量、小波熵）</translation>
    </message>
    <message>
        <source>Multi-wavelet comparison report</source>
        <translation>多小波基对比报告</translation>
    </message>
    <message>
        <source>Traces sampled for comparison</source>
        <translation>对比抽样道数</translation>
    </message>
    <message>
        <source>Output name prefix</source>
        <translation>输出名称前缀</translation>
    </message>
    <message>
        <source>Boundary mode: circular (kernel fixed, not configurable). Input signal length is zero-padded to a power of two internally; all output volumes keep the original grid.</source>
        <translation>边界方式：周期延拓（内核固定，不可配置）。输入道长度在内部居中补零至 2 的幂；所有输出体保持原始网格。</translation>
    </message>
    <message>
        <source>&amp;Run decomposition</source>
        <translation>执行分解(&amp;R)</translation>
    </message>
    <message>
        <source>DWT run log</source>
        <translation>小波分解运行日志</translation>
    </message>
    <message>
        <source>Please select an input 3D seismic volume.</source>
        <translation>请选择输入三维地震体。</translation>
    </message>
    <message>
        <source>Cannot open the selected input volume.</source>
        <translation>无法打开所选输入体。</translation>
    </message>
    <message>
        <source>The selected object is not a 3D seismic volume.</source>
        <translation>所选对象不是三维地震体。</translation>
    </message>
    <message>
        <source>Decomposition level must be between 1 and 12.</source>
        <translation>分解级数必须在 1 到 12 之间。</translation>
    </message>
    <message>
        <source>Select at least one output component.</source>
        <translation>请至少选择一个输出分量。</translation>
    </message>
    <message>
        <source>Starting DWT decomposition...</source>
        <translation>开始小波分解...</translation>
    </message>
    <message>
        <source>Comparison report: %1</source>
        <translation>对比报告：%1</translation>
    </message>
    <message>
        <source>Failed to write comparison report.</source>
        <translation>写入对比报告失败。</translation>
    </message>
    <message>
        <source>Failed on component: %1</source>
        <translation>分量处理失败：%1</translation>
    </message>
    <message>
        <source>Completed: %1</source>
        <translation>已完成：%1</translation>
    </message>
    <message>
        <source>DWT decomposition completed. %1 volumes written.</source>
        <translation>小波分解完成，已写出 %1 个体。</translation>
    </message>
    <message>
        <source>DWT decomposition finished with errors. See the run log.</source>
        <translation>小波分解过程中出现错误，请查看运行日志。</translation>
    </message>
</context>
<context>
    <name>uiGeoDevKitMLCenterDlg</name>
    <message>
        <source>Project</source>
        <translation>工程</translation>
    </message>
    <message>
        <source>Machine Learning Control Center (UI Shell)</source>
        <translation>机器学习控制中心（界面占位）</translation>
    </message>
    <message>
        <source>Geological interpretation - self-developed UI placeholder</source>
        <translation>地质解释——自研界面占位</translation>
    </message>
    <message>
        <source>Input source</source>
        <translation>输入来源</translation>
    </message>
    <message>
        <source>Wells</source>
        <translation>井</translation>
    </message>
    <message>
        <source>Seismic</source>
        <translation>地震</translation>
    </message>
    <message>
        <source>Seismic + Wells</source>
        <translation>地震+井</translation>
    </message>
    <message>
        <source>Pre-trained Models</source>
        <translation>预训练模型</translation>
    </message>
    <message>
        <source>Neural Networks</source>
        <translation>神经网络</translation>
    </message>
    <message>
        <source>3D Seismic</source>
        <translation>三维地震</translation>
    </message>
    <message>
        <source>Horizon-based Seismic Facies</source>
        <translation>基于层位的地震相分析</translation>
    </message>
    <message>
        <source>Seismic Classification (Supervised)</source>
        <translation>有监督地震分类</translation>
    </message>
    <message>
        <source>Seismic Image Segmentation</source>
        <translation>地震图像分割</translation>
    </message>
    <message>
        <source>Seismic Image Regression</source>
        <translation>地震图像回归</translation>
    </message>
    <message>
        <source>Go ...</source>
        <translation>转到...</translation>
    </message>
    <message>
        <source>Using self-developed UI shell - Python backend not connected</source>
        <translation>使用自研界面占位——Python 后端未接入</translation>
    </message>
    <message>
        <source>%1 is listed in the requirement screenshot but is out of this shell's scope. Only Seismic Image Segmentation and Regression are previewed here.</source>
        <translation>%1 出现在需求截图中，但不在本占位界面范围内。此处仅预览地震图像分割与回归。</translation>
    </message>
</context>
<context>
    <name>uiGeoDevKitMLWizDlg</name>
    <message>
        <source>Seismic Image Segmentation - 3D</source>
        <translation>地震图像分割——三维</translation>
    </message>
    <message>
        <source>Seismic Image Regression - 3D</source>
        <translation>地震图像回归——三维</translation>
    </message>
    <message>
        <source>Extract / Train / Apply - UI shell, algorithm not connected</source>
        <translation>样本提取 / 模型训练 / 模型应用——界面占位，算法未接入</translation>
    </message>
    <message>
        <source>New</source>
        <translation>新建</translation>
    </message>
    <message>
        <source>Open ...</source>
        <translation>打开...</translation>
    </message>
    <message>
        <source>Save</source>
        <translation>保存</translation>
    </message>
    <message>
        <source>Filter</source>
        <translation>过滤</translation>
    </message>
    <message>
        <source>Seismic Data</source>
        <translation>地震数据</translation>
    </message>
    <message>
        <source>KAIYUAN_Inline_290_D4_corr</source>
        <translation>KAIYUAN_Inline_290_D4_corr</translation>
    </message>
    <message>
        <source>Prestack_seismic</source>
        <translation>Prestack_seismic</translation>
    </message>
    <message>
        <source>Select ...</source>
        <translation>选择...</translation>
    </message>
    <message>
        <source>Target Data</source>
        <translation>目标数据</translation>
    </message>
    <message>
        <source>Label mask / horizon attribute</source>
        <translation>标签掩码 / 层位属性</translation>
    </message>
    <message>
        <source>Import labels and keep sample size one-to-one with the seismic data.</source>
        <translation>导入标签，并确保与地震数据的样本大小一一对应。</translation>
    </message>
    <message>
        <source>Extract for</source>
        <translation>提取目标</translation>
    </message>
    <message>
        <source>3D model</source>
        <translation>三维模型</translation>
    </message>
    <message>
        <source>2D model</source>
        <translation>二维模型</translation>
    </message>
    <message>
        <source>Image dimensions (nr)</source>
        <translation>图像尺寸（nr）</translation>
    </message>
    <message>
        <source>Image dimensions (z)</source>
        <translation>图像尺寸（z）</translation>
    </message>
    <message>
        <source>Symmetric</source>
        <translation>对称</translation>
    </message>
    <message>
        <source>Selection</source>
        <translation>选择</translation>
    </message>
    <message>
        <source>Range</source>
        <translation>范围</translation>
    </message>
    <message>
        <source># images</source>
        <translation>图像数</translation>
    </message>
    <message>
        <source>Selection 1</source>
        <translation>选择 1</translation>
    </message>
    <message>
        <source>426-426, 700-1200, 400-400</source>
        <translation>426-426, 700-1200, 400-400</translation>
    </message>
    <message>
        <source>15 x (inl: 1 crl: 32 z: 32)</source>
        <translation>15 x (inl: 1 crl: 32 z: 32)</translation>
    </message>
    <message>
        <source>Extract along</source>
        <translation>沿方向提取</translation>
    </message>
    <message>
        <source>In-line</source>
        <translation>主测线</translation>
    </message>
    <message>
        <source>Cross-line</source>
        <translation>联络测线</translation>
    </message>
    <message>
        <source>In-line range</source>
        <translation>主测线范围</translation>
    </message>
    <message>
        <source>Cross-line range</source>
        <translation>联络测线范围</translation>
    </message>
    <message>
        <source>Time range (ms)</source>
        <translation>时间范围（毫秒）</translation>
    </message>
    <message>
        <source>426 - 426</source>
        <translation>426 - 426</translation>
    </message>
    <message>
        <source>700 - 1200</source>
        <translation>700 - 1200</translation>
    </message>
    <message>
        <source>400 - 448</source>
        <translation>400 - 448</translation>
    </message>
    <message>
        <source>Step / Overlap (%) / Shift</source>
        <translation>步长 / 重叠率（%）/ 平移</translation>
    </message>
    <message>
        <source>2 / 50 / 2</source>
        <translation>2 / 50 / 2</translation>
    </message>
    <message>
        <source>Total number of images</source>
        <translation>图像总数</translation>
    </message>
    <message>
        <source>Expected dataset size</source>
        <translation>预期数据集大小</translation>
    </message>
    <message>
        <source>120.00 kB</source>
        <translation>120.00 kB</translation>
    </message>
    <message>
        <source>Output Deep Learning Example Data</source>
        <translation>输出深度学习样本数据</translation>
    </message>
    <message>
        <source>Extract</source>
        <translation>提取样本</translation>
    </message>
    <message>
        <source>UI shell only - no algorithm is connected; no data is written.</source>
        <translation>仅为界面占位——未接入算法，不会写入数据。</translation>
    </message>
    <message>
        <source>Input Deep Learning Example Data</source>
        <translation>输入深度学习样本数据</translation>
    </message>
    <message>
        <source>Training Type</source>
        <translation>训练类型</translation>
    </message>
    <message>
        <source>Other</source>
        <translation>其他</translation>
    </message>
    <message>
        <source>Training mode</source>
        <translation>训练模式</translation>
    </message>
    <message>
        <source>Resume</source>
        <translation>续训</translation>
    </message>
    <message>
        <source>Transfer</source>
        <translation>迁移学习</translation>
    </message>
    <message>
        <source>Output Deep Learning Model</source>
        <translation>输出深度学习模型</translation>
    </message>
    <message>
        <source>model</source>
        <translation>model</translation>
    </message>
    <message>
        <source>Training</source>
        <translation>训练参数</translation>
    </message>
    <message>
        <source>Parameters</source>
        <translation>参数</translation>
    </message>
    <message>
        <source>Advanced</source>
        <translation>高级</translation>
    </message>
    <message>
        <source>dGB UNet Segmentation</source>
        <translation>dGB UNet 分割</translation>
    </message>
    <message>
        <source>dGB UNet Regression</source>
        <translation>dGB UNet 回归</translation>
    </message>
    <message>
        <source>Validation Split</source>
        <translation>验证集比例</translation>
    </message>
    <message>
        <source>0.20</source>
        <translation>0.20</translation>
    </message>
    <message>
        <source>Batch Size</source>
        <translation>批大小</translation>
    </message>
    <message>
        <source>Epochs</source>
        <translation>训练轮数</translation>
    </message>
    <message>
        <source>Early Stopping</source>
        <translation>早停轮数</translation>
    </message>
    <message>
        <source>Initial Learning Rate (1e)</source>
        <translation>初始学习率（1e）</translation>
    </message>
    <message>
        <source>Epoch drop (%)</source>
        <translation>轮次衰减（%）</translation>
    </message>
    <message>
        <source>33.33</source>
        <translation>33.33</translation>
    </message>
    <message>
        <source>Random seed</source>
        <translation>随机种子</translation>
    </message>
    <message>
        <source>Decimate Input</source>
        <translation>输入抽稀</translation>
    </message>
    <message>
        <source>Advanced network parameters will appear here when the algorithm backend is connected in P3.</source>
        <translation>P3 接入算法后端后，此处将显示高级网络参数。</translation>
    </message>
    <message>
        <source>Advanced options are reserved for the P3 algorithm backend.</source>
        <translation>高级选项预留给 P3 算法后端。</translation>
    </message>
    <message>
        <source>Reset</source>
        <translation>重置</translation>
    </message>
    <message>
        <source>Back</source>
        <translation>返回</translation>
    </message>
    <message>
        <source>Train</source>
        <translation>训练模型</translation>
    </message>
    <message>
        <source>Input seismic volume</source>
        <translation>输入地震体</translation>
    </message>
    <message>
        <source>Output volume</source>
        <translation>输出地震体</translation>
    </message>
    <message>
        <source>segmentation_result</source>
        <translation>segmentation_result</translation>
    </message>
    <message>
        <source>regression_result</source>
        <translation>regression_result</translation>
    </message>
    <message>
        <source>Segmentation outputs a 0/1 label volume.</source>
        <translation>分割输出 0/1 标签体。</translation>
    </message>
    <message>
        <source>Regression outputs a continuous prediction volume.</source>
        <translation>回归输出连续预测体。</translation>
    </message>
    <message>
        <source>Apply</source>
        <translation>应用模型</translation>
    </message>
    <message>
        <source>This is a UI shell rebuilt from the requirement screenshots. No data is read or written, no training or inference runs.

The real algorithm backend will be added in P3 after the client confirms:/n  D1 - self-developed PyTorch U-Net vs. dGB commercial license;
  D4 - labelled training samples and their format.</source>
        <translation>这是按需求截图重建的界面占位。不会读取或写入数据，也不会运行训练或推理。

真正的算法后端将在 P3 阶段、甲方确认以下事项后接入：
  D1——自研 PyTorch U-Net 还是购买 dGB 商业授权；
  D4——带标注的训练样本及其格式。</translation>
    </message>
</context>
</TS>
