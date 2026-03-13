Param(
    [string]$QtBin = "C:\Qt\6.9.3\msvc2022_64\bin",
    [string]$QtPrefix = "",
    [string]$BuildDir = "build-win",
    [string]$Config = "Release",
    [string]$Version = "",
    [string]$Tag = "",
    [string]$Repo = "",
    [switch]$SkipUpload,
    [switch]$NoClobber
)

$ErrorActionPreference = "Stop"

function Require-Command([string]$name) {
    if (-not (Get-Command $name -ErrorAction SilentlyContinue)) {
        throw "Missing required command: $name"
    }
}

function Get-VersionFromHeader([string]$path) {
    if (!(Test-Path $path)) {
        throw "Version file not found: $path"
    }
    $verLine = Select-String -Path $path -Pattern 'LIBMOEX_VERSION "([^"]+)"' | Select-Object -First 1
    if (-not $verLine) {
        throw "Failed to parse version from $path"
    }
    return $verLine.Matches[0].Groups[1].Value
}

Require-Command gh

$root = Split-Path -Parent $PSScriptRoot
$verFile = Join-Path $root "src/libmoex/ver.h"
$stagePath = Join-Path $root "dist/windows/MachOExplorer"
$installerDir = Join-Path $root "dist/windows/installer"

if ([string]::IsNullOrWhiteSpace($Version)) {
    $Version = Get-VersionFromHeader $verFile
}
if ([string]::IsNullOrWhiteSpace($Tag)) {
    $Tag = "v$Version"
}
if ([string]::IsNullOrWhiteSpace($QtPrefix)) {
    $QtPrefix = Split-Path -Parent $QtBin
}

if ($Repo) {
    $repoArgs = @("--repo", $Repo)
} else {
    $repoArgs = @()
}

gh auth status | Out-Null
gh release view $Tag @repoArgs | Out-Null

& (Join-Path $PSScriptRoot "build_windows_installer.ps1") `
    -QtBin $QtBin `
    -QtPrefix $QtPrefix `
    -BuildDir $BuildDir `
    -Config $Config `
    -Version $Version

$portableZip = Join-Path $root ("dist/windows/MachOExplorer-{0}-windows-x64-portable.zip" -f $Version)
$installerExe = Join-Path $installerDir ("MachOExplorer-Setup-{0}.exe" -f $Version)

if (!(Test-Path $stagePath)) {
    throw "Staged app directory not found: $stagePath"
}
if (!(Test-Path $installerExe)) {
    throw "Installer not found: $installerExe"
}

if (Test-Path $portableZip) {
    Remove-Item -Force $portableZip
}
Compress-Archive -Path (Join-Path $stagePath "*") -DestinationPath $portableZip

Write-Host "Windows assets ready:"
Write-Host "  portable zip: $portableZip"
Write-Host "  installer   : $installerExe"

if ($SkipUpload) {
    Write-Host "Skipping GitHub release upload."
    exit 0
}

$uploadArgs = @("release", "upload", $Tag, $portableZip, $installerExe)
$uploadArgs += $repoArgs
if (-not $NoClobber) {
    $uploadArgs += "--clobber"
}

gh @uploadArgs

Write-Host "Uploaded Windows assets to GitHub release $Tag"
