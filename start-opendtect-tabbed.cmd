@echo off
rem ============================================================
rem  Double-click to launch the locally built OpendTect
rem  with the custom TABBED VIEW WINDOW already open.
rem
rem  Same environment as start-opendtect.cmd, plus
rem  OD_GEODEVKIT_TABBED=1 -> a "视图 1 / 视图 2" tab bar is
rem  created automatically inside the main window on startup.
rem  The original tiled views are untouched; you can always
rem  switch back via View menu -> 标签页视窗（定制）-> 平铺视窗（原有）.
rem ============================================================
set "VC=E:\VS2022\VC\Tools\MSVC\14.44.35207"
set "KIT=C:\Program Files (x86)\Windows Kits\10"
set "SDKVER=10.0.22621.0"
set "COND=D:\OpendTect\.pixi\envs\default\Library"
set "PATH=%VC%\bin\Hostx64\x64;%KIT%\bin\%SDKVER%\x64;%COND%\bin;%PATH%"
set "QT_QPA_PLATFORM_PLUGIN_PATH=%COND%\lib\qt6\plugins\platforms"
set "QT_PLUGIN_PATH=%COND%\lib\qt6\plugins"
set "DTECT_DATA=E:\OpendTectData"
set "OD_GEODEVKIT_TABBED=1"
cd /d "D:\OpendTect"
start "OpendTect Tabbed" "D:\OpendTect\inst\bin\win64\Release\od_main.exe" --dataroot "E:\OpendTectData" --survey F3_Demo_2023
