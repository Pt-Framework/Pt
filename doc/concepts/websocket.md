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
  the session. An unanswered ping is facade state.

- [websocket-shutdown](websocket-message.md#wsm-close).
  `shutdown(code, reason)` enqueues a close frame and does not release
  the stream. The pump writes it after the current data frame and does
  not wait for `endSend()`. A second `shutdown()` throws. 1005, 1006,
  and 1015 throw and stay off the wire.

- [websocket-peer-close](websocket-message.md#wsm-close).
  A received close is answered by `shutdown` when no close frame was
  sent. A peer code that must not be written is not echoed. The engine
  calls `close()` after that reply is written.

- [websocket-close](websocket-message.md#wsm-close).
  `close()` releases the stream and writes no frame. It does not block.
  The destructor calls it. Inside, `cancel` stops a pending transfer
  and discards the buffer. The framed stream is not resumed.
  `beginSend()` and `beginReceive()` then throw. `closed()` is not
  emitted from the destructor. No close frame leaves the code at 1006.

- [websocket-nonce](websocket-session.md#wss-handshake).
  The handshake key and the masking key come from an unpredictable
  source.
