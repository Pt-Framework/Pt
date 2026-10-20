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

`WebSocketChannel` starts input only for an outstanding receive or a
locally queued close. The output pump already prefers queued controls
over data fragments and can run while a read is outstanding, because
`Stream` allows one input and one output at the same time. A received
ping enqueues a pong. `ping` and `shutdown` enqueue frames without an
operation completion or an operation-specific I/O error.

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
class Frame
{
    public:
        enum Kind { Data, Control };

        Kind kind() const;
        unsigned opcode() const;
        bool fin() const;

        const char* data() const;
        std::size_t size() const;
        std::size_t avail() const;
        std::size_t pos() const;
        std::size_t remaining() const;
};

class WebSocketError : public std::runtime_error
{
    public:
        explicit WebSocketError(unsigned code);
        unsigned code() const;
};

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

        const Frame* import(std::streamsize n = 0);
};
```

The unit of the buffer is the current wire frame. `WebSocketChannel`
turns data frames into the public data message. A continuation frame
therefore stays visible to the buffer, while the channel preserves the
message type it received from the first text or binary frame. `import`
returns a pointer to the current frame, or null when it cannot make
progress. The pointer is valid until the next `import`.

`opcode` and `fin` are the values of the current wire frame header.
`size` is its declared payload length, `pos` is the imported payload
length, and `remaining` is the payload length still to import. A data
result refers to the streambuf get area through `data` and `avail`; it
may contain one chunk of an unfinished data frame. A control result is
returned only after the complete control payload is buffered.

The buffer does not advance past a data frame until its payload is
imported and its get area is consumed. A following control frame cannot
overwrite unread data. Once the current frame is complete, the next
`import` reuses the same frame storage for the following wire frame.

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
boundaries. Their payload, at most 125 bytes, is buffered in the
current frame and is not the get area. The channel reads a complete
control from the returned metadata and does not call a consume. A
close discards an open data body and does not finish that message.

The parser has one state for each parsing task. Every state performs
its work and returns the next state:

```text
OnHeader           read the 2-byte base header
OnLength           read the 2-byte or 8-byte extended length
OnMask             read the 4-byte masking key
OnPayload          initialize the current frame
OnDataPayload      expose or append one data payload chunk
OnControlPayload   collect and report a complete control payload
```

Header, length, and mask are filled with `sgetn` into fixed
storage owned by the current frame. Data payload is filled with
`sgetn` into the unused part of the get area. Control payload is
filled into the frame buffer.

A 16-bit length is valid only when it is at least 126. A 64-bit
length is valid only when it is at least 65536 and its high bit is
clear. RSV bits, a control frame without FIN, a control payload
over 125, a continuation with no open message, a data opcode while
a message is open, and a wrong mask direction each throw
`WebSocketError` with 1002. A data message over `maxMessageSize`
throws with 1009. Invalid UTF-8 throws with 1007. The buffer does
not close a stream.

The buffer does not know the channel, the signals, or the public
messages. The channel calls `import`. A null result means idle. A
control frame is handled from the returned metadata. A data frame
sets the incoming type only for opcode 0x01 or 0x02; a continuation
keeps the existing type. The channel reports a finished public message
only when `fin` is set and `remaining` is 0. A `WebSocketError`
becomes `protocolFail`.

Tests feed a `stringbuf` and call `import`. They cover the current
data frame and its get-area chunk, a zero-length data frame, a frame
split across calls, a partial payload smaller than the frame, the
import budget, and a control frame that is reported only when complete.
They also cover two fragments of one message, a ping that does not
enter the get area, a ping between two fragments, an invalid length
form, RSV, a wrong mask direction, a control without FIN, a
continuation with no open message, UTF-8, and a payload over the size
limit. No socket, no loop, no channel.

## Input operations {#input-operations}

The input pump runs only for an outstanding `beginReceive` or for a
locally initiated shutdown waiting for the peer close. No input is
started from `open`. A receive delivers data frames through `incoming`.
It also handles ping, pong, and close internally. Ping enqueues a pong.
Pong runs the pong notification. Neither becomes `incoming` or emits
`inputReady`. A received close ends the receive without delivering an
incomplete data message.

`beginShutdown` owns input until the peer close arrives. It throws when
a receive is outstanding. While it owns input, ping and pong are handled
internally and data is not delivered. `beginReceive` throws until the
shutdown result is consumed by `endShutdown`.

The engine never cancels an outstanding input to start an output, or
an outstanding output to start an input. `Stream` already allows one
of each. `pumpOutput` writes a queued control or the next data
fragment whenever the output channel is free. A control is inserted
between data fragments. It does not cut a data frame. It does not
emit `outputReady` for a control operation.

## Control operations {#control-operations}

`beginPing(payload)` enqueues one ping and starts its output. Its ready
signal runs after that ping frame was written. `endPing` returns that
operation to idle or throws its stored I/O error. It does not wait for
or match a pong.

`beginShutdown(code, reason)` validates the close code and reason,
enqueues one close frame, and starts input for the peer close. It does
not wait for `endSend` and does not cut the current data frame. A second
shutdown throws. 1005, 1006, and 1015 throw and stay off the wire.
Its ready signal runs after the stream detached, before `closed`. Then
`endShutdown` returns the operation to idle or throws its stored I/O
error. `closed` is a lifecycle notification, not shutdown completion.

## Pong visible {#pong-visible}

A received pong is consumed by the parser. It does not become
`incoming` and does not emit `inputReady`. The facade sees the pong:
`pong()` on the client, `onPong()` on the session. The idle timer
restarts on a received ping and on a received pong.

## Peer close {#peer-close}

A received close is consumed by the parser. It does not become
`incoming` and does not emit `inputReady`. It is processed only while
a receive or a local shutdown owns input. If this side has not sent a
close frame, the engine enqueues a reply. A peer code that must not be
written is not echoed. The reply is 1000. If a data message was open,
its body is discarded and `onInput` does not run for that incomplete
message. After the reply is written, the engine ends the stream. It
does not call `close` on the facade. A peer close during shutdown
completes that shutdown; one during receive only emits `closed`.
