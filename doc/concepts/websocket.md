# WebSocket

## Documents

- [Session](websocket-session.md)
- [Message](websocket-message.md)

## Open

- [websocket-client](websocket-session.md#wss-client).
  Headers set before `beginConnect()` are kept. `endConnect()` checks
  status 101 and `Sec-WebSocket-Accept`.

- [websocket-decline](websocket-session.md#wss-decline).
  A missing or invalid key is 400. A version other than 13 is 426
  with `Sec-WebSocket-Version: 13`.

- [websocket-pong](websocket-message.md#wsm-control).
  A received pong is visible: `pong()` on the client, `onPong()` on
  the session. An unanswered ping is facade state. 1005, 1006, and
  1015 stay off the wire.

- [websocket-nonce](websocket-session.md#wss-handshake).
  The handshake key and the masking key come from an unpredictable
  source.
