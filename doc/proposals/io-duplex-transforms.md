---
proposal: io-duplex.md
---

# I/O Duplex Transforms

## Baseline

ZBuffer holds one z_stream* _zstr used for both inflate (read) and
deflate (write). import() and overflow() each call discard() and
reinitialize _zstr for their own direction when the other direction's
buffer area has not been set up yet, tearing down whatever the other
direction had in progress.

BasicTextBuffer holds one codec state_type _state used for both
decode (read) and encode (write). import() calls discard() whenever
output is still pending (pptr() set), which resets _state and drops
the buffered, not yet written output.

## SSL want-write handling {#ssl-want-write}

Connection::read() must treat the cross-direction want-code the same
way it already treats its own: zero bytes read, caller retries later.
Connection::write() must treat the cross-direction want-code the same
way: zero bytes written, caller retries later. This does not remove
the _isReading/_isWriting guard; it only makes the two public,
data-phase operations tolerate the outcome that guard produces,
matching the handshake functions that already tolerate it.

## Codec direction state {#codec-direction-state}

A transform is full duplex only if starting one
direction cannot discard or corrupt state the other direction still
needs. ZBuffer and BasicTextBuffer do not meet that bar: they silently
discard() the other direction's buffered output and reinitialize the
shared transform state. An implementation task closing this item must
choose one of:

- give each direction its own transform state (a second z_stream for
  ZBuffer, a second state_type for BasicTextBuffer), so interleaved use
  is safe, or
- keep the shared state and turn the silent discard() into a reported
  error when the other direction still has unfinished, unflushed work,
  so unsupported interleaved use fails loudly instead of losing data.
