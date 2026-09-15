---
description: "Forms invalidation, layout, painting, graphics backends, and pixmaps"
---

- Forms layouting and painting group overview (Pt-Forms-Updating):
  `include/Pt/Forms/Api-Updating.h`
- Invalidate a widget or handle InvalidateEvent to schedule a redraw:
  `include/Pt/Forms/InvalidateEvent.h`
- Handle MeasureEvent, LayoutEvent, and RescaleEvent for measure/layout/DPI-rescale passes:
  `include/Pt/Forms/LayoutEvent.h`
- Handle PaintEvent and its dirty rectangle:
  `include/Pt/Forms/PaintEvent.h`
- Use PaintContext for an active painting session and its clip:
  `include/Pt/Forms/PaintContext.h`
- Use Painter to draw pixmaps and graphics on a paint surface or context:
  `include/Pt/Forms/Painter.h`
- Use PaintSurface as a render target and query its size:
  `include/Pt/Forms/PaintSurface.h`
- Implement IPixmapImpl or use Pixmap to load, scale, and render platform pixmaps:
  `include/Pt/Forms/Pixmap.h`
- Implement GraphicsBackend to create pixmap implementations and enumerate fonts:
  `include/Pt/Forms/GraphicsBackend.h`
- Configure SizePolicy Mode and size hints used by Control::measure():
  `include/Pt/Forms/SizePolicy.h`