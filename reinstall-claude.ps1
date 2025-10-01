# Claude Code Clean Reinstall Script
# This removes the old installation and installs fresh

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Claude Code Clean Reinstall" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$claudePath = "$env:USERPROFILE\.local\bin\claude.exe"
$backupPath = "$env:USERPROFILE\.local\bin\claude.exe.old"

# Step 1: Check current installation
Write-Host "Step 1: Checking current installation..." -ForegroundColor Yellow
if (Test-Path $claudePath) {
    $file = Get-Item $claudePath
    Write-Host "  Found existing installation:" -ForegroundColor Gray
    Write-Host "  Location: $claudePath" -ForegroundColor Gray
    Write-Host "  Size: $($file.Length) bytes" -ForegroundColor Gray
    Write-Host "  Created: $($file.CreationTime)" -ForegroundColor Gray
    Write-Host "  Modified: $($file.LastWriteTime)" -ForegroundColor Gray
    Write-Host ""
    
    # Try to get version
    Write-Host "  Testing current version..." -ForegroundColor Gray
    try {
        $oldVersion = & $claudePath --version 2>&1
        Write-Host "  Version: $oldVersion" -ForegroundColor Cyan
    } catch {
        Write-Host "  Could not determine version" -ForegroundColor Red
    }
    Write-Host ""
    
    # Step 2: Backup and remove
    Write-Host "Step 2: Backing up old installation..." -ForegroundColor Yellow
    try {
        Copy-Item $claudePath $backupPath -Force
        Write-Host "  ✓ Backed up to: $backupPath" -ForegroundColor Green
    } catch {
        Write-Host "  ✗ Backup failed: $_" -ForegroundColor Red
        Write-Host "  Continuing anyway..." -ForegroundColor Yellow
    }
    Write-Host ""
    
    Write-Host "Step 3: Removing old installation..." -ForegroundColor Yellow
    try {
        Remove-Item $claudePath -Force
        Write-Host "  ✓ Old version removed" -ForegroundColor Green
    } catch {
        Write-Host "  ✗ Could not delete: $_" -ForegroundColor Red
        Write-Host ""
        Write-Host "The file might be in use. Please:" -ForegroundColor Yellow
        Write-Host "  1. Close all Claude Code windows" -ForegroundColor Gray
        Write-Host "  2. Close all PowerShell/CMD windows" -ForegroundColor Gray
        Write-Host "  3. Run this script again" -ForegroundColor Gray
        Write-Host ""
        pause
        exit 1
    }
} else {
    Write-Host "  No existing installation found" -ForegroundColor Gray
}

Write-Host ""
Write-Host "Step 4: Installing Claude Code 2.0.1..." -ForegroundColor Yellow
Write-Host "  Running official installer..." -ForegroundColor Gray
Write-Host ""

# Run the install script
try {
    $installScript = Invoke-WebRequest -Uri "http://claude.ai/install.sh" -UseBasicParsing
    $installScript.Content | bash
    
    Write-Host ""
    Write-Host "Step 5: Verifying installation..." -ForegroundColor Yellow
    
    if (Test-Path $claudePath) {
        $newFile = Get-Item $claudePath
        Write-Host "  ✓ Installation successful!" -ForegroundColor Green
        Write-Host "  Location: $claudePath" -ForegroundColor Gray
        Write-Host "  Size: $($newFile.Length) bytes" -ForegroundColor Gray
        Write-Host "  Created: $($newFile.CreationTime)" -ForegroundColor Gray
        Write-Host ""
        
        # Verify version
        $newVersion = & $claudePath --version 2>&1
        Write-Host "  New version: $newVersion" -ForegroundColor Cyan
        
        if ($newVersion -like "*2.0*") {
            Write-Host ""
            Write-Host "========================================" -ForegroundColor Green
            Write-Host "  SUCCESS! Claude Code 2.0.1 installed!" -ForegroundColor Green
            Write-Host "========================================" -ForegroundColor Green
            Write-Host ""
            Write-Host "You can now use:" -ForegroundColor White
            Write-Host "  .\launch-claude-FORCE.ps1" -ForegroundColor Cyan
            Write-Host ""
        } else {
            Write-Host ""
            Write-Host "⚠ Warning: Version might not be 2.0.1" -ForegroundColor Yellow
            Write-Host "  Try running the install manually:" -ForegroundColor Gray
            Write-Host "  curl -fsSL http://claude.ai/install.sh | bash" -ForegroundColor Cyan
        }
    } else {
        Write-Host "  ✗ Installation failed - file not created" -ForegroundColor Red
    }
    
} catch {
    Write-Host "  ✗ Installation error: $_" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please install manually:" -ForegroundColor Yellow
    Write-Host "  curl -fsSL http://claude.ai/install.sh | bash" -ForegroundColor Cyan
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
pause
