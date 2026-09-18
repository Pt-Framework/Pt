---
description: "SSL/TLS Communication"
---

- Module chapter, group order:
  `include/Pt/Ssl/Api-Ssl.h`
- Load PKCS12 or PEM certificate data, find certificates, iterate certificate stores:
  `include/Pt/Ssl/Api-Certificates.h`
  `include/Pt/Ssl/CertificateStore.h`
- Inspect X509 certificate subjects:
  `include/Pt/Ssl/Certificate.h`
- Configure SSL/TLS protocols, certificate verification, trusted CAs, server or client identities:
  `include/Pt/Ssl/Api-Context.h`
  `include/Pt/Ssl/Context.h`
- Handle SSL runtime failures, failed handshakes, and invalid certificates:
  `include/Pt/Ssl/SslError.h`
- Wrap an iostream in an SSL stream, manage handshakes, encrypted I/O, shutdown, and cipher inspection:
  `include/Pt/Ssl/Api-Streams.h`
  `include/Pt/Ssl/IOStream.h`
- Implement SSL stream buffering, peer-name verification, handshakes, encrypted I/O, and shutdown:
  `include/Pt/Ssl/StreamBuffer.h`
