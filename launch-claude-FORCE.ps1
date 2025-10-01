# Force Launch Claude Code 2.0.1 - Bypass PATH
# This script absolutely ensures we run the NEW version

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Claude Code 2.0.1 Force Launcher" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$newClaudePath = "$env:USERPROFILE\.local\bin\claude.exe"
$projectPath = "C:\Rift\Server\DFR"

# Verify the file exists
if (-not (Test-Path $newClaudePath)) {
    Write-Host "ERROR: Claude 2.0.1 not found!" -ForegroundColor Red
    Write-Host "Expected location: $newClaudePath" -ForegroundColor Gray
    Write-Host ""
    Write-Host "The installation might have failed." -ForegroundColor Yellow
    Write-Host "Try reinstalling:" -ForegroundColor Yellow
    Write-Host "  curl -fsSL http://claude.ai/install.sh | bash" -ForegroundColor Cyan
    Write-Host ""
    pause
    exit 1
}

Write-Host "Found: $newClaudePath" -ForegroundColor Green

# Get actual file info
$fileInfo = Get-Item $newClaudePath
Write-Host "File size: $($fileInfo.Length) bytes" -ForegroundColor Gray
Write-Host "Modified: $($fileInfo.LastWriteTime)" -ForegroundColor Gray
Write-Host ""

# Navigate to project
Set-Location $projectPath
Write-Host "Working directory: $projectPath" -ForegroundColor Green
Write-Host ""

# CRITICAL: Clear PATH to prevent interference
$env:PATH = "$env:USERPROFILE\.local\bin;$env:SystemRoot\system32;$env:SystemRoot"

Write-Host "Launching Claude Code 2.0.1 directly..." -ForegroundColor Yellow
Write-Host "Using: $newClaudePath" -ForegroundColor Gray
Write-Host ""

# Launch with full path
& "$newClaudePath"

Write-Host ""
Write-Host "Claude Code closed." -ForegroundColor Gray
