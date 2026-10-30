# WebSocket

A WebSocket is a bidirectional stream of messages after an HTTP
upgrade. The application sends and receives text or binary messages.
Framing, fragmentation, masking, and control frames stay inside the
engine. The HTTP core knows no WebSocket frames.

This is the model. Implementation state lives in the proposal.

## Two facades

`WebSocket` is the client. The application constructs it, holds the
`Client` that performs the handshake, and stores its own state beside
it. After a finished handshake the client still owns the connection
and the stream.

`WebSocketSession` is the server. The service creates one session per
accepted stream. The application derives the session and holds its
domain there. The session is the server facade. There is no socket
beside it.

Both expose the same message operations. One accepted stream is one
bind. A second bind throws.

## Message

The public unit is a message. A message is text or binary. It has a
body. It has no opcode, no FIN bit, no mask, and no RSV bits.

Each facade owns two messages, `incoming()` and `outgoing()`, the way
`Client` owns `request()` and `reply()`. The application does not
construct them. They live for the life of the stream.

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

`Unknown` means no data type yet. The application sets the type of
the outgoing message. The engine sets the type of the incoming
message from the first data frame. `discard()` drops the buffered
body and leaves the type. `clear()` drops the body and sets the type
to `Unknown`. After a finished message the application calls
`clear()`.

## Two directions

One receive and one send may be outstanding. A begin is outstanding
until the matching end. A second begin on the same direction throws.
After an end that is not finished, the next begin continues the same
message. After `finished()`, the next begin starts a new message.
Starting one direction does not cancel the other.

The matching ready notification completes a step. `endSend()` and
`endReceive()` are valid only after `outputReady()` or `inputReady()`,
respectively, or after the corresponding I/O failure notification.
They throw `std::logic_error` while the operation is still pending or
after a local `close()`.

```cpp
void beginSend(bool finished = true);
MessageProgress endSend();
Signal<WebSocket&>& outputReady();

void beginReceive();
MessageProgress endReceive();
Signal<WebSocket&>& inputReady();
```

A send writes `outgoing()`. The application sets the type, writes
the body, and calls `beginSend()`. `beginSend(false)` means more
body will follow. `beginSend(true)` is the last part of the
message. `beginSend()` throws if the type is `Unknown`.
`outputReady()` reports that data bytes were sent. `endSend()`
returns `MessageProgress`. The engine may still split this step
into several frames. The application does not see that split.

A receive fills `incoming()`. `inputReady()` reports that data bytes
were received. `endReceive()` returns `MessageProgress`. If the
receive is not finished, the application discards the consumed body
and calls `beginReceive()` again. The type stays. `finished()` means
the data message is complete.

Progress follows HTTP. Either end can report body bytes before the
message is finished. `finished()` means the message is complete, not
that a frame was parsed. The application does not learn how many
frames those bytes came from. Neither ready signal waits for the
whole message.

`beginSend()` with the default completion flag has a complete body
before the call. Partial `outputReady()` steps still occur when the
engine splits that body. `beginSend(false)` produces the body while
the send is in progress.

## Frames stay internal

The engine is the only code that writes or parses a frame.

On input it reassembles until FIN. A continuation extends the current
data message and does not change its type. A control frame between
two fragments is consumed by the control plane. It does not become
`incoming()`, it does not emit `inputReady()`, and it does not finish
the data message. A new data opcode before FIN is a protocol error.

On output the engine fragments the body at its frame limit. Every
fragment but the last has FIN clear. The last has FIN set. A client
masks. A server does not. The application does not supply a mask.

`beginReceive()` starts frame processing. Until then, frames stay in
the stream. During a receive the engine consumes control frames and
delivers data payloads to `incoming()` through `inputReady()`. The
application consumes or discards those bytes before it starts the
next receive step. One direction is never cancelled to free the
other.

A text message is UTF-8 over the whole message, not over one
fragment. The engine validates it as the bytes arrive. A sequence
that ends in the middle of a code unit is valid so far if the next
fragment can complete it. It is invalid when FIN arrives and the
sequence is still incomplete, and it is invalid as soon as a byte
cannot continue the sequence. `discard()` does not turn the check
off. Invalid text from the peer fails the stream with close code
1007. Binary is not checked.

`maxMessageSize()` limits one data message. The count runs from the
first data opcode to FIN, including bytes the application has already
discarded. It is not a frame limit and not a buffer limit. A declared
length that would exceed it fails the stream with close code 1009
before that payload is buffered. `beginSend()` of an outgoing message
over the limit throws. Zero disables the limit. How much unread data
the engine holds ahead of the application is flow control. That limit
is later.

RSV bits without a negotiated extension, a reserved opcode, a
continuation with no open message, and a masking error each fail the
stream with close code 1002. An extension, if one is added later, is
negotiated at the handshake and applied by the engine before the body
is visible.

