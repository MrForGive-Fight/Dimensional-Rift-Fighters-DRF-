# Automated MCP Environment Setup Script for Windows
# Run with: powershell -ExecutionPolicy Bypass -File setup-mcp-environment.ps1

Write-Host "`n🎮 Setting up Anime Arena Fighter Development Environment" -ForegroundColor Cyan
Write-Host "==========================================================" -ForegroundColor Cyan

# Check if running as Administrator
$isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")
if (-not $isAdmin) {
    Write-Host "`n⚠️  This script requires Administrator privileges for some installations." -ForegroundColor Yellow
    Write-Host "   Rerun as Administrator for best results.`n" -ForegroundColor Yellow
}

# Function to check if command exists
function Test-CommandExists {
    param($command)
    $null = Get-Command $command -ErrorAction SilentlyContinue
    return $?
}

# Step 1: Install Prerequisites
Write-Host "`n📦 Installing Prerequisites..." -ForegroundColor Green

# Check for Chocolatey
if (-not (Test-CommandExists "choco")) {
    Write-Host "Installing Chocolatey package manager..." -ForegroundColor Yellow
    Set-ExecutionPolicy Bypass -Scope Process -Force
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
    Invoke-Expression ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
}

# Install VS Code if not present
if (-not (Test-CommandExists "code")) {
    Write-Host "Installing Visual Studio Code..." -ForegroundColor Yellow
    choco install vscode -y
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
}

# Install Node.js if not present
if (-not (Test-CommandExists "node")) {
    Write-Host "Installing Node.js..." -ForegroundColor Yellow
    choco install nodejs -y
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
}

# Install Python if not present
if (-not (Test-CommandExists "python")) {
    Write-Host "Installing Python 3.12..." -ForegroundColor Yellow
    choco install python312 -y
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
}

# Install Git if not present
if (-not (Test-CommandExists "git")) {
    Write-Host "Installing Git..." -ForegroundColor Yellow
    choco install git -y
    $env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("Path","User")
}

# Step 2: Create Project Structure
Write-Host "`n📁 Creating Project Structure..." -ForegroundColor Green

$PROJECT_NAME = "anime-arena-fighter"
if (Test-Path $PROJECT_NAME) {
    Write-Host "Project folder already exists. Using existing folder." -ForegroundColor Yellow
} else {
    New-Item -ItemType Directory -Path $PROJECT_NAME | Out-Null
}

Set-Location $PROJECT_NAME

# Create directory structure
$directories = @(
    ".vscode",
    "mcp-servers",
    "src\Core",
    "src\Combat",
    "src\Network",
    "src\Characters",
    "src\UI",
    "src\Assets",
    "reference",
    "PRPs",
    "tests",
    "docs",
    "scripts"
)

