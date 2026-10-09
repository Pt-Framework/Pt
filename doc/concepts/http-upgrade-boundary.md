---
topic: http-upgrade.md
---

# HTTP Upgrade and WebSocket

HTTP Upgrade turns a successfully completed HTTP exchange into a long-lived,
bidirectional protocol channel. The HTTP server remains responsible for
routing, authentication, logging, resource limits, and transport lifetime. A
protocol channel receives only a logical stream, never the underlying socket
or the whole connection.

This model separates HTTP infrastructure from application protocols. The HTTP
core knows no WebSocket frames, masking, or ping/pong semantics. WebSocket is
one channel that uses such a stream. Other protocols can use the same boundary
when their HTTP transport supports it.

## Industry Examples

Several HTTP stacks separate HTTP message handling from the protocol stream.
Rust's hyper exposes an `OnUpgrade` future obtained from the request or
response. After the upgrade succeeds, the future yields an `Upgraded`
transport that a WebSocket library, such as tokio-tungstenite, can consume.
Hyper itself does not parse WebSocket frames.

Node.js exposes a different HTTP/1.1 boundary. Its `upgrade` event receives
the request, a raw duplex socket, and any bytes already read after the
headers. The event handler writes the handshake response and then owns
protocol I/O on that socket.

For HTTP/2, Node.js exposes an `Http2Stream` for each request rather than
handing out the TCP socket. Extended CONNECT is enabled through the HTTP/2
settings and uses `:protocol`; the handler responds and continues I/O on that
one stream, while the HTTP/2 connection retains ownership of the transport.

The APIs differ, but the common principle remains: HTTP decides the protocol
transition; the extension processes the byte stream only afterwards.

## Unified Lifecycle

Every request follows the same HTTP pipeline: routing, authentication, service,
and responder. Logging, telemetry, rate limiting, and error handling therefore
apply to upgrade requests as well as to ordinary replies.

A successful upgrade reply does not end the server's responsibility. It moves
processing from HTTP messages to a channel:

1. The responder creates an upgrade reply valid for the HTTP protocol in use.
2. The server writes that reply completely and successfully.
3. The server creates the logical stream and reports it to the responsible
   upgrade handler.
4. The handler accepts the stream by binding a channel, or declines it.
5. The server closes a stream that has no bound channel.

The server owns the connection and the stream. A channel owns neither socket
nor connection; it binds exclusively to the stream. `Channel::close()` calls
`Stream::close()` and then drops its stream pointer. `Stream::close()` cancels
that stream, detaches it from the connection, and then tells the channel.
The stream does not clear the channel pointer. While that stream is the only
stream of the connection, closing it also closes the connection. The connection
stays alive until `Connection::closed()` returns. The owner deletes the
connection after that slot returns. Closing the connection does not delete the
stream.

## Protocol Transitions

### HTTP/1.1

An HTTP/1.1 upgrade is valid only when the request and successful reply contain
the required upgrade fields. The reply uses status 101 together with
`Connection: Upgrade` and a non-empty `Upgrade` protocol. The resulting stream
represents the entire HTTP/1.1 connection.

### HTTP/2

HTTP/2 uses Extended CONNECT for WebSocket and comparable protocols. After the
endpoints have enabled Extended CONNECT, the request identifies the protocol
through the `:protocol` pseudo-header and a successful reply has status 200.
The resulting stream represents only the selected multiplexed HTTP/2 stream.
Other streams on the same connection remain independent and continue to be
processed as HTTP.

HTTP/3 follows the same principle: a channel owns a logical stream, not the
underlying transport.

## Stream Contract

A stream is a bidirectional, asynchronous byte channel. It provides input,
output, completion, cancellation, timeout, and a clearly defined execution
context. It encapsulates HTTP/1.1 connections as well as multiplexed HTTP/2 or
HTTP/3 streams. It provides no access to a socket or the whole connection.

At most one transfer may be active per direction. Input and output operations
may run concurrently. Completing or cancelling a stream affects only that
logical stream; for multiplexed protocols, it does not end other streams.

## Timing, Timeouts, and Execution

The server must not forward application data for the new channel until it has
written the HTTP reply that confirms the transition completely and
successfully. Until then, neither the channel nor the upgrade handler may write
bytes to the stream. This keeps HTTP reply data and protocol data separate.

When the channel begins, HTTP-specific request and keep-alive timeouts
end for that stream. The channel then configures its own application-level idle
and transfer timeouts.

An upgrade does not require a thread transition. All operations and callbacks
of one stream must, however, be serialized on one clearly defined event loop or
executor.

## Upgrade Signalling

The reply describes only the HTTP protocol result. It does not provide access
to a connection or socket. The server derives the state transition from a valid
upgrade reply and delivers the stream to the responsible handler through the
defined upgrade boundary.
