---
description: "HTTP Servers and Services"
---

- Create HTTP servers, listen on endpoints, configure timeouts, HTTPS and threading:
  `include/Pt/Http/Server.h`
- Handle request lifecycles, read request bodies and send replies:
  `include/Pt/Http/Responder.h`
- Implement services that create and destroy request responders:
  `include/Pt/Http/Service.h`
- Route requests to services, map request types and attach authorizers:
  `include/Pt/Http/Servlet.h`
- Authorize HTTP server replies with an asynchronous lifecycle:
  `include/Pt/Http/Authorizer.h`