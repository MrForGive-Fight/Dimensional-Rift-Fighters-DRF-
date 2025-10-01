# Claude Code Clean Reinstall Script
# This removes the old installation and installs fresh

Write-Host "========================================"
Write-Host "  Claude Code Clean Reinstall"
Write-Host "========================================"
Write-Host ""

$claudePath = "$env:USERPROFILE\.local\bin\claude.exe"
$backupPath = "$env:USERPROFILE\.local\bin\claude.exe.old"

# Step 1: Check current installation
Write-Host "Step 1: Checking current installation..." -ForegroundColor Yellow
if (Test-Path $claudePath) {
    $file = Get-Item $claudePath
    Write-Host "  Found existing installation"
    Write-Host "  Location: $claudePath"
    Write-Host "  Size: $($file.Length) bytes"
    Write-Host "  Created: $($file.CreationTime)"
    Write-Host ""
    
    # Try to get version
    Write-Host "  Testing current version..."
    try {
        $oldVersion = & $claudePath --version 2>&1
        Write-Host "  Version: $oldVersion"
    } catch {
        Write-Host "  Could not determine version"
    }
    Write-Host ""
    
    # Step 2: Backup
    Write-Host "Step 2: Backing up old installation..." -ForegroundColor Yellow
    try {
        Copy-Item $claudePath $backupPath -Force
        Write-Host "  Backed up successfully"
    } catch {
        Write-Host "  Backup failed, continuing anyway..."
    }
    Write-Host ""
    
    # Step 3: Remove
    Write-Host "Step 3: Removing old installation..." -ForegroundColor Yellow
    try {
        Remove-Item $claudePath -Force
        Write-Host "  Old version removed successfully" -ForegroundColor Green
    } catch {
        Write-Host "  ERROR: Could not delete file" -ForegroundColor Red
        Write-Host "  The file might be in use."
        Write-Host ""
        Write-Host "Please close all Claude Code windows and try again."
        Write-Host ""
        pause
        exit 1
    }
} else {
    Write-Host "  No existing installation found"
}

Write-Host ""
Write-Host "Step 4: Installing Claude Code..." -ForegroundColor Yellow
Write-Host "  Please run this command manually:"
Write-Host ""
Write-Host "  curl -fsSL http://claude.ai/install.sh | bash" -ForegroundColor Cyan
Write-Host ""
Write-Host "After running that command, verify with:"
Write-Host "  claude --version" -ForegroundColor Cyan
Write-Host ""
Write-Host "It should show version 2.0.1 or higher"
Write-Host ""
Write-Host "Then use: .\launch-claude-FORCE.ps1" -ForegroundColor Green
Write-Host ""
Write-Host "========================================"
pause
