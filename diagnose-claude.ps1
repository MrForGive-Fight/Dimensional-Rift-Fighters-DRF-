# Claude Code Installation Diagnostics

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Claude Code Installation Check" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check for both versions
$newPath = "$env:USERPROFILE\.local\bin\claude.exe"
$oldPath = "B:\~BUN\root\claude.exe"

Write-Host "Checking NEW installation (2.0.1):" -ForegroundColor Yellow
if (Test-Path $newPath) {
    $newFile = Get-Item $newPath
    Write-Host "  ✓ EXISTS" -ForegroundColor Green
    Write-Host "  Location: $newPath" -ForegroundColor Gray
    Write-Host "  Size: $($newFile.Length) bytes" -ForegroundColor Gray
    Write-Host "  Modified: $($newFile.LastWriteTime)" -ForegroundColor Gray
    
    Write-Host "`n  Testing version directly:" -ForegroundColor Gray
    try {
        $version = & $newPath --version 2>&1
        Write-Host "  Version output: $version" -ForegroundColor Cyan
    } catch {
        Write-Host "  ERROR running --version: $_" -ForegroundColor Red
    }
} else {
    Write-Host "  ✗ NOT FOUND" -ForegroundColor Red
    Write-Host "  Expected at: $newPath" -ForegroundColor Gray
}

Write-Host "`n----------------------------------------`n" -ForegroundColor Gray

Write-Host "Checking OLD installation (1.0.58):" -ForegroundColor Yellow
if (Test-Path $oldPath) {
    $oldFile = Get-Item $oldPath
    Write-Host "  ✓ EXISTS (This is the problem!)" -ForegroundColor Red
    Write-Host "  Location: $oldPath" -ForegroundColor Gray
    Write-Host "  Size: $($oldFile.Length) bytes" -ForegroundColor Gray
    Write-Host "  Modified: $($oldFile.LastWriteTime)" -ForegroundColor Gray
} else {
    Write-Host "  ✗ NOT FOUND" -ForegroundColor Green
}

Write-Host "`n----------------------------------------`n" -ForegroundColor Gray

Write-Host "Current PATH priority:" -ForegroundColor Yellow
$pathEntries = $env:PATH -split ';'
$claudePathIndex = 0
foreach ($entry in $pathEntries) {
    $claudePathIndex++
    if ($entry -like "*claude*" -or $entry -like "*.local\bin*" -or $entry -like "*BUN*") {
        Write-Host "  [$claudePathIndex] $entry" -ForegroundColor Cyan
    }
}

Write-Host "`n----------------------------------------`n" -ForegroundColor Gray

Write-Host "Which 'claude' command runs by default:" -ForegroundColor Yellow
try {
    $whichClaude = Get-Command claude -ErrorAction SilentlyContinue
    if ($whichClaude) {
        Write-Host "  Location: $($whichClaude.Source)" -ForegroundColor Cyan
        $currentVersion = claude --version 2>&1
        Write-Host "  Version: $currentVersion" -ForegroundColor Cyan
    } else {
        Write-Host "  'claude' command not found in PATH" -ForegroundColor Yellow
    }
} catch {
    Write-Host "  Error: $_" -ForegroundColor Red
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "RECOMMENDATION:" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan

if (Test-Path $newPath) {
    Write-Host "The NEW version exists, but PATH is wrong." -ForegroundColor White
    Write-Host ""
    Write-Host "OPTION 1: Use launch-claude-FORCE.ps1" -ForegroundColor Green
    Write-Host "  This bypasses PATH completely" -ForegroundColor Gray
    Write-Host ""
    Write-Host "OPTION 2: Fix PATH permanently" -ForegroundColor Green
    Write-Host "  1. Win+X → System → Advanced → Env Variables" -ForegroundColor Gray
    Write-Host "  2. Edit 'Path' under User variables" -ForegroundColor Gray
    Write-Host "  3. Move '$env:USERPROFILE\.local\bin' to TOP" -ForegroundColor Gray
    Write-Host "  4. Restart PowerShell" -ForegroundColor Gray
    Write-Host ""
    Write-Host "OPTION 3: Delete old version" -ForegroundColor Green
    Write-Host "  Delete: $oldPath" -ForegroundColor Gray
} else {
    Write-Host "The NEW version was NOT installed correctly!" -ForegroundColor Red
    Write-Host "Run this command to reinstall:" -ForegroundColor Yellow
    Write-Host "  curl -fsSL http://claude.ai/install.sh | bash" -ForegroundColor Cyan
}

Write-Host "`n========================================`n" -ForegroundColor Cyan
pause
