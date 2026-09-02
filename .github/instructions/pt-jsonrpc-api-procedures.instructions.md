---
description: "JSON-RPC 2.0 Clients, Services, and Procedures"
---

- Call JSON-RPC 2.0 remote procedures, handle call failures, and configure named parameters:
  `include/Pt/JsonRpc/Client.h`
- Invoke JSON-RPC procedures over HTTP, configure targets, SSL, timeouts, and keep-alive:
  `include/Pt/JsonRpc/HttpClient.h`
- Implement JSON-RPC request dispatch, incremental request parsing, result formatting, and faults:
  `include/Pt/JsonRpc/Responder.h`
- Expose a JSON-RPC service through the HTTP service API:
  `include/Pt/JsonRpc/HttpService.h`
- Serialize JSON-RPC parameters and results, and incrementally deserialize JSON nodes:
  `include/Pt/JsonRpc/Formatter.h`
- Represent JSON-RPC protocol error codes and fault exceptions:
  `include/Pt/JsonRpc/Fault.h`
- Map JSON-RPC named parameters to positional procedure arguments:
  `include/Pt/JsonRpc/ProcedureDeclaration.h`
- Associate named-parameter declarations with service procedure names:
  `include/Pt/JsonRpc/ServiceDeclaration.h`