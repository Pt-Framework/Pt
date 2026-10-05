# WebSocket Message {#websocket-message}

This document is the design for the public read and write API of a
WebSocket in Platinum HTTP. It is a framework concept, not an
application sketch. The goal is that the application sends and
receives messages, while framing, fragmentation, masking, and control
frames stay inside the engine.

The HTTP upgrade boundary in
[HTTP Upgrade](../requirements/http-upgrade.md) stays as it is. The
HTTP core still knows no WebSocket frames. The session model in
[WebSocket Session](websocket-session.md) stays as it is, except where
this chapter changes what `onInput()` and `onOutput()` mean: a
message step, not a frame. Both facades are defined there, and both
forward the same message operations.

This chapter covers:

- [Purpose](#wsm-purpose)
- [Current state](#wsm-current)
- [Principles](#wsm-principles)
- [WebSocketMessage](#wsm-message)
- [Two directions](#wsm-directions)
- [Frames stay internal](#wsm-frames)
- [Control plane](#wsm-control)
- [Progress](#wsm-progress)
- [Engine state](#wsm-state)
- [Session callbacks](#wsm-session)
- [What changes](#wsm-changes)
- [Out of scope](#wsm-scope)

## Purpose {#wsm-purpose}

An HTTP exchange already separates the message from the connection.
`Client` holds a `Request` and a `Reply`. The application fills one
and reads the other. Send and receive do not share a body. Chunk
framing is not part of that surface. `MessageProgress` reports how
far one step moved, and `finished()` means the message is complete.

A WebSocket needs the same split. The application has a text or
binary payload to write, and a text or binary payload to read. Those
are two messages. They must be able to move at the same time. The
upgrade contract already says that at most one transfer may be active
per direction, and that input and output may run concurrently.

Popular WebSocket stacks hide the frame from that surface. Beast,
websocketpp, tokio-tungstenite, and the browser deliver a message.
Continuation is not an application event. A ping between two
fragments does not replace the data message. Platinum should do the
same. The frame remains the encoding on the stream. It is not the
object the application reads and writes.

## Current state {#wsm-current}

`WebSocket` exposes one `body()`. Send and receive share that stream
and the payload buffer behind it. `beginSend(Frame)` writes whatever
is buffered as one frame. `beginReceive()` parses one frame back into
the same body. `frame()` is the opcode of that frame. `inputReady()`
means the frame is complete.

One `_state` covers the handshake, the receive steps, and the send.
A receive in progress cannot coexist with a send. `sendPing()` and
`close()` call `beginOutput()` and `endOutput()` on the stream
directly, so a control write overwrites a data write that has not
finished.

The implementation always sets FIN on a written frame. A
continuation opcode is not a public type. On input it becomes
`Unknown`. `setMaxMessageSize()` limits one frame payload, not the
reassembled message. The public unit is therefore a frame, and
fragmentation is neither hidden nor implemented.

That surface cannot meet the stream contract, and it does not match
the message model the rest of HTTP already uses.

## Principles {#wsm-principles}

The public unit is a message. A message is text or binary. It has a
body. It has no opcode, no FIN bit, no mask, and no RSV bits. A text
message is UTF-8. Invalid UTF-8 fails the stream.

The socket owns two messages, `incoming()` and `outgoing()`, the way
`Client` owns `request()` and `reply()`. The application does not
construct them. One message object on the socket would only rename
the shared body.

One receive and one send may be outstanding. A second begin on the
same direction is an error. The other direction is unaffected.

Fragmentation is an engine concern. The application never sees a
continuation. `maxMessageSize()` is the limit of one data message,
not of one frame and not of the unread buffer.

Ping, pong, and close are not messages. They are socket operations.
A received ping is answered by the engine. A received pong is
consumed. Neither is delivered, and neither frees the data channel.
A close is a stream end, not a body the application reads as
`incoming()`. It carries a status code and a reason.

Progress follows HTTP. `endReceive()` can report body bytes before
the message is finished. `finished()` means the message is complete,
not that a frame was parsed. The application still does not learn how
many frames those bytes came from.

The HTTP core stays free of this. Framing lives in the WebSocket
engine that already binds the `Stream`. The session does not parse
frames, and a callback does not receive the message again.
`socket().incoming()` is already there.

## WebSocketMessage {#wsm-message}

`WebSocketMessage` is the payload object. It is the peer of
`Message`, not of `Request` or `Reply`. It has no header fields and
no URL. The only type it exposes is the data type.

```cpp
class WebSocketMessage
{
    public:
        enum Type { Text, Binary };

        Type type() const;
        void setType(Type type);

        std::iostream& body();
        std::size_t available() const;
        std::size_t pending() const;
        void discard();
        void clear();
};
```

`setType()` is for the outgoing message. The engine sets the type of
the incoming message when it has parsed the first data frame of that
message. `body()` is the iostream, the same surface `Message` uses.
`available()` is how many payload bytes can be read.
`pending()` is how many payload bytes are waiting to be sent.
`discard()` drops the buffered body. `clear()` drops the body and
unsets the type, so the same object can carry the next message.

Callers do not construct a `WebSocketMessage`. The socket constructs
both, and keeps them for the life of the stream. A later pool of
messages is not part of this surface. The HTTP client does not take
an external request either.

## Two directions {#wsm-directions}

`WebSocket` keeps the handshake and the stream bind. The message
operations replace `body()`, `frame()`, and `beginSend(Frame)`.

```cpp
class WebSocket
{
    public:
        WebSocketMessage& incoming();
        WebSocketMessage& outgoing();

        void beginSend();
        void endSend();
        Signal<WebSocket&>& outputReady();

        void beginReceive();
        MessageProgress endReceive();
        Signal<WebSocket&>& inputReady();

        void setMaxMessageSize(std::size_t maxSize);
};
```

A send writes `outgoing()`. The application sets the type, writes
the body, and calls `beginSend()`. `outputReady()` reports that the
message has left the stream buffer. `endSend()` completes it. The
engine may have split the body into several frames. The application
does not see that split.

A receive fills `incoming()`. `beginReceive()` starts it.
`inputReady()` reports that a step has completed. `endReceive()`
returns `MessageProgress`. Body bytes, when present, are readable
from `incoming().body()`. `finished()` means this data message is
complete. The application then clears the incoming message, or
discards what it has read, and starts the next receive if the stream
is still open.

`beginSend()` while a send is outstanding throws. `beginReceive()`
while a receive is outstanding throws. Starting one does not cancel
the other.

The first public send is a finished message: the body is complete
before `beginSend()`. A `beginSend(false)` that produces the body
while the send is in progress, the chunked-request pattern, can be
added later. FIN is then an engine flag, set when the completion
argument is true. It is not on the message.

## Frames stay internal {#wsm-frames}

The engine is the only code that writes or parses a frame header.

On input it reassembles until FIN. A continuation extends the
current data message. It does not emit `inputReady()` by itself, and
it does not change `incoming().type()`. A control frame between two
fragments is consumed by the control plane. It does not become
`incoming()`, and it does not finish the data message. A new data
opcode before FIN is a protocol error and fails the stream.

On output the engine fragments `outgoing().body()` at its frame
limit. Every fragment but the last has FIN clear. The last has FIN
set. The opcode of the first fragment is the message type.
Continuations follow. Masking stays a mode of the engine: a client
masks, a server does not. The application does not supply a mask.

A text message is a UTF-8 sequence over the whole message, not over
one fragment. The engine validates it as the bytes arrive. A
sequence that ends in the middle of a code unit is valid so far if
the next fragment can complete it. It is invalid when FIN arrives
and the sequence is still incomplete, and it is invalid as soon as a
byte cannot continue the sequence. `discard()` does not turn the
check off. The engine has already seen those bytes, and the
application does not have to retain them for the check to hold.
Invalid text fails the stream with close code 1007. Binary is not
checked. An outgoing text message is checked the same way before it
is written.

`maxMessageSize()` limits one data message. The count runs from the
first data opcode to FIN. Each fragment adds its declared payload
length before those bytes are read. The sum is what matters, not the
size of one frame and not the bytes still sitting in `incoming()`.
A peer that splits a large message into small continuations does not
get a fresh budget per frame.

`discard()` frees the unread remainder of `incoming()`. It does not
subtract from the count. The message is the same message until FIN.
The buffer may stay small while the sum grows to the limit. How much
unread data the engine holds ahead of the application is flow
control, and it is a different limit.

Control frames do not count. Their payload is at most 125 bytes.
Zero disables the limit. A declared length that would make the sum
exceed it fails the stream before that payload is buffered. The close
code is 1009. The same check applies to a message the engine is
sending: `beginSend()` fails the stream rather than writing a message
over the limit.

RSV bits and extensions are not on the message. An extension, if one
is added later, is negotiated at the handshake and applied by the
engine before the body is visible.

## Control plane {#wsm-control}

Control frames are socket operations. They are not a
`WebSocketMessage`, and they are not a second public write. Ping and
pong do not emit `inputReady()` or `outputReady()`. They do not
finish a data message, and they do not free the data channel.

```cpp
void ping(const char* payload = 0, std::size_t n = 0);
void close(unsigned code = 1000,
           const std::string& reason = std::string());

unsigned closeCode() const;
const std::string& closeReason() const;
Signal<WebSocket&>& closed();
```

`ping()` enqueues a ping. The payload is at most 125 bytes. A longer
payload fails the call. It does not call `beginOutput()` itself, and
it does not require the data send to be idle. The output pump writes
it when no data frame is in the middle of being written, or between
two data fragments of the current message. The data send stays
outstanding. `outputReady()` still means that data message has left
the stream buffer, not that the ping has been written.

A received ping is consumed by the parser. It does not become
`incoming()`, and it does not emit `inputReady()`. The engine
enqueues a pong with the same payload, at most 125 bytes. That pong
is written by the same pump, under the same rule, and it does not
emit `outputReady()`. The application does not call `sendPong()`.
Answering the ping restarts the idle timer. A signal for a received
ping can be added if an application needs to see it. The default is
to answer and not deliver.

A received pong, opcode `0xA`, takes the same read path. The parser
reads the payload and discards it. It does not touch `incoming()`,
and it does not emit `inputReady()`. If a `ping()` is still
outstanding, the payload is compared with that ping and the ping is
retired. A pong that matches nothing is still valid. An unsolicited
pong is allowed and takes this path. Either way the data message is
unchanged, and the idle timer restarts. The application does not see
the pong. A signal for it can be added later. The default is to
consume it.

`close()` enqueues a close frame and ends the stream after the close
handshake. The frame payload is the status code and the reason. The
code defaults to 1000. The reason defaults to empty. It is an
UTF-8 text, not a message body, and it is not delivered through
`incoming()`. A received close is the same end. `closeCode()` and
`closeReason()` are the values from that handshake. A local close
keeps the values passed to `close()` when the peer sends none.
`closed()` stays the signal that the stream has ended. Peer close, an
I/O error, a close frame, and destruction of the stream all emit it.
An I/O error leaves the code at 1006 and the reason empty, because no
close frame arrived. `incoming().type()` is never close.

This keeps the one-transfer rule. The application still has one
outstanding data send. The engine may insert a ping or a pong around
the data frames of that send. A ping during a large message is
therefore possible without a second `beginSend()`.

## Progress {#wsm-progress}

`MessageProgress` is reused. `header()` means the message type is
known. `body()` means payload bytes were processed on this step.
`finished()` means the data message is complete. Trailer is unused.

A short message often completes in one receive step, in which case
type, body, and finished are all true. A large message reports body
bytes before finished. The application reads `incoming().body()`,
discards what it has consumed, and calls `beginReceive()` again until
finished. What it reads is payload. It is not a frame, and it is not
aligned to a frame boundary. Discarding consumed bytes does not reset
`maxMessageSize()`, and it does not skip the UTF-8 check of a text
message.

Delivering only whole messages would match the browser and Beast. It
would also force the socket to buffer up to `maxMessageSize()` before
the first callback. HTTP does not do that for a reply body. This
chapter does not do it for a WebSocket message either.

`endSend()` completes the send. A send progress can be added if a
later incremental send needs it. The first surface does not, because
the outgoing body is complete before `beginSend()`.

## Engine state {#wsm-state}

The engine splits the single state enum. Input and output each have
their own state. The handshake remains a third, and it finishes
before either message transfer starts. No session writes the stream
until the upgrade reply has been written, as the upgrade contract
already requires.

The parser runs from `Stream::inputReady()`. The writer runs from
`Stream::outputReady()`. The stream already allows one input and one
output at the same time. The engine must not take a second begin on
either. A control frame is a write of the output pump, not a second
user transfer.

`idleTimeout()` restarts when a send or a receive step finishes, as
it does today. It is not restarted by an internal continuation alone
if that continuation did not complete a step the application saw.
A received ping that the engine answers restarts it, and so does a
received pong. The peer is alive. Writing a ping does not by itself
restart it. The answer does.

## Session callbacks {#wsm-session}

`WebSocketSession` does not grow a message accessor. The message is
reached through `socket().incoming()` and `socket().outgoing()`.
`onInput()`, `onOutput()`, and `onClose()` still take no arguments.

`onInput()` runs when a receive step has completed, not when a frame
has been parsed. The derived session calls `socket().endReceive()`.
If the progress reports body bytes, it reads `socket().incoming()`.
If the progress is not finished, it calls `beginReceive()` again. If
it is finished, it handles the message and starts the next receive,
or a reply. A ping or a pong does not run `onInput()`.

`onOutput()` runs when the outgoing message has left the stream
buffer. The derived session calls `socket().endSend()` and starts the
next send when it still has a message. It does not run once per
fragment, and it does not run for a ping or a pong.

`onClose()` is unchanged. It is the last look at the session. A close
handshake leaves `socket().closeCode()` and `socket().closeReason()`
set. An I/O error leaves the code at 1006 and the reason empty.

The client uses the same two messages on the `WebSocket` it
constructed. It connects `inputReady()` and `outputReady()` itself.
There is still no client session.

## What changes {#wsm-changes}

`body()`, `frame()`, `available()`, `pending()`, and `discard()` move
from `WebSocket` to `WebSocketMessage`. The socket forwards nothing
in their place except `incoming()` and `outgoing()`.

`beginSend(Frame)` becomes `beginSend()`. The type is
`outgoing().setType()`. `beginReceive()` and `endReceive()` stay, and
`endReceive()` returns `MessageProgress`.

`sendPing()` and `sendPong()` leave the public surface. `ping()`
enqueues a ping of at most 125 bytes. A received ping is answered
with a pong that mirrors the payload. A received pong, including an
unsolicited one, is consumed and restarts the idle timer. Neither
emits `inputReady()` or `outputReady()`, and neither frees the data
channel. There is no `sendPong()`. `close()` remains, and gains a
status code and a reason. `closeCode()` and `closeReason()` report
the handshake that ended the stream. `close()` no longer writes the
stream as a side door around the output pump.

The receive path reassembles. The send path fragments. Control frames
between fragments are consumed or inserted by the engine.
`maxMessageSize()` limits one data message. It counts declared
payload bytes from the first data opcode to FIN, including bytes the
application has already discarded. It is not a frame limit and not a
buffer limit. Exceeding it closes the stream with 1009.

A text message must be valid UTF-8 across all of its fragments. The
engine checks the bytes as they arrive. Discarding them does not
remove them from the check. Invalid text closes the stream with
1007. Binary is not checked.

The session chapter's statement that `onInput()` means one whole
frame, and that `onOutput()` means one frame has left the buffer, is
superseded here. The callback names stay.

## Out of scope {#wsm-scope}

This chapter does not change the upgrade boundary, the handshake
responder, or who owns the stream. It does not decide the client and
server facade split. It does not add permessage-deflate, subprotocols
as a message property, or an application-supplied message pool.
Incremental send with a completion flag is allowed later and is not
the first surface. A signal for a received ping or pong is not part
of the first surface either.
