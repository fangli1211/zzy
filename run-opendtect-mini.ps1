# Launch the locally built OpendTect (main branch, Release).
# The build environment is assembled manually (MSVC + Windows SDK + Pixi/conda),
# so the executable needs these paths on PATH to find Qt / OSG / CRT DLLs.

$vc   = 'E:\VS2022\VC\Tools\MSVC\14.44.35207'
$kit  = 'C:\Program Files (x86)\Windows Kits\10'
$ver  = '10.0.22621.0'
$cond = 'D:\OpendTect\.pixi\envs\default\Library'
$exe  = 'D:\OpendTect\inst\bin\win64\Release\od_main.exe'
$root = 'D:\OpendTectData'

$env:PATH = "$vc\bin\Hostx64\x64;$kit\bin\$ver\x64;$cond\bin;" + $env:PATH
$env:QT_QPA_PLATFORM_PLUGIN_PATH = "$cond\lib\qt6\plugins\platforms"
$env:QT_PLUGIN_PATH              = "$cond\lib\qt6\plugins"
$env:DTECT_DATA                  = $root
New-Item -ItemType Directory -Force -Path $root | Out-Null

if (-not (Test-Path $exe)) {
    throw "OpendTect executable not found: $exe`nBuild it first with build-opendtect.ps1"
}

Set-Location 'D:\OpendTect'
$survey  = 'F3_Demo_2023'
$argList = @('--dataroot', $root, '--survey', $survey)
Start-Process -FilePath $exe -ArgumentList $argList -WorkingDirectory 'D:\OpendTect' -PassThru |
    Select-Object Id, ProcessName, StartTime
