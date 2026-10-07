# WebSocket

The chapters are the target. A change to a linked section updates the
matching item in the same change. A decision that lands in the tree
drops the item. A decision that changes is rewritten in the chapter,
and the item here is rewritten with it.

Open work is not repeated in `doc/pages/`. A page describes the built
API.

## Documents

- [Session](websocket_session.md)
- [Message](websocket_message.md)

## Open

- [websocket-protocol](websocket_session.md#wss-protocol).
  The client offers names with `addProtocol()`. The responder echoes
  one. `protocol()` reports the agreed name.

- [websocket-handshake](websocket_session.md#wss-handshake).
  `WebSocketHandshake` is copied before the responder is released and
  passed to `onGetSession()`. The session keeps the copy, not the request.

- [websocket-client](websocket_session.md#wss-client).
  Headers set before `beginConnect()` are kept. `endConnect()` checks
  status 101 and `Sec-WebSocket-Accept`.

- [websocket-decline](websocket_session.md#wss-decline).
  A missing or invalid key is 400. A version other than 13 is 426
  with `Sec-WebSocket-Version: 13`.

- [websocket-pong](websocket_message.md#wsm-control).
  A received pong is visible: `pong()` on the client, `onPong()` on
  the session. An unanswered ping is facade state. 1005, 1006, and
  1015 stay off the wire.

- [websocket-nonce](websocket_session.md#wss-handshake).
  The handshake key and the masking key come from an unpredictable
  source.
