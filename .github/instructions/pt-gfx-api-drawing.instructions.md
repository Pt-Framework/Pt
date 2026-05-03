---
description: "Painter, paint surface, path, text, and drawing state APIs in Pt::Gfx."
---

# Pt::Gfx Drawing API

The Pt::Gfx drawing API provides the high-level types used to render 2D
graphics, text, images and bitmaps on paint surfaces and off-screen bitmaps.

For detailed documentation with examples, read:
- `doc/groups/drawing.dox` — Drawing concepts, painter lifecycle and API boundaries
- `doc/pages/drawing.page` — Composed drawing overview page
- `include/Pt/Gfx/Painter.h` — Concrete painter entry point
- `include/Pt/Gfx/PainterBase.h` — Shared drawing operations and paint state
- `include/Pt/Gfx/PaintSurface.h` — Abstract drawing target
- `include/Pt/Gfx/PaintContext.h` — Active painting session on a target
- `include/Pt/Gfx/Canvas.h` — Backend canvas interface used by painters
- `include/Pt/Gfx/Bitmap.h` — Off-screen drawing target backed by an image
- `include/Pt/Gfx/Paint.h` — Composition, pen, brush and font state
- `include/Pt/Gfx/Path.h` — Paths, path elements and path iteration
- `include/Pt/Gfx/Pen.h` — Outline stroke configuration
- `include/Pt/Gfx/Brush.h` — Fill configuration, textures and gradients
- `include/Pt/Gfx/Transform.h` — Coordinate transforms
- `include/Pt/Gfx/CompositionMode.h` — Blend/composition modes
- `include/Pt/Gfx/Font.h` — Font requests for text drawing
- `include/Pt/Gfx/FontMetrics.h` — Font-wide metrics
- `include/Pt/Gfx/TextMetrics.h` — Measured text bounds and advance
- `include/Pt/Gfx/Scaling.h` — Logical/physical coordinate conversion

## When to Use Which

| Type | Use when | Notes |
|------|----------|-------|
| `Painter` | You want to issue drawing commands directly on a `PaintSurface` or `PaintContext` | Main entry point for application code |
| `PainterBase` | You are extending or wrapping the painter API | Holds shared drawing operations and state |
| `PaintSurface` | You need an abstract rendering target | Surface owns backend resources and canvas creation |
| `PaintContext` | You already have an active painting session | Keeps surface and painter lifetime coordinated |
| `Canvas` | You are implementing a drawing backend | Executes backend-specific drawing primitives |
| `Bitmap` | You need an off-screen render target backed by image data | Bridges drawing and image APIs |
| `Paint` | You want to bundle pen, brush, font and composition state | Reuse across multiple drawing operations |
| `Pen` | You need to stroke outlines | Controls width, color, caps, joins and dashes |
| `Brush` | You need to fill closed shapes or textured areas | Supports solid, texture and gradient fills |
| `ColorStop` / `ColorStops` | You need a gradient definition | Used to build linear and radial gradients |
| `Path` | You need reusable vector geometry | Supports lines, curves, arcs and flattening |
| `PathElement` / `PathIterator` | You need to inspect path contents | Read-only iteration over path commands |
| `Transform` | You need to transform drawing coordinates | Use for translation, scaling, rotation and shear |
| `CompositionMode` | You need to control blending | `SourceCopy` and `SourceOver` are the key modes |
| `Font` | You need to select a font for text drawing or measurement | Describes family, size and style request |
| `FontMetrics` | You need ascent, descent or line metrics | Font-wide metrics for layout |
| `TextMetrics` | You need measured bounds or advance for one text run | Returned by `PainterBase::textMetrics()` and `Canvas::textMetrics()` |
| `Scaling` | You need logical/physical coordinate conversion or pixel alignment | Important for crisp rendering on scaled targets |

## Agent Guidance

- Use `Painter` for application-facing drawing code.
- Use `Paint` when several operations should share the same pen, brush, font or composition mode.
- Use `Path` for reusable geometry, especially when the same outline may be stroked and filled.
- Use `Bitmap` when drawing output must be reused as an image later.
- Use `Scaling` and `Transform` instead of hand-written coordinate conversions.
- Do not use the drawing API for direct pixel iteration; for that, switch to the image and pixel APIs documented in `pt-gfx-api-images.instructions.md`.