Failing the stream is not a close handshake. The frame stream is
unusable from that point, so the engine completes an outstanding
receive with that error, writes its close frame, and does not wait
for the peer to answer. The stream stays bound. `closeCode()` is the
code of the violation. A later `beginReceive()` throws the same
error. When no receive is outstanding, `closed()` runs. The
application calls `close()`.

## Control plane

Ping and pong are socket operations. They are not a
`WebSocketMessage` and not a second public write. They do not emit
`inputReady()` or `outputReady()`. They do not finish a data message,
and they do not free the data channel.

The engine sees an incoming control frame only while it reads, that
is while a receive or a close handshake holds the input. An endpoint
that never starts a receive answers neither a ping nor a close.

```cpp
void ping(const char* payload = 0, std::size_t n = 0);
Signal<WebSocket&, const std::string&>& pong();
```

`ping()` enqueues a ping of at most 125 bytes. A longer payload
throws. A second `ping()` enqueues another. Each is written, and the
peer answers each. `ping()` does not require the data send to be
idle. A ping may be written between two data fragments of the current
message. `outputReady()` still means that data bytes were sent. A
close frame is the last frame this side writes, so a `ping()` after a
close is queued throws.

A received ping is answered by the engine with a pong of the same
payload. The application does not call `sendPong()`. Answering the
ping restarts the idle timer. A received ping is not delivered. A
ping that arrives after a close is queued restarts the idle timer and
is not answered. A signal for a received ping can be added later.

A received pong is consumed. It does not become `incoming()`. If a
`ping()` is still outstanding, the payload is compared and that ping
is retired. An unsolicited pong is valid. The facade sees the pong:
`pong()` on the client, `onPong()` on the session. An unanswered ping
is facade state. The application may shut the stream down from it.
The idle timeout remains the limit without a finished transfer.

## Close

Ending a WebSocket is three events. `shutdown()` is the close
handshake the application starts. `closed()` is the lifetime event
when the engine ends the protocol. `close()` unbinds the stream.

```cpp
void shutdown(unsigned code = 1000,
              const std::string& reason = std::string());
Signal<WebSocket&>& closed();
void close();

unsigned closeCode() const;
const std::string& closeReason() const;
```

`shutdown()` enqueues a close frame. The code defaults to 1000. The
reason is UTF-8 and at most 123 bytes. A longer reason or invalid
UTF-8 throws. `shutdown(1005)`, `shutdown(1006)`, and
`shutdown(1015)` throw. Those codes are not written on the wire. A
second `shutdown()` throws. After a local close is queued,
`beginSend()` throws. The close frame is written after the current
data frame. It does not wait for `endSend()` and does not cut a frame.
After a local close is queued, the engine keeps reading until the
peer close or `closeTimeout()`, even when no `beginReceive()` is
outstanding. A peer that never starts a receive never answers, so
that timeout is the bound on the wait. Zero leaves the wait
unbounded. The client and the service default it to ten seconds. During that close-only phase it discards data frames and
continues to process control frames. When a receive is outstanding,
data that arrives before the peer close is still delivered normally.
The handshake completes through `shutdownReady()`. `endShutdown()`
returns that operation to idle or throws. The application then calls
`close()`. `closed()` does not run for this handshake.

A received close is consumed. It does not become `incoming()` and
does not emit `inputReady()`. If this side has not sent a close
frame, the engine answers with a close frame. A peer code that must
not be written is not echoed. The reply is 1000. If a data message
was open, its body is discarded. After the reply has been written,
the protocol has ended. The engine does not call `close()` on the
facade and does not unbind the stream. Send, receive, and ping that
are still outstanding stay pending. `beginSend()`, `beginReceive()`,
`beginPing()`, and `beginShutdown()` throw. `closeCode()` and
`closeReason()` are already set.

When no local shutdown is outstanding, that protocol end is
`closed()` on the client and `onClosed()` on the session. The
application calls `close()` there. `close()` unbinds the stream,
cancels pending operations without their ready signals, and emits
nothing. The destructor calls it. An idle timeout and a read that
returns no bytes take the same path, with close code 1006.

One event has one reporter. An outstanding shutdown reports the
handshake. An I/O or protocol error on an outstanding receive
reports that receive. `closed()` runs when neither of those
operations reports the end.

An I/O error on an outstanding receive or send stores the exception
and leaves the stream open. The ready signal of that transfer runs,
and the matching end throws. The other direction is unaffected. The
application calls `close()`. The direction that failed keeps the
reason: a later `beginSend()`, `beginPing()`, or `beginShutdown()`
throws the write failure, and a later `beginReceive()` throws the
read failure.

`closeCode()` and `closeReason()` are the values from the handshake
that ended the stream. A local `shutdown()` keeps the values passed
to it when the peer sends none. An empty peer close is 1005 unless a
local code was already set. No close frame leaves the code at 1006.

## Session

