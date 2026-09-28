# GPU 2D Backends beyond NanoVG {#vulkan}

This document records the options for a hardware-accelerated 2D backend
that can sit beside Direct2D, Core Graphics, Blend2D, and NanoVG. The
trigger is Linux / Wayland with a real GPU. Skia is rejected as a
dependency. Blend2D stays the CPU pixmap path. NanoVG stays the GLES2
fallback; the analysis of that binding is in [nanovg.md](nanovg.md).

The public surface stays `Pt::Gfx::Canvas` / `Painter` on a `Pixmap`.
A new library is another `GraphicsBackend`, not a second painting API.

This chapter covers:

- [Constraints](#vk-constraints)
- [What is not a candidate](#vk-reject)
- [NanoVG family](#vk-nanovg-family)
- [ThorVG](#vk-thorvg)
- [TGFX](#vk-tgfx)
- [VKVG](#vk-vkvg)
- [LVGL](#vk-lvgl)
- [Text, Arabic, Hebrew](#vk-text)
- [Painter versus HTML5 Canvas](#vk-canvas)
- [TGFX versus VKVG](#vk-compare)
- [Recommended shape](#vk-shape)
- [Out of scope](#vk-scope)

## Constraints {#vk-constraints}

Pt-Forms already vendors several painters. A replacement does not have
to look like NanoVG. It may be larger than Blend2D if the CMake or
Meson story is still something a person can vendor. Extracting an
engine from another toolkit is acceptable when the cut is clean.

Must-haves:

- hardware acceleration usable on Linux (OpenGL 3 / GLES 3 and/or Vulkan)
- a drawing model that maps onto Painter (paths, fill, stroke, clip,
  transform, images, text)
- text that is either first-class or combinable with FreeType
- later: Arabic and Hebrew, which means HarfBuzz plus a bidi pass

Nice-to-have:

- GLES3 *and* Vulkan behind one API (one backend, two present paths)
- a glyph-run entry point so HarfBuzz can live in Pt, not in the GPU
  library
- a surface that can target an existing FBO or Vulkan image, the way
  `NanoVGDevice::bindRenderTarget` already does

Vivante / i.MX8 remains the GLES2 floor. A Vulkan- or GLES3-only
engine does not retire NanoVG; it sits next to it.

## What is not a candidate {#vk-reject}

- **Skia** — the size and build tax that started this search.
- **Vello / piet-gpu** — Rust, wgpu, compute shaders. Weak or missing
  on Intel and Vivante. No C++ drop-in.
- **Pathfinder 3** — strong model, Rust, incomplete C bindings,
  development stalled.
- **Rive renderer** — C++, GL and Vulkan, built for animated vector
  artboards, not for `FillRect` of a button strip.
- **nanovg_vulkan (danilw)** — same NanoVG API on Vulkan 1.0. Unmaintained.
  Immediate-mode; does not use Vulkan as a retained API.
- **rvg (nyorain)** — retained Vulkan, NanoVG-inspired, unmaintained.
- **AmanithVG / OpenVG** — licence or a dead standard.
- **NV_path_rendering** — NVIDIA only.
- **Cairo-GL** — abandoned in practice.
- **Magnum** — GL middleware, not a 2D vector engine.

## NanoVG family {#vk-nanovg-family}

**nanovgXC** keeps the NanoVG API and replaces fringe coverage with
exact-coverage / GPU winding. GLES3 / GL3, optional software. Dashed
strokes, more than two gradient stops, SDF text. The only realistic
NanoVG upgrade. On pure GLES2 it falls back to a two-FBO path or to
CPU. Useful if the GLES2 backend stays and path quality becomes the
problem. It does not solve Arabic and it does not add Vulkan.

**vg-renderer** is NanoVG ideas plus ImGui-style batching on bgfx.
Only interesting if Pt takes bgfx as a multi-API layer.

Keep classic NanoVG (or XC) as the GLES2 backend. Do not grow it into
the Arabic or Vulkan story.

## ThorVG {#vk-thorvg}

C++, MIT, Meson, already vendored by Godot. Core plus selectable
backends: CPU/SIMD, OpenGL 3.3 / GLES 3.0, WebGPU. No first-class
Vulkan; WebGPU on Linux usually sits on `wgpu-native` / Vulkan.

API is `Canvas` plus a retained `Paint` / `Shape` / `Scene` graph.
`GlCanvas::target(...)` can bind an existing FBO. Immediate use
(create a `Shape` per `FillRect`) works but wastes the library.
Widget paths that do not change should be retained nodes.

Text is an in-tree SFNT parser. Glyphs become vector paths. Metrics
exist. There is no HarfBuzz and no bidi. Horizontal Latin and simple
CJK are fine. Arabic and Hebrew through `Text::setText` will be wrong.
Shaped glyph ids can still be turned into paths if Pt owns HarfBuzz.

Build is closer to Blend2D than to TGFX: Meson flags for `engines=sw,gl`
and the TTF loader, no Node vendor tool. Do not extract only the GL
engine. The `Paint` / `Canvas` layer *is* the API.

ThorVG is the small GLES3 experiment. It is not the text engine for
complex scripts.

## TGFX {#vk-tgfx}

Tencent Graphics. BSD-3. CMake plus a `vendor/` / `depsync` tree.
Larger than ThorVG or VKVG, much smaller than Skia. Device → Context →
Surface → Canvas. Immediate `drawRect` / `drawPath` / `drawImage` /
`drawGlyphs` / `drawTextBlob`, plus `Picture` recording and `saveLayer`.

Backends: OpenGL 3.2, GLES 3, Vulkan 1.1, WebGPU, Metal in progress.
Linux is supported. Stock Linux demos often fall back to SwiftShader;
the EGL or Vulkan context still has to be bound the way NanoVGDevice
binds EGL today.

FreeType is a CMake switch (`TGFX_USE_FREETYPE`). `drawGlyphs` is the
hook for a Pt-owned HarfBuzz run. Blend modes, image filters, and
Picture recording are in the Skia-shaped feature set.

The tax is the build, not the API. It is not an amalgamated Blend2D
source file. Take TGFX when one engine should cover GLES3 and Vulkan
and a later HTML5-like canvas is on the table.

## VKVG {#vk-vkvg}

Vulkan Vector Graphics. C API after Cairo. MIT. CMake ≥ 3.16, Vulkan
≥ 1.1. Precompiled SPIR-V lives in the tree. Status: alpha, core API
called stable, one primary maintainer.

Objects: Device, Surface, Context, Pattern. Calls match Painter almost
one to one: `move_to` / `line_to` / `curve_to` / `fill` / `stroke` /
`clip` / `save` / `restore` / `set_source`.

Implemented: even-odd and non-zero fill, stroke caps/joins/dashes,
linear and radial patterns, surface patterns, font cache, PNG I/O.
Optional `-DVKVG_USE_FREETYPE`, `-DVKVG_USE_HARFBUZZ`,
`-DVKVG_USE_FONTCONFIG`. Without HarfBuzz, Latin through FreeType is
still usable. An experimental recording flag exists.

Compositing is the hole. `vkvg.h` only enables Clear, Source, Over,
and Difference. The rest of the Cairo operators are commented out.
There is no `saveLayer`, no image filter, no conic gradient.

Vulkan-only. Vivante Vulkan quality is the risk, not the mapping to
Painter. Use VKVG as a Wayland-Vulkan painter next to Blend2D (CPU)
and NanoVG (GLES2), not as the HTML5 canvas implementation.

## LVGL {#vk-lvgl}

Do not extract LVGL draw units.

`LV_USE_DRAW_OPENGLES` software-rasters tiles and caches them as
textures. That is not a vector painter. It loses when the frame
changes every vsync.

`LV_USE_DRAW_NANOVG` is NanoVG behind `lv_draw_task_t`. Pt already has
the library without the LVGL task model.

Both units are coupled to LVGL layers and caches. Taking them means
taking the toolkit. Quad and atlas ideas can be copied. The source
should not.

## Text, Arabic, Hebrew {#vk-text}

Two steps happen before any backend paints a glyph:

1. **Bidi** — logical order to visual runs (FriBidi, SheenBidi, or ICU).
   HarfBuzz does not do this.
2. **Shaping** — HarfBuzz reads GSUB/GPOS (init/medi/fina/isol,
   lam-alef, mark placement). Without it, Arabic stays isolated letters.

Direct2D and Core Graphics already shape through the platform.
Blend2D has `fillGlyphRun` but no shaper; a small wrapper
(`blend2d_shaping`) shows the pattern. NanoVG `nvgText` and ThorVG
`Text::setText` must not be the Arabic path.

HarfBuzz belongs in Pt-Gfx / Forms, once, vendored like Blend2D.
`hb-ft` attaches to the same FreeType face the NanoVG font provider
already opens. The GPU library receives a glyph run:

```
Pt::String
    -> bidi (runs + direction)
    -> HarfBuzz (glyph id, advance, offset)
    -> backend drawGlyphRun(font, glyphs[])
```

| Backend        | Built-in shaping     | Glyph-run hook                         |
|----------------|----------------------|----------------------------------------|
| Direct2D / CG  | platform             | native text, or skip Pt shaper         |
| Blend2D        | no                   | `fillGlyphRun`                         |
| NanoVG         | no                   | per-glyph quad or path only            |
| ThorVG         | cmap only, LTR       | paths from shaped ids, not `Text`      |
| VKVG           | optional HarfBuzz    | text run + glyph positions             |
| TGFX           | FreeType             | `drawGlyphs` / `drawTextBlob`          |

Prove the stack on Blend2D first (Amiri / Noto Naskh and a Hebrew
face). When baseline and mark placement match the raster backend, the
GPU choice is only who blits the run.

## Painter versus HTML5 Canvas {#vk-canvas}

`Pt::Painter` is pen *and* brush, clip, font, transform — Cairo /
QPainter / Direct2D. HTML5 Canvas is one context (`fillStyle`,
`strokeStyle`, `globalCompositeOperation`) plus `Path2D` and ImageData.

Neither TGFX nor VKVG is a drop-in for either API. Both can carry the
Forms painter. Only TGFX is in the league of a later Canvas2D facade.

| Capability                 | Forms needs | HTML5        | TGFX                         | VKVG                         |
|----------------------------|-------------|--------------|------------------------------|------------------------------|
| Affine + save/restore      | yes         | yes          | yes                          | yes                          |
| Clip rect / path           | yes         | yes          | `clipRect` / `clipPath`      | `vkvg_clip`                  |
| Fill / stroke path         | yes         | yes          | yes                          | yes                          |
| Caps, joins, dash          | yes         | yes          | via `Paint`                  | yes                          |
| Solid + linear/radial      | yes         | yes (+conic) | shaders                      | patterns                     |
| Image / texture brush      | yes         | pattern      | `drawImageRect`, shader      | surface pattern              |
| Text + metrics             | yes         | fill/measure | simple text + glyphs         | `show_text` + text run       |
| Glyph run (HarfBuzz)       | later       | internal     | `drawGlyphs`                 | glyph positions              |
| Porter-Duff / blend        | partial     | full         | Skia blend modes             | Clear / Source / Over / Diff |
| saveLayer / group + filter | rare        | offscreen    | `saveLayer` + ImageFilter    | no                           |
| Readback                   | pixmap      | ImageData    | surface readback             | surface → host               |
| Path boolean               | no          | no (addPath) | PathOps-class                | no                           |

Forms today (rect, round rect, ellipse, path, solid/gradient/image
brush, clip, transform, opacity, text, blit) fits both. The native
Direct2D / CG backends still have more blend and layer behaviour than
VKVG. NanoVG already does not implement full HTML5 compositing; a
VKVG backend would not make that worse for widgets.

A future Pt Canvas2D object would wrap TGFX far more cheaply than VKVG.

## TGFX versus VKVG {#vk-compare}

Both are the serious options once ThorVG is ruled out as a text engine.

**VKVG** maps onto Painter with almost no conceptual glue. FreeType +
HarfBuzz can be switched on in CMake. The build is an ordinary C
project. The limits are maturity, Vulkan-only present, and a short
operator list. Good first Vulkan painter for Forms. Bad HTML5 stand-in.

**TGFX** maps onto “one Paint per call” (Skia). Stroke plus fill is two
paints, the same as any Skia backend. GLES3 and Vulkan share the
Canvas. Blend, layer, filter, Picture, and `drawGlyphs` are the reason
to pay the vendor-tool build. Good if the next years need one GPU 2D
engine on the desktop stack and maybe a Canvas2D facade. Heavier than
necessary if the only goal is Wayland widgets.

Do not ship both. The Pt boundary is `Painter` plus `drawGlyphRun`.
The library under that is replaceable.

| Goal                                         | Choice |
|----------------------------------------------|--------|
| GLES3 FBO spike, small vendor tree           | ThorVG (text via Pt glyph runs only) |
| Vulkan Painter, Cairo mental model, FT+HB    | VKVG   |
| GLES3 *and* Vulkan, later Canvas2D           | TGFX   |
| GLES2 boards                                 | keep NanoVG / XC |
| CPU, headless, readback                      | keep Blend2D |

## Recommended shape {#vk-shape}

Three layers, one new GPU engine at most:

1. **Blend2D** remains the portable CPU pixmap backend.
2. **NanoVG** remains GLES2. Fast-path quads for rect / image / glyph
   stay a NanoVG-document change, not a reason to add a fourth library.
3. **HarfBuzz + bidi** live in Pt-Gfx. First consumer is Blend2D
   `fillGlyphRun`. Every later GPU backend only implements
   `drawGlyphRun`.
4. **One** of VKVG or TGFX as the modern GPU backend:
   - spike VKVG if the next platform work is a Wayland Vulkan swapchain
     and a Cairo-shaped `IPixmapImpl`;
   - spike TGFX if the same backend must also speak GLES3 and a richer
     canvas is expected.

A spike is a `GraphicsBackend` that targets the existing shared
offscreen target (FBO or Vulkan image), implements `FillRect`,
`DrawImage`, `DrawText` / `drawGlyphRun`, and compares metrics against
Blend2D. If that does not stand up in a short integration, pick the
other library. Do not start three integrations.

## Out of scope {#vk-scope}

This document does not pick a swapchain story, a Wayland Vulkan WSI
path, or a change to the public Canvas API. It does not schedule
retiring NanoVG. Compute-based renderers and a from-scratch GLES quad
engine are left as later work if both TGFX and VKVG fail the spike.
