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

## WebSocket buffer {#websocket-buffer}

`WebSocketBuffer` is the internal frame parser and the body of an
incoming message. It is not a public type. It is a `streambuf`,
the way `HttpBuffer` is the body of an HTTP message. The channel
owns it. `incoming().body()` uses it for the life of a receive.
`outgoing().body()` stays the payload buffer the application writes.

```cpp
class WebSocketBuffer : public std::streambuf
{
    public:
        WebSocketBuffer();
        explicit WebSocketBuffer(std::streambuf& sb);

        void attach(std::streambuf& sb);
        void reset();
        void reset(std::streambuf& sb);

        void setMaxMessageSize(std::size_t n);
        void setMasking(bool masked);

        std::streamsize import(std::streamsize n = 0);

        bool header() const;
        unsigned opcode() const;
        bool finished() const;
        unsigned failCode() const;

        bool hasControl() const;
        unsigned controlOpcode() const;
        const char* controlData() const;
        std::size_t controlSize() const;
        void consumeControl();
};
```

The unit is the data message. Frames stay inside the buffer, the
way chunks stay inside `HttpBuffer`. A continuation extends the
current message and does not change its type. The first text or
binary opcode is `header()`. `opcode()` is that opcode until the
message ends. `finished()` is the FIN of the message. The next
`import` after `finished()` starts the next message.

`beginReceive` points `incoming().body()` at this buffer.
`import` copies what the socket buffer holds into a fixed get
area. One copy: the socket into that window. A masked payload is
unmasked in place at the offset into the frame, so a window that
is not the start of the frame stays correct. `available()` is
`in_avail()`. The application reads `incoming().body()`.
`discard()` drains the get area. `underflow` returns EOF when the
get area is empty. It does not read the socket. At the FIN of a
data message the get area ends, the way `HttpBuffer::isEnd` ends
an HTTP body.

`import` never waits for a whole frame or a whole message.
`MessageProgress::body()` can be true in the middle of a frame.
The window is a few kilobytes, not `payloadLength`. `sgetn` may
return fewer bytes than asked. The state and the got counters
stay. The next `import` continues there. It never blocks.

Control frames are a side channel. They occur only at frame
boundaries. Their payload, at most 125 bytes, is not the get
area. `hasControl()` is true when a ping, pong, or close is
complete. The channel answers a ping, reports a pong, and
handles a close, then calls `consumeControl()`. A control
between two data fragments leaves the get area as it is. A close
discards an open data body and does not finish that message.

When no receive is outstanding, `import` still runs. It consumes
data payload without filling the get area, and it still completes
control frames.

The parser is a need-N-bytes machine. One state, one task, the
transition at the end:

```text
NeedHeader    2 bytes
NeedLength    2 or 8
NeedMask      4
NeedPayload   N
```

Header, length, and mask are filled with `sgetn` into fixed
buffers. Data payload is filled with `sgetn` into the unused
part of the get area. Control payload is filled into the control
buffer.

A 16-bit length is valid only when it is at least 126. A 64-bit
length is valid only when it is at least 65536 and its high bit is
clear. RSV bits, a control frame without FIN, a control payload
over 125, a continuation with no open message, a data opcode while
a message is open, and a wrong mask direction each fail the frame
with 1002. A data message over `maxMessageSize` fails with 1009.
Invalid UTF-8 fails with 1007. `failCode()` is that code. Zero
means no failure. The buffer does not close a stream.

The buffer does not know the channel, the signals, or the
messages. The channel calls `import`, reads `failCode`,
consumes a control, sets the incoming type from `opcode` when
`header` becomes true, and sets progress from `header`,
`in_avail`, and `finished`.

Tests feed a `stringbuf` and call `import`. They cover a
complete data frame in the get area, a frame split across
calls, a partial payload smaller than the frame, two fragments
of one message in the same get area, a ping that does not enter
the get area, a ping between two fragments, an invalid length
form, RSV, a wrong mask direction, a control without FIN, a
continuation with no open message, and a payload over the size
limit. No socket, no loop, no channel.

## Permanent input {#permanent-input}

The input pump runs from `open` until the stream ends. It does not
depend on `beginReceive`. Controls and close are processed on every
input step. Data payloads fill the get area of `incoming()` and are
reported by `inputReady` only when a receive is outstanding. When
no receive is outstanding, `import` consumes a data payload without
filling the get area. It does not emit `inputReady`.

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
