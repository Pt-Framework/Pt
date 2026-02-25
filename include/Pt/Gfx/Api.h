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

class Canvas;
class Paint;
class Painter;
class PaintSurface;

//
// Images and views
//

template <typename FormatT>
struct ImageTraits;

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class BasicImage;

template <typename FormatT, typename TraitsT = ImageTraits<FormatT> >
class BasicConstImage;

template <typename FormatT>
struct ViewTraits;

template <typename FormatT, typename TraitsT = ViewTraits<FormatT> >
class BasicPixelView; 

template <typename FormatT, typename TraitsT = ViewTraits<FormatT> >
class BasicConstPixelView;

template <typename FormatT, typename TraitsT = ViewTraits<FormatT> >
class BasicLineView; 

template <typename FormatT, typename TraitsT = ViewTraits<FormatT> >
class BasicConstLineView;

//
// Generic images
//

class ImageFormat;
struct ColorImageTraits;
struct ColorFormatTraits;

typedef BasicConstImage<ImageFormat> ConstImage;

typedef BasicImage<ImageFormat, ColorImageTraits> ColorImage;

typedef BasicConstImage<ImageFormat, ColorImageTraits> ConstColorImage;

typedef BasicPixelView<ImageFormat> PixelView;

typedef BasicConstPixelView<ImageFormat> ConstPixelView;

typedef BasicPixelView<ImageFormat, ColorFormatTraits> ColorPixelView;

typedef BasicConstPixelView<ImageFormat, ColorFormatTraits> ConstColorPixelView;

typedef BasicLineView<ImageFormat> LineView;

typedef BasicConstLineView<ImageFormat> ConstLineView;

typedef BasicLineView<ImageFormat, ColorFormatTraits> ColorLineView;

typedef BasicConstLineView<ImageFormat, ColorFormatTraits> ConstColorLineView;

//
// ARGB-32
//

class Argb32;

/** @brief ARGB-32 image.
*/
typedef BasicImage<Argb32> Argb32Image;

/** @brief ARGB-32 const image.
*/
typedef BasicConstImage<Argb32> Argb32ConstImage;

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

} // namespace

} // namespace

#endif