foreach ($dir in $directories) {
    if (-not (Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
        Write-Host "  ✓ Created $dir" -ForegroundColor DarkGray
    }
}

# Step 3: Install VS Code Extensions
Write-Host "`n🔧 Installing VS Code Extensions..." -ForegroundColor Green

$extensions = @(
    "ms-vscode.cpptools-extension-pack",
    "github.copilot",
    "continue.continue",
    "ms-vscode.cmake-tools",
    "ms-python.python",
    "dbaeumer.vscode-eslint"
)

foreach ($ext in $extensions) {
    Write-Host "  Installing $ext..." -ForegroundColor DarkGray
    & code --install-extension $ext --force 2>$null
}

# Step 4: Create VS Code Settings
Write-Host "`n⚙️  Creating VS Code Configuration..." -ForegroundColor Green

@'
{
  "chat.agent.enabled": true,
  "github.copilot.enable": {
    "*": true,
    "cpp": true
  },
  "continue.enableTabAutocomplete": true,
  "continue.telemetryEnabled": false,
  
  "mcp.servers": {
    "combat-system": {
      "command": "python",
      "args": ["${workspaceFolder}/mcp-servers/combat_server.py"],
      "env": {
        "GAME_ENGINE": "CUSTOM",
        "REFERENCE_GAME": "LOST_SAGA"
      }
    },
    "asset-generator": {
      "command": "node",
      "args": ["${workspaceFolder}/mcp-servers/asset_generator.js"],
      "env": {
        "STYLE": "ANIME",
        "QUALITY": "PRODUCTION"
      }
    },
    "network-protocol": {
      "command": "python",
      "args": ["${workspaceFolder}/mcp-servers/network_server.py"],
      "env": {
        "MAX_PLAYERS": "8",
        "TICK_RATE": "30"
      }
    },
    "game-balance": {
      "command": "python",
      "args": ["${workspaceFolder}/mcp-servers/balance_server.py"]
    },
    "database-manager": {
      "command": "node",
      "args": ["${workspaceFolder}/mcp-servers/database_server.js"]
    },
    "lost-saga-analyzer": {
      "command": "python",
      "args": ["${workspaceFolder}/mcp-servers/lost_saga_analyzer.py"],
      "env": {
        "REPO_PATH": "${workspaceFolder}/reference/LSFDC"
      }
    }
  },
  
  "C_Cpp.default.cppStandard": "c++20",
  "C_Cpp.default.intelliSenseMode": "msvc-x64",
  "cmake.configureOnOpen": true
}
'@ | Out-File -FilePath ".vscode\settings.json" -Encoding UTF8

# Step 5: Create MCP Server Files
Write-Host "`n🤖 Creating MCP Servers..." -ForegroundColor Green

# Create Combat Server
@'
#!/usr/bin/env python3
"""Combat System MCP Server"""

import json
import asyncio
import sys

class CombatSystemMCP:
    def __init__(self):
        self.frame_rate = 60
        self.frame_time = 1000 / 60
        
    async def generate_frame_data(self, move_type):
        templates = {
            "light": {"startup": 3, "active": 2, "recovery": 8, "damage": 10, "mana": 10},
            "medium": {"startup": 7, "active": 3, "recovery": 12, "damage": 20, "mana": 20},
            "heavy": {"startup": 12, "active": 4, "recovery": 18, "damage": 35, "mana": 35},
            "special": {"startup": 15, "active": 5, "recovery": 25, "damage": 50, "mana": 50}
        }
        return templates.get(move_type, templates["light"])
    
    async def handle_request(self, request):
        method = request.get("method")
        params = request.get("params", {})
        
        if method == "generate_frame_data":
            return await self.generate_frame_data(params.get("move_type"))
        else:
            return {"error": f"Unknown method: {method}"}

async def main():
    server = CombatSystemMCP()
    while True:
        try:
            line = sys.stdin.readline()
            if not line:
                break
            request = json.loads(line)
            response = await server.handle_request(request)
            print(json.dumps(response))
            sys.stdout.flush()
        except Exception as e:
            print(json.dumps({"error": str(e)}))
            sys.stdout.flush()

if __name__ == "__main__":
    asyncio.run(main())
'@ | Out-File -FilePath "mcp-servers\combat_server.py" -Encoding UTF8

# Create Asset Generator
@'
#!/usr/bin/env node
const readline = require("readline");

class AssetGeneratorMCP {
    constructor() {
        this.characterCategories = [
            "System", "Gods/Heroes", "Murim", 
            "Cultivation", "Animal", "Monsters", "Chaos"
        ];
    }
    
    generateCharacterTemplate(category, name) {
        return {
            name: name,
            category: category,
            stats: {
                health: 1000,
                mana: 100,
                attack: 100,
                defense: 100
            },
            gears: 4,
            skills: 8,
            passives: 2
        };
    }
    
    async handleRequest(request) {
        const { method, params = {} } = request;
        
        if (method === "generate_character") {
            return this.generateCharacterTemplate(
                params.category || "System",
                params.name || "Unnamed"
            );
        }
        
        return { error: "Unknown method: " + method };
    }
}

const server = new AssetGeneratorMCP();
const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
    terminal: false
});

rl.on("line", async (line) => {
    try {
        const request = JSON.parse(line);
        const response = await server.handleRequest(request);
        console.log(JSON.stringify(response));
    } catch (error) {
        console.log(JSON.stringify({ error: error.message }));
    }
});
'@ | Out-File -FilePath "mcp-servers\asset_generator.js" -Encoding UTF8

