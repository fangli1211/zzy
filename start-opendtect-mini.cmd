@echo off
rem Double-click to launch the locally built OpendTect with the F3 Demo survey.
rem The build environment is assembled manually, so these paths must be set first.
set "VC=E:\VS2022\VC\Tools\MSVC\14.44.35207"
set "KIT=C:\Program Files (x86)\Windows Kits\10"
set "SDKVER=10.0.22621.0"
set "COND=D:\OpendTect\.pixi\envs\default\Library"
set "PATH=%VC%\bin\Hostx64\x64;%KIT%\bin\%SDKVER%\x64;%COND%\bin;%PATH%"
set "QT_QPA_PLATFORM_PLUGIN_PATH=%COND%\lib\qt6\plugins\platforms"
set "QT_PLUGIN_PATH=%COND%\lib\qt6\plugins"
set "DTECT_DATA=D:\OpendTectData"
cd /d "D:\OpendTect"
start "OpendTect" "D:\OpendTect\inst\bin\win64\Release\od_main.exe" --dataroot "D:\OpendTectData" --survey F3_Demo_2023
