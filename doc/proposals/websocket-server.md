---
proposal: websocket.md
---

# WebSocket Server

The server session is the I/O facade of one accepted stream. Ready
notification is begin / signal / end, the same contract as the
client. The model is
[WebSocket](../concepts/websocket.md). This file
states the ready surface.

## Session close {#session-close}

The servlet owns every live session. `peerClosed()` notifies the
application that the engine has ended the protocol. It is not an
error. `inputReady()` and `outputReady()` do not run for it.
`endReceive()` and `endSend()` do not report it. Outstanding send
and receive stay pending. The stream stays bound.

After `peerClosed()` returns, the session closes. A `peerClosed()`
slot need not call `close()`. A session that connects no
`peerClosed()` slot is still released.

`close()` closes the stream and does not destroy the session.
The stream ends the bind. The servlet moves the session from the
live list to a closing list and requests a later turn of the loop
through a `System::Yield`. The request carries no session pointer.
The handler releases every session on the closing list. Several
requests coalesce and a stale request finds the list empty.
`peerClosed()` does not run for a local `close()`. `released()`
runs when the bind has ended, including a local `close()` and
destruction of the connection. The destructor closes a stream
that is still bound and does not release again.

An `IOError` from a ready slot, including `WebSocketError`, closes
the session and does not reach the event loop. After
`shutdownReady()` returns, the session closes. A slot calls
`endShutdown()` to observe a handshake error. `close()` remains the
explicit abort from the constructor, a kick, or the application.

`WebSocketServlet::onSessionClosed` queues that release on the loop
of the servlet. A session
already gone from the live list is ignored. The servlet destructor
releases remaining sessions on both lists.

A domain object that holds a session reference drops it in
`released()`.

`close()` during construction closes the stream. The session is
not listed. `accept()` releases it through `releaseSession()`.

`size()` is the live list. `socketCount()` drops when
`releaseSession()` destroys the session.

## Session ready {#session-ready}

`WebSocketSession` is a concrete `Connectable`. It exposes the same
ready signals as `WebSocket`, with `WebSocketSession&`:

```cpp
Signal<WebSocketSession&>& inputReady();
Signal<WebSocketSession&>& outputReady();
Signal<WebSocketSession&>& pingReady();
Signal<WebSocketSession&>& shutdownReady();
Signal<WebSocketSession&>& peerClosed();
Signal<WebSocketSession&>& released();
Signal<WebSocketSession&, const std::string&>& pong();
```

The application derives the session to hold domain state. It
connects slots in the constructor after its members are initialized,
then starts the first `beginReceive()`. Frames stay in the stream
until that call. Keepalive and incoming control frames run while a
receive is outstanding.

`endReceive()` and `endSend()` are valid only after the matching
ready signal, or after the corresponding I/O failure notification.
They throw `std::logic_error` while the operation is still pending
or after a local `close()`.

A session that connects no slot is valid. A receive that completes
with no `inputReady` slot stays unfinished until `endReceive()`,
the same as a client with no slot connected.

Message-level callbacks are not part of this surface. An adapter
connects the same signals.

```cpp
class EchoSession : public Pt::Http::WebSocketSession
{
    public:
        EchoSession(Pt::Http::WebSocketService& service,
                    Pt::Http::Stream& stream,
                    const Pt::Http::Reply& reply)
        : Pt::Http::WebSocketSession(service, stream, reply)
        {
            inputReady() += Pt::slot(*this, &EchoSession::onInputReady);
            outputReady() += Pt::slot(*this, &EchoSession::onOutputReady);
            beginReceive();
        }

    private:
        void onInputReady(Pt::Http::WebSocketSession&)
        {
            Pt::Http::WebSocketProgress progress = endReceive();
            if( ! progress.finished() )
            {
                incoming().discard();
                beginReceive();
                return;
            }

            incoming().clear();
            beginReceive();
        }

        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            endSend();
        }
};
```
