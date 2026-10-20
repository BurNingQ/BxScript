# ============================================================
#  Companion checker for regression.bx
#
#  Covers the two things the .bx script cannot assert by itself
#  (they need Win32 to prove):
#    1. owner relation + window style bits
#    2. parent disabled during modal / restored after close
#
#  Usage (from repo root):
#    powershell -File regression_check.ps1
#
#  It launches regression.bx itself, asserts during the modal
#  window, then prints the script's full output.
#
#  NOTE: THIS FILE MUST STAY PURE ASCII.
#  PowerShell reads .ps1 as ANSI; UTF-8 Chinese in it breaks quoting.
# ============================================================

$ErrorActionPreference = 'Stop'
# Emit UTF-8 so Chinese in the .bx log displays correctly (console or redirected).
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

$ROOT   = Split-Path -Parent $MyInvocation.MyCommand.Path
$EXE    = Join-Path $ROOT 'cmake-build-release\bin\BxScript_GUI.exe'
$SCRIPT = Join-Path $ROOT 'regression.bx'
$LOG    = Join-Path $env:TEMP 'bx_regression_out.log'

if (-not (Test-Path $EXE))    { Write-Host "EXE not found: $EXE"; exit 1 }
if (-not (Test-Path $SCRIPT)) { Write-Host "Script not found: $SCRIPT"; exit 1 }

# MinGW runtime DLLs are not next to the exe; without this the
# process fails to start (exit code 127).
$mingw = 'D:\scoop\apps\mingw\current\bin'
if (Test-Path $mingw) { $env:PATH = "$mingw;$env:PATH" }

Get-Process BxScript_GUI -ErrorAction SilentlyContinue | Stop-Process -Force
Start-Sleep -Milliseconds 500

$code = @"
using System;
using System.Runtime.InteropServices;
public class BxChk {
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern IntPtr FindWindow(string c, string n);
    [DllImport("user32.dll")] public static extern IntPtr GetWindow(IntPtr h, uint cmd);
    [DllImport("user32.dll", EntryPoint="GetWindowLongPtrW")] public static extern IntPtr GetWindowLongPtr(IntPtr h, int idx);
    [DllImport("user32.dll")] public static extern bool IsWindowEnabled(IntPtr h);
}
"@
Add-Type -TypeDefinition $code

$EX_TOOLWINDOW = 0x00000080
$EX_APPWINDOW  = 0x00040000
$NS = [NullString]::Value

$pass = 0; $fail = 0
function Assert-It { param($name, $ok)
    if ($ok) { Write-Host ("  [PASS] " + $name); $script:pass++ }
    else     { Write-Host ("  [FAIL] " + $name); $script:fail++ }
}

Write-Host "Launching regression.bx ..."
$proc = Start-Process -FilePath $EXE -ArgumentList "`"$SCRIPT`"" -PassThru -RedirectStandardOutput $LOG

# Timeline: modal opens at t=3500ms, closes at t=5000ms. Sample at 4200ms.
Start-Sleep -Milliseconds 4200

Write-Host ""
Write-Host "-- owner / style / modal (Win32 level) ----"

$main  = [BxChk]::FindWindow($NS, "TESTWIN_MAIN")
$modal = [BxChk]::FindWindow($NS, "TESTWIN_MODAL")

if ($main -eq [IntPtr]::Zero)  { Write-Host "  [FAIL] TESTWIN_MAIN not found"; exit 1 }
if ($modal -eq [IntPtr]::Zero) { Write-Host "  [FAIL] TESTWIN_MODAL not found (modal did not open?)"; exit 1 }

$owner   = [BxChk]::GetWindow($modal, 4)                    # GW_OWNER
$exModal = [int64][BxChk]::GetWindowLongPtr($modal, -20)    # GWL_EXSTYLE
$exMain  = [int64][BxChk]::GetWindowLongPtr($main,  -20)

Write-Host ("  modal.owner   = 0x{0:X}" -f [int64]$owner)
Write-Host ("  main          = 0x{0:X}" -f [int64]$main)
Write-Host ("  modal.exstyle = 0x{0:X}" -f $exModal)
Write-Host ""

Assert-It "owner: modal's owner IS the main window"   ($owner -eq $main)
Assert-It "owner: modal has WS_EX_TOOLWINDOW"         (($exModal -band $EX_TOOLWINDOW) -ne 0)
Assert-It "owner: modal lacks WS_EX_APPWINDOW"        (($exModal -band $EX_APPWINDOW) -eq 0)
Assert-It "owner: main still has WS_EX_APPWINDOW"     (($exMain -band $EX_APPWINDOW) -ne 0)
Assert-It "modal: main DISABLED while modal shown"    (-not [BxChk]::IsWindowEnabled($main))

# After the modal closes, main must be usable again
Start-Sleep -Milliseconds 2200
$main2 = [BxChk]::FindWindow($NS, "TESTWIN_MAIN")
$enabled = if ($main2 -eq [IntPtr]::Zero) { $false } else { [BxChk]::IsWindowEnabled($main2) }
Assert-It "modal: main ENABLED again after close"     $enabled

$proc.WaitForExit(30000) | Out-Null

Write-Host ""
Write-Host "-- full output of regression.bx --"
if (Test-Path $LOG) { Get-Content $LOG -Encoding UTF8 | ForEach-Object { Write-Host $_ } }

Write-Host ""
Write-Host "============================================"
Write-Host ("  Win32-level assertions: {0} passed, {1} failed" -f $pass, $fail)
Write-Host "============================================"
if ($fail -gt 0) { exit 1 }
