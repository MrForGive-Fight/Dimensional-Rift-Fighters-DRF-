# SIMPLE FIX - Follow These Steps

## Step 1: Delete the Old File

Run this command:
```powershell
Remove-Item "$env:USERPROFILE\.local\bin\claude.exe" -Force
```

If it says "file in use", close all Claude Code windows first.

## Step 2: Install Fresh Version

Run this command:
```bash
curl -fsSL http://claude.ai/install.sh | bash
```

Wait for it to complete. It should say "Claude Code successfully installed!"

## Step 3: Verify Installation

Run this command:
```powershell
& "$env:USERPROFILE\.local\bin\claude.exe" --version
```

It should show version **2.0.1** or higher.

## Step 4: Launch Claude Code

Run this command:
```powershell
cd C:\Rift\Server\DFR
.\launch-claude-FORCE.ps1
```

## If Step 1 Fails (File in Use)

1. Close all Claude Code windows
2. Close all PowerShell/CMD windows except this one
3. Try Step 1 again

## Alternative: Just Delete Manually

1. Open File Explorer
2. Navigate to: `C:\Users\ibeth\.local\bin`
3. Delete `claude.exe`
4. Then do Step 2, 3, and 4 above

---

## Quick Copy-Paste Commands

```powershell
# Delete old
Remove-Item "$env:USERPROFILE\.local\bin\claude.exe" -Force

# Install new
curl -fsSL http://claude.ai/install.sh | bash

# Verify
& "$env:USERPROFILE\.local\bin\claude.exe" --version

# Launch
cd C:\Rift\Server\DFR
.\launch-claude-FORCE.ps1
```

That's it! 🎉
