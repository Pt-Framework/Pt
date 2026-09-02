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

#ifndef PT_GFX_API_IMAGES_H
#define PT_GFX_API_IMAGES_H

/** @defgroup Pt-Gfx-Images Images and Image Views

    @brief Images, image formats and image views.

    Images own pixel data. A typed image binds its pixel format at compile
    time: %Argb32Image, %Rgb32Image, %Rgb16Image, and %Yuv12Image are
    specializations of %BasicImage with a concrete format type. Their views
    expose the matching concrete pixel type, such as %Argb32Pixel. Use a
    typed image when its format is known by the calling code.

    %Image and %ImageF select their format at runtime through the abstract
    %ImageFormat interface. %Image exposes %Pixel<Color>, while %ImageF
    exposes %Pixel<ColorF>. Use these images when the format is chosen by an
    image decoder, caller, or other runtime input. The generic pixels use the
    selected %ImageFormat for their pixel operations.

    Both image families provide the same non-owning views. Use %view() to
    access a rectangular region, %pixelView() to traverse individual pixels,
    and %lineView() to traverse rows as pixel spans. Each view preserves the
    source image's compile-time or runtime pixel behavior and does not copy
    pixel data.

    Pixel objects refer to a position in an image or view. Their
    copy-assignment operator is deleted to prevent accidentally rebinding a
    position. Call %reset() to bind a pixel to another position. Write a color
    with its color assignment operator or %assign().

    Add a format by deriving it from %ImageFormat, implementing its pixel
    storage operations, and providing the matching concrete pixel types and
    %ImageTraits specialization. The runtime image API then uses the format
    through %ImageFormat, while typed images use it through %BasicImage.
*/

#endif
