---
description: "TCP and UDP network sockets"
---

- Networking module group, sockets as I/O devices, endpoints, AddressInUse:
  `include/Pt/Net/Api-Net.h`
- Construct host, port, IPv4 and IPv6 endpoint addresses:
  `include/Pt/Net/Endpoint.h`
- Handle local addresses already occupied by a network socket:
  `include/Pt/Net/AddressInUse.h`
- TCP sockets group, listen, accept and connect:
  `include/Pt/Net/Api-Tcp.h`
- Listen for TCP connections, configure accept backlog, receive pending connections:
  `include/Pt/Net/TcpServer.h`
- Connect, accept, read, write and configure TCP client sockets:
  `include/Pt/Net/TcpSocket.h`
- UDP sockets group, bind, connect, setTarget, broadcast and multicast:
  `include/Pt/Net/Api-Udp.h`
- Bind, connect, send, receive, broadcast and join multicast UDP sockets:
  `include/Pt/Net/UdpSocket.h`
