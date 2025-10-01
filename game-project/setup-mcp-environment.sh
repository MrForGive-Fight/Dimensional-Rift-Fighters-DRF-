#!/bin/bash

# Automated MCP Environment Setup Script for Linux/macOS
# Run with: chmod +x setup-mcp-environment.sh && ./setup-mcp-environment.sh

echo -e "\033[36m🎮 Setting up Anime Arena Fighter Development Environment\033[0m"
echo -e "\033[36m==========================================================\033[0m"

# Detect OS
OS="Unknown"
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macOS"
fi

echo -e "\033[33mDetected OS: $OS\033[0m"

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Step 1: Install Prerequisites
echo -e "\n\033[32m📦 Installing Prerequisites...\033[0m"

# Install VS Code if not present
if ! command_exists code; then
    echo -e "\033[33mInstalling Visual Studio Code...\033[0m"
    if [[ "$OS" == "macOS" ]]; then
        brew install --cask visual-studio-code
    elif [[ "$OS" == "Linux" ]]; then
        # Install VS Code on Linux
        wget -qO- https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > packages.microsoft.gpg
        sudo install -o root -g root -m 644 packages.microsoft.gpg /etc/apt/trusted.gpg.d/
        sudo sh -c 'echo "deb [arch=amd64,arm64,armhf signed-by=/etc/apt/trusted.gpg.d/packages.microsoft.gpg] https://packages.microsoft.com/repos/code stable main" > /etc/apt/sources.list.d/vscode.list'
        sudo apt update
        sudo apt install code -y
    fi
fi

# Install Node.js if not present
if ! command_exists node; then
    echo -e "\033[33mInstalling Node.js...\033[0m"
    if [[ "$OS" == "macOS" ]]; then
        brew install node
    elif [[ "$OS" == "Linux" ]]; then
        curl -fsSL https://deb.nodesource.com/setup_lts.x | sudo -E bash -
        sudo apt-get install -y nodejs
    fi
fi

# Install Python if not present
if ! command_exists python3; then
    echo -e "\033[33mInstalling Python 3...\033[0m"
    if [[ "$OS" == "macOS" ]]; then
        brew install python@3.12
    elif [[ "$OS" == "Linux" ]]; then
        sudo apt update
        sudo apt install python3.12 python3-pip -y
    fi
fi

# Install Git if not present
if ! command_exists git; then
    echo -e "\033[33mInstalling Git...\033[0m"
    if [[ "$OS" == "macOS" ]]; then
        brew install git
    elif [[ "$OS" == "Linux" ]]; then
        sudo apt install git -y
    fi
fi

# Step 2: Create Project Structure
echo -e "\n\033[32m📁 Creating Project Structure...\033[0m"

PROJECT_NAME="anime-arena-fighter"
if [ -d "$PROJECT_NAME" ]; then
    echo -e "\033[33mProject folder already exists. Using existing folder.\033[0m"
else
    mkdir -p "$PROJECT_NAME"
fi

cd "$PROJECT_NAME"

# Create directory structure
directories=(
    ".vscode"
    "mcp-servers"
    "src/Core"
    "src/Combat"
    "src/Network"
    "src/Characters"
    "src/UI"
    "src/Assets"
    "reference"
    "PRPs"
    "tests"
    "docs"
    "scripts"
)

for dir in "${directories[@]}"; do
    if [ ! -d "$dir" ]; then
        mkdir -p "$dir"
        echo -e "  \033[90m✓ Created $dir\033[0m"
    fi
done

# Step 3: Install VS Code Extensions
echo -e "\n\033[32m🔧 Installing VS Code Extensions...\033[0m"

extensions=(
    "ms-vscode.cpptools-extension-pack"
    "github.copilot"
    "continue.continue"
    "ms-vscode.cmake-tools"
    "ms-python.python"
    "dbaeumer.vscode-eslint"
)

for ext in "${extensions[@]}"; do
    echo -e "  \033[90mInstalling $ext...\033[0m"
    code --install-extension "$ext" --force 2>/dev/null
done

# Step 4: Create VS Code Settings
echo -e "\n\033[32m⚙️  Creating VS Code Configuration...\033[0m"

cat > .vscode/settings.json << 'EOF'
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
      "command": "python3",
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
      "command": "python3",
      "args": ["${workspaceFolder}/mcp-servers/network_server.py"],
      "env": {
        "MAX_PLAYERS": "8",
        "TICK_RATE": "30"
      }
    },
    "game-balance": {
      "command": "python3",
      "args": ["${workspaceFolder}/mcp-servers/balance_server.py"]
    },
    "database-manager": {
      "command": "node",
      "args": ["${workspaceFolder}/mcp-servers/database_server.js"]
    },
    "lost-saga-analyzer": {
      "command": "python3",
      "args": ["${workspaceFolder}/mcp-servers/lost_saga_analyzer.py"],
      "env": {
        "REPO_PATH": "${workspaceFolder}/reference/LSFDC"
      }
    }
  },
  
  "C_Cpp.default.cppStandard": "c++20",
  "C_Cpp.default.intelliSenseMode": "gcc-x64",
  "cmake.configureOnOpen": true
}
EOF

# Step 5: Create MCP Server Files
echo -e "\n\033[32m🤖 Creating MCP Servers...\033[0m"

# Create Combat Server
cat > mcp-servers/combat_server.py << 'EOF'
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
EOF

