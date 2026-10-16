---
proposal: io-duplex.md
---

# I/O Duplex Transforms

## SSL want-write handling {#ssl-want-write}

Connection::read() must treat the cross-direction want-code the same
way it already treats its own: zero bytes read, caller retries later.
Connection::write() must treat the cross-direction want-code the same
way: zero bytes written, caller retries later. This does not remove
the _isReading/_isWriting guard; it only makes the two public,
data-phase operations tolerate the outcome that guard produces,
matching the handshake functions that already tolerate it.

## Codec direction state

ZBuffer keeps independent zlib state and working buffers for inflate
and deflate. BasicTextBuffer likewise keeps independent codec state and
conversion buffers for decoding and encoding. Starting one direction
therefore preserves buffered data and transform state in the other.