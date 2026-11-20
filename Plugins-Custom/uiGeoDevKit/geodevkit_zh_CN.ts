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
</TS>
