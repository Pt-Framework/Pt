---
topic: websocket.md
---

# WebSocket Message {#websocket-message}

This document is the design for the public read and write API of a
WebSocket in Platinum HTTP. It is a framework concept, not an
application sketch. The goal is that the application sends and
receives messages, while framing, fragmentation, masking, and control
frames stay inside the engine.

The HTTP upgrade boundary in
[HTTP Upgrade](http-upgrade.md) stays as it is. The
HTTP core still knows no WebSocket frames. The session model in
[WebSocket Session](websocket_session.md) stays as it is, except where
this chapter changes what `onInput()` and `onOutput()` mean: a
message step, not a frame. `WebSocket` and `WebSocketSession` retain
the client and server roles defined there, and both expose the same
message operations directly.

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

The frame engine is `WebSocketChannel`. `WebSocket` and
`WebSocketSession` do not keep a payload of their own. Both forward
the same frame operations to that connection.

The connection exposes one `body()`. Send and receive share that
stream and the payload buffer behind it. `beginSend(Frame)` writes
whatever is buffered as one frame. `beginReceive()` parses one frame
back into the same body. `frame()` is the opcode of that frame.
`inputReady()` means the frame is complete.

One `_state` on the connection covers the receive steps and the send.
The handshake is a flag on the client socket, and it finishes before
either transfer starts. A receive in progress cannot coexist with a
send. `sendPing()`, `sendPong()`, and `close()` call `beginOutput()`
and `endOutput()` on the stream directly, so a control write
overwrites a data write that has not finished.

The implementation always sets FIN on a written frame. A
continuation opcode is not a public type. On input it becomes
`Unknown`. `setMaxMessageSize()` limits one frame payload, not the
reassembled message. The public unit is therefore a frame, and
fragmentation is neither hidden nor implemented.

A received ping or pong is delivered as that frame. `inputReady()`
runs, and the engine does not answer. A received close does not
become a frame the application reads. The parser calls `failStream()`
as soon as the opcode is close, so the payload is not read and no
status code or reason is kept.

That surface cannot meet the stream contract, and it does not match
the message model the rest of HTTP already uses.

## Principles {#wsm-principles}

The public unit is a message. A message is text or binary. It has a
body. It has no opcode, no FIN bit, no mask, and no RSV bits. A text
message is UTF-8. Invalid UTF-8 from the peer fails the stream.
A local contract error throws.

The socket owns two messages, `incoming()` and `outgoing()`, the way
`Client` owns `request()` and `reply()`. The application does not
construct them. One message object on the socket would only rename
the shared body.

One receive and one send may be outstanding. A begin is outstanding
until the matching end. A second begin on the same direction while
that begin is outstanding is an error. After an end that is not
finished, the next begin continues the same message. After
`finished()`, the next begin starts a new message. The other
direction is unaffected.

Fragmentation is an engine concern. The application never sees a
continuation. `maxMessageSize()` is the limit of one data message,
not of one frame and not of the unread buffer.

Ping, pong, and close are not messages. They are socket operations.
A received ping is answered by the engine. A received pong is consumed as a message. The facade still sees
it. Neither frees the data channel.
A close is a stream end, not a body the application reads as
`incoming()`. It carries a status code and a reason. A received
close is answered by the engine. After a local `close()`, no more
data frames are sent. Receive continues until the peer close or
`idleTimeout()`.

Progress follows HTTP. `endReceive()` and `endSend()` each report
one I/O step. Either can report body bytes before the message is
finished. `finished()` means the message is complete, not that a
frame was parsed. The application still does not learn how many
frames those bytes came from. `inputReady()` runs when bytes were
received. `outputReady()` runs when bytes were sent. Neither waits
for the whole message.

The HTTP core stays free of this. Framing lives in the WebSocket
engine that already binds the `Stream`. The session does not parse
frames, and a callback does not receive the message again.
`incoming()` and `outgoing()` are operations of the facade itself.

## WebSocketMessage {#wsm-message}

`WebSocketMessage` is a public class. It is the payload object, the
peer of `Message`, not of `Request` or `Reply`. The application
names it as the result of `incoming()` and `outgoing()`. It has no
header fields and no URL. The only type it exposes is the data type.
`WebSocketMessage::Type` is the public data type of a message.

```cpp
class WebSocketMessage
{
    public:
        enum Type { Unknown, Text, Binary };

        Type type() const;
        void setType(Type type);

        std::iostream& body();
        std::size_t available() const;
        std::size_t pending() const;
        void discard();
        void clear();
};
```

