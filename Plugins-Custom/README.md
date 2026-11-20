# OpendTect 定制插件工作区（PLUGIN_DIR）

本目录是 `fangli1211/zzy` 仓库内的**定制开发子目录**，对应《OpendTect 二次开发计划》的 P0/P2 阶段。所有定制代码放这里，不改 OpendTect 内核源码。

- 源码树：`D:\OpendTect`（`main @ 20d1792`，v8.1.0，GPLv3+）
- 插件目录：`D:\OpendTect\Plugins-Custom`（本目录，仓库内 `Plugins-Custom/`）
- 构建树 / 安装树：`D:\OpendTect\build_pixi` / `D:\OpendTect\inst`
- 详细编译说明：`D:\OpendTect-Docs\OpendTect-插件开发-编译说明.md`

## 目录结构

```
Plugins-Custom/
├── CMakeLists.txt                  # PLUGIN_DIR 入口，显式列出要构建的插件
├── .gitignore
├── README.md                       # 本文件
└── uiGeoDevKit/                    # 地质解释定制工具包插件
    ├── CMakeLists.txt              # 模块依赖、源码显式枚举、翻译生成与安装
    ├── geodevkitpi.cc              # 插件入口、菜单注入、生命周期
    ├── uigeodevkitinfo.h/.cc       # 工区与运行环境自检对话框（P0）
    ├── uigeodevkitintersect.h/.cc  # 断层—层位交线提取（P2）
    ├── geodevkitdxf.h/.cc          # ASCII DXF R12 写出引擎（P2 · ioDXF）
    ├── uigeodevkitexport.h/.cc     # 成果导出中心：层位/断层/交线/井/测线 → DXF/ASCII（P2）
    ├── uigeodevkitcontour.h/.cc    # 等值线成图导出：IsoContourTracer → DXF（P2）
    ├── uigeodevkitt2d.h/.cc        # 时深转换向导（封装官方对话框，P2）
    ├── uigeodevkitbuildfault.h/.cc # 断层棍生成断层面向导（FSS→Fault3D，P2 收尾）
    ├── uigeodevkitsectiondxf.h/.cc # 剖面矢量 DXF（Inline/Xline/2D 采样，P2 收尾）
    └── geodevkit_zh_CN.ts          # 独立中文翻译源（lrelease 生成 .qm）
```

## 已实现功能

1. 主窗口 **实用工具 → 地质解释定制工具包** 子菜单，作为 P1/P2/P3 向导的统一挂载点；
2. **工区与运行环境自检**对话框：显示当前工区、数据根目录与后续模块规划；
3. **断层—层位交线提取**对话框（P2 首个功能）：
   - 选择三维层位（`.hor`）与三维断层（`.flt`），设置层位垂向偏移；
   - 复用内核 `Geometry::FaultBinIDSurfaceIntersector` 计算交线；
   - 结果按点序显示 X/Y/Z，可导出带注释头的 ASCII 文件（制表符分隔）；
   - F3 Demo 2023 里可用 `Surfaces/Demo_*.hor` 与 `Surfaces/Fault_A.flt` 实测。
4. **成果导出中心**（P2）：
   - 层位（`.hor`）→ 逐 Inline 折线；断层（`.flt`）→ 逐断层棍折线；
   - 交线（层位+断层+垂向偏移）→ 折线，复用内核求交算法；
   - 井 → 井口点 + 井轨迹折线；二维测线 → 平面折线 + 线名标注；
   - 统一可选 **DXF（AutoCAD R12）** 或 **ASCII（X Y Z 表）**，图层命名 `HORIZON_*` / `FAULT_*` / `INTERSECTION_*` / `WELL_*` / `SURVEY_LINE`；
   - 全部几何读取走公开 EM/Survey API，DXF 格式化集中在 `geodevkitdxf.{h,cc}`；
