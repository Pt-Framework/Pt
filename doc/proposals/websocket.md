# WebSocket

## Documents

- [Message](websocket-message.md)
- [Session](websocket-session.md)
- [Keepalive](websocket-keepalive.md)
- [Concept](../concepts/websocket.md)

## Open Tasks

- [client-handshake](websocket-session.md#client-handshake).
  Headers set before beginConnect are kept. endConnect checks status
  101 and Sec-WebSocket-Accept.

- [decline](websocket-session.md#decline).
  A missing or invalid key is 400. A version other than 13 is 426
  with Sec-WebSocket-Version: 13.

- [nonce](websocket-session.md#nonce).
  The handshake key and the masking key come from an unpredictable
  source.

## Priority

Handshake checks last.
