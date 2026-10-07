# WebSocket status {#websocket-status}

This file records work the WebSocket chapters describe and the tree
does not have yet. The chapters stay the target. They do not say
what is already built. A change to an anchored section listed here
updates the matching row in the same change. A decision that lands
in the tree drops the row. A decision that changes is rewritten in
the chapter, and the point here is rewritten with it.

Open work is not repeated in `doc/pages/`. A page describes the
built API.

| Chapter | Anchor | Point | Status |
| --- | --- | --- | --- |
| [websocket-session.md](websocket-session.md) | [wss-protocol](websocket-session.md#wss-protocol) | Client offers `Sec-WebSocket-Protocol`. The responder selects one name and echoes it. `protocol()` reports it. | planned |
| [websocket-session.md](websocket-session.md) | [wss-handshake](websocket-session.md#wss-handshake) | `WebSocketHandshake` is copied before the responder is released and passed to `onGetSession()`. The session keeps the copy, not the request. | planned |
| [websocket-session.md](websocket-session.md) | [wss-client](websocket-session.md#wss-client) | Headers set before `beginConnect()` are kept. `endConnect()` checks status 101 and `Sec-WebSocket-Accept`. | planned |
| [websocket-session.md](websocket-session.md) | [wss-decline](websocket-session.md#wss-decline) | A missing or invalid key is 400. A version other than 13 is 426 with `Sec-WebSocket-Version: 13`. | planned |
| [websocket-message.md](websocket-message.md) | [wsm-control](websocket-message.md#wsm-control) | A received pong is visible: `pong()` on the client, `onPong()` on the session. An unanswered ping is facade state. 1005, 1006, and 1015 stay off the wire. | planned |
| [websocket-session.md](websocket-session.md) | [wss-handshake](websocket-session.md#wss-handshake) | The handshake key and the masking key come from an unpredictable source. | planned |
