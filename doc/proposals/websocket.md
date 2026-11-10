# WebSocket

## Documents

- [Message](websocket-message.md)
- [Session](websocket-session.md)
- [Server](websocket-server.md)
- [Keepalive](websocket-keepalive.md)
- [Concept](../concepts/websocket.md)

## Open Tasks

- [decline](websocket-session.md#decline).
  A missing or invalid key is 400. A version other than 13 is 426
  with Sec-WebSocket-Version: 13.

- [nonce](websocket-session.md#nonce).
  The handshake key and the masking key come from an unpredictable
  source.

## Priority

Handshake checks last.
