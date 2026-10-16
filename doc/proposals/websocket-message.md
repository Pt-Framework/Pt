---
proposal: websocket.md
---

# WebSocket Message

The public unit is a message. Framing, fragmentation, masking, and
control frames stay inside the engine. The model is
[WebSocket Message](../concepts/websocket-message.md). This file
states the rules the implementation still has to follow, and the
behavior it has today.

## Baseline

`WebSocketChannel` starts the input pump only from `beginReceive`.
`open` connects the stream signals and does not begin a read.
A silent peer therefore leaves controls and close unprocessed until
the application starts a receive. The output pump already prefers
queued controls over data fragments and can run while a read is
outstanding, because `Stream` allows one input and one output at the
same time. A received ping enqueues a pong. A received pong is not
yet reported on the facade. `shutdown` enqueues a close frame.
Peer close is not yet answered by a reply close before the stream
ends.

## Permanent input {#permanent-input}

The input pump runs from `open` until the stream ends. It does not
depend on `beginReceive`. Controls and close are processed on every
input step. Data payloads are written into `incoming()` and reported
by `inputReady` only when a receive is outstanding. When no receive
is outstanding, a data frame is parsed and its payload is held or
discarded according to the message rules, but it does not emit
`inputReady`.

The engine never cancels an outstanding input to start an output, or
an outstanding output to start an input. `Stream` already allows one
of each. `pumpOutput` writes a queued control or the next data
fragment whenever the output channel is free. A control is inserted
between data fragments. It does not cut a data frame. It does not
emit `outputReady`. `beginSend`, `ping`, and `shutdown` only enqueue
and call `pumpOutput`.

An I/O error on one direction stores the exception and leaves the
other direction and the stream open. The ready signal of the failed
transfer runs, and the matching end throws. The application calls
`close`.

## Pong visible {#pong-visible}

A received pong is consumed by the parser. It does not become
`incoming` and does not emit `inputReady`. If a `ping` is still
outstanding, the payload is compared and that ping is retired. An
unsolicited pong is still valid. The facade sees the pong: `pong()`
on the client, `onPong()` on the session. Neither frees the data
channel. An unanswered ping is facade state. The application may
shut the stream down from it. The idle timer restarts on a received
ping and on a received pong.

## Shutdown {#shutdown}

`shutdown(code, reason)` enqueues a close frame and does not end the
stream. The pump writes it after the current data frame, if a send
is in progress. It does not wait for `endSend` and does not cut a
frame. A second `shutdown` throws. `shutdown(1005)`,
`shutdown(1006)`, and `shutdown(1015)` throw and stay off the wire.
After a local close is queued, `beginSend` throws. Receive continues
until the peer close or the idle timeout, including when no
`beginReceive` is outstanding.

## Peer close {#peer-close}

A received close is consumed by the parser. It does not become
`incoming` and does not emit `inputReady`. If this side has not sent
a close frame, the engine enqueues the reply with `shutdown`. A peer
code that must not be written is not echoed. The reply is 1000. If a
data message was open, its body is discarded and `onInput` does not
run for that incomplete message. After the reply has been written,
the engine ends the stream. It does not call `close` on the facade.
