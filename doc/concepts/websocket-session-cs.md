# WebSocket Client and Server {#websocket-session-cs}

This note revises the client and server split in
[WebSocket Session](websocket-session.md). It does not revise the
upgrade boundary. [HTTP Upgrade](../requirements/http-upgrade.md)
still holds: the HTTP core knows no frames, the responder writes the
upgrade reply, and the server then opens a `Stream` and delivers it
to `Service::onUpgrade()`. Accepting still means binding one session
to that stream.

The current layering is otherwise sound. What is wrong is the role
named `WebSocketServer`, and the use of one `WebSocket` type as both
client and server.

## Release scope {#wscs-scope}

`WebSocketService` owns the sessions. `onGetSession()` creates one,
`onReleaseSession()` destroys it, and the allocator lives in the
derived service, as it does for responders. A pool or any other
detach lives there too.

The service destructor cannot call `onReleaseSession()`. The derived
destructor has already run, so the virtual call and the allocator
are gone. A function pointer captured at creation does not help: it
still refers to the derived object, and a copied allocator would
only cover a pure deallocate, not a pool or the rest of the
teardown.

The object constructed with the service and destroyed before it is
therefore required. It holds the session pointers so release runs
while the derived service is still fully constructed. It does not
decide the session type, the allocator, or the domain teardown. It
is the release scope of the service, not a second owner, and not a
server. The HTTP server already owns the connection and the stream.

`WebSocketServer` is the current name of that scope, and the name is
wrong. `Servlet` is an acceptable name for the role. It is not the
existing `Servlet`: that type maps a request to a service and stays.
The scope does not map, and a `MapUrl` is still required so the
handshake reaches the service. One service has one registered
scope. A second registration replaces it. The previous scope keeps
the sessions it already holds and releases them itself.

```text
Connection  owns  Stream
WebSocketService  owns the session policy
        ^
        | registers, destroyed first
   release scope  holds sessions for onReleaseSession()
```

## Frame connection {#wscs-connection}

The impurity in `WebSocket` is the handshake, not the frame. The
type stores a `Client*`, distinguishes client and server, offers
`beginConnect()`, and also has a protected server constructor and
`accept()`. After a finished 101 neither side needs a `Client`.

HTTP already separates that. `Connection` lives in `src/Pt-Http`,
not in `include`. It knows the request and the reply. The public
`Client` does not expose it. WebSocket gets the same split.

`WebSocketConnection` is the internal frame engine. It is the
`StreamSession`, and so it is the one bind. It parses and writes
frames, and it holds the payload, the mask, the size limit, and the
idle timer. Masking is a mode set when it opens: a client masks, a
server does not. It has no `Client`, no URL, and no
`Sec-WebSocket-Key`. The receive and send states live only here.

## Two facades {#wscs-facades}

`WebSocket` stays the client facade. It holds the `Client`, and it
implements `beginConnect()`, `endConnect()`, and `connected()`. After
the 101 it opens a `WebSocketConnection` on the stream from
`Client::upgrade()` and forwards `body()`, `beginSend()`, and
`beginReceive()`. The server constructor and `accept()` leave this
type.

`WebSocketSession` is the server facade. It owns the
`WebSocketConnection` and forwards the frame operations: `body()`,
`frame()`, `beginSend()`, `beginReceive()`, ping, and close. There
is no public `WebSocketStream`, and `socket()` goes away. The
application derives the session and does not see the connection,
the same way an HTTP caller sees `Client` and not `Connection`.

The session is not a `StreamSession`. The connection is the bind. A
second bind would throw, and the parser does not belong in the type
the application derives.

The two facades share the connection. They do not share a public
frame type. That is the separation the `Client*` currently mixes.

```text
WebSocket          client handshake, holds Client*
    |
    +-- WebSocketConnection   internal, the StreamSession
                |
                +-- Stream --> Connection

WebSocketSession   server facade, frame methods, application state
    |
    +-- WebSocketConnection   same engine, no Client*
```

## What stays {#wscs-stays}

`WebSocketResponder` stays the handshake. It writes 101, 503, or
404 and is released before the server opens the stream. It is not
the session.

`Service::onUpgrade()` stays the generic boundary and stays final on
`WebSocketService`. The application implements `onGetSession()` and
the session callbacks. A null session declines the stream. The
server closes a stream that has no bound session.

The connection owns the stream. The session owns neither the stream
nor the HTTP connection. Closing the frame connection closes the
stream.
