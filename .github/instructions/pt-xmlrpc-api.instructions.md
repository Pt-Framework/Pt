---
description: "XML-RPC Clients, Procedures, and HTTP Services"
---

- Implement XML-RPC remote procedure calls and process method results:
  `include/Pt/XmlRpc/Client.h`
- Represent XML-RPC fault codes and serialize fault responses:
  `include/Pt/XmlRpc/Fault.h`
- Format and parse XML-RPC method calls, values, and responses:
  `include/Pt/XmlRpc/Formatter.h`
- Call XML-RPC procedures over HTTP, configure endpoints, TLS, or timeouts:
  `include/Pt/XmlRpc/HttpClient.h`
- Expose an XML-RPC service definition through HTTP:
  `include/Pt/XmlRpc/HttpService.h`
- Dispatch XML-RPC requests and format procedure or fault responses:
  `include/Pt/XmlRpc/Responder.h`