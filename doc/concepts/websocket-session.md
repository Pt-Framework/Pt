# WebSocket Session {#websocket-session}

This document is the design for the server-side WebSocket session in
Platinum HTTP. It is a framework concept, not an application sketch.
The goal is that an accepted WebSocket upgrade produces one
application object that owns the state of that connection, holds
references into the application's domain, and stays alive across
reads and writes.

The HTTP upgrade boundary in [HTTP Upgrade](http-upgrade.md) stays as
it is. This chapter decides what the WebSocket service does with the
stream that boundary delivers.

This chapter covers:

- [Purpose](#wss-purpose)
- [Principles](#wss-principles)
- [Current state](#wss-current)
- [Object model](#wss-model)
- [WebSocket](#wss-socket)
- [WebSocketSession](#wss-session)
- [WebSocketService](#wss-service)
- [Handshake responder](#wss-handshake)
- [Lifetime](#wss-lifetime)
- [Writing a stream](#wss-write)
- [HTTP/2](#wss-http2)
- [Decline and failure](#wss-decline)
- [What changes](#wss-changes)
- [Out of scope](#wss-scope)

## Purpose {#wss-purpose}

An ordinary HTTP exchange already has a place for application state.
`Service` creates a `Responder` for one request, the responder writes
the reply, and the service releases the responder when the exchange
ends. The application derives the responder and stores domain
references there.

A WebSocket upgrade is not that exchange. The handshake responder is
released before the protocol session begins. What remains is a
bidirectional stream that outlives the HTTP reply. Frames arrive and
leave over a long time. Anything the application wants to remember
between those frames, a subscription, a cursor, a user, a feed, has
to live somewhere that is created with the upgrade and destroyed with
the stream.

Today that place does not exist. `WebSocketService::onUpgrade()`
constructs a bare `WebSocket`, stores it, and emits `accepted()`. The
slot receives a reference to a socket the service owns. Application
state has to be attached from outside, usually by a map from the
socket address to some other object. That is the gap this design
closes.

The session is the server-side peer of the responder. The service
creates one session per accepted upgrade. The application derives the
session, holds its domain there, and implements the frame callbacks.
The socket is a member of that session. The application reaches it
through an accessor. Callbacks do not pass it again.

## Principles {#wss-principles}

`WebSocket` is the frame device on both sides. It formats frames into
a `Stream` and parses frames out of it. It is not an I/O device, and
it is not the application context. The client constructs it with a
`Client` and performs the handshake. The server never does that. On
the server the socket only accepts a stream that the handshake has
already upgraded.

The session is the application context, and only the server has one.
A client already owns the `WebSocket` it constructed, so it can store
its own state beside that object. A server must mint one context per
accepted stream, because the application did not construct the
stream.

One accepted upgrade is one `WebSocketSession` and one `WebSocket`.
The session contains the socket. They are created together and
destroyed together. A second socket inside the same session is not
part of the model. HTTP/2 does not change that. Extended CONNECT
produces another stream, and that stream gets another session.

The service is the factory, the same role `Service` has for
responders. It creates the session, applies limits, and releases the
session when the stream ends. It does not become the place where
application code keeps per-connection state.

The handshake stays an HTTP exchange. `WebSocketResponder` answers
the upgrade request. It is not the session, and it is not renamed
into one. The name is already taken by that handshake type. The
session type is `WebSocketSession`.

`Service::onUpgrade()` stays the generic HTTP boundary. The HTTP core
still knows no WebSocket frames. `WebSocketService` implements that
boundary and does not pass it on to the application. The application
implements `onGetSession()` and the session callbacks.

The event loop is an explicit argument of session construction. The
application does not recover it by walking from the socket to the
stream. The loop passed to the session is the loop that serializes
that stream. A different loop is an error.

No callback repeats an object the session already holds. `onInput()`,
`onOutput()`, and `onClose()` take no arguments. The socket, the
service, and the loop are members, reached through accessors.

## Current state {#wss-current}

`WebSocket` is a `StreamSession`. The client constructor stores a
`Client&` and does not own it. `beginConnect()` sends the handshake.
`endConnect()` completes it. After a successful 101 the socket binds
the stream the client owns and no longer uses the client.

The server constructor takes a `Stream&` and binds it immediately.
`accept()` is the same bind, used when the socket was not constructed
from a stream. `beginConnect()` throws if the socket has no client.
So the type already has two entries, and the server entry does not
perform a handshake.

`WebSocketService` is a `Service`. Its responder answers a WebSocket
upgrade with 101, with 503 when `maxSockets()` accepted sockets are
already open, or with 404 when the request is not a WebSocket
upgrade. After the 101 the server calls `Service::onUpgrade()`. The
service constructs a `WebSocket`, applies `maxMessageSize()` and
`idleTimeout()`, connects `closed()`, pushes the socket into an
internal vector, and emits `accepted()`.

`closed()` erases the socket from that vector. The service destructor
deletes every socket still tracked. The application does not free the
socket. It also cannot replace the socket with a derived type. The
only extension point is the `accepted()` slot, and that slot runs
after the socket already exists, with no application object attached
to it.

`Responder` cannot fill this role. The server releases the responder
before `onUpgrade()` runs. A responder that stored domain state for
the coming session would already be gone when the first frame
arrived.

The upgrade concept already says that the handler accepts the stream
by binding a session, or declines it, and that the server closes a
stream that has no bound session. What is missing is the session type
the application implements.

## Object model {#wss-model}

The split follows the HTTP server split.

| HTTP | WebSocket |
| --- | --- |
| `Service` | `WebSocketService` |
| `Responder` | `WebSocketSession` |
| `Request` and `Reply` | `WebSocket` |

`WebSocketService` is mapped with a servlet like any other service.
Its handshake responder finishes the HTTP exchange. On a successful
upgrade the service creates a `WebSocketSession`. The session
contains a `WebSocket`. The socket binds the `Stream` the server
already owns.

The application derives `WebSocketSession`. That derived object is
where a subscription, a cursor, or a reference to a feed lives. The
service owns the session. The session does not own the stream, and
the stream does not own the session. Binding is still the accept, as
`StreamSession` defines it. The socket is the session that binds.
The `WebSocketSession` is the application object around that bind.

```text
servlet --> WebSocketService
                 |
                 +-- WebSocketResponder   (one HTTP handshake)
                 |
                 +-- WebSocketSession      (one accepted stream)
                          |
                          +-- WebSocket --> Stream --> Connection
```

A later protocol that also uses `Service::onUpgrade()` does not
derive `WebSocketSession`. It binds its own `StreamSession`. The
session type in this chapter is the WebSocket case only.

## WebSocket {#wss-socket}

`WebSocket` stays the framed device described by its class chapter.
`body()` is the payload stream. `beginSend()` writes one frame from
that body. `beginReceive()` reads one frame into it. `frame()` is the
opcode. Ping, pong, and close stay control operations on this type.

On the client nothing in this design changes. The application
constructs `WebSocket` with a `Client`, connects `connected()`, and
calls `beginConnect()`.

On the server the application does not construct a `WebSocket`. The
session does. `WebSocket(Stream&)` becomes the server entry the
session uses, and it is not part of the application-facing server
API. `accept()` remains the bind. `beginConnect()` remains a client
operation and still throws when the socket has no client.

The socket remains a `StreamSession`. That is the bind to one
`Stream`. `WebSocketSession` does not also derive `StreamSession`.
Two session bases on one stream would be a second bind, and a second
bind throws.

`closed()` stays the socket signal that the stream has ended. The
session connects it. Application code uses `onClose()` on the
session, not the socket signal.

## WebSocketSession {#wss-session}

`WebSocketSession` is the per-stream application object. It is a
`Connectable`. It is not a `StreamSession` and not a `Responder`.

```cpp
class WebSocketSession : public Connectable
{
    public:
        WebSocketSession(WebSocketServer& server,
                         System::EventLoop& loop,
                         Stream& stream);
        ~WebSocketSession();

        WebSocketService& service();
        System::EventLoop& loop();
        WebSocket& socket();

    protected:
        virtual void onInput() = 0;
        virtual void onOutput() = 0;
        virtual void onClose() = 0;

    private:
        WebSocketService* _service;
        System::EventLoop* _loop;
        WebSocket _socket;
};
```

The base constructor receives the server, the loop, and the stream.
The service is `server.service()`.
It binds `socket()` to that stream, copies `maxMessageSize()` and
`idleTimeout()` from the service onto the socket, stores the loop,
and connects the socket signals to `onInput()`, `onOutput()`, and
`onClose()`. The stream is an argument. It is not recovered from
thread-local state.

The derived constructor runs after that. Its members are initialized,
`socket()` is open, and `loop()` is the loop that serializes this
stream. That constructor holds the domain references and starts the
first transfer, `beginReceive()` or `beginSend()`. There is no
`onAccept()`. By the time the derived constructor body runs, the base
has already done the work an accept callback would do: the stream is
bound, the limits are set, and the loop is known. A callback that
only restates the constructor would force every session to implement
an empty entry point.

`onInput()`, `onOutput()`, and `onClose()` stay. They are later
events. A constructor cannot receive them. They take no arguments.
The session, the socket, and the loop are the same objects the
constructor stored. Passing them again would imply that a callback
might see a different socket or a different loop, and it does not.

`service()` is how a session reaches shared state: the feed, the
registry, the limits, anything that belongs to every connection of
this endpoint rather than to one stream. `loop()` is how a session
attaches a timer or posts work onto the loop that owns this stream.
`socket()` is the frame device.

The loop is an explicit constructor argument because recovering it
from `socket().stream()->loop()` is an implicit walk through two
objects, and it is unavailable as a stable accessor. The server
passes the loop that already serializes the stream. The base
constructor rejects a different loop. All callbacks of one session
run on that loop. The session does not migrate.

`onInput()` runs when one whole frame has been received. The derived
session calls `socket().endReceive()`, reads `socket().body()`, and
starts the next receive or a reply. `onOutput()` runs when a frame
has left the stream buffer. The derived session calls
`socket().endSend()` and starts the next send when it still has data.
`onClose()` runs while the session object is still alive. The stream
has already cleared its session pointer. Peer close, an I/O error, a
close frame, and destruction of the stream all end here. The service
releases the session after `onClose()` returns.

The destructor of the base closes the socket, which closes the stream.
If the derived constructor throws, that destructor still runs, so a
session that fails during construction does not leave an accepted
stream without an owner.

## WebSocketService {#wss-service}

`WebSocketService` is the factory and the endpoint policy. It does
not keep the sessions it creates.

```cpp
class WebSocketService : public Service
{
    protected:
        virtual WebSocketSession* onGetSession(WebSocketServer& server,
                                               System::EventLoop& loop,
                                               Stream& stream) = 0;
        virtual void onReleaseSession(WebSocketSession* session) = 0;
};
```

`onGetSession()` creates the session. `onReleaseSession()` destroys
it. The two must match, including the allocator, as
`onGetResponder()` and `onReleaseResponder()` must match.
`BasicWebSocketService<S>` is that factory for one session type.

A `WebSocketServer` is the owner of the sessions. It is constructed
with the service and destroyed before it. That construction order is
the lifetime guarantee: when the server destructor releases the last
session, the derived service is still fully constructed. The service
destructor does not release sessions. One service has one registered
server. A second server replaces the registration. The previous
server keeps the sessions it already accepted.

`onUpgrade()` stays on `Service` and stays generic. `WebSocketService`
implements it and forwards the stream to the registered
`WebSocketServer`. No registered server leaves the stream unbound.
The server reads `stream.loop()`. The HTTP server has already
activated the connection, so that loop is the loop of the upgrade.
It then calls `onGetSession(loop, stream)`. A null return declines
the upgrade before any bind. A session binds in its constructor,
which accepts the upgrade. The application does not override
`onUpgrade()` to receive WebSocket streams.

The HTTP `Stream` does not know `WebSocketSession` or
`WebSocketServer`. It knows the `WebSocket` the session bound. When
that socket closes, the session tells the server that owns it, and
the server releases it.

`maxSockets()`, `idleTimeout()`, and `maxMessageSize()` stay on the
service. They are endpoint policy, not per-session policy. The
handshake responder reads `maxSockets()` and answers 503 when the
number of live sessions has reached the limit. That count is
internal to the service. There is no public `size()`. The session base reads the other two and
applies them to the socket before the derived constructor runs. A
session may still tighten the socket limits after that. It does not
raise them past the service limit.

`accepted()` goes away. The derived constructor is the accept. A
signal that fires after the service has already constructed a bare
socket leaves the application with no object to put state in.
`size()` as a count of owned sockets goes away with the socket vector.
A service that still wants a count counts the sessions it creates.

The WebSocket server owns every session it accepted. Destroying that
server releases them while the service is still alive. Releasing a
session closes its socket and therefore its stream. Destroying the
service does not walk the sessions.

## Handshake responder {#wss-handshake}

`WebSocketResponder` stays the HTTP responder for the opening
handshake. It inspects the upgrade request, writes 101, 503, or 404,
and is released when that reply has been sent. It does not hold
connection state, and it is not derived by the application when the
only goal is to handle frames.

The handshake and the session are different lifetimes. The handshake
is one HTTP exchange. The session begins only after that exchange has
completed and the server has opened the stream. Keeping them as two
types is what makes the release of the responder safe. Nothing in the
responder needs to survive into the first frame.

An application that must reject an upgrade for a reason the
handshake responder cannot see, for example an application-level
admission check that runs after 101, returns null from
`onGetSession()`. The server then closes the unbound stream. The
preferred rejection is still an HTTP error from the handshake, so
the client learns the reason as a status code.

## Lifetime {#wss-lifetime}

The order on the server is fixed.

1. The servlet maps the request to the `WebSocketService`.
2. The service creates a `WebSocketResponder`.
3. The responder writes the handshake reply.
4. The server releases the responder.
5. A finished 101 makes the server open a `Stream` and call
   `Service::onUpgrade()` on the server thread.
6. `WebSocketService` forwards the stream to the registered
   `WebSocketServer`. That server reads the loop and calls
   `onGetSession(loop, stream)`.
7. A null session declines the stream. The server closes it.
8. Otherwise the session constructor binds `socket()` to the stream.
   That bind accepts the upgrade.
9. The derived constructor starts the first transfer.
10. Frame callbacks run on the same loop until the stream ends.
11. `onClose()` runs while the session is still alive.
12. The `WebSocketServer` calls `onReleaseSession()`. The session
    destructor runs. The socket is already unbound. The service is
    still alive.

The server owns the connection and the stream. The service owns the
session. The session owns the socket as a member. The socket owns
neither the stream nor the connection. Closing the socket closes the
stream. While that stream is the only stream of the connection,
closing it also closes the connection. The server deletes that
connection on its event loop after the close that requested it has
returned.

`onClose()` is the application's last look at the session. Domain
objects that hold a `WebSocketSession&` must drop it there. After
`onReleaseSession()` returns, the reference is gone, and so is
`socket()`.

A pooled session is released back to the pool from
`onReleaseSession()`, not destroyed. The next upgrade binds the
member socket again before the pooled object is handed out as a live
session. The callbacks still see that one socket. Pooling does not
mean one session object serves two streams at once.

## Writing a stream {#wss-write}

A server that pushes a data stream writes from the session, because
the session is what survives from one write to the next.

```cpp
class FeedSession : public WebSocketSession
{
    public:
        FeedSession(WebSocketServer& server,
                    System::EventLoop& loop,
                    Stream& stream,
                    Feed& feed)
        : WebSocketSession(server, loop, stream)
        , _feed(feed)
        {
            _feed.attach(*this);
            writeNext();
        }

        ~FeedSession()
        {
            _feed.detach(*this);
        }

    protected:
        virtual void onOutput()
        {
            socket().endSend();
            writeNext();
        }

        virtual void onInput()
        {
            socket().endReceive();
            socket().beginReceive();
        }

        virtual void onClose()
        {
            _feed.detach(*this);
        }

    private:
        void writeNext();

        Feed& _feed;
};
```

`writeNext()` formats the next chunk into `socket().body()` and calls
`socket().beginSend()`. `onOutput()` continues the stream. The cursor,
the subscription, and the back-reference from the feed all live in
the session. The feed holds `FeedSession&` or `WebSocket&`. It does
not own the session. It drops the reference in `onClose()` or in the
destructor, whichever runs first for that detach.

A timer that paces the feed is started on `loop()` in the derived
constructor. The timer callback is serialized with the frame
callbacks, because it is the same loop.

`BasicWebSocketService<FeedSession>` cannot pass the `Feed&` unless
the session constructor can reach the feed through `service()`. A
session that needs constructor arguments beyond the service, the
loop, and the stream uses a small service subclass whose
`onGetSession()` constructs `FeedSession` with those arguments. That
is the same reason a custom `Service` exists beside `BasicService`.

## HTTP/2 {#wss-http2}

HTTP/2 uses Extended CONNECT. After the endpoints have enabled it,
the request names the protocol with `:protocol`, and a successful
reply has status 200. The resulting `Stream` is one multiplexed
stream, not the whole connection. Other streams on that connection
continue as HTTP.

This design already matches that boundary. `Service::onUpgrade()`
receives one `Stream`. `WebSocketService` reads that stream's loop
and creates one `WebSocketSession` for that stream. `socket()` binds
that stream and no other. `Stream::close()` ends that stream and
does not end the other streams of the connection.

HTTP/2 therefore multiplies sessions, not sockets inside one session.
Each Extended CONNECT is a new `onGetSession(loop, stream)` call, a
new session, and a new member socket. Several sessions of one connection may hold
the same `EventLoop&`, because the server serializes those streams on
one loop. They may also hold the same service, and through
`service()` the same domain objects. Shared state stays on the
service or in the domain. Per-stream state stays in the session.

The session constructor signature does not grow an HTTP-version
parameter. The loop argument is already the loop the server chose for
that stream. The stream argument is already hidden inside the bind
the service performs. Application code that only uses `socket()`,
`loop()`, and `service()` runs unchanged when the same service later
accepts an HTTP/2 WebSocket stream.

What HTTP/2 does not justify is a session that owns many sockets, or
a callback that receives a socket because the session might have
several. One session remains one stream. A fan-out feed that writes
to many peers holds many session references, one per stream, and
writes each through that session's `socket()`.

The handshake responder must eventually accept the HTTP/2 form of the
upgrade as well as the HTTP/1.1 101 form. That is a responder change,
not a session change. The session still begins only after the server
has opened the stream.

## Decline and failure {#wss-decline}

Decline has two places, and they mean different things.

The handshake responder declines with an HTTP status. 404 is not a
WebSocket upgrade. 503 is the accepted-session limit. Further status
codes belong here when the application can decide from the request
alone. The client sees the status. No stream is opened.

`onGetSession()` declines by returning null. The server closes the
stream that has no bound session. Use this when the decision needs
the service's live state at the moment the stream is opened, and
when an HTTP status is no longer available. A client that already
finished the handshake then sees a closed stream.

A throw from the derived constructor is a failed accept. The base
destructor closes the stream. The service does not emit a callback
and does not keep the half-built session. This is for construction
failure, not for an ordinary refusal. Ordinary refusal is a null
return, before the session exists.

`onClose()` is not a decline. The upgrade was accepted. The stream
ended later. The application drops domain references there and does
not try to receive or send again.

## What changes {#wss-changes}

`WebSocketService` stops owning a vector of `WebSocket*`. It owns the
sessions it creates, through `onGetSession()` and
`onReleaseSession()`. `accepted()`, the internal socket list, and
`size()` as a count of owned sockets are removed.

`WebSocket(Stream&)` is no longer a public constructor. The server
entry is a protected default constructor plus a protected `accept`.
`WebSocketSession` is the only caller. The client constructor,
`beginConnect()`, and `endConnect()` stay public.

`WebSocketResponder` stays the handshake responder. Its name stays.
New application code derives `WebSocketSession`, not the responder.

Existing tests that connect a slot to `accepted()` move to a test
session whose constructor starts the receive and whose `onInput()`
and `onClose()` record the result. A decline test still uses a
service that leaves the stream unbound, either by returning null from
`onGetSession()` or by using a plain `Service` whose `onUpgrade()`
binds nothing.

The break is limited to `WebSocketService` and to the server
constructor of `WebSocket`. Client handshakes stay source-compatible.
The break is the point of the change: the current service API cannot
hold the state a data stream needs between writes.

## Out of scope {#wss-scope}

This design does not change `Stream`, `StreamSession`, or
`Service::onUpgrade()`. Those stay the generic upgrade boundary.

It does not add a client-side session type. The client already owns
the `WebSocket` it constructs.

It does not merge the handshake responder into the session. The
responder is released before the session begins.

It does not define HTTP/2 Extended CONNECT in the server. It only
requires that the session model still holds when that upgrade arrives
as one `Stream` and one loop. The handshake responder learning the
HTTP/2 status and headers is separate work.

It does not introduce a multi-socket session, a socket pool inside
one session, or callback arguments that repeat `socket()`, `loop()`,
or `service()`.
