applyTo: "include/Pt/Mcp/**,src/Pt-Mcp/**"
description: "Use when implementing an MCP server with Pt, including tool declarations, tool parameter schemas, result content, HTTP transport, or standard-I/O transport."
---

- Define MCP tool parameter schemas with primitive, object, array, enum, and nullable types:
  `include/Pt/Mcp/Type.h`
- Declare MCP servers and tools, configure parameters, result content, and protocol capabilities:
  `include/Pt/Mcp/ToolDeclaration.h`
- Format MCP tool results as text or image content blocks and provide custom content formatters:
  `include/Pt/Mcp/ContentType.h`
- Serve MCP tool declarations through an HTTP service:
  `include/Pt/Mcp/HttpService.h`
- Dispatch Content-Length-framed MCP requests synchronously through standard streams:
  `include/Pt/Mcp/Service.h`
- Run MCP services over standard I/O with an event loop for asynchronous procedures:
  `include/Pt/Mcp/StdioService.h`