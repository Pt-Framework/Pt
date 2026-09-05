---
description: "Images, formats, pixels, views, conversion, scaling, and image codecs in Pt::Gfx."
---

- Image ownership, formats, views, and pixel access concepts:
	`include/Pt/Gfx/Api-Images.h`
- Create format-generic images, own or wrap image data:
	`include/Pt/Gfx/Image.h`
- Define runtime image formats and access generic pixels:
	`include/Pt/Gfx/ImageFormat.h`
- Define image and view traits for compile-time pixel access:
	`include/Pt/Gfx/ImageTraits.h`
- Access and traverse an individual pixel:
	`include/Pt/Gfx/Pixel.h`
- Build format-specific pixel access implementations:
	`include/Pt/Gfx/PixelBase.h`
- Color types for generic runtime image formats:
	`include/Pt/Gfx/Color.h`
- Use ARGB-32 colors, pixels, and image formats:
	`include/Pt/Gfx/Argb32.h`
- Use premultiplied RGB-32 colors, pixels, and image formats:
	`include/Pt/Gfx/Rgb32.h`
- Use RGB-16 colors, pixels, and image formats:
	`include/Pt/Gfx/Rgb16.h`
- Use YUV-12 colors, pixels, and image formats:
	`include/Pt/Gfx/Yuv12.h`
- Create an owning image with ARGB-32 pixels:
	`include/Pt/Gfx/Argb32Image.h`
- Create an owning image with premultiplied RGB-32 pixels:
	`include/Pt/Gfx/Rgb32Image.h`
- Create an owning image with YUV-12 pixels:
	`include/Pt/Gfx/Yuv12Image.h`
- Access a rectangular image region without copying:
	`include/Pt/Gfx/View.h`
- Work with the common base of image views:
	`include/Pt/Gfx/ViewBase.h`
- Access a single pixel as a non-owning view:
	`include/Pt/Gfx/PixelView.h`
- Access a single image row as a non-owning view:
	`include/Pt/Gfx/LineView.h`
- Work with contiguous pixel spans:
	`include/Pt/Gfx/Span.h`
- Copy or fill pixel ranges efficiently:
	`include/Pt/Gfx/CopyPixel.h`
- Convert between color representations:
	`include/Pt/Gfx/ConvertColor.h`
- Downscale image blocks:
	`include/Pt/Gfx/BlockScale.h`
- Read JPEG images:
	`include/Pt/Gfx/JpegReader.h`
- Read PNG images:
	`include/Pt/Gfx/PngReader.h`
- Write PNG images:
	`include/Pt/Gfx/PngWriter.h`
