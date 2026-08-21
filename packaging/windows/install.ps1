# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

[CmdletBinding()]
param(
    [string] $InstallRoot = "$env:ProgramFiles\Qualcomm\QEPM",
    [string] $DataRoot    = "$env:ProgramData\Qualcomm\QEPM",
    [switch] $NoElevate
)

$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$appName    = 'QEPM'
$appDisplay = 'Qualcomm Embedded Power Measurement'
$publisher  = 'Qualcomm Technologies, Inc.'
$version = (Get-Content (Join-Path $root 'version.txt') -ErrorAction SilentlyContinue | Select-Object -First 1)
if (-not $version) { $version = '0.0.0' }

$ExamplesRoot = 'C:\QEPM\examples'

# Install log
$logFile = Join-Path $env:TEMP 'QEPM-install.log'
function Log($m) { try { Add-Content -LiteralPath $logFile -Value ("{0} [{1}] {2}" -f (Get-Date -Format 'HH:mm:ss'), $PID, $m) } catch {} }
trap { Log "FATAL: $($_.Exception.Message)"; throw }
Log "start root=[$root] NoElevate=$NoElevate cmd=[$($MyInvocation.Line)]"

function Test-Admin {
    $id = [Security.Principal.WindowsIdentity]::GetCurrent()
    (New-Object Security.Principal.WindowsPrincipal $id).IsInRole(
        [Security.Principal.WindowsBuiltinRole]::Administrator)
}

if (-not $NoElevate -and -not (Test-Admin)) {
    $argList = @('-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', "`"$PSCommandPath`"",
                 '-InstallRoot', "`"$InstallRoot`"", '-DataRoot', "`"$DataRoot`"")
    Start-Process -FilePath 'powershell.exe' -ArgumentList $argList -Verb RunAs -Wait
    exit $LASTEXITCODE
}

