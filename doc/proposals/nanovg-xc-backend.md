---
proposal: nanovg-xc.md
---

# NanoVG XC Backend

The NanoVG backend is replaced by nanovgXC. The public Canvas and
Pixmap model stays. Recording into a display list and replaying at
flush time stays. The concept is [NanoVG](../concepts/nanovg.md).

## Baseline

The current backend uses classic NanoVG with fringe antialiasing.
Dashed strokes, multi-stop gradients, and even-odd fill rules are
not available. The software path does not exist. OpenGL ES 2 is
the minimum.

## Backend {#backend}

nanovgXC replaces the classic NanoVG implementation. The same
recording and replay structure is kept. The backend selects an
OpenGL or a software renderer at context creation.

The OpenGL path uses the nanovgXC GL or vector-texture backend.
The software path uses the nanovgXC software rasterizer. Both
paths accept the same recorded commands.

Exact-coverage antialiasing is used. Even-odd and non-zero fill
rules are supported. Multi-stop gradients and dashed strokes are
available through the nanovgXC API. The existing PaintCommand or
display-list recording does not change for these features.

Text continues to use the existing font provider. Glyph runs may
be passed to nanovgXC. SDF text is optional and not required for
the first integration.

The device still owns one shared context. Pixmaps still flush into
one frame. The window blit stays a textured quad. No public API
changes are required.
