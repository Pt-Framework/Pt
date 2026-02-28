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

#define PT_GFX_VERSION_MAJOR 1
#define PT_GFX_VERSION_MINOR 3
#define PT_GFX_VERSION_REVISION 1

#if defined(PT_GFX_API_EXPORT)
#  define PT_GFX_API PT_EXPORT
#else
#  define PT_GFX_API PT_IMPORT
#endif

namespace Pt {

/** @namespace Pt::Gfx
    @brief Graphics and Imaging

    A 2D painting API is provided that can be implemented to
    draw on different output targets with an implementation to
    draw on off-screen images. Image classes support planar
    and interleaved images and algorithms to manipulate them.
*/
namespace Gfx {

typedef float Float;
typedef std::ptrdiff_t Int;

class Canvas;
class Paint;
class Painter;
class PaintSurface;

//
// Images and views
//

class ColorF;
class Argb32Color;

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
class BasicCursorView; 

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class BasicConstCursorView;

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

struct ColorImageTraits;

class Image;

class ConstImage;

typedef BasicImage<ImageFormat, ColorImageTraits> ColorImage;

typedef BasicConstImage<ImageFormat, ColorImageTraits> ConstColorImage;

typedef BasicView<ImageFormat> ImageView; 

typedef BasicConstView<ImageFormat> ConstImageView;

typedef BasicPixelView<ImageFormat> PixelView;

typedef BasicConstPixelView<ImageFormat> ConstPixelView;

typedef BasicPixelView<ImageFormat, ColorImageTraits> ColorPixelView;

typedef BasicConstPixelView<ImageFormat, ColorImageTraits> ConstColorPixelView;

typedef BasicCursorView<ImageFormat> CursorView;

typedef BasicConstCursorView<ImageFormat> ConstCursorView;

typedef BasicCursorView<ImageFormat, ColorImageTraits> ColorCursorView;

typedef BasicConstCursorView<ImageFormat, ColorImageTraits> ConstColorCursorView;

typedef BasicLineView<ImageFormat> LineView;

typedef BasicConstLineView<ImageFormat> ConstLineView;

typedef BasicLineView<ImageFormat, ColorImageTraits> ColorLineView;

typedef BasicConstLineView<ImageFormat, ColorImageTraits> ConstColorLineView;

typedef Span<ImageFormat> ImageSpan;

typedef ConstSpan<ImageFormat> ConstImageSpan;

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

/** @brief ARGB-32 conmst view.
*/
typedef BasicView<Argb32> Argb32ConstView;

/** @brief ARGB-32 pixel view.
*/
typedef BasicPixelView<Argb32> Argb32PixelView;

/** @brief ARGB-32 const pixel view.
*/
typedef BasicConstPixelView<Argb32> Argb32ConstPixelView;

/** @brief ARGB-32 cursor view.
*/
typedef BasicCursorView<Argb32> Argb32CursorView;

/** @brief ARGB-32 const cursor view.
*/
typedef BasicConstCursorView<Argb32> Argb32ConstCursorView;

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
typedef ConstSpan<Argb32 > Argb32ConstSpan;

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

/** @brief RGB-16 cursor view.
*/
typedef BasicCursorView<Rgb16> Rgb16CursorView;

/** @brief RGB-16 const cursor view.
*/
typedef BasicConstCursorView<Rgb16> Rgb16ConstCursorView;

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

/** @brief YUV-12 cursor view.
*/
typedef BasicCursorView<Yuv12> Yuv12CursorView;

/** @brief YUV-12 const cursor view.
*/
typedef BasicConstCursorView<Yuv12> Yuv12ConstCursorView;

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