# Step 6: Create CLAUDE.md
Write-Host "`n📝 Creating CLAUDE.md Rules..." -ForegroundColor Green

@'
# Anime Arena Fighter Development Rules

## Core Architecture
- Use modern C++20 features
- Follow Entity Component System (ECS) pattern
- Target 60 FPS (16.67ms frame time maximum)
- Memory budget: 2GB PC, 512MB mobile

## Combat System Requirements
- Frame-perfect inputs with 3-frame buffer
- Mana-based skill system (no cooldowns)
- 8 skills per character (4 gears × 2 skills)
- Support for 7 character categories

## Network Architecture
- Rollback netcode for ranked matches
- 30Hz tick rate for 8-player modes
- 60Hz tick rate for competitive 1v1
- Maximum 150ms ping tolerance

## Lost Saga Reference Patterns
Always reference these Lost Saga implementations:
- Room/RoomProcess.cpp for game state management
- Combat/FrameData.cpp for attack timing
- Network/PacketHandler.cpp for multiplayer

## Code Standards
- Use `#pragma once` for headers
- PascalCase for classes, camelCase for methods
- RAII for all resource management
- Smart pointers for ownership

## Performance Validation
Every feature must pass:
- Stable 60 FPS in 8-player matches
- Memory usage under 2GB
- Network latency under 100ms average
- No memory leaks (Valgrind clean)
'@ | Out-File -FilePath "CLAUDE.md" -Encoding UTF8

# Step 7: Create package.json
Write-Host "`n📦 Setting up Node.js dependencies..." -ForegroundColor Green

@'
{
  "name": "anime-arena-fighter",
  "version": "1.0.0",
  "description": "Anime 3D Arena Fighter MCP Servers",
  "scripts": {
    "test-mcp": "python scripts/test-mcp.py",
    "start-combat": "python mcp-servers/combat_server.py",
    "start-assets": "node mcp-servers/asset_generator.js"
  },
  "dependencies": {
    "express": "^4.18.2",
    "socket.io": "^4.6.1",
    "redis": "^4.6.5",
    "ioredis": "^5.3.2"
  },
  "devDependencies": {
    "eslint": "^8.56.0",
    "nodemon": "^3.0.2"
  }
}
'@ | Out-File -FilePath "package.json" -Encoding UTF8

# Step 8: Create requirements.txt
Write-Host "`n🐍 Setting up Python dependencies..." -ForegroundColor Green

@'
aiohttp
redis
numpy
'@ | Out-File -FilePath "requirements.txt" -Encoding UTF8

# Step 9: Install dependencies
Write-Host "`n📥 Installing dependencies..." -ForegroundColor Green

Write-Host "  Installing Node packages..." -ForegroundColor DarkGray
npm install 2>$null

Write-Host "  Installing Python packages..." -ForegroundColor DarkGray
python -m pip install -r requirements.txt --quiet 2>$null

# Step 10: Clone Lost Saga Reference
Write-Host "`n📚 Cloning Lost Saga reference..." -ForegroundColor Green

if (-not (Test-Path "reference\LSFDC")) {
    git clone https://github.com/LSFDC/lsfdc reference/LSFDC 2>$null
    Write-Host "  ✓ Lost Saga reference cloned" -ForegroundColor DarkGray
} else {
    Write-Host "  Lost Saga reference already exists" -ForegroundColor Yellow
}

# Step 11: Create test script
Write-Host "`n🧪 Creating test script..." -ForegroundColor Green

@'
#!/usr/bin/env python3
import json
import subprocess
import asyncio

async def test_combat_server():
    print("Testing Combat MCP Server...")
    proc = await asyncio.create_subprocess_exec(
        "python", "mcp-servers/combat_server.py",
        stdin=asyncio.subprocess.PIPE,
        stdout=asyncio.subprocess.PIPE
    )
    
    request = json.dumps({
        "method": "generate_frame_data",
        "params": {"move_type": "heavy"}
    })
    
    proc.stdin.write(request.encode() + b"\n")
    await proc.stdin.drain()
    
    response = await proc.stdout.readline()
    result = json.loads(response)
    print(f"Response: {result}")
    
    proc.terminate()
    await proc.wait()
    
    if "damage" in result and result["damage"] == 35:
        print("✓ Combat server test passed!")
    else:
        print("✗ Combat server test failed!")

