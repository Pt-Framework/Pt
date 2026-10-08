---
description: "HTTP WebSocket"
---

- HTTP module group, messages, clients, servers and WebSocket:
  `include/Pt/Http/Api-Http.h`
- WebSocket group, handshake, frames and message I/O:
  `include/Pt/Http/Api-WebSocket.h`
- Client handshake group:
  `include/Pt/Http/Api-WebSocketClient.h`
- Server session group:
  `include/Pt/Http/Api-WebSocketServer.h`
- Payload of one text or binary WebSocket message:
  `include/Pt/Http/WebSocketMessage.h`
- Connect, accept, send and receive WebSocket messages:
  `include/Pt/Http/WebSocket.h`
- Handshake service for WebSocket upgrades:
  `include/Pt/Http/WebSocketService.h`
- Handshake responder for WebSocket upgrades:
  `include/Pt/Http/WebSocketResponder.h`
- Server-side session of one accepted WebSocket stream:
  `include/Pt/Http/WebSocketSession.h`
- Endpoint policy and owner of the sessions a WebSocket service creates:
  `include/Pt/Http/WebSocketServlet.h`
- Stream of an upgraded HTTP connection:
  `include/Pt/Http/Stream.h`
