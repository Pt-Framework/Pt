# NanoVG Forms Backend {#nanovg}

This document describes the NanoVG renderer used by Pt-Forms, how it
maps onto the existing Canvas and Pixmap model, where the costs sit,
and what to change before replacing the library. It is a framework
concept, not an application sketch.

The backend lives under `src/Pt-Forms/nanovg/` and is enabled with
`--with-nanovg` / `WITH_FORMS_NANOVG`. On Wayland it is selected with
`PT_FORMS_WAYLAND_NANOVG`.

This chapter covers:

- [Purpose](#nanovg-purpose)
- [Architecture](#nanovg-architecture)
- [What is already right](#nanovg-right)
- [Where the cost sits](#nanovg-cost)
- [Comparison with other OpenGL 2D renderers](#nanovg-compare)
- [Would this be built differently](#nanovg-differently)
- [Improvements](#nanovg-improve)
- [Out of scope](#nanovg-scope)

## Purpose {#nanovg-purpose}

Pt-Forms paints through `Pt::Gfx::Canvas` onto a `Pixmap`. That is a
retained back-buffer model, closer to QPainter on QPixmap than to a
NanoVG demo that issues `nvgBeginFrame` directly against the window.
The NanoVG backend has to honour that model while using a GPU vector
library that only wants one open frame at a time.

The first useful result is not a general SVG engine. It is a GLES2
path that draws widgets with acceptable antialiasing on Wayland and
embedded EGL (Mesa, Vivante / i.MX8), shares one context across all
pixmaps, and presents the window without a second tessellation pass.

NanoVG is the right library for scalable paths and fringe antialiasing.
It is the wrong sole primitive engine for axis-aligned widget
rectangles, icons, and glyph blits. The architecture should keep the
library for the slow path and stop sending every `FillRect` through it.

## Architecture {#nanovg-architecture}

```
Widget paint
    |
    v
NanoVGPixmapCanvas          records PaintCommand
    |
    v
NanoVGPixmapImpl            owns one nanovg image (GLES2 texture)
    |                       flush() replays into one nvg frame
    v
NanoVGDevice                process-wide EGL + GLES2 + NVGcontext
    |                       shared offscreen FBO + stencil RBO
    v
WindowImpl::commitFrameNanovg
                            textured quad onto the EGL window surface
```

`NanoVGDevice` is a process-wide singleton. The platform layer (the
Wayland application) creates it from a native display handle. It owns:

- the EGL display, config, and GLES2 context
- a 1x1 pbuffer, or a surfaceless context when the config has no
  `EGL_PBUFFER_BIT`
- one `NVGcontext`, created as
  `nvgCreateGLES2(NVG_ANTIALIAS | NVG_STENCIL_STROKES)`
- `NanoVGFontProvider`
- a shared render-target FBO and an 8-bit stencil renderbuffer
- a small fullscreen-quad program used only for window compositing

Each pixmap is one nanovg image, created with `NVG_IMAGE_FLIPY` and
`NVG_IMAGE_PREMULTIPLIED`. Drawing commands are not issued while the
widget tree paints. `NanoVGPixmapCanvas` appends a `PaintCommand` per
Canvas operation. `NanoVGPixmapImpl::flush()` binds the shared FBO with
the pixmap texture as colour attachment, opens one `nvgBeginFrame` /
`nvgEndFrame` pair, replays the list, and clears it. Nested nanovg
frames never occur.

`nvgBeginFrame` is called with a device pixel ratio of `1.0`. HiDPI is
applied through the canvas transform and, on Wayland, through
`wl_surface_set_buffer_scale`. That works, but tessellation tolerance
then follows logical units rather than physical pixels.

Window present does not go back through NanoVG. After `flush()`,
`NanoVGDevice::renderTexturedQuad` draws the pixmap texture onto the
current EGL window surface. That avoids a second path flatten for a
fullscreen blit.

`PaintCommand` is a single struct with a type tag. State commands store
the resolved nanovg parameters (colour, width, cap, join, font handle,
clip). Draw commands own variable payloads: point lists, `Gfx::Path`,
`Gfx::Image`, UTF-8 text, or a source pixmap image handle. Composition
is forced to `NVG_SOURCE_OVER` because stencil text and fringe coverage
break under `NVG_COPY`.

Font metrics do not take fontstash's line-gap-adjusted ascender at face
value. The provider reads FreeType `ascender / units_per_EM` and the
OS/2 cap-height and x-height fields so widget baselines match the
raster backend.

## What is already right {#nanovg-right}

One `NVGcontext` for the process is the correct sharing model. The font
atlas, shaders, and GL objects stay on one context. Creating a context
per pixmap would multiply atlases and break `drawPixmap`.

Recording instead of drawing during `onBeginPaint` is required. NanoVG
does not nest frames. A pixmap that is both a window back buffer and a
blit source cannot open a frame while another frame is open on the same
context.

The shared FBO and the stencil renderbuffer that only resizes on size
change are the right resource policy for GLES2. `eglMakeCurrent` is
skipped when the context and surface are already current.

The window quad is the right present path. The expensive mistake in
many NanoVG embeddings is to fill the swapchain with `nvgImagePattern`.
Pt already avoids that.

EGL config selection tries window-plus-pbuffer first and falls back to
window-only plus surfaceless. That is what Mesa on WSL and Vivante on
i.MX8 actually expose.

Premultiplied textures and the explicit FLIPY flag match how nanovg
samples images when the FBO texture is later used as a pattern or as a
quad.

None of that should be thrown away if the renderer behind `flush()`
changes.

## Where the cost sits {#nanovg-cost}

Classic NanoVG on GLES2 is a CPU tessellator with a small GL frontend.
Each fill or stroke flattens curves on the CPU, builds fringe triangles
for antialiasing, and for non-convex fills walks the stencil buffer.
`NVG_STENCIL_STROKES` adds further stencil passes when strokes overlap.
GLES2 has no uniform buffer, so each batched draw updates uniforms with
individual `glUniform` calls.

A widget frame is mostly axis-aligned rectangles, text, and icons. A
`FillRect` of a button face is not a two-triangle colour quad. It is a
path with fringe geometry. That is the wrong hot path for Forms.

The layer above NanoVG adds more cost than the library itself on typical
UI frames.

`PaintCommand` is a fat record. Every entry carries transform, brush,
path, image, text, and point storage even when the type uses two
fields. `DrawImage` copies a whole `Gfx::Image` into the list.

The list is discarded after `flush()`. There is no generation counter
and no dirty rectangle. An unchanged widget tree is recorded again and
tessellated again.

`DrawImage` inside `flush()` converts premultiplied BGRA to tightly
packed RGBA on the CPU, calls `nvgCreateImageRGBA`, fills once, and
calls `nvgDeleteImage`. Every icon blit is an upload. Brush textures
are created on first fill and deleted on the next `SetBrush`.

`getBitmap` calls `flush()`, binds the FBO, `glReadPixels`, swaps R/B
in a CPU loop, and flips Y. That stalls the GPU. The file already notes
that an RGBA `ImageFormat` would remove the swap.

Several pixmaps sharing one FBO means every `flush()` reattaches a
colour texture. Offscreen caches and the window pixmap fight over the
same framebuffer object.

Composition is always source-over. That is required for coverage AA,
but it also means there is no clear or copy fast path for opaque widget
backgrounds.

`renderTexturedQuad` restores only the program. Blend, scissor, and
stencil are whatever NanoVG left behind. Premultiplied output wants
`GL_ONE, GL_ONE_MINUS_SRC_ALPHA` set explicitly.

Dashed pens fall back to a solid stroke. That is a feature gap, not a
hot path, but it belongs on the same list as "NanoVG is used as the
whole painter".

The window blit itself is cheap. Frame time is record plus tessellation
plus texture upload plus FBO bind.

## Comparison with other OpenGL 2D renderers {#nanovg-compare}

Classic NanoVG (GL2 / GLES2 / GL3 / GLES3) is what Pt vendors. The API
follows HTML canvas. Quality comes from fringe antialiasing and optional
stencil strokes. GL3 and GLES3 are cheaper than GLES2 because uniforms
move in a block and the shader language is less constrained. The
library is small and has no retained path cache across frames.

nanovgXC keeps a NanoVG-shaped API and changes the fill. Exact coverage
and GPU winding replace fringe triangles. Backends use framebuffer
fetch, image atomics, a two-FBO fallback, or a vector-texture shader.
It adds SDF text, dashed strokes, and gradients with more than two
stops. Complex paths get cheaper. Many tiny UI rectangles do not,
because the two-pass winding path still has setup cost.

Skia GPU (Ganesh) batches, caches tessellated paths, keeps a glyph
atlas, and can use MSAA. That is the production bar for GPU 2D UI. It
is also a large dependency with its own shader and resource lifetime
model. It does not fit Pt as a drop-in behind `Canvas`.

Custom GLES UI paths (Dear ImGui, Qt Scene Graph, LVGL's GLES draw
unit) do not tessellate buttons. They draw instanced quads and atlas
glyphs. LVGL measurements on embedded Linux showed a native GLES tile
unit around 113 FPS average against NanoVG around 62 FPS, with layers
and FBO switches as the NanoVG bottleneck. The comparison is not a
library verdict. It is evidence that widget primitive mix punishes a
general vector backend.

Blend2D is a CPU SIMD rasterizer. For small primitives and no GPU
round trip it often beats NanoVG GLES2. It is a fallback or a software
backend, not a replacement for the Wayland EGL present path.

The pattern across these renderers is the same. Fast path for rectangles,
images, and glyphs. Slow path for true vector. Pt currently sends both
through the slow path and only accelerates the final window copy.

## Would this be built differently {#nanovg-differently}

The Forms binding would not. Canvas in, pixmap texture out, one nanovg
frame per flush, quad to the window — that is the right shape for this
toolkit.

The execution inside `flush()` would. A current 2D GL UI looks like:

```
Canvas API
    -> compact command list, dirty rectangles
        -> fast path: GLES quads (FillRect, image, glyph)
        -> slow path: NanoVG for Path, ellipse, rounded stroke, AA gradient
            -> one nvgBeginFrame on the pixmap FBO
    -> window: textured quad (already present)
```

GLES3 would be probed at context creation, with GLES2 as the fallback
that exists today. nanovgXC would be an optional slow-path backend once
the UI has real path volume. Skia would not be introduced to solve
rectangle batching.

## Improvements {#nanovg-improve}

Work is ordered by leverage. Do not switch libraries before the fast
path exists.

### Fast path for common primitives

In `flush()`, keep NanoVG for `DrawPath`, `FillPath`, ellipses,
non-axis-aligned transforms, and gradients that need coverage AA.

Send solid, axis-aligned `FillRect` through a coloured quad or through
`glScissor` plus `glClear`. Send `DrawImage` and `DrawPixmap` through
`glBindTexture` and the existing quad program. Text can stay on
`nvgText` until a glyph-quad path reads the fontstash atlas directly.

That split is the largest expected win on widget frames.

### Drop stencil strokes for UI

`NVG_STENCIL_STROKES` is a quality flag for wide overlapping strokes.
Widget hairlines do not need it. Make it optional. Prefer MSAA on the
window surface and omit fringe AA when the target is already
multisampled.

### Compact, reusable commands

Replace the fat `PaintCommand` with a tagged union or a compact
variable-length stream. Images in the list should be a cache key, not a
pixel buffer.

Keep the list across frames. A generation or content hash that matches
the previous flush skips tessellation. Dirty rectangles plus `glScissor`
limit both the clear and the nanovg frame to the damaged region. The
pixmap model already allows this. The backend does not use it.

### Image and brush cache

Intern icons and brush textures on the device. Key by pointer plus
size plus a generation, or by a content hash when the pixels are not
stable. Delete on pixmap destruction or on an explicit cache trim, not
after a single fill.

If the driver exposes `GL_BGRA_EXT`, upload without the CPU R/B swap in
`buildRgba`.

### GLES3 and device pixel ratio

Try an ES 3 context and compile the GLES3 NanoVG implementation when it
succeeds. Keep the current ES 2 path as fallback.

Pass the real buffer scale into `nvgBeginFrame` so flattening tolerance
and fringe width are in physical pixels. Wayland already knows that
scale.

### Framebuffer policy

Keep the shared FBO. Stop rebinding it for every small offscreen pixmap
when the frame only needs the window pixmap. Draw the window contents
into the default framebuffer when no pixmap blit source is required.
Reserve offscreen textures for `drawPixmap` sources and for
`getBitmap`.

Avoid `getBitmap` on the animation path. If readback stays, use a pixel
buffer object and an RGBA image format so the CPU does not walk every
pixel.

### Pin GL state on the window blit

`renderTexturedQuad` should set blend, disable stencil and depth, and
either disable scissor or apply the dirty rectangle. Premultiplied
blend is `GL_ONE, GL_ONE_MINUS_SRC_ALPHA`.

### Suggested order

1. Image cache and no pixel copy inside `PaintCommand`.
2. Fast path for `FillRect` and `DrawPixmap`.
3. Optional `NVG_STENCIL_STROKES`, skip flush when the command stream is
   unchanged.
4. GLES3 probe and correct device pixel ratio.
5. Only then a path cache or a nanovgXC slow path.

## Out of scope {#nanovg-scope}

This document does not propose a Vulkan or Metal NanoVG port, a Skia
backend, or a change to the public `Canvas` / `Pixmap` API. Those are
later platform work. They must not leak into the first fast-path changes.

Dash patterns, more than two gradient stops, and even-odd fill rules
are feature gaps. They wait until the slow path is the only path that
still runs through NanoVG.
