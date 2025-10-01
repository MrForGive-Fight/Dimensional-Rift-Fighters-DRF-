# Claude Code Setup - DFR Project

## What I Fixed

### 1. Settings File Issue ✅
- **Backed up** your original settings to `.claude\settings.local.json.backup`
- **Simplified** the settings file to work with version 1.0.58
- Your custom game configuration is preserved in the backup

### 2. Version Problem ⚠️
You have TWO Claude Code installations:
- **OLD:** B:\~BUN\root\claude.exe (v1.0.58) ← Currently active
- **NEW:** %USERPROFILE%\.local\bin\claude.exe (v2.0.1) ← Better version

## How to Use the NEW Version

### Option 1: Quick Launch (Easiest)
Run the PowerShell script:
```powershell
.\launch-claude-new.ps1
```

### Option 2: Manual Launch
```powershell
cd C:\Rift\Server\DFR
$env:USERPROFILE\.local\bin\claude.exe
```

### Option 3: Fix PATH Permanently (Recommended)
1. Press `Win + X` → System
2. Advanced system settings → Environment Variables
3. Edit "Path" under User variables
4. Move `%USERPROFILE%\.local\bin` to the **TOP**
5. Restart PowerShell/CMD

### Option 4: Remove Old Version
Delete: `B:\~BUN\root\claude.exe`

## Restoring Your Custom Settings

Once you're using version 2.0.1, you can restore your game config:
```powershell
copy .claude\settings.local.json.backup .claude\settings.local.json
```

Your custom settings include:
- DFR game rules (mana, characters, balance)
- C++ coding conventions
- LSFDC framework patterns
- Custom commands for character generation
- MCP servers configuration

## Current Status

✅ Settings file fixed - no more errors
✅ Backup created of your game configuration
✅ Launch script created
⚠️ Still using old version (1.0.58) until PATH is fixed

## Questions?

Type `/help` in Claude Code for commands
Type `/doctor` to check installation status