# If a previous QEPM install is present, remove it completely before installation
$arpKey = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$appName"
if ((Test-Path $arpKey) -or (Test-Path $InstallRoot)) {
    Write-Host "Existing $appDisplay installation detected - removing it first..."
    Log "existing install detected (ARP=$(Test-Path $arpKey) InstallRoot=$(Test-Path $InstallRoot)) - uninstalling before install"

    # Stop running app processes so their files aren't locked during removal/overwrite.
    $procNames = @('EPM', 'EPMScope', 'EPMViewer', 'EPMConfigurationEditor', 'BugWriter',
                   'EPMDump', 'PSOCProgrammer', 'SCLDump')
    foreach ($p in $procNames) {
        Get-Process -Name $p -ErrorAction SilentlyContinue | ForEach-Object {
            Log "stopping running process $($_.Name) (pid $($_.Id))"
            Stop-Process -Id $_.Id -Force -ErrorAction SilentlyContinue
        }
    }
    Start-Sleep -Milliseconds 500

    $uninstallScript = Join-Path $root 'uninstall.ps1'
    if (Test-Path $uninstallScript) {
        & powershell.exe -NoProfile -ExecutionPolicy Bypass -File $uninstallScript `
            -InstallRoot $InstallRoot -DataRoot $DataRoot -NoElevate
        Log "previous-install uninstall.ps1 exit code: $LASTEXITCODE"
    } else {
        Log "WARNING: uninstall.ps1 not found at $uninstallScript - skipping pre-install cleanup"
    }

    if (Test-Path $InstallRoot) {
        Write-Warning "Some files under $InstallRoot could not be removed (likely still in use). Installation will continue and overwrite what it can."
        Log "InstallRoot still present after pre-install uninstall"
    } else {
        Log "previous install removed successfully"
    }
}

function Copy-Tree($from, $to) {
    if (-not (Test-Path $from)) { return }
    New-Item -ItemType Directory -Force -Path $to | Out-Null
    Copy-Item -Path (Join-Path $from '*') -Destination $to -Recurse -Force
}

$src = $root
$zip = Join-Path $root 'payload.zip'
if (Test-Path $zip) {
    $src = Join-Path $root '_payload'
    Remove-Item $src -Recurse -Force -ErrorAction SilentlyContinue
    Log "expanding payload.zip -> $src"
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    [System.IO.Compression.ZipFile]::ExtractToDirectory($zip, $src)
}

Write-Host "Installing $appDisplay $version"
Write-Host "  Program files : $InstallRoot"
Write-Host "  Shared data   : $DataRoot"
Write-Host "  Examples      : $ExamplesRoot"

# 1. Application binaries + Qt runtime
Copy-Tree (Join-Path $src 'app') $InstallRoot

# 2. Shared, machine-wide data
Copy-Tree (Join-Path $src 'configurations') (Join-Path $DataRoot 'configurations')

# 2a. Interfaces (Python + C# SDKs)
Copy-Tree (Join-Path $src 'interfaces') (Join-Path $DataRoot 'interfaces')
Log "interfaces copied to $(Join-Path $DataRoot 'interfaces')"

# 2b. Examples -> fixed top-level
Copy-Tree (Join-Path $src 'examples') $ExamplesRoot
Log "examples copied to $ExamplesRoot"

# 3. Bundle the uninstaller alongside the app
Copy-Item (Join-Path $root 'uninstall.ps1') $InstallRoot -Force
Copy-Item (Join-Path $root 'uninstall.exe') $InstallRoot -Force -ErrorAction SilentlyContinue
Copy-Item (Join-Path $root 'version.txt')   $InstallRoot -Force -ErrorAction SilentlyContinue

# 4. All-users Start Menu folder with shortcuts for each application
$startMenuFolder = Join-Path $env:ProgramData "Microsoft\Windows\Start Menu\Programs\$appDisplay"
New-Item -ItemType Directory -Force -Path $startMenuFolder | Out-Null
Log "start-menu folder: $startMenuFolder"

$wsh = New-Object -ComObject WScript.Shell
$shortcuts = @(
    [ordered]@{ Name = 'Embedded Power Monitor';          Exe = 'EPM.exe';                    Desc = 'Qualcomm Embedded Power Monitor' },
    [ordered]@{ Name = 'EPM Scope';                       Exe = 'EPMScope.exe';               Desc = 'EPM Scope Application' },
    [ordered]@{ Name = 'EPM Viewer';                      Exe = 'EPMViewer.exe';              Desc = 'EPM Viewer Application' },
    [ordered]@{ Name = 'EPM Configuration Editor';        Exe = 'EPMConfigurationEditor.exe'; Desc = 'EPM Configuration Editor Application' },
    [ordered]@{ Name = 'Bug Writer';                      Exe = 'BugWriter.exe';              Desc = 'Bug Writer Application' },
    [ordered]@{ Name = 'Uninstall QEPM';                  Exe = 'uninstall.exe';              Desc = 'QEPM Uninstall' }
)
foreach ($s in $shortcuts) {
    $exePath = Join-Path $InstallRoot $s.Exe
    if (Test-Path $exePath) {
        $lnk = Join-Path $startMenuFolder "$($s.Name).lnk"
        $sc = $wsh.CreateShortcut($lnk)
        $sc.TargetPath       = $exePath
        $sc.WorkingDirectory = $InstallRoot
        $sc.IconLocation     = "$exePath,0"
        $sc.Description      = $s.Desc
        $sc.Save()
        Log "shortcut created: $lnk"
    } else {
        Log "skipped shortcut (not found): $exePath"
    }
}

# 5. Register .ccnf file association (EPM configuration files)
$epmConfigEditorExe = Join-Path $InstallRoot 'EPMConfigurationEditor.exe'
if (Test-Path $epmConfigEditorExe) {
    $ccnfProgId = 'QEPM.EPMConfig'

    New-Item -Path 'HKLM:\SOFTWARE\Classes\.ccnf' -Force | Out-Null
    Set-ItemProperty 'HKLM:\SOFTWARE\Classes\.ccnf' '(Default)' $ccnfProgId

    New-Item -Path "HKLM:\SOFTWARE\Classes\$ccnfProgId" -Force | Out-Null
    Set-ItemProperty "HKLM:\SOFTWARE\Classes\$ccnfProgId" '(Default)' 'QEPM Configuration File'

    New-Item -Path "HKLM:\SOFTWARE\Classes\$ccnfProgId\DefaultIcon" -Force | Out-Null
    Set-ItemProperty "HKLM:\SOFTWARE\Classes\$ccnfProgId\DefaultIcon" '(Default)' "`"$epmConfigEditorExe`",0"

    New-Item -Path "HKLM:\SOFTWARE\Classes\$ccnfProgId\shell\open\command" -Force | Out-Null
    Set-ItemProperty "HKLM:\SOFTWARE\Classes\$ccnfProgId\shell\open\command" '(Default)' "`"$epmConfigEditorExe`" `"%1`""

    try {
        Add-Type -Namespace QEPM -Name Shell32 -MemberDefinition @'
[System.Runtime.InteropServices.DllImport("shell32.dll")]
public static extern void SHChangeNotify(int wEventId, int uFlags, IntPtr dwItem1, IntPtr dwItem2);
'@ -ErrorAction Stop
        [QEPM.Shell32]::SHChangeNotify(0x08000000, 0x0000, [IntPtr]::Zero, [IntPtr]::Zero)
        Log ".ccnf association change notified to Explorer (SHChangeNotify)"
    } catch {
        Log "WARNING: SHChangeNotify failed (non-fatal, association still registered): $($_.Exception.Message)"
    }

    Log ".ccnf file association registered -> $epmConfigEditorExe"
} else {
    Log "skipped .ccnf file association (EPMConfigurationEditor.exe not found): $epmConfigEditorExe"
}

# 6. Add/Remove Programs
$epmExe       = Join-Path $InstallRoot 'EPM.exe'
$uninstallExe = Join-Path $InstallRoot 'uninstall.exe'
$uninstallCmd = if (Test-Path $uninstallExe) { "`"$uninstallExe`"" } `
                else { "powershell.exe -NoProfile -ExecutionPolicy Bypass -File `"$InstallRoot\uninstall.ps1`"" }
$key = "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$appName"
New-Item -Path $key -Force | Out-Null
Set-ItemProperty $key DisplayName     $appDisplay
Set-ItemProperty $key DisplayVersion  $version
Set-ItemProperty $key Publisher       $publisher
Set-ItemProperty $key InstallLocation $InstallRoot
Set-ItemProperty $key DisplayIcon     $epmExe
Set-ItemProperty $key UninstallString $uninstallCmd
Set-ItemProperty $key NoModify 1 -Type DWord
Set-ItemProperty $key NoRepair 1 -Type DWord

Write-Host "$appDisplay $version installed."
Log "done"