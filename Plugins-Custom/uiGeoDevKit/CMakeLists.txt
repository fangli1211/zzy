#________________________________________________________________________
#
# uiGeoDevKit —— 地质解释定制开发工具包（P0 插件骨架）
#
# 关键点：
#  1. OD_IS_PLUGIN yes  -> 生成动态库而不是静态内核模块。
#  2. 不设置 OD_NO_ALO_ENTRY -> 让 OD_ADD_ALO_ENTRIES 生成 .alo 条目，
#     否则 DLL 编得出来但 od_main 启动时不会加载它。
#  3. OD_PLUGIN_ALO_EXEC 指定允许加载该插件的可执行程序。
#  4. 源码/头文件显式枚举（OpendTect 从不用 glob）。
#  5. 翻译单独使用 "geodevkit" 包名，避免改动官方 od_zh_CN.qm。
#________________________________________________________________________

set( OD_MODULE_DEPS uiODMain uiEarthModel EarthModel Geometry uiIo
                    uiBase uiFlatView uiSeis uiTools Well Algo Seis )
set( OD_FOLDER "OD Custom Plugins" )
set( OD_IS_PLUGIN yes )

set( OD_MODULE_SOURCES
    geodevkitpi.cc
    uigeodevkitinfo.cc
    uigeodevkitintersect.cc
    geodevkitdxf.cc
    uigeodevkitexport.cc
    uigeodevkitt2d.cc
    uigeodevkitcontour.cc
    uigeodevkitbuildfault.cc
    uigeodevkitsectiondxf.cc
    geodevkitdwt.cc
    uigeodevkitdwt.cc
    uigeodevkitml.cc
    uigeodevkitmdi.cc
)

set( OD_MODULE_INCFILES
    uigeodevkitinfo.h
    uigeodevkitintersect.h
    geodevkitdxf.h
    uigeodevkitexport.h
    uigeodevkitt2d.h
    uigeodevkitcontour.h
    uigeodevkitbuildfault.h
    uigeodevkitsectiondxf.h
    geodevkitdwt.h
    uigeodevkitdwt.h
    uigeodevkitml.h
    uigeodevkitmdi.h
)

set( OD_PLUGIN_ALO_EXEC ${OD_MAIN_EXEC} )

OD_INIT_MODULE()

#-----------------------------------------------------------------------
# 中文翻译：geodevkit_zh_CN.ts -> geodevkit_zh_CN.qm -> inst/share/localizations
#
# OpendTect 的 TextTranslatorLanguage::load() 会为当前语言加载
# "*_<locale>.qm"，并以文件名第一个下划线之前的部分作为 application 名；
# uiString 通过 mTextTranslationClass(..., "geodevkit") 指定同一个包名，
# 因此插件可以拥有独立的 .qm，无需合并进官方 580 KB 的 od_zh_CN.qm。
# 英文界面下找不到 geodevkit_en_US.qm，自然回退为源码里的英文原文。
#-----------------------------------------------------------------------
find_program( GEODEVKIT_LRELEASE
    NAMES lrelease lrelease.exe
    HINTS "${QT_INSTALL_BINS}"
    NO_DEFAULT_PATH )

if ( GEODEVKIT_LRELEASE )
    set( GEODEVKIT_TS "${CMAKE_CURRENT_SOURCE_DIR}/geodevkit_zh_CN.ts" )
    set( GEODEVKIT_QM "${CMAKE_CURRENT_BINARY_DIR}/geodevkit_zh_CN.qm" )
    # conda 的 Qt 把 DLL 放在 Library/bin，而 lrelease.exe 在 lib/qt6/bin；
    # 直接执行会因找不到 Qt6Core.dll 而退出 127，故显式补 PATH。
    add_custom_command(
        OUTPUT "${GEODEVKIT_QM}"
        COMMAND "${CMAKE_COMMAND}" -E env
                "PATH=${QT_INSTALL_BINS};$ENV{PATH}"
                "${GEODEVKIT_LRELEASE}" "${GEODEVKIT_TS}"
                -qm "${GEODEVKIT_QM}"
        DEPENDS "${GEODEVKIT_TS}"
        COMMENT "Compiling uiGeoDevKit Chinese translation"
        VERBATIM )
    add_custom_target( uiGeoDevKitTranslation ALL
                       DEPENDS "${GEODEVKIT_QM}" )
    install( FILES "${GEODEVKIT_QM}"
             DESTINATION "${OD_DATA_INSTALL_RELPATH}/localizations" )
else()
    message( WARNING
        "lrelease not found: uiGeoDevKit Chinese translation will be missing. "
        "Set QT_INSTALL_BINS to the Qt bin directory and reconfigure." )
endif()

# MSVC 默认按本地代码页解释无 BOM 源文件，会把 UTF-8 中文注释与字符串
# 读成乱码。仅对本插件打开 /utf-8，不影响官方内核源码。
if ( MSVC )
    target_compile_options( uiGeoDevKit PRIVATE /utf-8 )
endif()
