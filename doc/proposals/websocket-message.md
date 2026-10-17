---
proposal: websocket.md
---

# WebSocket Message

The public unit is a message. Framing, fragmentation, masking, and
control frames stay inside the engine. The model is
[WebSocket](../concepts/websocket.md). This file
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

Input and output already have separate state. The public surface is
already `incoming()`, `outgoing()`, and `beginSend()` without a
frame argument. Those are not open tasks.

A received close is parsed. The channel stores the code and the
reason, discards an open data message, and enqueues a reply close.
The reply echoes a writable peer code. It does not always use 1000.
After both closes have been exchanged and the output is idle, the
channel ends the stream and emits `closed()`.

The frame parser lives inside `WebSocketChannel::parseAvailable`.
It reads one byte at a time with `sbumpc`. Header, length, and mask
share one block. A 16-bit or 64-bit length is not checked against
the minimal form. The parser cannot be tested without a channel.

## Frame reader {#frame-reader}

`FrameReader` is the internal frame parser. It is not a public type.
Its surface follows `JsonReader`: `attach`, `reset`, `get`,
`advance`. It returns a `Frame`, not a `Node`. There is no iterator
and no event interface.

```cpp
class Frame
{
    public:
        enum Type { None, Data, Control, Fail };

        Type type() const;
        unsigned opcode() const;
        bool fin() const;
        const char* data() const;
        std::size_t size() const;
        unsigned failCode() const;
};

class FrameReader : private NonCopyable
{
    public:
        FrameReader();
        explicit FrameReader(std::streambuf& sb);

        void attach(std::streambuf& sb);
        void reset();
        void reset(std::streambuf& sb);

        void setMaxMessageSize(std::size_t n);
        void setExpectMasked(bool masked);

        Frame& get();
        Frame& next();
        Frame* advance();
};
```

`advance` processes what the buffer holds and pauses anywhere.
It returns the `Frame` when one is complete, otherwise nullptr.
`get` is the current frame, valid until the next `advance` or
`next`. `next` pulls the next complete frame. The channel uses
`advance` only.

The reader is a need-N-bytes machine. One state, one task, the
transition at the end:

```text
NeedHeader    2 bytes
NeedLength    2 or 8
NeedMask      4
NeedPayload   N
Idle
```

Header, length, and mask are filled with `sgetn` into fixed
buffers. The payload is read in blocks with `sgetn`, not one byte
at a time. A masked payload is unmasked in words after alignment.
`sgetn` may return fewer bytes than asked. The state and the got
counters stay. The next `advance` continues there. It never blocks.

A 16-bit length is valid only when it is at least 126. A 64-bit
length is valid only when it is at least 65536 and its high bit is
clear. RSV bits, a control frame without FIN, a control payload
over 125, a continuation with no open message, a data opcode while
a message is open, and a wrong mask direction each fail the frame
with 1002. A data message over `maxMessageSize` fails with 1009.
Invalid UTF-8 fails with 1007. A failed frame is `Frame::Fail`.
The reader does not close a stream.

The channel owns the reader. It builds the message from a data
frame, answers a ping, and reports a pong. The reader does not know
the channel, the signals, or the messages.

Tests feed a `stringbuf` and call `advance`. They cover a complete
frame, a frame split across calls, an invalid length form, RSV,
a wrong mask direction, a control without FIN, a continuation with
no open message, and a payload over the size limit. No socket, no
loop, no channel.

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
The close is processed and `closed()` is emitted even when no
`beginReceive` is outstanding.
