/* Copyright (C) 2015 Marc Boris Duerner 
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  
  02110-1301 USA
*/

#ifndef PT_GFX_API_H
#define PT_GFX_API_H

#include <Pt/Api.h>
#include <cstddef>

#define PT_GFX_VERSION_MAJOR PT_VERSION_MAJOR
#define PT_GFX_VERSION_MINOR PT_VERSION_MINOR
#define PT_GFX_VERSION_REVISION PT_VERSION_REVISION
#define PT_GFX_VERSION_PRERELEASE PT_VERSION_PRERELEASE

#if defined(PT_GFX_API_EXPORT)
#  define PT_GFX_API PT_EXPORT
#else
#  define PT_GFX_API PT_IMPORT
#endif

namespace Pt {

/** @namespace Pt::Gfx
    @brief Graphics and imaging services.

  %Pt::Gfx provides APIs for drawing 2D shapes and text on different paint
  surfaces, such as in-memory bitmaps. It also provides image, view and pixel
  classes for storing image data, accessing pixels and working with image
  formats.
*/
namespace Gfx {

typedef double Float;
typedef std::ptrdiff_t Int;

class Canvas;
class FontMetrics;
class Paint;
class PainterBase;
class Painter;
class PaintSurface;
class TextMetrics;

//
// Images and views
//

class ColorF;
class Color;
typedef Color Color;

template <typename FormatT>
struct ImageTraits;

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class BasicImage;

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class BasicConstImage;

template <typename FormatT, typename TraitsT = ImageTraits<FormatT>>
class BasicView; 

template <typename FormatT, typename TraitsT = ImageTraits<FormatT>>
class BasicConstView;

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class BasicPixelView; 

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class BasicConstPixelView;

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class BasicLineView; 

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class BasicConstLineView;

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class Span; 

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class ConstSpan;

//
// Generic images
//

class ImageFormat;

template <typename ColorT>
class Pixel;

template <typename ColorT>
class ConstPixel;


typedef BasicImage<ImageFormat> Image;

typedef BasicConstImage<ImageFormat> ConstImage;

typedef BasicView<ImageFormat> ImageView;

typedef BasicConstView<ImageFormat> ConstImageView;

typedef BasicPixelView<ImageFormat> PixelView;

typedef BasicConstPixelView<ImageFormat> ConstPixelView;

typedef BasicLineView<ImageFormat> LineView;

typedef BasicConstLineView<ImageFormat> ConstLineView;

typedef Span<ImageFormat> ImageSpan;

typedef ConstSpan<ImageFormat> ConstImageSpan;


struct ImageTraitsF;

typedef BasicImage<ImageFormat, ImageTraitsF> ImageF;

typedef BasicConstImage<ImageFormat, ImageTraitsF> ConstImageF;

typedef BasicView<ImageFormat, ImageTraitsF> ImageViewF;

typedef BasicConstView<ImageFormat, ImageTraitsF> ConstImageViewF;

typedef BasicPixelView<ImageFormat, ImageTraitsF> PixelViewF;

typedef BasicConstPixelView<ImageFormat, ImageTraitsF> ConstPixelViewF;

typedef BasicLineView<ImageFormat, ImageTraitsF> LineViewF;

typedef BasicConstLineView<ImageFormat, ImageTraitsF> ConstLineViewF;

typedef Span<ImageFormat, ImageTraitsF> ImageSpanF;

typedef ConstSpan<ImageFormat, ImageTraitsF> ConstImageSpanF;

//
// ARGB-32
//

class Argb32;
class Argb32Pixel;
class Argb32ConstPixel;

/** @brief ARGB-32 image.
*/
typedef BasicImage<Argb32> Argb32Image;

/** @brief ARGB-32 const image.
*/
typedef BasicConstImage<Argb32> Argb32ConstImage;

/** @brief ARGB-32 view.
*/
typedef BasicView<Argb32> Argb32View;

/** @brief ARGB-32 const view.
*/
typedef BasicConstView<Argb32> Argb32ConstView;

/** @brief ARGB-32 pixel view.
*/
typedef BasicPixelView<Argb32> Argb32PixelView;

/** @brief ARGB-32 const pixel view.
*/
typedef BasicConstPixelView<Argb32> Argb32ConstPixelView;

/** @brief ARGB-32 line view.
*/
typedef BasicLineView<Argb32> Argb32LineView;

/** @brief ARGB-32 const line view.
*/
typedef BasicConstLineView<Argb32> Argb32ConstLineView;

/** @brief ARGB-32 span.
*/
typedef Span<Argb32> Argb32Span;

/** @brief ARGB-32 const span.
*/
typedef ConstSpan<Argb32> Argb32ConstSpan;

//
// RGB-16
//

class Rgb16;
class Rgb16Pixel;
class Rgb16ConstPixel;

/** @brief RGB-16 image.
*/
typedef BasicImage<Rgb16> Rgb16Image;

/** @brief RGB-16 const image.
*/
typedef BasicConstImage<Rgb16> Rgb16ConstImage;

/** @brief RGB-16 view.
*/
typedef BasicView<Rgb16> Rgb16View; 

/** @brief RGB-16 const view.
*/
typedef BasicConstView<Rgb16> Rgb16ConstView;

/** @brief RGB-16 pixel view.
*/
typedef BasicPixelView<Rgb16> Rgb16PixelView;

/** @brief RGB-16 const pixel view.
*/
typedef BasicConstPixelView<Rgb16> Rgb16ConstPixelView;

/** @brief RGB-16 line view.
*/
typedef BasicLineView<Rgb16> Rgb16LineView;

/** @brief RGB-16 const line view.
*/
typedef BasicConstLineView<Rgb16> Rgb16ConstLineView;

/** @brief RGB-16 span.
*/
typedef Span<Rgb16> Rgb16Span;

/** @brief RGB-16 const span.
*/
typedef ConstSpan<Rgb16> Rgb16ConstSpan;

//
// RGB-32 (Premultiplied ARGB-32)
//

class Rgb32;
class Rgb32Pixel;
class Rgb32ConstPixel;

/** @brief RGB-32 image.
*/
typedef BasicImage<Rgb32> Rgb32Image;

/** @brief RGB-32 const image.
*/
typedef BasicConstImage<Rgb32> Rgb32ConstImage;

/** @brief RGB-32 view.
*/
typedef BasicView<Rgb32> Rgb32View; 

/** @brief RGB-32 const view.
*/
typedef BasicConstView<Rgb32> Rgb32ConstView;

/** @brief RGB-32 pixel view.
*/
typedef BasicPixelView<Rgb32> Rgb32PixelView;

/** @brief RGB-32 const pixel view.
*/
typedef BasicConstPixelView<Rgb32> Rgb32ConstPixelView;

/** @brief RGB-32 line view.
*/
typedef BasicLineView<Rgb32> Rgb32LineView;

/** @brief RGB-32 const line view.
*/
typedef BasicConstLineView<Rgb32> Rgb32ConstLineView;

/** @brief RGB-32 span.
*/
typedef Span<Rgb32> Rgb32Span;

/** @brief RGB-32 const span.
*/
typedef ConstSpan<Rgb32> Rgb32ConstSpan;

//
// YUV-12
//

class Yuv12;
class Yuv12Pixel;
class Yuv12ConstPixel;

/** @brief YUV-12 image.
*/
typedef BasicImage<Yuv12> Yuv12Image;

/** @brief YUV-12 const image.
*/
typedef BasicConstImage<Yuv12> Yuv12ConstImage;

/** @brief YUV-12 view.
*/
typedef BasicView<Yuv12> Yuv12View; 

/** @brief YUV-12 conmst view.
*/
typedef BasicView<Yuv12> Yuv12ConstView;

/** @brief YUV-12 pixel view.
*/
typedef BasicPixelView<Yuv12> Yuv12PixelView;

/** @brief YUV-12 const pixel view.
*/
typedef BasicConstPixelView<Yuv12> Yuv12ConstPixelView;

/** @brief YUV-12 line view.
*/
typedef BasicLineView<Yuv12> Yuv12LineView;

/** @brief YUV-12 const line view.
*/
typedef BasicConstLineView<Yuv12> Yuv12ConstLineView;

/** @brief YUV-12 span.
*/
typedef Span<Yuv12> Yuv12Span;

/** @brief YUV-12 const span.
*/
typedef ConstSpan<Yuv12> Yuv12ConstSpan;
} // namespace

} // namespace

#endif
