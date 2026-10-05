# WebSocket Session {#websocket-session}

This document is the design for a WebSocket after the HTTP upgrade in
Platinum HTTP. It is a framework concept, not an application sketch.
An accepted upgrade produces one application object on the server.
That object owns the state of the connection, holds references into
the application's domain, and stays alive across reads and writes.
The client already owns the `WebSocket` it constructs, so it does not
need a second object.

The HTTP upgrade boundary in
[HTTP Upgrade](../requirements/http-upgrade.md) stays as it is. The
HTTP core knows no frames. The handshake responder writes the upgrade
reply. The server then opens a `Stream` and delivers it to
`Service::onUpgrade()`. Accepting still means binding one session to
that stream. This chapter decides what the WebSocket types do with
that stream, and how the client reaches the same frame engine after
its own handshake.

This chapter covers:

- [Purpose](#wss-purpose)
- [Principles](#wss-principles)
- [Object model](#wss-model)
- [Frame connection](#wss-connection)
- [Client](#wss-client)
- [Session](#wss-session)
- [Service](#wss-service)
- [Release scope](#wss-servlet)
- [Handshake responder](#wss-handshake)
- [Lifetime](#wss-lifetime)
- [Writing a stream](#wss-write)
- [HTTP/2](#wss-http2)
- [Decline and failure](#wss-decline)
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

On the server that place is `WebSocketSession`. The service creates
one session per accepted upgrade. The application derives the
session, holds its domain there, and implements the frame callbacks.
The session is the server facade. Frame operations are methods of the
session. The application does not construct a socket beside it.

On the client that place is the `WebSocket` the application
constructed. It holds the `Client` that performs the handshake, and
after a finished 101 it formats the stream that client owns. The
application stores its own state beside that object. A client session
type would only rename an object the application already owns.

## Principles {#wss-principles}

One accepted stream is one bind. The bind is an internal frame
connection, a `StreamSession`. A second bind on the same stream
throws. The parser, the mask, the payload, and the idle timer live
in that connection. They do not live in the type the application
derives, and they do not live in a public frame type.

Two facades share that connection. They do not share a public frame
type. `WebSocket` is the client facade. It holds a `Client*` and
performs the handshake. `WebSocketSession` is the server facade. It
forwards the frame operations and holds the application state. After
a finished handshake neither facade needs a `Client`. HTTP already
separates the same way: `Connection` is internal, and the public
`Client` does not expose it.

The session is the application context, and only the server has one.
A server must mint one context per accepted stream, because the
application did not construct the stream. The client constructed its
own facade, so the client facade is the context.

The service is the factory, the same role `Service` has for
responders. It creates the session, applies limits, and names the
allocator. It does not hold the session pointers. The object that
holds them is the release scope. It is constructed with the service
and destroyed before it, so release runs while the derived service
is still fully constructed. That object is `WebSocketServlet`. It is
not a server. The HTTP server already owns the connection and the
stream. It is not a `Servlet`. A `Servlet` maps a request to a
service. `WebSocketServlet` does not map, and a `MapUrl` is still
required so the handshake reaches the service.

The handshake stays an HTTP exchange. `WebSocketResponder` answers
the upgrade request. It is not the session, and it is not renamed
into one. The name is already taken by that handshake type.

`Service::onUpgrade()` stays the generic HTTP boundary. The HTTP core
still knows no WebSocket frames. `WebSocketService` implements that
boundary and does not pass it on to the application. The application
implements `onGetSession()` and the session callbacks.

The event loop is an explicit argument of session construction. The
application does not recover it by walking from the connection to the
stream. The loop passed to the session is the loop that serializes
that stream. A different loop is an error.

No callback repeats an object the session already holds. `onInput()`,
`onOutput()`, and `onClose()` take no arguments. The service and the
loop are members, reached through accessors. The frame operations are
methods, so a callback does not receive a socket either.

## Object model {#wss-model}

The server split follows the HTTP server split, with one extra object
for release.

| HTTP | WebSocket |
| --- | --- |
| `Service` | `WebSocketService` |
| `Responder` | `WebSocketSession` |
| release of responders | `WebSocketServlet` |
| `Request` and `Reply` | frame methods on the session |

`WebSocketService` is mapped with a servlet like any other service.
Its handshake responder finishes the HTTP exchange. On a successful
upgrade the registered `WebSocketServlet` asks the service for a
`WebSocketSession` and holds it. The session opens a frame connection
on the `Stream` the server already owns.

```text
Connection  owns  Stream
WebSocketService  owns the session policy
        ^
        | registers, destroyed first
WebSocketServlet  holds sessions for onReleaseSession()
        |
        +-- WebSocketSession          server facade, application state
                 |
                 +-- WebSocketConnection --> Stream
```

The client has no factory and no release scope.

```text
WebSocket          client facade, holds Client*
    |
    +-- WebSocketConnection --> Stream --> Connection
```

A later protocol that also uses `Service::onUpgrade()` does not
derive `WebSocketSession`. It binds its own `StreamSession`. The
session type in this chapter is the WebSocket case only.

## Frame connection {#wss-connection}

`WebSocketConnection` is the internal frame engine. It is the
`StreamSession`, and so it is the one bind. It parses and writes
frames, and it holds the payload, the mask, the size limit, and the
idle timer. Masking is a mode set when it opens: a client masks, a
server does not. It has no `Client`, no URL, and no
`Sec-WebSocket-Key`. The receive and send states live only here.

The connection is not a public type. There is no public
`WebSocketStream`, and application code does not construct, bind, or
name the connection. The client facade opens it after a finished 101.
The server facade opens it in its constructor. Both forward
`body()`, `beginSend()`, `beginReceive()`, `frame()`, ping, pong, and
close. Closing the frame connection closes the stream. The connection
owns neither the stream nor the HTTP connection.

## Client {#wss-client}

`WebSocket` is the client facade. Construct it with the `Client` that
performs the handshake. The socket stores a reference and does not own
that client, so the client must outlive the socket, including a
handshake that is still waiting on the loop. Host, port, event loop,
timeout, and TLS are settings of that client.

Connect `connected()` and call `beginConnect()` with the request
path, or with a `ws://` URL whose host and port are the client's
endpoint. The handshake is an HTTP request and reply on that client.
`endConnect()` completes it and throws if it failed. A finished 101
opens a `WebSocketConnection` on the stream from the client upgrade.
After the handshake the socket no longer uses the client for frames.
The client still owns the connection and the stream.

Frame events are signals on this facade. `inputReady()` reports that
one whole frame has been received. `outputReady()` reports that a
frame has left the connection stream buffer. `closed()` is emitted
while the socket is still alive. Peer close, an I/O error, a close
frame, and destruction of the stream all emit it. The application
deletes the socket. There are no virtual callbacks, because the
application constructed the object and can connect slots to it.

`body()` is the payload stream. `beginSend()` writes one frame from
that body. `beginReceive()` reads one frame into it. `frame()` is the
opcode. A client masks every frame it writes. That mode is fixed when
the upgraded stream opens.

```cpp
void onConnected(Pt::Http::WebSocket& socket)
{
    socket.endConnect();
    socket.body() << "hello";
    socket.beginSend(Pt::Http::WebSocket::Text);
}

Pt::System::MainLoop loop;
Pt::Net::Endpoint ep("localhost", 80);
Pt::Http::Client client(loop, ep);
Pt::Http::WebSocket socket(client);
socket.connected() += Pt::slot(onConnected);
socket.beginConnect("/ws");
loop.run();
```

The server does not construct a `WebSocket`. There is no server
constructor and no `accept()` on this type.

## Session {#wss-session}

`WebSocketSession` is the per-stream application object and the server
facade. It is a `Connectable`. It is not a `StreamSession` and not a
`Responder`.

```cpp
class WebSocketSession : public Connectable
{
    public:
        WebSocketSession(WebSocketServlet& servlet,
                         System::EventLoop& loop,
                         Stream& stream);
        ~WebSocketSession();

        WebSocketService& service();
        System::EventLoop& loop();

        std::iostream& body();
        WebSocket::Frame frame() const;
        void beginSend(WebSocket::Frame frame);
        void endSend();
        void beginReceive();
        void endReceive();

    protected:
        virtual void onInput() = 0;
        virtual void onOutput() = 0;
        virtual void onClose() = 0;

    private:
        WebSocketService* _service;
        WebSocketServlet* _servlet;
        System::EventLoop* _loop;
        WebSocketConnection* _connection;
};
```

The servlet constructs the session with itself, with the loop that
serializes this stream, and with the stream of this upgrade. The base
constructor opens the frame connection on that stream, copies
`maxMessageSize()` and `idleTimeout()` from the service onto that
connection, stores the loop, and connects the connection signals to
`onInput()`, `onOutput()`, and `onClose()`. The stream is an
argument. It is not recovered from thread-local state. A loop that is
not the loop of the stream is an error.

The derived constructor runs after that. Its members are initialized,
the stream is open, and `loop()` is the loop that serializes this
stream. That constructor holds the domain references and starts the
first transfer, `beginReceive()` or `beginSend()`. There is no
`onAccept()`. By the time the derived constructor body runs, the base
has already done the work an accept callback would do: the stream is
bound, the limits are set, and the loop is known. A callback that
only restates the constructor would force every session to implement
an empty entry point.

`onInput()`, `onOutput()`, and `onClose()` stay. They are later
events. A constructor cannot receive them. They take no arguments.
The session and the loop are the same objects the constructor stored.
Passing them again would imply that a callback might see a different
session or a different loop, and it does not. The frame operations
are methods of the session, so the callback does not receive a socket
either. There is no `socket()`.

`service()` is how a session reaches shared state: the feed, the
registry, the limits, anything that belongs to every connection of
this endpoint rather than to one stream. `loop()` is how a session
attaches a timer or posts work onto the loop that owns this stream.

`onInput()` runs when one whole frame has been received. The derived
session calls `endReceive()`, reads `body()`, and starts the next
receive or a reply. `onOutput()` runs when a frame has left the stream
buffer. The derived session calls `endSend()` and starts the next
send when it still has data. `onClose()` runs while the session object
is still alive. The stream has already cleared its session pointer.
Peer close, an I/O error, a close frame, and destruction of the
stream all end here. The servlet releases the session after
`onClose()` returns.

The destructor closes the stream. If the derived constructor throws,
that destructor still runs, so a session that fails during
construction does not leave an accepted stream without an owner.

A server does not mask the frames it writes. Ping, pong, and close
are methods of the session, the same names the client facade uses.

## Service {#wss-service}

`WebSocketService` is the factory and the endpoint policy. It does
not keep the sessions it creates.

```cpp
class WebSocketService : public Service
{
    protected:
        virtual WebSocketSession* onGetSession(WebSocketServlet& servlet,
                                               System::EventLoop& loop,
                                               Stream& stream) = 0;
        virtual void onReleaseSession(WebSocketSession* session) = 0;
};
```

`onGetSession()` creates the session. `onReleaseSession()` destroys
it. The two must match, including the allocator, as
`onGetResponder()` and `onReleaseResponder()` must match. A pool, or
any other detach, lives in the derived service.
`BasicWebSocketService<S>` is that factory for one session type. Its
session constructor takes the servlet, the loop, and the stream. A
session that needs further constructor arguments uses a small service
subclass whose `onGetSession()` passes them. That is the same reason
a custom `Service` exists beside `BasicService`.

`onUpgrade()` stays on `Service` and stays generic. `WebSocketService`
implements it and forwards the stream to the registered
`WebSocketServlet`. The implementation is final. The application does
not override `onUpgrade()` to receive WebSocket streams. No
registered servlet leaves the stream unbound. The servlet reads
`stream.loop()`. The HTTP server has already activated the
connection, so that loop is the loop of the upgrade. It then calls
`onGetSession(servlet, loop, stream)`. A null return declines the
upgrade before any bind. A session binds in its constructor, which
accepts the upgrade.

The HTTP `Stream` does not know `WebSocketSession` or
`WebSocketServlet`. It knows the frame connection the session bound.
When that connection closes, the session tells the servlet that holds
it, and the servlet releases it.

`maxSockets()`, `idleTimeout()`, and `maxMessageSize()` stay on the
service. They are endpoint policy, not per-session policy. The
handshake responder reads `maxSockets()` and answers 503 when the
number of live sessions has reached the limit. That count is internal
to the service. There is no public `size()` on the service. The
session base reads the other two and applies them to the connection
before the derived constructor runs.

## Release scope {#wss-servlet}

The service destructor cannot call `onReleaseSession()`. The derived
destructor has already run, so the virtual call and the allocator are
gone. A function pointer captured at creation does not help: it still
refers to the derived object, and a copied allocator would only cover
a pure deallocate, not a pool or the rest of the teardown.

The object constructed with the service and destroyed before it is
therefore required. It holds the session pointers so release runs
while the derived service is still fully constructed. It does not
decide the session type, the allocator, or the domain teardown. It is
the release scope of the service, not a second owner, and not a
server.

`WebSocketServlet` is that scope. Bare `Servlet` is already the HTTP
mapping type. `WebSocketServlet` is not that `Servlet`. A `MapUrl` is
still required so the handshake reaches the service. One service has
one registered scope. A second registration replaces it. The previous
servlet keeps the sessions it already holds and releases them itself.
No registration, which is the state before the first servlet and
after the last destructor, declines the next upgrade. The HTTP server
closes a stream that has no bound session.

```cpp
typedef Pt::Http::BasicWebSocketService<EchoSession> EchoService;

EchoService service;
Pt::Http::WebSocketServlet sockets(service);
Pt::Http::MapUrl mapUrl("/ws", service);
server.addServlet(mapUrl);
```

Destroy the servlet before the service. The servlet destructor
releases every session it still holds through `onReleaseSession()`.
The service destructor does not walk the sessions. Releasing a
session closes its connection and therefore its stream.

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
   `WebSocketServlet`. That servlet reads the loop and calls
   `onGetSession(servlet, loop, stream)`.
7. A null session, or no registered servlet, declines the stream.
   The server closes it.
8. Otherwise the session constructor binds the frame connection to
   the stream. That bind accepts the upgrade.
9. The derived constructor starts the first transfer.
10. Frame callbacks run on the same loop until the stream ends.
11. `onClose()` runs while the session is still alive.
12. The `WebSocketServlet` calls `onReleaseSession()`. The session
    destructor runs. The connection is already unbound. The service
    is still alive.

The HTTP server owns the connection and the stream. The servlet holds
the session. The service owns the session policy and the allocator.
The session owns the frame connection as a member. The connection
owns neither the stream nor the HTTP connection. Closing the
connection closes the stream. While that stream is the only stream of
the HTTP connection, closing it also closes the connection. The
server deletes that connection on its event loop after the close that
requested it has returned.

`onClose()` is the application's last look at the session. Domain
objects that hold a `WebSocketSession&` must drop it there. After
`onReleaseSession()` returns, the reference is gone.

A pooled session is released back to the pool from
`onReleaseSession()`, not destroyed. The next upgrade opens a new
frame connection before the pooled object is handed out as a live
session. Pooling does not mean one session object serves two streams
at once.

## Writing a stream {#wss-write}

A server that pushes a data stream writes from the session, because
the session is what survives from one write to the next.

```cpp
class FeedSession : public WebSocketSession
{
    public:
        FeedSession(WebSocketServlet& servlet,
                    System::EventLoop& loop,
                    Stream& stream,
                    Feed& feed)
        : WebSocketSession(servlet, loop, stream)
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
            endSend();
            writeNext();
        }

        virtual void onInput()
        {
            endReceive();
            beginReceive();
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

`writeNext()` formats the next chunk into `body()` and calls
`beginSend()`. `onOutput()` continues the stream. The cursor, the
subscription, and the back-reference from the feed all live in the
session. The feed holds `FeedSession&`. It does not own the session.
It drops the reference in `onClose()` or in the destructor, whichever
runs first for that detach.

A timer that paces the feed is started on `loop()` in the derived
constructor. The timer callback is serialized with the frame
callbacks, because it is the same loop.

`BasicWebSocketService<FeedSession>` cannot pass the `Feed&` unless
the session constructor can reach the feed through `service()`. A
session that needs constructor arguments beyond the servlet, the
loop, and the stream uses a small service subclass whose
`onGetSession()` constructs `FeedSession` with those arguments.

## HTTP/2 {#wss-http2}

HTTP/2 uses Extended CONNECT. After the endpoints have enabled it,
the request names the protocol with `:protocol`, and a successful
reply has status 200. The resulting `Stream` is one multiplexed
stream, not the whole connection. Other streams on that connection
continue as HTTP.

This design already matches that boundary. `Service::onUpgrade()`
receives one `Stream`. `WebSocketService` reads that stream's loop
and creates one `WebSocketSession` for that stream. The frame
connection binds that stream and no other. `Stream::close()` ends
that stream and does not end the other streams of the connection.

HTTP/2 therefore multiplies sessions, not connections inside one
session. Each Extended CONNECT is a new `onGetSession()` call, a new
session, and a new frame connection. Several sessions of one
connection may hold the same `EventLoop&`, because the server
serializes those streams on one loop. They may also hold the same
service, and through `service()` the same domain objects. Shared
state stays on the service or in the domain. Per-stream state stays
in the session.

The session constructor signature does not grow an HTTP-version
parameter. The loop argument is already the loop the server chose for
that stream. Application code that only uses the frame methods,
`loop()`, and `service()` runs unchanged when the same service later
accepts an HTTP/2 WebSocket stream.

What HTTP/2 does not justify is a session that owns many connections,
or a callback that receives a connection because the session might
have several. One session remains one stream. A fan-out feed that
writes to many peers holds many session references, one per stream,
and writes each through that session's frame methods.

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

`onGetSession()` declines by returning null. No registered servlet
declines the same way: the stream stays unbound. The server closes
the stream that has no bound session. Use a null return when the
decision needs the service's live state at the moment the stream is
opened, and when an HTTP status is no longer available. A client that
already finished the handshake then sees a closed stream.

A throw from the derived constructor is a failed accept. The base
destructor closes the stream. The servlet does not keep the
half-built session. This is for construction failure, not for an
ordinary refusal. Ordinary refusal is a null return, before the
session exists.

`onClose()` is not a decline. The upgrade was accepted. The stream
ended later. The application drops domain references there and does
not try to receive or send again.

## Out of scope {#wss-scope}

This design does not change `Stream`, `StreamSession`, or
`Service::onUpgrade()`. Those stay the generic upgrade boundary.
`WebSocketService::onUpgrade()` stays final and forwards to the
registered servlet.

It does not add a client-side session type. The client already owns
the `WebSocket` it constructs.

It does not merge the handshake responder into the session. The
responder is released before the session begins.

It does not publish `WebSocketConnection`. The two facades forward
the frame operations. Application code does not name the connection.

It does not define HTTP/2 Extended CONNECT in the server. It only
requires that the session model still holds when that upgrade arrives
as one `Stream` and one loop. The handshake responder learning the
HTTP/2 status and headers is separate work.

It does not introduce a multi-connection session, a connection pool
inside one session, or callback arguments that repeat the frame
operations, `loop()`, or `service()`.
