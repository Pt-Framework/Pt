# WebSocket

## Documents

- [Message](websocket-message.md)
- [Session](websocket-session.md)
- [Concept Message](../concepts/websocket-message.md)
- [Concept Session](../concepts/websocket-session.md)

## Open Tasks

- [frame-reader](websocket-message.md#frame-reader).
  FrameReader is the internal frame parser. Its surface follows
  JsonReader: attach, reset, get, advance. It returns a Frame.
  Need-N-bytes, non-blocking, pausable at any byte. Own tests on a
  stringbuf, no channel.

- [permanent-input](websocket-message.md#permanent-input).
  The input pump runs from open until the stream ends. Controls and
  close are processed even when no beginReceive is outstanding.
  Data payloads are delivered only when a receive is outstanding.
  The engine never cancels one direction to free the other.

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
  sent. A peer code that must not be written is not echoed. After
  that reply is written, the engine ends the stream.

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

FrameReader first, with its own tests. It is the parser the rest of
the engine sits on, and it can land without a channel. Permanent
input next. It uses the reader and the concurrent read and write
that Stream already allows. Pong visibility, shutdown, and peer
close after that. Handshake checks last.
