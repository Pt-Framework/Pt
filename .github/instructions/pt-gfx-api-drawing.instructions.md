---
description: "Drawing Graphics and Text"
---

- Concepts, painter lifecycle, API boundaries:
  `doc/groups/pt-gfx-drawing.dox`
- Draw lines, rectangles, circles, ellipses, arcs, pie, chord, polygon:
  `include/Pt/Gfx/PainterBase.h`
- Draw and fill paths, draw text, draw images, set clip, set pen/brush/font:
  `include/Pt/Gfx/PainterBase.h`
- Create painter, begin painting on surface or context:
  `include/Pt/Gfx/Painter.h`
- Abstract render target, surface size, format, scaling:
  `include/Pt/Gfx/PaintSurface.h`
- Active painting session, manage painter lifetime:
  `include/Pt/Gfx/PaintContext.h`
- Backend canvas, low-level drawing primitives:
  `include/Pt/Gfx/Canvas.h`
- Off-screen bitmap, in-memory render target, extract image:
  `include/Pt/Gfx/Bitmap.h`
- Bundle pen, brush, font, blend mode as reusable state:
  `include/Pt/Gfx/Paint.h`
- Line width, line color, dash, dot, cap style, join style:
  `include/Pt/Gfx/Pen.h`
- Solid fill, texture fill, linear gradient, radial gradient, color stops:
  `include/Pt/Gfx/Brush.h`
- Build paths, move/line/curve/arc, rounded rect, point containment:
  `include/Pt/Gfx/Path.h`
- Translate, scale, rotate, shear, compose, invert transforms:
  `include/Pt/Gfx/Transform.h`
- Blend mode, source copy, source over alpha:
  `include/Pt/Gfx/CompositionMode.h`
- Font family, size, weight, italic, stretch, custom font files:
  `include/Pt/Gfx/Font.h`
- Ascent, descent, line height, leading, underline position:
  `include/Pt/Gfx/FontMetrics.h`
- Measure text width, text bounding box, text advance, bearing:
  `include/Pt/Gfx/TextMetrics.h`
- Logical/physical coords, pixel alignment, scale factor, DPI:
  `include/Pt/Gfx/Scaling.h`