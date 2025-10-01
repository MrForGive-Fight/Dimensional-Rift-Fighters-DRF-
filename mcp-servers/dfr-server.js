const { MCPServer } = require("@modelcontextprotocol/sdk");
const fs = require("fs");
const path = require("path");

// Create MCP server instance
const server = new MCPServer({
  name: "DFR-Server",
  version: "1.0.0",
  description: "MCP Server for DFR Project"
});

// Define available tools
server.tool("list-prps", {
  description: "List all PRP files in the project",
  parameters: {},
  execute: async () => {
    const prpPath = path.join(__dirname, "..", "PRPs");
    const files = fs.readdirSync(prpPath).filter(f => f.endsWith(".md"));
    return { files };
  }
});

server.tool("read-prp", {
  description: "Read a specific PRP file",
  parameters: {
    filename: { type: "string", description: "Name of the PRP file" }
  },
  execute: async ({ filename }) => {
    const filePath = path.join(__dirname, "..", "PRPs", filename);
    const content = fs.readFileSync(filePath, "utf8");
    return { content };
  }
});

server.tool("get-project-status", {
  description: "Get current project status and structure",
  parameters: {},
  execute: async () => {
    const projectRoot = path.join(__dirname, "..");
    const structure = {
      src: fs.readdirSync(path.join(projectRoot, "src")),
      assets: fs.readdirSync(path.join(projectRoot, "assets")),
      docs: fs.readdirSync(path.join(projectRoot, "docs"))
    };
    return { structure };
  }
});

// Start the server
server.start();
console.log("DFR MCP Server started on stdio");
