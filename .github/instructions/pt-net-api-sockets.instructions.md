---
description: "TCP and UDP network sockets"
---

- Construct host, port, IP4, and IP6 endpoint addresses:
  `include/Pt/Net/Endpoint.h`
- Handle local addresses already occupied by a network socket:
  `include/Pt/Net/AddressInUse.h`
- Listen for TCP connections, configure accept backlog, receive pending connections:
  `include/Pt/Net/TcpServer.h`
- Connect, accept, read, write, and configure TCP client sockets:
  `include/Pt/Net/TcpSocket.h`
- Bind, connect, send, receive, broadcast, and join multicast UDP sockets:
  `include/Pt/Net/UdpSocket.h`