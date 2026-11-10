---
proposal: websocket.md
---

# WebSocket Session

The server session is the application context of one accepted
stream. The client facade is the `WebSocket` the application
constructed. The model is
[WebSocket](../concepts/websocket.md). This file
states the handshake rules the implementation still has to follow,
and the behavior it has today.

## Baseline

A missing or invalid key is not yet 400. A version other than 13
is not yet 426 with `Sec-WebSocket-Version: 13`. The handshake key
and the masking key are not yet drawn from an unpredictable source.

## Client handshake {#client-handshake}

`WebSocket::header()` refers directly to `Client::request().header()`.
Headers set there before `beginConnect` stay in the handshake request,
including custom names and repeated fields. There is no separate
header copy. The client must be idle while the application prepares
the handshake. The body is discarded with the existing `discard()`;
`body().clear()` resets the stream error state without clearing the
header. No new request-reset API is needed.

The handshake is an empty HTTP/1.1 GET. Old query parameters and
body-framing fields are reset. The socket controls Connection,
Upgrade, version, key and the protocol offer. Origin stays the
optional second argument: a nonempty argument replaces a prepared
Origin field, otherwise that field stays. `Authorization`, a cookie,
and other application fields are preserved. Host, port, and TLS stay
settings of the client.

`endConnect` accepts only status 101, and only when
`Sec-WebSocket-Accept` is the digest of the key this socket sent. A
missing, empty or different Accept is a failed handshake. HTTP
whitespace around the Accept is ignored; the digest is case-sensitive.
No stream is opened. `connected()` reports completion, and
`endConnect()` reports the stored handshake error.

The client and server share the internal, public-static
`WebSocketChannel::getAcceptKey(const std::string&)`. It uses SHA-1
and Base64 and needs no channel instance or open stream. The expected
Accept is stored for the key actually sent by each client handshake.

## Decline {#decline}

A missing or invalid `Sec-WebSocket-Key` is 400. A version other
than 13 is 426 with `Sec-WebSocket-Version: 13`. The handshake
responder writes those replies. No stream is opened.

## Nonce {#nonce}

The handshake key and the masking key come from an unpredictable
source. The key is not derived from the clock. The masking key is
not a linear congruential sequence seeded from the clock.
