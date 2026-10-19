# WebSocket

## Documents

- [Message](websocket-message.md)
- [Session](websocket-session.md)
- [Concept](../concepts/websocket.md)

## Open Tasks

- [pong-visible](websocket-message.md#pong-visible).
  A received pong is reported: pong() on the client, onPong() on the
  session. An unanswered ping is facade state.

- [shutdown](websocket-message.md#shutdown).
  shutdown enqueues a close frame and does not end the stream. The
  pump writes it after the current data frame and does not wait for
  endSend. A second shutdown throws. 1005, 1006, and 1015 throw and
  stay off the wire.

- [peer-close](websocket-message.md#peer-close).
  A received close is answered by shutdown when no close frame was
  sent. A peer code that must not be written is not echoed. The
  reply is 1000. After that reply is written, the engine ends the
  stream. The close is processed and closed() is emitted even when
  no beginReceive is outstanding.

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

Pong visibility, shutdown, and peer close first. Handshake checks
last.
