---
proposal: websocket.md
---

# WebSocket Keepalive

The engine may write a ping after inbound silence. The public ping
operation stays `beginPing()`. The model is
[WebSocket](../concepts/websocket.md). This file
states the keepalive clock.

## Ping interval {#ping-interval}

`setPingInterval(ms)` is a fourth engine clock. Zero disables it.
The client and the service start at zero. The server copies it from
the service onto each session with the other clocks.

The interval is inbound silence before the engine writes a ping.
Incoming data, a received ping, and a received pong restart it.
Sending does not. The clock runs only in `Open`. The engine writes
an empty ping only while a receive is outstanding. That ping uses
the same output as `beginPing()` and does not emit `pingReady()`.

A tick is skipped when a ping is outstanding or a ping wait is
already running. `beginPing()` throws while that ping is
outstanding. The next pong ends the wait.

`setPingTimeout()` bounds an unanswered interval ping the same way
it bounds `beginPing()`. A pong or an incoming ping stops it. An
unanswered wait ends the protocol with close code 1006. Idle stays
a death clock on inbound silence.
