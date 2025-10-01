# Claude Code 2.0.1 Launcher for DFR Project
# This script launches the NEW version of Claude Code

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  DFR Project - Claude Code Launcher" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$newClaudePath = "$env:USERPROFILE\.local\bin\claude.exe"
$projectPath = "C:\Rift\Server\DFR"

# Check if new version exists
if (Test-Path $newClaudePath) {
    Write-Host "✓ Found Claude Code 2.0.1" -ForegroundColor Green
    
    # Get version
    $version = & $newClaudePath --version 2>&1
    Write-Host "  Version: $version" -ForegroundColor Gray
    Write-Host ""
    
    # Navigate to project
    Set-Location $projectPath
    Write-Host "✓ Changed to project directory: $projectPath" -ForegroundColor Green
    Write-Host ""
    
    Write-Host "Launching Claude Code 2.0.1..." -ForegroundColor Yellow
    Write-Host ""
    
    # Launch Claude Code
    & $newClaudePath
    
} else {
    Write-Host "✗ Claude Code 2.0.1 not found at:" -ForegroundColor Red
    Write-Host "  $newClaudePath" -ForegroundColor Gray
    Write-Host ""
    Write-Host "Please reinstall using:" -ForegroundColor Yellow
    Write-Host "  curl -fsSL http://claude.ai/install.sh | bash" -ForegroundColor Cyan
}

Write-Host ""
Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
