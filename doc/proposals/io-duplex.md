# I/O Duplex

## Documents

- [Device](io-duplex-device.md)
- [Transforms](io-duplex-transforms.md)
- [Concept](../concepts/io-duplex.md)

## Open Tasks

- [codec-direction-state](io-duplex-transforms.md#codec-direction-state).
  ZBuffer and BasicTextBuffer hold one piece of transform state shared
  by both directions and silently discard the other direction's
  buffered data when they switch.

## Priority

The codec direction state is the largest change and the one with the
least existing demand: zlib and text codec transforms are mostly used
one-directionally today. Resolve it by giving each direction
independent state, or by replacing the silent discard with a reported
error, whichever the implementation task chooses.