`Unknown` means that a message has no data type yet. `setType()` is
for the outgoing message, and accepts `Text` or `Binary`. The engine
sets the type of the incoming message when it has parsed the first
data frame of that message. `body()` is the iostream, the same
surface `Message` uses.
`available()` is how many payload bytes can be read.
`pending()` is how many payload bytes are waiting to be sent.
`discard()` drops the buffered body and leaves the type. That is the
call during a message that is not finished. `clear()` drops the body
and sets the type to `Unknown`, so the same object can carry the next
message. After `finished()`, the application calls `clear()`.

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
        MessageProgress endSend();
        Signal<WebSocket&>& outputReady();

        void beginReceive();
        MessageProgress endReceive();
        Signal<WebSocket&>& inputReady();

        void setMaxMessageSize(std::size_t maxSize);
};
```

A send writes `outgoing()`. The application sets the type, writes
the body, and calls `beginSend()`. `beginSend()` throws if the type
is `Unknown`. `outputReady()` reports that bytes were sent.
`endSend()` returns `MessageProgress`. If the send is not finished,
`beginSend()` continues the same message. If it is finished, the
message is complete. The engine may have split the body into several
frames. The application does not see that split.

`WebSocketSession` exposes the same `incoming()`, `outgoing()`,
`beginSend()`, `endSend()`, `beginReceive()`, and `endReceive()`
operations directly. It keeps its existing service, loop, and
lifetime API. It does not add a `socket()` accessor, because the
session is already the server facade of its stream.

A receive fills `incoming()`. `beginReceive()` starts it.
`inputReady()` reports that bytes were received. `endReceive()`
returns `MessageProgress`. Body bytes, when present, are readable
from `incoming().body()`. If the receive is not finished, the
application discards the consumed body and calls `beginReceive()`
again. The type stays. `finished()` means this data message is
complete. The application then calls `clear()` and starts the next
receive if the stream is still open.

`beginSend()` while a send is outstanding throws. `beginReceive()`
while a receive is outstanding throws. A begin is outstanding until
the matching end. Starting one direction does not cancel the other.

The first public send has a complete body before `beginSend()`.
Partial `outputReady()` steps still occur: the engine may write
only some of that body before it asks the application to begin
again. A `beginSend(false)` that produces the body while the send
is in progress, the chunked-request pattern, can be added later.
FIN is then an engine flag, set when the completion argument is
true. It is not on the message.

## Frames stay internal {#wsm-frames}

The engine is the only code that writes or parses a frame header.

On input it reassembles until FIN. A continuation extends the
current data message. It does not change `incoming().type()`.
Payload bytes from that continuation are reported by the current
receive step. A control frame between two fragments is consumed by
the control plane. It does not become `incoming()`, it does not
emit `inputReady()`, and it does not finish the data message. A new
data opcode before FIN is a protocol error and fails the stream.

On output the engine fragments `outgoing().body()` at its frame
limit. Every fragment but the last has FIN clear. The last has FIN
set. The opcode of the first fragment is the message type.
Continuations follow. A send step may complete after part of a
fragment, or after several fragments. Masking stays a mode of the
engine: a client masks, a server does not. The application does not
supply a mask.

A text message is a UTF-8 sequence over the whole message, not over
one fragment. The engine validates it as the bytes arrive. A
sequence that ends in the middle of a code unit is valid so far if
the next fragment can complete it. It is invalid when FIN arrives
and the sequence is still incomplete, and it is invalid as soon as a
byte cannot continue the sequence. `discard()` does not turn the
check off. The engine has already seen those bytes, and the
application does not have to retain them for the check to hold.
Invalid text from the peer fails the stream with close code 1007.
Binary is not checked. An outgoing text message is checked the same
way before it is written. Invalid outgoing text throws.

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
control. That limit is later. The first surface has only
`maxMessageSize()`.

Control frames do not count. Their payload is at most 125 bytes.
Zero disables the limit. A declared length that would make the sum
exceed it fails the stream before that payload is buffered. The close
code is 1009. The same size check applies to a message the engine is
sending: `beginSend()` throws rather than writing a message over the
limit.

RSV bits and extensions are not on the message. An extension, if one
is added later, is negotiated at the handshake and applied by the
engine before the body is visible. RSV bits without a negotiated
extension, a reserved opcode, a continuation with no open message,
and a masked frame on a server or an unmasked frame on a client
each fail the stream with close code 1002.

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
Signal<WebSocket&, const char*, std::size_t>& pong();
```

`ping()` enqueues a ping. The payload is at most 125 bytes. A longer
payload throws. A second `ping()` while another ping is still queued
or unanswered enqueues another ping. Each ping is written, and the
peer answers each. `ping()` and `close()` throw if the handshake is
not finished.

`ping()` does not call `beginOutput()` itself, and it does not
require the data send to be idle. The output pump writes it when no
data frame is in the middle of being written, or between two data
fragments of the current message. If no data send is outstanding,
the pump starts the stream output itself. That is still one output
transfer of the engine, not a user `beginSend()`. The data send stays
outstanding when there is one. `outputReady()` still means that data
bytes were sent, not that the ping has been written.

