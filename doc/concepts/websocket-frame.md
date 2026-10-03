# WebSocket Frame {#websocket-frame}

This document is the design for the WebSocket frame object in Platinum
HTTP. It is a framework concept, not an application sketch. The goal is
that reading and writing a WebSocket uses the same message shape as an
HTTP request and reply: metadata and a body stream, owned by the
transport, one object per direction.

The HTTP upgrade boundary in
[HTTP Upgrade](../requirements/http-upgrade.md) stays as it is. The
session model in [WebSocket Session](websocket-session.md) stays as it
is. This chapter decides what a frame is, and how `WebSocket` exposes
the frame it sends and the frame it receives.

This chapter covers:

- [Purpose](#wsf-purpose)
- [Principles](#wsf-principles)
- [Current state](#wsf-current)
- [Object model](#wsf-model)
- [WebSocketFrame](#wsf-frame)
- [Input and output](#wsf-slots)
- [Send and receive](#wsf-transfer)
- [Control frames](#wsf-control)
- [Fragmentation](#wsf-fragment)
- [What stays](#wsf-stays)
- [What changes](#wsf-changes)
- [Out of scope](#wsf-scope)

## Purpose {#wsf-purpose}

An HTTP exchange already has a place for the bytes of one message.
`Request` and `Reply` are `Message` objects. Each has a header and a
`body()` iostream. A `Client` owns one request and one reply. The
caller fills the request and reads the reply. Send and receive do not
share a buffer. The method lives on the request, and the status lives
on the reply, next to the payload they describe.

A WebSocket transfer is the same kind of fact on a different protocol.
RFC 6455 carries opcode, FIN, and payload in one frame. The opcode is
what the method is on a request and what the status is on a reply. The
payload is the body. The stream contract already allows one active
transfer per direction, and it allows those directions to run at the
same time. That is two message slots, not one shared buffer.

Today `WebSocket` is both the stream session and the message. `body()`
is the payload of the last receive and the buffer of the next send.
`frame()` is the opcode of the last receive, while `beginSend()` takes
the opcode as an argument. Ping, pong, and close are methods beside
that path. A caller that still holds a received payload cannot prepare
the next outbound frame without first copying the received bytes out.
`discard()` is ambiguous, because it drops whichever payload the one
stream currently holds.

The frame is the missing object. `WebSocket` owns two of them, input
and output. The caller fills `output()` and sends it, and reads
`input()` after a receive. The socket remains the device that formats
frames into the upgraded stream and parses frames out of it. It does
not become a second HTTP message type, and the HTTP core still does
not know frames.

## Principles {#wsf-principles}

`WebSocketFrame` is not a `Message`. `Message` is bound to an HTTP
connection, header fields, chunked transfer, and keep-alive. A frame
has a 2- to 14-byte header, an opcode, a FIN bit, and a payload.
Sharing the iostream body surface is the analogy. Inheritance is not.
A frame that derived `Message` would pull HTTP into the session, which
is the upgrade boundary crossed in the other direction.

The HTTP core still knows no frames. `WebSocketFrame` lives in the
WebSocket group. `Service`, `Responder`, `Request`, and `Reply` do not
mention it. The handshake remains an HTTP exchange. The frame exists
only after the server has written the upgrade reply and opened the
stream.

The session still binds only the stream. A frame does not hold a
`Stream` or a `Connection`. `WebSocket` owns the frames and serializes
them into the stream it already binds. That is why `beginSend()` and
`beginReceive()` stay on the socket. `Request::beginSend()` lives on
the message because `Connection` is a friend of `Request`. Here the
session already owns the stream, so the socket operates on the frames.

One input frame and one output frame. The caller does not pass a frame
into `beginSend()` or `beginReceive()`. That matches `Client` owning
`request()` and `reply()`, and it matches one transfer per direction.
A queue of caller-owned frames would make signal lifetime and buffer
ownership a second design. It is not required by the stream contract.

Masking stays inside `WebSocket`. The client masks every frame it
sends, and the server rejects a masked frame it did not expect and
unmasks a frame the client sent. The frame API does not expose a mask.
RSV stays internal until an extension exists. An extension is a later
protocol feature, not a field the caller sets on an ordinary frame.

A receive still completes one whole frame. FIN is visible so a
continuation frame can be represented. Reassembly into a message is
not this type. `inputReady()` means one frame has arrived, as it does
today.

`sendPing()`, `sendPong()`, and `close()` may remain helpers. They
fill `output()` and send it, and they are valid only when no output
transfer is active. Close code and reason are accessors on a Close
frame, not a second message type.

## Current state {#wsf-current}

`WebSocket` formats frames into the stream buffer of an upgraded HTTP
connection. It is not an I/O device. The payload is `body()`, an
iostream, the same surface a `Message` uses for its body. The caller
writes that stream and sends it as one frame. A receive parses one
frame from the connection stream and leaves the payload in `body()`.

`beginSend(Frame)` writes one frame. The opcode is the argument. The
payload is what was written to `body()` since the previous send.
`outputReady()` reports that the frame has left the connection stream
buffer, and `endSend()` completes the send. `beginReceive()` reads one
frame. `inputReady()` reports that the frame is complete.
`endReceive()` completes the read. `frame()` is the opcode, and
`body()` then holds the payload. `available()` is how many of those
bytes can be read. A short read stays inside the socket until the
frame is complete.

Ping and pong are control frames, but the public path is
`sendPing()` and `sendPong()` rather than a frame the caller fills.
`close()` writes a close frame and closes the stream. The opcode
enumeration is nested on `WebSocket` as `Frame`, with `Unknown`,
`Text`, `Binary`, `Ping`, `Pong`, and `Close`. There is no FIN flag
and no continuation opcode on that enumeration.

The session chapter maps `Request` and `Reply` to `WebSocket`. That
mapping names the device, not the message. This chapter splits that
cell. The device stays `WebSocket`. The message becomes
`WebSocketFrame`.

## Object model {#wsf-model}

The split follows the HTTP message split, one level below the session.

| HTTP | WebSocket |
| --- | --- |
| `Client` or `Connection` | `WebSocket` |
| `request()` | `output()` |
| `reply()` | `input()` |
| `Request` and `Reply` | `WebSocketFrame` |
| method, status | opcode, FIN |
| `Message::body()` | `WebSocketFrame::body()` |

`WebSocket` remains the frame device on both sides. It formats frames
into a `Stream` and parses frames out of it. On the client the
application constructs it with a `Client` and performs the handshake.
On the server the session contains it and binds it to the upgraded
stream. Neither side constructs a frame. The socket constructs the two
frames and returns them from `input()` and `output()`.

```text
WebSocketSession
    |
    +-- WebSocket --> Stream --> Connection
            |
            +-- input()   WebSocketFrame
            +-- output()  WebSocketFrame
```

A client has the same two frames on the `WebSocket` it constructed.
The session type is still server-only. The frame type is not.

## WebSocketFrame {#wsf-frame}

`WebSocketFrame` is one WebSocket frame: an opcode, a FIN bit, and a
payload stream. It is not a session, not a stream, and not an HTTP
message.

```cpp
class WebSocketFrame
{
    public:
        enum Type
        {
            Continuation,
            Text,
            Binary,
            Close,
            Ping,
            Pong
        };

        Type type() const;
        void setType(Type type);

        bool fin() const;
        void setFin(bool fin);

        std::iostream& body();
        std::size_t available() const;
        std::size_t pending() const;
        void discard();
        void clear();

        unsigned short closeCode() const;
        void setCloseCode(unsigned short code);
        const std::string& closeReason() const;
        void setCloseReason(const std::string& reason);
};
```

`type()` is the opcode. `Text` and `Binary` are data. `Continuation`
is a following fragment of a data message. `Close`, `Ping`, and `Pong`
are control. There is no `Unknown`. A frame the caller has cleared is
`Text` with FIN set and an empty body, the same way a new `Request`
is GET and a new `Reply` is 200. A frame the socket has just received
has the opcode the peer sent.

`fin()` is the FIN bit. A data message that fits in one frame has FIN
set. A fragmented message clears FIN on every frame but the last and
uses `Continuation` after the first. Control frames are always FIN.
`setFin(false)` on a control type is an error when the socket sends
that frame.

`body()` is the payload iostream. The caller writes it before
`beginSend()` and reads it after `endReceive()`. `available()` is how
many payload bytes can be read. `pending()` is how many payload bytes
are waiting to be sent. `discard()` drops the buffered payload and
leaves the opcode and FIN as they are. `clear()` resets opcode, FIN,
close fields, and payload so the same frame can be filled again.

`closeCode()` and `closeReason()` are meaningful when `type()` is
`Close`. On send, the socket writes the code and reason into the
payload. On receive, the socket parses them out of the payload and
leaves the raw payload in `body()` as well, so a caller that wants the
bytes still has them. For any other opcode the close accessors are
empty and setting them does not change the frame the socket writes.

The frame does not expose the mask, the RSV bits, or the header
length. Those are wire details of the socket.

## Input and output {#wsf-slots}

`WebSocket` owns the two frames for the life of the socket.

```cpp
class WebSocket
{
    public:
        WebSocketFrame& input();
        const WebSocketFrame& input() const;

        WebSocketFrame& output();
        const WebSocketFrame& output() const;

        void beginSend();
        void endSend();

        void beginReceive();
        void endReceive();
};
```

`output()` is the frame the next `beginSend()` writes. The caller sets
the opcode and FIN, writes `output().body()`, and calls `beginSend()`.
`input()` is the frame the next `beginReceive()` fills. After
`endReceive()`, `input().type()` is the opcode and `input().body()`
holds the payload.

The two frames are distinct buffers. A receive does not overwrite the
outbound payload, and a send does not consume the inbound payload. The
caller may fill `output()` while a receive is in progress, and may
read `input()` while a send is in progress. That is the concurrency
the stream contract already allows.

One transfer per direction still holds. A second `beginSend()` before
`endSend()` is an error. A second `beginReceive()` before
`endReceive()` is an error. `discard()` on a frame that has a transfer
in flight is an error. `clear()` is the same. The caller finishes the
transfer, then reuses the frame.

`body()` and `frame()` on `WebSocket` go away. They were the single
shared slot. `available()` and `pending()` on the socket go away with
them. The same questions are `input().available()` and
`output().pending()`.

## Send and receive {#wsf-transfer}

A send is fill, begin, ready, end.

```cpp
WebSocketFrame& out = socket.output();
out.setType(WebSocketFrame::Text);
out.setFin(true);
out.body() << "hello";
socket.beginSend();
```

`outputReady()` reports that the frame has left the connection stream
buffer. `endSend()` completes the send. The output frame still holds
what was sent. The caller clears it or overwrites it before the next
send. Nothing in `endSend()` discards the body. That matches a reply
that stays readable after it has been sent.

A receive is begin, ready, end, read.

```cpp
socket.beginReceive();
```

`inputReady()` reports that one whole frame is complete.
`endReceive()` completes the read. The caller then reads
`input().type()` and `input().body()`. A short read stays inside the
socket until the frame is complete, so the ready signal still means
one whole frame. The input frame is replaced by the next successful
receive, not by a send.

On the server the session callbacks do not change shape. `onInput()`
still runs when one whole frame has been received. The derived session
calls `socket().endReceive()`, reads `socket().input()`, and starts
the next receive or a reply. `onOutput()` still runs when a frame has
left the stream buffer. The derived session calls `socket().endSend()`
and starts the next send when it still has data.

The first transfer still starts in the derived session constructor, or
in the client slot that handles `connected()`. The frame objects
already exist at that point. The constructor does not create them.

## Control frames {#wsf-control}

Ping, pong, and close are frames. The caller can fill `output()` with
`Ping`, `Pong`, or `Close` and send it, the same way it sends text.

The helpers stay for the common case.

```cpp
void sendPing();
void sendPong();
void close();
```

`sendPing()` and `sendPong()` set the output opcode, leave the payload
empty unless the caller has already written `output().body()`, and
send that frame. They are valid only when no output transfer is
active. A ping that must carry a payload is an ordinary send of a
`Ping` frame. The helper does not invent a second buffer.

`close()` sets the output type to `Close`, writes the close code and
reason the caller set on `output()`, sends that frame, and then closes
the stream. A caller that needs a code calls `setCloseCode()` and
`setCloseReason()` on `output()` before `close()`. The default code is
1000. `close()` is still the shutdown path from the session chapter.
It is not a bare `StreamSession::close()`, because that would skip the
close frame.

A received close frame arrives on `input()` like any other frame.
`onInput()` sees `input().type() == WebSocketFrame::Close`, reads
`closeCode()` and `closeReason()`, and the socket then ends the
stream. The session `onClose()` still runs after the stream has ended.
A close frame is not a substitute for `onClose()`.

A received ping is also an input frame. The socket does not answer it
by itself in this design. The session that wants the usual peer
behavior sends a pong from `onInput()`. Automatic pong is a policy of
the session, not a hidden write inside the frame parser. A hidden
write would break the one-output-transfer rule whenever the session
already had a send in flight.

## Fragmentation {#wsf-fragment}

The ready signal means one frame, not one reassembled message. A peer
may split a data message into a first frame with FIN clear and one or
more `Continuation` frames, the last of them with FIN set. The socket
delivers each of those frames on `input()`. It does not concatenate
them.

`setMaxMessageSize()` stays on the socket. It closes the stream when a
single frame exceeds the limit. A limit across a fragmented message is
a later check, applied by the session that reassembles, or by the
socket once reassembly exists. This chapter does not add that check.

The caller that wants a whole message reads `input()` until a data
frame with FIN set has been seen, and copies the payloads aside. That
copy is application code. `WebSocketFrame` is the wire unit. A
`WebSocketMessage` that hides fragments is a later type, and only if a
caller task needs it. The name of this type stays frame so the two are
not confused.

UTF-8 validation of a text message is the same later check. The socket
does not validate text payload in this chapter. A text frame is an
opcode plus bytes.

## What stays {#wsf-stays}

The upgrade boundary is unchanged. The HTTP core knows no frames,
masking, or ping and pong. The responder writes the upgrade reply. The
server writes that reply completely before any session byte is
written. The server then opens a `Stream` and delivers it to the
upgrade handler. The handler accepts by binding a session, or declines.
The server closes a stream that has no bound session.

The session model is unchanged. `WebSocketSession` contains one
`WebSocket`. The socket is the frame device. `onInput()`,
`onOutput()`, and `onClose()` take no arguments. The service creates
the session and the server releases it. The session owns neither the
stream nor the connection.

The stream contract is unchanged. At most one transfer is active per
direction. Input and output may run concurrently. Completing or
cancelling a stream affects only that logical stream. Timeouts stay on
the socket: the idle timeout, the transfer timeout, and the maximum
frame size.

Client handshake and server accept are unchanged. `beginConnect()` and
`endConnect()` still perform the client handshake. The server still
does not construct a `WebSocket`. Binding still accepts the upgrade.

## What changes {#wsf-changes}

`WebSocket::Frame` moves to `WebSocketFrame::Type` and gains
`Continuation`. `Unknown` goes away. `beginSend()` takes no opcode.
The opcode and FIN are already on `output()`.

`WebSocket::body()`, `WebSocket::frame()`, `WebSocket::available()`,
`WebSocket::pending()`, and `WebSocket::discard()` go away. The frame
accessors replace them. Call sites that wrote `socket.body()` before
`beginSend(WebSocket::Text)` write `socket.output()` instead. Call
sites that read `socket.frame()` and `socket.body()` after
`endReceive()` read `socket.input()` instead.

The session chapter's table cell that maps `Request` and `Reply` to
`WebSocket` is read as the device. The message mapping is the table in
this chapter. The session text that says `body()` is the payload
stream and `frame()` is the opcode is superseded here. The callbacks
and the lifetime are not.

`sendPing()`, `sendPong()`, and `close()` stay, but they are defined
on top of `output()` rather than on a private buffer. A call while an
output transfer is active is an error.

## Out of scope {#wsf-scope}

Message reassembly, UTF-8 checks, and a maximum size summed across
fragments are out of scope. So are RSV bits, `permessage-deflate`, and
any other extension. A caller-owned frame queue, a frame pool, and
passing a frame into `beginSend()` or `beginReceive()` are out of
scope. Automatic pong is out of scope.

The HTTP core does not change. `Message`, `Request`, `Reply`,
`Service`, and `Responder` do not gain a frame type. The upgrade
requirements do not change. The session type does not change, except
that its chapter's description of `body()` and `frame()` follows this
one once the API does.
