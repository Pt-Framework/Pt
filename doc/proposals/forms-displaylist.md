# Forms Display Lists

## Documents

- [Commands](forms-displaylist-commands.md)
- [NanoVG](forms-displaylist-nanovg.md)

## Open Tasks

- [commands](forms-displaylist-commands.md#commands).
  A flat sequence of drawing operations. Each command carries its
  arguments. Resources are referenced by id.

- [builder](forms-displaylist-commands.md#builder).
  Appends commands. Holds the current transform, clip, and style
  until the next change.

- [replay](forms-displaylist-commands.md#replay).
  Plays the sequence on a painter. The painter sees the same
  operations the builder recorded.

- [nanovg](forms-displaylist-nanovg.md#nanovg).
  The NanoVG backend records into a display list and replays it
  at the end of the frame. The previous paint-command queue is
  removed.

## Priority

Commands and the NanoVG backend first.