A received ping is consumed by the parser. It does not become
`incoming()`, and it does not emit `inputReady()`. The engine
enqueues a pong with the same payload, at most 125 bytes. Each
received ping gets its own pong. That pong is written by the same
pump, under the same rule, and it does not emit `outputReady()`.
The application does not call `sendPong()`. Answering the ping
restarts the idle timer. A signal for a received ping can be added
if an application needs to see it. The default is to answer and not
deliver.

A received pong, opcode `0xA`, takes the same read path. The parser
reads the payload. It does not touch `incoming()`, and it does not
emit `inputReady()`. If a `ping()` is still outstanding, the payload
is compared with that ping and the ping is retired. A pong that
matches nothing is still valid. An unsolicited pong is allowed and
takes this path. Either way the data message is unchanged, and the
idle timer restarts.

The facade sees that pong. On the client, `pong()` is emitted with
the payload. On the session, `onPong()` runs with the same payload.
Neither is a data-ready signal, and neither frees the data channel.
An unanswered ping is a state of the facade. The application may
close from it. The idle timeout remains the limit without a finished
transfer. Codes 1005, 1006, and 1015 are recorded locally and are
not written on the wire.

`close()` enqueues a close frame and ends the stream after the close
handshake. The frame payload is the status code and the reason. The
code defaults to 1000. The reason defaults to empty. It is UTF-8,
at most 123 bytes, not a message body, and it is not delivered
through `incoming()`. A longer reason or invalid UTF-8 throws.
`close(1005)`, `close(1006)`, and `close(1015)` throw. Those codes
are not written on the wire. A second `close()` throws.

The output pump writes the close frame after the current data frame,
if a send is in progress. It does not wait for the rest of the
outgoing message, and it does not cut a frame in the middle. The
same frame-boundary rule applies when the engine closes for 1002,
1007, or 1009. After the local close is queued, `beginSend()` throws.
Receive continues. Complete data messages still run `inputReady()`
until the peer close. The engine keeps reading until that close or
`idleTimeout()`, including when no `beginReceive()` is outstanding.

A received close is consumed by the parser. It does not become
`incoming()`, and it does not emit `inputReady()`. The engine
enqueues a close frame if it has not already sent one, under the
same pump rule. If a data message was open, its body is discarded.
`onInput()` does not run for that incomplete message. `incoming().type()`
is never close.

`closeCode()` and `closeReason()` are the values from that handshake.
A local close keeps the values passed to `close()` when the peer
sends none. A peer close with an empty payload is 1005 and an empty
reason, unless a local code was already set. An I/O error leaves the
code at 1006 and the reason empty, because no close frame arrived.
`closed()` stays the signal that the stream has ended. Peer close,
an I/O error, a close frame, and destruction of the stream all emit
it. `closed()` ends an outstanding send or receive. The application
does not call `endSend()` or `endReceive()` after it. Those calls
throw.

This keeps the one-transfer rule. The application still has one
outstanding data send. The engine may insert a ping, a pong, or a
close around the data frames of that send. A ping during a large
message is therefore possible without a second `beginSend()`.

## Progress {#wsm-progress}

`MessageProgress` is reused for both directions. `header()` means
the message type is known. `body()` means payload bytes were
processed on this step. `finished()` means the data message is
complete. Trailer is unused.

`inputReady()` runs when bytes were received. `outputReady()` runs
when bytes were sent. Neither waits for the whole message. A short
message often completes in one step, in which case type, body, and
finished are all true. A large message reports body bytes before
finished. The application reads `incoming().body()`, discards the consumed
body, and calls `beginReceive()` again until finished. `discard()`
does not change the type. What it reads is payload. It is not a
frame, and it is not aligned to a frame boundary. Discarding
consumed bytes does not reset `maxMessageSize()`, and it does not
skip the UTF-8 check of a text message. After `finished()`,
`clear()` drops the body and sets the type to `Unknown`.

The same step rule applies to send. `endSend()` returns
`MessageProgress`. The outgoing body is complete before the first
`beginSend()`. A large write still takes more than one step: the
application calls `endSend()`, and if the send is not finished it
calls `beginSend()` again. It does not append more body between
those steps. A `beginSend(false)` that produces the body while the
send is in progress can be added later.

Delivering only whole messages would match the browser and Beast. It
would also force the socket to buffer up to `maxMessageSize()` before
the first callback. HTTP does not do that for a reply body. This
chapter does not do it for a WebSocket message either.

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
user transfer. When the output is idle, the pump may start
`Stream::beginOutput()` to write a queued ping, pong, or close.

