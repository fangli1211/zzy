#________________________________________________________________________
#
# OpendTect 外部插件工作区（PLUGIN_DIR 入口）
#
# 载入方式：由 D:\OpendTect\CMakeLists.txt 第 138-142 行执行
#     include( ${PLUGIN_DIR}/CMakeLists.txt )
# 该 include 发生在 OD_WRITE_ALOFILES() 之前，所以此处登记的插件会自动
# 生成 od_main.<插件名>.alo 描述文件，主程序启动时即自动加载。
#
# 配置方式（在本机构建命令里追加）：
#     -D "PLUGIN_DIR=D:/OpendTect-Plugins"
#
# 约束：源码显式枚举，绝不使用 file(GLOB)；新增插件必须同时在此列表和
#       自己的子目录 CMakeLists.txt 中登记。
#________________________________________________________________________

set( GEODEVKIT_PLUGINS
    uiGeoDevKit
)

OD_ADD_MODULES( "${PLUGIN_DIR}" ${GEODEVKIT_PLUGINS} )