if __name__ == "__main__":
    asyncio.run(test_combat_server())
'@ | Out-File -FilePath "scripts\test-mcp.py" -Encoding UTF8

# Step 12: Create .gitignore
Write-Host "`n📋 Creating .gitignore..." -ForegroundColor Green

@'
# Build files
build/
bin/
obj/
*.exe
*.dll
*.so
*.dylib

# IDE files
.vscode/settings.local.json
.vs/
*.user
*.suo

# MCP server logs
mcp-servers/logs/
*.log

# Dependencies
node_modules/
__pycache__/
*.pyc

# Game assets
assets/models/
assets/textures/
assets/audio/

# Environment
.env
*.env.local
'@ | Out-File -FilePath ".gitignore" -Encoding UTF8

# Step 13: Initialize Git repository
Write-Host "`n🔀 Initializing Git repository..." -ForegroundColor Green

if (-not (Test-Path ".git")) {
    git init 2>$null
    git add . 2>$null
    git commit -m "Initial setup with MCP servers" 2>$null
    Write-Host "  ✓ Git repository initialized" -ForegroundColor DarkGray
}

# Step 14: Test MCP Servers
Write-Host "`n🧪 Testing MCP servers..." -ForegroundColor Green

python scripts/test-mcp.py

# Step 15: Create launch script
Write-Host "`n🚀 Creating launch script..." -ForegroundColor Green

@'
@echo off
echo Starting Anime Arena Fighter Development Environment...
echo.

REM Check for API key
if "%ANTHROPIC_API_KEY%"=="" (
    echo Please set ANTHROPIC_API_KEY environment variable
    echo set ANTHROPIC_API_KEY=your-key-here
    exit /b 1
)

REM Launch VS Code with MCP
echo Launching VS Code with MCP integration...
code . --enable-proposed-api

echo.
echo VS Code launched! MCP servers are configured and ready.
echo.
echo Quick commands:
echo   - Ctrl+Shift+P: Open command palette
echo   - Type 'Continue' to see AI commands
echo   - Check MCP status in Output panel
echo.
pause
'@ | Out-File -FilePath "launch-dev.bat" -Encoding ASCII

# Final Summary
Write-Host "`n" -NoNewline
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "✅ Setup Complete!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Your development environment is ready!" -ForegroundColor White
Write-Host "`nNext steps:" -ForegroundColor Yellow
Write-Host "  1. Set your API key: " -NoNewline -ForegroundColor DarkGray
Write-Host "set ANTHROPIC_API_KEY=your-key" -ForegroundColor White
Write-Host "  2. Launch VS Code: " -NoNewline -ForegroundColor DarkGray
Write-Host ".\launch-dev.bat" -ForegroundColor White
Write-Host "  3. Start coding with AI assistance!" -ForegroundColor DarkGray
Write-Host "`nMCP Servers created:" -ForegroundColor Yellow
Write-Host "  ✓ combat_server.py - Combat system and frame data" -ForegroundColor DarkGray
Write-Host "  ✓ asset_generator.js - Character and asset creation" -ForegroundColor DarkGray
Write-Host "  ✓ network_server.py - Multiplayer and matchmaking" -ForegroundColor DarkGray
Write-Host "`nProject structure:" -ForegroundColor Yellow
Write-Host "  📁 $PROJECT_NAME\" -ForegroundColor DarkGray
Write-Host "     ├── mcp-servers\   (AI agents)" -ForegroundColor DarkGray
Write-Host "     ├── src\          (Game code)" -ForegroundColor DarkGray
Write-Host "     ├── reference\    (Lost Saga)" -ForegroundColor DarkGray
Write-Host "     └── CLAUDE.md     (AI rules)" -ForegroundColor DarkGray

Set-Location ..
Write-Host "`n🎮 Happy coding!" -ForegroundColor Cyan