---
description: "Remote calls, typed procedures, service registration, request dispatch, and faults"
---

- Implement a transport-specific client for remote procedure requests and replies:
  `include/Pt/Remoting/Client.h`
- Implement begin, synchronous, completion, or cancellation callbacks for a client transport:
  `include/Pt/Remoting/Client.h`
- Inspect the active client call or report whether its transport failed:
  `include/Pt/Remoting/Client.h`
- Define a typed, named remote procedure and its return and argument signature:
  `include/Pt/Remoting/RemoteProcedure.h`
- Invoke a typed remote procedure synchronously with call() or operator():
  `include/Pt/Remoting/RemoteProcedure.h`
- Begin an asynchronous remote procedure call and handle its finished signal:
  `include/Pt/Remoting/RemoteProcedure.h`
- Access a typed remote call Result, response value, or failure state:
  `include/Pt/Remoting/RemoteProcedure.h`
- Cancel an individual named remote call:
  `include/Pt/Remoting/RemoteProcedure.h`
- Define a named registry of remote service procedures:
  `include/Pt/Remoting/ServiceDefinition.h`
- Register a free function, member function, or callable as a synchronous service procedure:
  `include/Pt/Remoting/ServiceDefinition.h`
- Register a factory for an asynchronous service procedure:
  `include/Pt/Remoting/ServiceDefinition.h`
- Implement a transport-specific responder for request dispatch and result delivery:
  `include/Pt/Remoting/Responder.h`
- Select the requested service procedure, deserialize its arguments, and serialize its result:
  `include/Pt/Remoting/Responder.h`
- Cancel a dispatched request or inspect its active service procedure:
  `include/Pt/Remoting/Responder.h`
- Implement the base lifecycle for service procedure arguments, invocation, and results:
  `include/Pt/Remoting/ServiceProcedure.h`
- Define a factory that creates a service procedure for a responder:
  `include/Pt/Remoting/ServiceProcedure.h`
- Adapt callable functions and methods into synchronous service procedures:
  `include/Pt/Remoting/BasicProcedure.h`
- Implement an event-loop-driven asynchronous service procedure with onInvoke():
  `include/Pt/Remoting/ActiveProcedure.h`
- Set an asynchronous service procedure ready and return its result with onResult():
  `include/Pt/Remoting/ActiveProcedure.h`
- Read a deserialized asynchronous procedure argument by template index:
  `include/Pt/Remoting/ActiveProcedure.h`
- Manage serialized argument storage for synchronous or asynchronous service procedures:
  `include/Pt/Remoting/Arguments.h`
- Throw, report, or handle a remote call or service procedure fault:
  `include/Pt/Remoting/Fault.h`