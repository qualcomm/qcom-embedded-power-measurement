# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

[CmdletBinding()]
param(
    [string] $SourceRoot,
    [string] $BinDir,
    [string] $SevenZipDir,
    [string] $OutDir,
    [string] $Version,
    [switch] $MockPayload
)

$ErrorActionPreference = 'Stop'

$Arch = 'x64'

$here = $PSScriptRoot
if (-not $here) { $here = Split-Path -Parent $MyInvocation.MyCommand.Path }
if (-not $SourceRoot) { $SourceRoot = (Resolve-Path (Join-Path $here '..\..')).Path }
if (-not $OutDir)   { $OutDir   = Join-Path $SourceRoot '__Builds\installer' }
if (-not $BinDir)   { $BinDir   = Join-Path $SourceRoot "__Builds\x64\Release\bin" }

function Find-Tool($fileNames, $extraDirs) {
    foreach ($n in $fileNames) {
        $cmd = Get-Command $n -ErrorAction SilentlyContinue
        if ($cmd) { return $cmd.Source }
    }
    foreach ($d in $extraDirs) {
        foreach ($n in $fileNames) {
            $p = Join-Path $d $n
            if (Test-Path $p) { return $p }
        }
    }
    return $null
}

function Build-IExpressSed {
    param(
        [Parameter(Mandatory)] [string]   $TargetName,
        [Parameter(Mandatory)] [string]   $FriendlyName,
        [Parameter(Mandatory)] [string]   $AppLaunched,
        [string]                          $InstallPrompt = '',
        [string]                          $FinishMessage = '',
        [Parameter(Mandatory)] [string[]] $Files,
        [Parameter(Mandatory)] [string]   $PkgDir
    )
    $lines = @(
        '[Version]', 'Class=IEXPRESS', 'SEDVersion=3',
        '[Options]', 'PackagePurpose=InstallApp', 'ShowInstallProgramWindow=0', 'HideExtractAnimation=1',
        'UseLongFileName=1', 'InsideCompressed=0', 'CAB_FixedSize=0', 'CAB_ResvCodeSigning=0', 'RebootMode=N',
        'TargetName=%TargetName%', 'FriendlyName=%FriendlyName%', 'AppLaunched=%AppLaunched%',
        'PostInstallCmd=%PostInstallCmd%', 'InstallPrompt=%InstallPrompt%', 'FinishMessage=%FinishMessage%',
        'DisplayLicense=%DisplayLicense%', 'SourceFiles=SourceFiles',
        '[Strings]',
        "TargetName=$TargetName",
        "FriendlyName=$FriendlyName",
        "AppLaunched=$AppLaunched",
        'PostInstallCmd=<None>',
        "InstallPrompt=$InstallPrompt",
        "FinishMessage=$FinishMessage",
        'DisplayLicense='
    )
    for ($i = 0; $i -lt $Files.Count; $i++) { $lines += "FILE$i=`"$($Files[$i])`"" }
    $lines += '[SourceFiles]'
    $lines += "SourceFiles0=$PkgDir\"
    $lines += '[SourceFiles0]'
    for ($i = 0; $i -lt $Files.Count; $i++) { $lines += "%FILE$i%=" }
    return $lines
}

$szDirs = @()
if ($SevenZipDir) { $szDirs += $SevenZipDir }
$szDirs += @("$env:ProgramFiles\7-Zip", "${env:ProgramFiles(x86)}\7-Zip")
$sevenZip = Find-Tool @('7z.exe') $szDirs
if (-not $sevenZip) { throw "7z.exe not found. Install 7-Zip (winget install 7zip.7zip) or pass -SevenZipDir." }
$iexpress = Join-Path $env:WinDir 'System32\iexpress.exe'
if (-not (Test-Path $iexpress)) { throw "iexpress.exe not found at $iexpress." }

if (-not $Version) {
    $vh = Get-Content (Join-Path $SourceRoot 'src\libraries\qcommon-console\version.h') -Raw -ErrorAction SilentlyContinue
    if ($vh -match 'QEPM_VERSION\s+"([^"]+)"') { $Version = $Matches[1] } else { $Version = '0.0.0' }
}

Write-Host "Building QEPM installer: arch=$Arch version=$Version"
Write-Host "  7-Zip    : $sevenZip"
Write-Host "  IExpress : $iexpress"

$pkg  = Join-Path $OutDir "pkg-$Arch"
$data = Join-Path $pkg '_data'
Remove-Item $pkg -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Force -Path (Join-Path $data 'app') | Out-Null

if ($MockPayload) {
    Set-Content (Join-Path $data 'app\EPM.exe') 'placeholder'
    Set-Content (Join-Path $data 'app\EPMScope.exe') 'placeholder'
    Set-Content (Join-Path $data 'app\Qt6Core.dll') 'placeholder'
} else {
    if (-not (Test-Path $BinDir)) {
        throw "Build output not found: $BinDir. Run build.bat first (see README > Build & Usage)."
    }
    Copy-Item (Join-Path $BinDir '*') (Join-Path $data 'app') -Recurse -Force
}

Copy-Item (Join-Path $SourceRoot 'configurations') (Join-Path $data 'configurations') -Recurse -Force

if (Test-Path (Join-Path $SourceRoot 'examples')) {
    Copy-Item (Join-Path $SourceRoot 'examples') (Join-Path $data 'examples') -Recurse -Force
} else {
    Write-Warning "No examples\ directory found at $SourceRoot\examples; examples will be absent from the installer."
}

$interfacesDest = Join-Path $data 'interfaces'
New-Item -ItemType Directory -Force -Path $interfacesDest | Out-Null
# C++ integrator headers (no compiled library -- consumers build against these
# directly). Matches PROD parity: PROD ships interfaces\C++\{EPMDev,UDASDev}\ headers.
foreach ($cppMod in @('EPMDev', 'UDASDev')) {
    $cppHeaderSrc = Join-Path $SourceRoot "interfaces\C++\$cppMod"
    if (Test-Path $cppHeaderSrc) {
        $cppHeaderDest = Join-Path $interfacesDest "C++\$cppMod"
        New-Item -ItemType Directory -Force -Path $cppHeaderDest | Out-Null
        Copy-Item (Join-Path $cppHeaderSrc '*.h') $cppHeaderDest -Force
    } else {
        Write-Warning "No interfaces\C++\$cppMod directory found; C++ headers will be absent from the installer."
    }
}

foreach ($lang in @('Python', 'C#')) {
    $srcLang = Join-Path $SourceRoot "interfaces\$lang"
    if (Test-Path $srcLang) {
        Copy-Item $srcLang (Join-Path $interfacesDest $lang) -Recurse -Force
    } else {
        Write-Warning "No interfaces\$lang directory found; $lang interface will be absent from the installer."
    }
}

$zip = Join-Path $pkg 'payload.zip'
Push-Location $data
try {
    & $sevenZip a -tzip -mx=7 $zip '*' | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "payload.zip creation failed." }
} finally { Pop-Location }
Remove-Item $data -Recurse -Force -ErrorAction SilentlyContinue

Copy-Item (Join-Path $here 'install.ps1')   $pkg -Force
Copy-Item (Join-Path $here 'uninstall.ps1') $pkg -Force
Set-Content (Join-Path $pkg 'version.txt') $Version -NoNewline

$uninstallExe = Join-Path $pkg 'uninstall.exe'
$uninstallSed = Join-Path $OutDir "QEPM-Uninstall-$Arch.sed"
Remove-Item $uninstallExe -Force -ErrorAction SilentlyContinue

$uninstallSedLines = Build-IExpressSed `
    -TargetName $uninstallExe `
    -FriendlyName 'Qualcomm Embedded Power Measurement Uninstaller' `
    -AppLaunched 'cmd.exe /c powershell.exe -NoProfile -ExecutionPolicy Bypass -File uninstall.ps1' `
    -InstallPrompt 'Before uninstalling, disconnect all USB devices from this computer, then click OK.' `
    -FinishMessage 'Qualcomm Embedded Power Measurement has been uninstalled.' `
    -Files @('uninstall.ps1') `
    -PkgDir $pkg
[IO.File]::WriteAllText($uninstallSed, ($uninstallSedLines -join "`r`n") + "`r`n", (New-Object System.Text.ASCIIEncoding))
& $iexpress /N $uninstallSed | Out-Null
if (-not (Test-Path $uninstallExe)) { throw "IExpress did not produce $uninstallExe." }
Write-Host "  uninstall.exe : $uninstallExe"

$setup = Join-Path $OutDir "QEPM-windows-$Arch.exe"
Remove-Item $setup -Force -ErrorAction SilentlyContinue
$sed = Join-Path $OutDir "QEPM-$Arch.sed"
$sedLines = Build-IExpressSed `
    -TargetName $setup `
    -FriendlyName 'Qualcomm Embedded Power Measurement' `
    -AppLaunched 'powershell.exe -NoProfile -ExecutionPolicy Bypass -File install.ps1' `
    -Files @('install.ps1', 'uninstall.ps1', 'version.txt', 'payload.zip', 'uninstall.exe') `
    -PkgDir $pkg
[IO.File]::WriteAllText($sed, ($sedLines -join "`r`n") + "`r`n", (New-Object System.Text.ASCIIEncoding))

& $iexpress /N $sed | Out-Null
if (-not (Test-Path $setup)) { throw "IExpress did not produce $setup." }

Write-Host ""
Write-Host "Installer created: $setup"
Write-Host ("  size: {0:N1} MB" -f ((Get-Item $setup).Length / 1MB))
