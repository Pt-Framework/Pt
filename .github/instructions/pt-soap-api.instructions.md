---
description: "SOAP Clients, Procedures, and HTTP Services"
---

- Invoke SOAP operations and process their results:
  `include/Pt/Soap/Client.h`
- Represent SOAP fault codes and serialize fault responses:
  `include/Pt/Soap/Fault.h`
- Format and parse SOAP envelopes, values, and operation parameters:
  `include/Pt/Soap/Formatter.h`
- Call SOAP operations over HTTP, configure endpoints, TLS, or timeouts:
  `include/Pt/Soap/HttpClient.h`
- Define SOAP schema types, parameters, operations, namespaces, and WSDL output:
  `include/Pt/Soap/ServiceDeclaration.h`
- Bind a SOAP service declaration to a remoting service definition:
  `include/Pt/Soap/ServiceDefinition.h`
- Dispatch SOAP requests and format operation or fault responses:
  `include/Pt/Soap/Responder.h`
- Bridge SOAP request handling to HTTP responder lifecycle events:
  `include/Pt/Soap/HttpResponder.h`
- Expose a SOAP service definition through HTTP:
  `include/Pt/Soap/HttpService.h`