`WebSocketSession` is the per-stream application object. It is a
`Connectable`. It is not a responder.

`onInput()` and `onOutput()` take no arguments. They do not receive
the session or a socket again. `onInput()` runs when data bytes were
received. `onOutput()` runs when data bytes were sent. Neither runs
for a ping, a pong, or the end of the stream. `onClosed()` runs when
the engine has ended the protocol and no local shutdown is
outstanding. The application calls `close()` from `onClosed()`, or
from `onShutdownReady()` after `endShutdown()`.

`WebSocketSession::close()` ends the stream and tells the service.
The destructor unbinds the stream and does not release the session
again. A domain object that holds a session reference drops it
before it calls `close()`. `close()` from `onClosed()` destroys the
session before `onClosed()` returns.

The session does not store the opening request or reply. Both are
valid only for the call that creates the session. `protocol()` is the
single name written on the 101, or empty when none was selected.

The event loop of the upgrade is the loop of the stream. The session
does not take it as a constructor argument. A derived session that
attaches a timer reads it from the stream.

## Service

`WebSocketService` is the factory. It owns the endpoint policy and
counts the sessions it has handed out. A `WebSocketServlet` owns the
live sessions. It must outlive every session and end before the
service. When the servlet ends, it releases its remaining sessions
while the derived service is still fully constructed.

`getSession()` creates a session and counts it. `releaseSession()`
destroys it and drops the count. The two must match, including the
allocator. A null return from `getSession()` declines the upgrade
before any bind. `socketCount()` is that count. It is not the size of
the servlet list.

`maxSockets()`, `idleTimeout()`, `pingInterval()`, `pingTimeout()`,
`closeTimeout()`, and `maxMessageSize()` are endpoint policy. They
apply before a session is constructed. No attached
servlet, or a count at the limit, is 503 from the handshake. No
stream is opened.

`Service::onUpgrade()` stays the generic HTTP boundary. The
application does not override it to receive WebSocket streams. It
implements `onGetSession()` and the session callbacks.

A pooled session is released back to the pool from
`onReleaseSession()`, not destroyed. The next upgrade opens a new
frame connection before the pooled object is handed out. One session
object does not serve two streams at once.

## Handshake

The handshake stays an HTTP exchange. The handshake responder is not
the session.

On the client, headers set before `beginConnect()` are kept.
`endConnect()` accepts only status 101, and only when
`Sec-WebSocket-Accept` is the digest of the key this socket sent. A
missing or different Accept is a failed handshake. No stream is
opened. The key and the masking key come from an unpredictable
source.

On the server, a missing or invalid key is 400. A version other than
13 is 426 with `Sec-WebSocket-Version: 13`. No stream is opened.

`addProtocol()` offers one subprotocol name. Several calls offer
several names, in order. The selected name, if the server echoes one,
is `protocol()` after the handshake. No echo means no protocol was
agreed.

## Timeouts

The handshake is an HTTP exchange, so its I/O bound is the HTTP
timeout. On the client that is `Client::setTimeout()`. On the server
it is `Server::setTimeout()`, and keep-alive between requests. A
finished 101 stops those clocks on the connection.

After the upgrade the socket has four engine clocks. Zero disables
each one except close, which defaults to ten seconds.

`setIdleTimeout()` is inbound silence while the socket is open.
Incoming data, a received ping, and a received pong restart it.
Sending does not. The client and the service start at zero.

`setPingInterval()` is inbound silence before the engine writes a
ping. Incoming data, a received ping, and a received pong restart
it. Sending does not. The engine writes an empty ping only while
the socket is open and a receive is outstanding. That ping uses the
same output as `beginPing()` and does not emit `pingReady()`. A
tick is skipped when a ping is outstanding or a ping wait is
already running. `beginPing()` throws while that ping is
outstanding. The client and the service start at zero.

`setPingTimeout()` bounds an unanswered ping. It starts when
`beginPing()` enqueues the frame, and when the engine writes a
keepalive ping. A pong stops it. An incoming ping does not.
The client and the service start at zero.

`setCloseTimeout()` bounds `beginShutdown()` after a local close is
queued. The client and the service default it to ten seconds.

An idle, ping, or close timeout ends the protocol with close code
1006. The server copies all four clocks from the service onto each
session.

The stream still has `setTimeout()` for a channel that bounds byte
I/O.

## HTTP/2

HTTP/2 uses Extended CONNECT. A successful reply has status 200. The
resulting stream is one multiplexed stream, not the whole connection.
Each Extended CONNECT is a new session. Several sessions of one
connection may run on the same event loop and may share the service.
Per-stream state stays in the session. Shared state stays on the
service or in the domain.

## Out of scope

Permessage-deflate. Subprotocols as a message property. An
application-supplied message pool. Incremental send with a completion
flag. A separate unread-buffer limit. Those can be added later
without changing the message as the public unit.
