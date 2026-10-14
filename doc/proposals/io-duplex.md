# I/O Duplex

## Documents

- [Device](io-duplex-device.md)
- [Transforms](io-duplex-transforms.md)
- [Concept](../concepts/io-duplex.md)

## Open

- [offset-coupling](io-duplex-device.md#offset-coupling).
  OverlappedIODeviceImpl advances both OVERLAPPED offsets after either
  direction completes, and position()/seek(0, cur) reads the stale OS
  file pointer. Give each direction its own offset and make the read
  offset the public current position.

- [ssl-want-write](io-duplex-transforms.md#ssl-want-write).
  Connection::read and Connection::write (openssl, mbedtls) treat the
  cross-direction want-code as a hard failure instead of a retryable
  state.

- [codec-direction-state](io-duplex-transforms.md#codec-direction-state).
  ZBuffer and BasicTextBuffer hold one piece of transform state shared
  by both directions and silently discard the other direction's
  buffered data when they switch.

## Priority

The SSL want-write handling comes first. It makes the data phase of
an already full-duplex-capable layer robust against protocol-internal
interleaving, without touching the existing reentrancy guard.

The offset coupling in the Windows overlapped implementation only
matters once a seekable device is driven with concurrent read and
write; it can trail the socket- and pipe-facing work above.

The codec direction state is the largest change and the one with the
least existing demand: zlib and text codec transforms are mostly used
one-directionally today. Resolve it by giving each direction
independent state, or by replacing the silent discard with a reported
error, whichever the implementation task chooses.
