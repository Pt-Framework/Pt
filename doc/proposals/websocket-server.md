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

The servlet owns every live session. `closed()` notifies the
application. It is not an error. `inputReady()` and `outputReady()`
do not run for it. `endReceive()` and `endSend()` do not report it.
Outstanding send and receive stay pending.

After `closed()` returns, the session calls
`WebSocketServlet::onSessionClosed`. That releases the session.
The destructor unbinds the stream. A `closed()` slot need not call
`close()`. A call to `close()` during `closed()` does not unbind
or release. A session that connects no `closed()` slot is still
released.

`close()` unbinds the stream and calls `onSessionClosed`. That is
the local abort. `closed()` does not run for it. The destructor
unbinds and does not release again.

`WebSocketServlet::onSessionClosed` is the only release. It is
idempotent: a session already gone from the list is ignored. The
servlet destructor releases remaining sessions through it.

A domain object that holds a session reference drops it in
`closed()`, because the servlet releases after that signal.

`close()` during construction unbinds the stream. The session is
not listed. `accept()` releases it through `releaseSession()`.

## Session ready {#session-ready}

`WebSocketSession` is a concrete `Connectable`. It exposes the same
ready signals as `WebSocket`, with `WebSocketSession&`:

```cpp
Signal<WebSocketSession&>& inputReady();
Signal<WebSocketSession&>& outputReady();
Signal<WebSocketSession&>& pingReady();
Signal<WebSocketSession&>& shutdownReady();
Signal<WebSocketSession&>& closed();
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
            try
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
            catch(const std::exception&)
            {
                close();
            }
        }

        void onOutputReady(Pt::Http::WebSocketSession&)
        {
            try
            {
                endSend();
            }
            catch(const std::exception&)
            {
                close();
            }
        }
};
```
