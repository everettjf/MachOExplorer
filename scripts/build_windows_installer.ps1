Param(
    [string]$QtBin = "C:\Qt\6.9.3\msvc2022_64\bin",
    [string]$BuildDir = "build-win",
    [string]$Config = "Release"
)

$ErrorActionPreference = "Stop"

function Require-Command([string]$name) {
    if (-not (Get-Command $name -ErrorAction SilentlyContinue)) {
        throw "Missing required command: $name"
    }
}

Require-Command cmake
Require-Command iscc

$root = Split-Path -Parent $PSScriptRoot
$buildPath = Join-Path $root $BuildDir
$stagePath = Join-Path $root "dist/windows/MachOExplorer"
$exePath = Join-Path $buildPath "src/$Config/MachOExplorer.exe"

cmake -S (Join-Path $root "src") -B $buildPath -DCMAKE_BUILD_TYPE=$Config
cmake --build $buildPath --config $Config -j 8

if (!(Test-Path $exePath)) {
    throw "Built executable not found: $exePath"
}

New-Item -Force -ItemType Directory -Path $stagePath | Out-Null
Copy-Item -Force $exePath (Join-Path $stagePath "MachOExplorer.exe")

$windeployqt = Join-Path $QtBin "windeployqt.exe"
if (!(Test-Path $windeployqt)) {
    throw "windeployqt not found: $windeployqt"
}

& $windeployqt --release --dir $stagePath (Join-Path $stagePath "MachOExplorer.exe")

$iss = Join-Path $root "packaging/windows/MachOExplorer.iss"
iscc $iss

Write-Host "Installer built under dist/windows/installer"