`idleTimeout()` restarts when a send or a receive step finishes, as
it does today. It is not restarted by an internal continuation alone
if that continuation did not complete a step the application saw.
A received ping that the engine answers restarts it, and so does a
received pong. The peer is alive. Writing a ping does not by itself
restart it. The answer does.

## Session callbacks {#wsm-session}

`WebSocketSession` does not grow a message accessor. The message is
reached through `incoming()` and `outgoing()` on the session itself.
`onInput()`, `onOutput()`, and `onClose()` still take no arguments.

`onInput()` runs when bytes were received, not when a frame has
been parsed. The derived session calls `endReceive()`. If the
progress reports body bytes, it reads `incoming()`. If the progress
is not finished, it calls `beginReceive()` again. If it is finished,
it handles the message and starts the next receive, or a reply. A ping or a pong does not run `onInput()`. A received pong runs
`onPong()` with its payload. The derived session uses that to
retire a keepalive. It does not read `incoming()` from `onPong()`.

`onOutput()` runs when bytes were sent. The derived session calls
`endSend()`. If the send is not finished, it calls `beginSend()`
again. If it is finished, it starts the next send when it still has
a message. The callback is not aligned to a fragment, and it does
not run for a ping or a pong.

`onClose()` is unchanged. It is the last look at the session. A close
handshake leaves `closeCode()` and `closeReason()` set. An I/O error
leaves the code at 1006 and the reason empty. `onClose()` does not
call `endReceive()` or `endSend()`. `closed()` has already ended
those transfers. A close in the middle of an incoming message does
not run `onInput()`.

The client uses the same two messages on the `WebSocket` it
constructed. It connects `inputReady()` and `outputReady()` itself.
There is still no client session.

## What changes {#wsm-changes}

`body()`, `frame()`, `available()`, `pending()`, and `discard()` move
from both facades to `WebSocketMessage`. `WebSocket` and
`WebSocketSession` forward nothing in their place except
`incoming()` and `outgoing()`.

`beginSend(Frame)` becomes `beginSend()`. The type is
`outgoing().setType()`. `WebSocket::Frame` leaves the public
surface. `WebSocketMessage::Type` is the only public data type:
`Unknown`, `Text`, and `Binary`. Ping, pong, and close are not
values of that type. `beginReceive()` and `endReceive()` stay.
`endReceive()` and `endSend()` return `MessageProgress`.

`sendPing()` and `sendPong()` leave the public surface. `ping()`
enqueues a ping of at most 125 bytes. A second `ping()` enqueues
another. Each received ping is answered with a pong that mirrors
the payload. A received pong, including an unsolicited one, is consumed as a
message, restarts the idle timer, and is reported on the facade.
Neither emits `inputReady()` or `outputReady()`, and neither frees
the data channel. There is no
`sendPong()`. `ping()` and `close()` throw if the handshake is not
finished. `close()` remains, and gains a status code and a reason.
The reason is UTF-8 and at most 123 bytes. `close(1005)`,
`close(1006)`, and `close(1015)` throw. A received close is answered
by the engine. After a local close, no more data frames are sent.
Receive continues until the peer close or `idleTimeout()`. A close
is written after the current data frame. `closed()` ends an
outstanding send or receive. `closeCode()` and `closeReason()`
report the handshake that ended the stream. An empty peer close is
1005 unless a local code was already set. An I/O error is 1006.
`close()` no longer writes the stream as a side door around the
output pump.

The receive path reassembles. The send path fragments. Control frames
between fragments are consumed or inserted by the engine.
`maxMessageSize()` limits one data message. It counts declared
payload bytes from the first data opcode to FIN, including bytes the
application has already discarded. It is not a frame limit and not a
buffer limit. Exceeding it on input closes the stream with 1009.
`beginSend()` of an outgoing message over the limit throws.

A text message must be valid UTF-8 across all of its fragments. The
engine checks the bytes as they arrive. Discarding them does not
remove them from the check. Invalid text from the peer closes the
stream with 1007. Invalid outgoing text throws. Binary is not
checked.

RSV bits without a negotiated extension, a reserved opcode, a
continuation with no open message, and a masking error close the
stream with 1002.

The session chapter's statement that `onInput()` means one whole
frame, and that `onOutput()` means one frame has left the buffer, is
superseded here. Each callback is an I/O step of a message. The
callback names stay.

## Out of scope {#wsm-scope}

This chapter does not change the upgrade boundary, the handshake
responder, or who owns the stream. It does not decide the client and
server facade split. It does not add permessage-deflate, subprotocols
as a message property, or an application-supplied message pool.
The selected protocol is a handshake result, owned by the session
chapter. Incremental send with a completion flag is allowed later
and is not the first surface. A separate unread-buffer or
flow-control limit is later. The first size limit is
`maxMessageSize()`.
