---
description: "Pixel types, image formats, and image access patterns in Pt::Gfx."
---

# Pixel Types

Pixel types provide access to pixel data in an image. They come in
mutable/const pairs. Every format class (e.g. `Argb32`) defines a
`Pixel` and `ConstPixel` typedef for its concrete pixel type.

The formal requirements are defined as C++20 concepts `PixelLike`
and `ConstPixelLike` in `Pixel.h`, guarded by `#ifdef __cpp_concepts`.
The project targets C++14, so the concepts serve as documentation only.

| Format   | Pixel         | ConstPixel         | ColorType | Header     |
|----------|---------------|--------------------|-----------|------------|
| `Argb32` | `Argb32Pixel` | `Argb32ConstPixel` | `Color`   | `Argb32.h` |
| `Rgb32`  | `Rgb32Pixel`  | `Rgb32ConstPixel`  | `Color`   | `Rgb32.h`  |
| `Rgb16`  | `Rgb16Pixel`  | `Rgb16ConstPixel`  | `Color`   | `Rgb16.h`  |
| `Yuv12`  | `Yuv12Pixel`  | `Yuv12ConstPixel`  | `ColorF`  | `Yuv12.h`  |

Generic: `Pixel<ColorT>` / `ConstPixel<ColorT>` in `ImageFormat.h`
with type erasure used with the ImageFormat.

`PixelBase` is the abstract base for the internal pixel implementation
used by `Pixel<ColorT>` / `ConstPixel<ColorT>`. Each format has a
concrete `XxxPixelBase` subclass.

- **`operator=` is deleted** on all pixel types. To rebind a pixel to
  a different position, use `reset()`. To write a color value, use
  `operator=(const ColorType&)` or `assign()`.

## When to Use Which

- **Concrete pixels** (`Argb32Pixel`, etc.) when the format is known
  at compile time. They are lightweight value types with no virtual
  dispatch and enable format-specific optimizations.
- **Generic pixels** (`Pixel<Color>`, `Pixel<ColorF>`) when the format
  is only known at runtime (e.g. generic image processing that works
  on any `BasicImage<ImageFormat>`).
- **`Color` (= `Argb32Color`)** is the default 8-bit color type.
  Use it unless you need higher precision.
- **`ColorF`** is the 16-bit floating-point color type. Required when 
  precision matters (e.g. gradients, blending).

## Adding a New Pixel Format

When adding a new format `Xxx`:
1. Create `XxxPixelBase` deriving from `PixelBase`, override
   `onGetType()`, `onAssignPixels()`, `onCopyPixels()`, and color
   accessors.
2. Create `XxxPixel` and `XxxConstPixel` satisfying `PixelLike` /
   `ConstPixelLike` (see `Pixel.h` for the full requirements).
3. Create the format class `Xxx` with `Pixel`/`ConstPixel` typedefs
   and static helpers (`getColorF`, `assign`, `fill`, etc.).
. Verify the new pixel type satisfies the concepts (if building with
   C++20) by instantiating `static_assert(PixelLike<XxxPixel>)` in a
   test.

