# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
# SPDX-License-Identifier: BSD-3-Clause

[CmdletBinding()]
param(
    [string] $InstallRoot = "$env:ProgramFiles\Qualcomm\QEPM",
    [string] $DataRoot    = "$env:ProgramData\Qualcomm\QEPM",
    [switch] $NoElevate,
    [switch] $KeepData
)

$ErrorActionPreference = 'Stop'
$appName    = 'QEPM'
$appDisplay = 'Qualcomm Embedded Power Measurement'
$ExamplesRoot = 'C:\QEPM\examples'

$logFile = Join-Path $env:TEMP 'QEPM-uninstall.log'
function Log($m) { try { Add-Content -LiteralPath $logFile -Value ("{0} [{1}] {2}" -f (Get-Date -Format 'HH:mm:ss'), $PID, $m) } catch {} }
trap { Log "FATAL: $($_.Exception.Message)"; throw }
Log "start InstallRoot=[$InstallRoot] NoElevate=$NoElevate KeepData=$KeepData"

function Test-Admin {
    $id = [Security.Principal.WindowsIdentity]::GetCurrent()
    (New-Object Security.Principal.WindowsPrincipal $id).IsInRole(
        [Security.Principal.WindowsBuiltinRole]::Administrator)
}

if (-not $NoElevate -and -not (Test-Admin)) {
    $scriptFile = if ($PSCommandPath) { $PSCommandPath } else { $MyInvocation.MyCommand.Path }
    $argList = @('-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', "`"$scriptFile`"",
                 '-InstallRoot', "`"$InstallRoot`"", '-DataRoot', "`"$DataRoot`"")
    if ($KeepData) { $argList += '-KeepData' }
    Start-Process -FilePath 'powershell.exe' -ArgumentList $argList -Verb RunAs -Wait
    exit $LASTEXITCODE
}

Write-Host "Uninstalling $appDisplay"
Log "running elevated"

$startMenuFolder = Join-Path $env:ProgramData "Microsoft\Windows\Start Menu\Programs\$appDisplay"
Remove-Item $startMenuFolder -Recurse -Force -ErrorAction SilentlyContinue
Log "start-menu folder removed"

Remove-Item "HKLM:\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$appName" `
    -Recurse -Force -ErrorAction SilentlyContinue
Log "ARP registry key removed"

# Remove .ccnf file association
$ccnfProgId = 'QEPM.EPMConfig'
Remove-Item "HKLM:\SOFTWARE\Classes\$ccnfProgId" -Recurse -Force -ErrorAction SilentlyContinue
Remove-Item 'HKLM:\SOFTWARE\Classes\.ccnf' -Recurse -Force -ErrorAction SilentlyContinue
Log ".ccnf file association removed"

# Stop running QEPM processes
$qepmProcessNames = @(
    'EPM', 'EPMScope', 'EPMViewer', 'EPMConfigurationEditor',
    'BugWriter', 'EPMDump', 'PSOCProgrammer', 'SCLDump'
)
foreach ($procName in $qepmProcessNames) {
    Get-Process -Name $procName -ErrorAction SilentlyContinue | ForEach-Object {
        try {
            $procPath = $_.Path
            if ($procPath -and $procPath.StartsWith($InstallRoot, [StringComparison]::OrdinalIgnoreCase)) {
                Write-Host "  Stopping running process $($_.ProcessName) (PID $($_.Id)) ..."
                Log "stopping process $($_.ProcessName) (PID $($_.Id))"
                Stop-Process -Id $_.Id -Force -ErrorAction SilentlyContinue
            }
        } catch {}
    }
}

Start-Sleep -Milliseconds 500

if (-not $KeepData) {
    Remove-Item $DataRoot -Recurse -Force -ErrorAction SilentlyContinue
    if (Test-Path $DataRoot) {
        Log "DataRoot still present after Remove-Item (access denied or files in use)"
    } else {
        Log "DataRoot removed"
    }

    if (Test-Path $ExamplesRoot) {
        Remove-Item $ExamplesRoot -Recurse -Force -ErrorAction SilentlyContinue
        if (Test-Path $ExamplesRoot) {
            Log "ExamplesRoot ($ExamplesRoot) still present after Remove-Item (access denied or files in use)"
        } else {
            Log "ExamplesRoot ($ExamplesRoot) removed"
            $examplesParent = Split-Path $ExamplesRoot -Parent
            if ((Test-Path $examplesParent) -and
                -not (Get-ChildItem -LiteralPath $examplesParent -Force -ErrorAction SilentlyContinue)) {
                Remove-Item $examplesParent -Force -ErrorAction SilentlyContinue
                Log "removed empty parent $examplesParent"
            }
        }
    } else {
        Log "ExamplesRoot ($ExamplesRoot) not present - nothing to remove"
    }
}

Set-Location $env:SystemRoot   # don't sit inside any directory being deleted

if (Test-Path $InstallRoot) {
    Remove-Item $InstallRoot -Recurse -Force -ErrorAction SilentlyContinue
}

if (Test-Path $InstallRoot) {
    Log "InstallRoot still present after initial Remove-Item; retrying"
    for ($i = 0; $i -lt 10 -and (Test-Path $InstallRoot); $i++) {
        Start-Sleep -Seconds 2
        Remove-Item $InstallRoot -Recurse -Force -ErrorAction SilentlyContinue
    }
}

if (Test-Path $InstallRoot) {
    Write-Warning "Could not fully remove: $InstallRoot. Please close any running QEPM applications and delete it manually."
    Log "InstallRoot still present after retries - giving up"
} else {
    Log "InstallRoot removed"
}

Write-Host "$appDisplay uninstalled."
Log "done"