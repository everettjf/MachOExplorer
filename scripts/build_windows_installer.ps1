Param(
    [string]$QtBin = "",
    [string]$QtPrefix = "",
    [string]$BuildDir = "build-win",
    [string]$Config = "Release",
    [string]$Version = "",
    [string]$CMakePath = "",
    [string]$ISCCPath = ""
)

$ErrorActionPreference = "Stop"

function Resolve-ToolPath([string]$preferred, [string]$commandName, [string[]]$candidates = @()) {
    if (-not [string]::IsNullOrWhiteSpace($preferred)) {
        if (!(Test-Path $preferred)) {
            throw "$commandName not found: $preferred"
        }
        return (Resolve-Path $preferred).Path
    }

    $command = Get-Command $commandName -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($command) {
        return $command.Source
    }

    foreach ($candidate in $candidates) {
        if ([string]::IsNullOrWhiteSpace($candidate)) {
            continue
        }
        if (Test-Path $candidate) {
            return (Resolve-Path $candidate).Path
        }
    }

    throw "Missing required command: $commandName"
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

function Resolve-QtBin([string]$preferred) {
    $candidateRoots = @(
        "D:\Qt\6.10.2\msvc2022_64\bin",
        "C:\Qt\6.10.2\msvc2022_64\bin",
        "D:\Qt\6.9.3\msvc2022_64\bin",
        "C:\Qt\6.9.3\msvc2022_64\bin"
    )

    $candidates = @()
    if (-not [string]::IsNullOrWhiteSpace($preferred)) {
        $candidates += $preferred
    }
    $candidates += $candidateRoots

    foreach ($candidate in $candidates) {
        if ([string]::IsNullOrWhiteSpace($candidate)) {
            continue
        }

        $binPath = $candidate
        if (Test-Path $candidate -PathType Leaf) {
            $binPath = Split-Path -Parent $candidate
        }

        $windeployqt = Join-Path $binPath "windeployqt.exe"
        if (Test-Path $windeployqt) {
            return (Resolve-Path $binPath).Path
        }
    }

    throw "Qt bin path not found. Pass -QtBin explicitly, for example D:\Qt\6.10.2\msvc2022_64\bin"
}

function Resolve-BuiltExecutable([string]$buildPath, [string]$config) {
    $candidates = @(
        (Join-Path $buildPath "$config/MachOExplorer.exe"),
        (Join-Path $buildPath "src/$config/MachOExplorer.exe"),
        (Join-Path $buildPath "MachOExplorer.exe"),
        (Join-Path $buildPath "src/MachOExplorer.exe")
    )

    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) {
            return (Resolve-Path $candidate).Path
        }
    }

    $fallback = Get-ChildItem -Path $buildPath -Recurse -Filter "MachOExplorer.exe" -ErrorAction SilentlyContinue |
        Sort-Object FullName |
        Select-Object -First 1
    if ($fallback) {
        return $fallback.FullName
    }

    throw "Built executable not found under $buildPath"
}

function Resolve-VcRedistDir() {
    $redistRoots = @(
        "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Redist\MSVC",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Redist\MSVC",
        "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Redist\MSVC"
    )

    $candidates = @()
    foreach ($root in $redistRoots) {
        if (!(Test-Path $root)) {
            continue
        }

        $versionDirs = Get-ChildItem -Path $root -Directory -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -match '^\d+\.\d+\.\d+$' }
        foreach ($versionDir in $versionDirs) {
            $crtDir = Join-Path $versionDir.FullName "x64\Microsoft.VC143.CRT"
            if (Test-Path $crtDir) {
                $candidates += [PSCustomObject]@{
                    Version = [version]$versionDir.Name
                    Path = $crtDir
                }
            }
        }
    }

    if ($candidates.Count -eq 0) {
        return $null
    }

    return ($candidates | Sort-Object Version -Descending | Select-Object -First 1).Path
}

function Copy-VcRuntime([string]$stagePath) {
    $vcRedistDir = Resolve-VcRedistDir
    if ([string]::IsNullOrWhiteSpace($vcRedistDir)) {
        Write-Warning "MSVC runtime redist directory not found; installer may require VC++ Redistributable on target machines."
        return
    }

    Get-ChildItem -Path $vcRedistDir -File -Filter "*.dll" | Copy-Item -Destination $stagePath -Force
}

$root = Split-Path -Parent $PSScriptRoot
$buildPath = Join-Path $root $BuildDir
$stagePath = Join-Path $root "dist/windows/MachOExplorer"
$installerDir = Join-Path $root "dist/windows/installer"
$verFile = Join-Path $root "src/libmoex/ver.h"
$iss = Join-Path $root "packaging/windows/MachOExplorer.iss"

$cmake = Resolve-ToolPath $CMakePath "cmake" @(
    "D:\Qt\Tools\CMake_64\bin\cmake.exe",
    "C:\Qt\Tools\CMake_64\bin\cmake.exe",
    "C:\Program Files\CMake\bin\cmake.exe",
    "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
)
$iscc = Resolve-ToolPath $ISCCPath "iscc" @(
    "D:\Program Files (x86)\Inno Setup 6\ISCC.exe",
    "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
)
$QtBin = Resolve-QtBin $QtBin
$windeployqt = Join-Path $QtBin "windeployqt.exe"

if ([string]::IsNullOrWhiteSpace($QtPrefix)) {
    $QtPrefix = Split-Path -Parent $QtBin
}
if ([string]::IsNullOrWhiteSpace($Version)) {
    $Version = Get-VersionFromHeader $verFile
}

& $cmake `
    -S (Join-Path $root "src") `
    -B $buildPath `
    -G "Visual Studio 17 2022" `
    -A x64 `
    "-DCMAKE_PREFIX_PATH=$QtPrefix"

& $cmake --build $buildPath --config $Config --parallel 8

$exePath = Resolve-BuiltExecutable $buildPath $Config

if (Test-Path $stagePath) {
    Remove-Item -Recurse -Force $stagePath
}
New-Item -Force -ItemType Directory -Path $stagePath | Out-Null
New-Item -Force -ItemType Directory -Path $installerDir | Out-Null
Copy-Item -Force $exePath (Join-Path $stagePath "MachOExplorer.exe")
& $windeployqt --release --compiler-runtime --dir $stagePath (Join-Path $stagePath "MachOExplorer.exe")
Copy-VcRuntime $stagePath

& $iscc "/DMyAppVersion=$Version" "/DMyAppSourceRoot=$stagePath" $iss

$installerExe = Join-Path $installerDir ("MachOExplorer-Setup-{0}.exe" -f $Version)
if (!(Test-Path $installerExe)) {
    throw "Installer not found: $installerExe"
}

Write-Host "Qt bin    : $QtBin"
Write-Host "Executable: $exePath"
Write-Host "Installer : $installerExe"
