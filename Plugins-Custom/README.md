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
    ├── uigeodevkitintersect.h/.cc  # 断层—层位交线提取（P2 首个落地功能）
    └── geodevkit_zh_CN.ts          # 独立中文翻译源（28 条，lrelease 生成 .qm）
```

## 已实现功能

1. 主窗口 **实用工具 → 地质解释定制工具包** 子菜单，作为 P1/P2/P3 向导的统一挂载点；
2. **工区与运行环境自检**对话框：显示当前工区、数据根目录与后续模块规划；
3. **断层—层位交线提取**对话框（P2 首个功能）：
   - 选择三维层位（`.hor`）与三维断层（`.flt`），设置层位垂向偏移；
   - 复用内核 `Geometry::FaultBinIDSurfaceIntersector` 计算交线；
   - 结果按点序显示 X/Y/Z，可导出带注释头的 ASCII 文件（制表符分隔）；
   - F3 Demo 2023 里可用 `Surfaces/Demo_*.hor` 与 `Surfaces/Fault_A.flt` 实测。
4. 自带独立中文翻译包 `geodevkit_zh_CN.qm`，**不改动官方 `od_zh_CN.qm`**，切英文界面自动回退英文原文。

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