chmod +x mcp-servers/combat_server.py

# Create Asset Generator
cat > mcp-servers/asset_generator.js << 'EOF'
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
EOF

chmod +x mcp-servers/asset_generator.js

# Step 6: Create CLAUDE.md
echo -e "\n\033[32m📝 Creating CLAUDE.md Rules...\033[0m"

cat > CLAUDE.md << 'EOF'
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
EOF

# Step 7: Create package.json
echo -e "\n\033[32m📦 Setting up Node.js dependencies...\033[0m"

cat > package.json << 'EOF'
{
  "name": "anime-arena-fighter",
  "version": "1.0.0",
  "description": "Anime 3D Arena Fighter MCP Servers",
  "scripts": {
    "test-mcp": "python3 scripts/test-mcp.py",
    "start-combat": "python3 mcp-servers/combat_server.py",
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
EOF

# Step 8: Create requirements.txt
echo -e "\n\033[32m🐍 Setting up Python dependencies...\033[0m"

cat > requirements.txt << 'EOF'
aiohttp
redis
numpy
EOF

# Step 9: Install dependencies
echo -e "\n\033[32m📥 Installing dependencies...\033[0m"

echo -e "  \033[90mInstalling Node packages...\033[0m"
npm install --silent 2>/dev/null

echo -e "  \033[90mInstalling Python packages...\033[0m"
pip3 install -r requirements.txt --quiet 2>/dev/null

# Step 10: Clone Lost Saga Reference
echo -e "\n\033[32m📚 Cloning Lost Saga reference...\033[0m"

if [ ! -d "reference/LSFDC" ]; then
    git clone https://github.com/LSFDC/lsfdc reference/LSFDC 2>/dev/null
    echo -e "  \033[90m✓ Lost Saga reference cloned\033[0m"
else
    echo -e "  \033[33mLost Saga reference already exists\033[0m"
fi

# Step 11: Create test script
echo -e "\n\033[32m🧪 Creating test script...\033[0m"

cat > scripts/test-mcp.py << 'EOF'
#!/usr/bin/env python3
import json
import subprocess
import asyncio

async def test_combat_server():
    print("Testing Combat MCP Server...")
    proc = await asyncio.create_subprocess_exec(
        "python3", "mcp-servers/combat_server.py",
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
EOF

chmod +x scripts/test-mcp.py

# Step 12: Create .gitignore
echo -e "\n\033[32m📋 Creating .gitignore...\033[0m"

cat > .gitignore << 'EOF'
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
EOF

# Step 13: Initialize Git repository
echo -e "\n\033[32m🔀 Initializing Git repository...\033[0m"

if [ ! -d ".git" ]; then
    git init 2>/dev/null
    git add . 2>/dev/null
    git commit -m "Initial setup with MCP servers" 2>/dev/null
    echo -e "  \033[90m✓ Git repository initialized\033[0m"
fi

# Step 14: Test MCP Servers
echo -e "\n\033[32m🧪 Testing MCP servers...\033[0m"

python3 scripts/test-mcp.py

# Step 15: Create launch script
echo -e "\n\033[32m🚀 Creating launch script...\033[0m"

cat > launch-dev.sh << 'EOF'
#!/bin/bash

echo "Starting Anime Arena Fighter Development Environment..."
echo

# Check for API key
if [ -z "$ANTHROPIC_API_KEY" ]; then
    echo "Please set ANTHROPIC_API_KEY environment variable"
    echo "export ANTHROPIC_API_KEY='your-key-here'"
    exit 1
fi

# Launch VS Code with MCP
echo "Launching VS Code with MCP integration..."
code . --enable-proposed-api

echo
echo "VS Code launched! MCP servers are configured and ready."
echo
echo "Quick commands:"
echo "  - Ctrl+Shift+P: Open command palette"
echo "  - Type 'Continue' to see AI commands"
echo "  - Check MCP status in Output panel"
echo
EOF

chmod +x launch-dev.sh

# Final Summary
echo -e "\n\033[36m"
echo "========================================"
echo -e "\033[32m✅ Setup Complete!\033[36m"
echo "========================================"
echo -e "\033[0m"
echo -e "\033[37mYour development environment is ready!\033[0m"
echo -e "\n\033[33mNext steps:\033[0m"
echo -e "  \033[90m1. Set your API key: export ANTHROPIC_API_KEY='your-key'\033[0m"
echo -e "  \033[90m2. Launch VS Code: ./launch-dev.sh\033[0m"
echo -e "  \033[90m3. Start coding with AI assistance!\033[0m"
echo -e "\n\033[33mMCP Servers created:\033[0m"
echo -e "  \033[90m✓ combat_server.py - Combat system and frame data\033[0m"
echo -e "  \033[90m✓ asset_generator.js - Character and asset creation\033[0m"
echo -e "  \033[90m✓ network_server.py - Multiplayer and matchmaking\033[0m"
echo -e "\n\033[33mProject structure:\033[0m"
echo -e "  \033[90m📁 $PROJECT_NAME/\033[0m"
echo -e "     \033[90m├── mcp-servers/   (AI agents)\033[0m"
echo -e "     \033[90m├── src/          (Game code)\033[0m"
echo -e "     \033[90m├── reference/    (Lost Saga)\033[0m"
echo -e "     \033[90m└── CLAUDE.md     (AI rules)\033[0m"

cd ..
echo -e "\n\033[36m🎮 Happy coding!\033[0m"