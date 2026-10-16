---
proposal: websocket.md
---

# WebSocket Session

The server session is the application context of one accepted
stream. The client facade is the `WebSocket` the application
constructed. The model is
[WebSocket Session](../concepts/websocket-session.md). This file
states the handshake rules the implementation still has to follow,
and the behavior it has today.

## Baseline

The client copies some headers onto the handshake request.
`endConnect` does not yet reject a non-101 status or a mismatched
`Sec-WebSocket-Accept`. A missing or invalid key is not yet 400. A
version other than 13 is not yet 426 with `Sec-WebSocket-Version:
13`. The handshake key and the masking key are not yet drawn from an
unpredictable source.

## Client handshake {#client-handshake}

Headers set on the socket before `beginConnect` are copied onto the
handshake request. `beginConnect` does not drop them. Origin stays
the optional second argument. `Authorization`, a cookie, and any
other handshake header are set this way. Host, port, and TLS stay
settings of the client.

`endConnect` accepts only status 101, and only when
`Sec-WebSocket-Accept` is the digest of the key this socket sent. A
missing or different Accept is a failed handshake. No stream is
opened.

## Decline {#decline}

A missing or invalid `Sec-WebSocket-Key` is 400. A version other
than 13 is 426 with `Sec-WebSocket-Version: 13`. The handshake
responder writes those replies. No stream is opened.

## Nonce {#nonce}

The handshake key and the masking key come from an unpredictable
source. The key is not derived from the clock. The masking key is
not a linear congruential sequence seeded from the clock.
