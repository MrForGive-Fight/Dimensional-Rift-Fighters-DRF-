# DFR MCP Server Setup

## Overview
This MCP (Model Context Protocol) server provides Claude with direct access to your DFR project structure and files.

## Installation

1. **Install Node.js dependencies:**
   ```bash
   cd mcp-servers
   npm install
   ```

2. **Configure Claude Desktop:**
   - Open Claude Desktop settings
   - Go to Developer > Model Context Protocol
   - Add the configuration from `claude-config.json`

## Available Tools

The MCP server provides the following tools to Claude:

- **list-prps**: Lists all PRP (Project Reference Protocol) files
- **read-prp**: Reads the content of a specific PRP file
- **get-project-status**: Gets the current project structure and status

## Testing the Server

To test the server manually:
```bash
cd mcp-servers
node dfr-server.js
```

## Integration with VS Code

When using Claude Code in VS Code:
1. The MCP server will automatically start when you open the project
2. Claude will have access to your project files through the defined tools
3. Use `/init` command in Claude Code to initialize the integration

## Project Structure
```
DFR/
├── mcp-servers/
│   ├── dfr-server.js      # Main MCP server implementation
│   └── package.json        # Node.js dependencies
├── PRPs/                   # Project Reference Protocols
├── src/                    # Source code
├── assets/                 # Game assets
└── claude-config.json      # Claude Desktop configuration
```

## Troubleshooting

- If the server doesn't start, ensure Node.js is in your PATH
- Check that all dependencies are installed with `npm install`
- Verify the paths in claude-config.json match your actual file locations