5. **等值线成图导出**（P2）：选三维层位 + 首条/等值距/末条等值线（限 1–500 级），复用内核 `IsoContourTracer`，每条等值线按 `CONTOUR_<level>` 分层输出 DXF，可叠加断层参照线；注释说明了「断层内不画等高线」的 polygon 裁剪待甲方 D6/D8 确认后启用；
6. **时深转换向导**（P2）：中文 5 步引导 + `canTransform()` 预检，一键打开官方 `EM::uiTime2DepthDlg`（转换引擎仍用内核 `SurfaceT2DTransformer`，不重复造轮子）；
7. **导出二维剖面图片**（P2）：把当前打开的 2D 剖面视图（`uiGraphicsScene`，含层位/断层叠加）导出为 1200×800 PNG；
8. **断层棍生成断层面**（P2 收尾）：多选 `FaultStickSet` → 用内核 `FSStoFault3DConverter` 合并转换为一个 `Fault3D` → `saver()` 落盘；对应"多剖面拾取后一键整理、转换、保存"；
9. **剖面矢量 DXF**（P2 收尾）：选 3D Inline / Xline / 2D 测线，采样多个层位（`Horizon3D::getZ`）与断层棍节点，输出 X=沿剖面距离、Z=层位/断层值的分层 DXF（`HORIZON_*` / `FAULT_*`）；
10. 自带独立中文翻译包 `geodevkit_zh_CN.qm`，**不改动官方 `od_zh_CN.qm`**，切英文界面自动回退英文原文。

> P2 仅剩「统一编辑撤销/版本管理」为后续项（框架级改动，建议二期）。层位自动追踪（`Horizon3DTracker`）与 viewer 数据缓存强绑定，留待结合具体工作流再封装。剖面矢量 DXF 的断层部分当前只画已拾取的棍节点（不补棍段与剖面的插值交点），如需真实交点可后续接入 `FaultTrcDataProvider::getFaultZVals`。

## 快速开始（本机）

```powershell
powershell -ExecutionPolicy Bypass -NoProfile -File D:\pixi\resume-opendtect-build.ps1
```

脚本已完成：装配 MSVC 14.44 + WinSDK 22621 + conda/Qt 环境 → CMake configure（`-D PLUGIN_DIR=D:/OpendTect/Plugins-Custom`）→ Release build → install。

> 构建/install 前先确认没有 `od_main.exe` 在跑，否则 `inst\` 里的 DLL 被占用会导致 install 失败（exit 1）。

> 不要用 `pixi run config/build`：本机 conda-forge `vs2022_win-64` 的激活脚本会调 `reg.exe`，被命令安全策略拦截，导致 `rc.exe`/`mt.exe`/SDK 头缺失。

## 新增插件 / 新增文件的规矩

1. 在顶层 `CMakeLists.txt` 的 `GEODEVKIT_PLUGINS` 里加插件目录名；
2. 新插件目录内 `CMakeLists.txt` 必须设置 `OD_IS_PLUGIN yes`、`OD_PLUGIN_ALO_EXEC ${OD_MAIN_EXEC}`，**不要**设 `OD_NO_ALO_ENTRY`（否则 DLL 编得出但不被加载）；
3. 每个 `.cc` / `.h` 都要显式写进 `OD_MODULE_SOURCES` / `OD_MODULE_INCFILES`，**从不用 glob**；
4. 插件入口 `.cc` 第一句 include 自动生成的 `<插件名>mod.h`（导出宏来源），再 include `odplugin.h`；
5. 所有用户可见文字走 `tr()`，并在 `geodevkit_zh_CN.ts` 里补词条；
6. 改完必须 `cmake --build` **和** `cmake --install` —— 运行的是 `inst\` 里的副本；
7. 源码 LF 换行、文件末尾换行、中文注释；禁用 `strcmp(` / `sqrt(` / `atan2(` / `ArrPtrMan<T> x = new`（OpendTect 自带 lint 会检查）。

## 版本与许可

插件基于 OpendTect 开发，衍生部分按 **GPLv3+** 交付完整源码。引入任何第三方库（Eigen / ONNX Runtime / PyTorch 等）前先核对许可证兼容性，商业插件二进制不得随源码再分发。
