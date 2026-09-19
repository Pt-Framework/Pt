---
applyTo: "include/Pt/Mcp/**,src/Pt-Mcp/**"
description: "Use when implementing an MCP server with Pt, including tool declarations, tool parameter schemas, result content, HTTP transport, or standard-I/O transport."
---

- Module groups and page:
  `include/Pt/Mcp/Api-Mcp.h`, `doc/pages/pt-mcp.md`
- Define MCP tool parameter schemas with primitive, object, array, enum, and nullable types:
  `include/Pt/Mcp/Type.h`, `include/Pt/Mcp/Api-Types.h`
- Declare MCP servers and tools, configure parameters, result content, and protocol capabilities:
  `include/Pt/Mcp/ToolDeclaration.h`, `include/Pt/Mcp/Api-Tools.h`
- Format MCP tool results as text or image content blocks and provide custom content formatters:
  `include/Pt/Mcp/ContentType.h`, `include/Pt/Mcp/Api-Content.h`
- Serve MCP tool declarations through an HTTP service:
  `include/Pt/Mcp/HttpService.h`, `include/Pt/Mcp/Api-Http.h`
- Dispatch Content-Length-framed MCP requests synchronously through standard streams:
  `include/Pt/Mcp/Service.h`, `include/Pt/Mcp/Api-Stdio.h`
- Run MCP services over standard I/O with an event loop for asynchronous procedures:
  `include/Pt/Mcp/StdioService.h`, `include/Pt/Mcp/Api-Stdio.h`
