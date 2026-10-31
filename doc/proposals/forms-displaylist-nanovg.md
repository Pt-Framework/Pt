---
proposal: forms-displaylist.md
---

# NanoVG

The NanoVG backend records drawing operations into a display list.

## NanoVG

The backend records into a display list and replays it at the end of
the frame. The previous paint-command queue is removed.
