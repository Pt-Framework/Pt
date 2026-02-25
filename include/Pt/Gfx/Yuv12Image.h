/* Copyright (C) 2016 Marc Boris Duerner

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef PT_GFX_YUV12_IMAGE_H
#define PT_GFX_YUV12_IMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Yuv12.h>
#include <Pt/Gfx/BasicPixelView.h>
#include <Pt/Gfx/BasicImage.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {

/** @brief YV-12 image.

    If the Y plane has pad bytes after each row, then the U and V planes have
    half as many pad bytes after their rows. In other words, two U/V rows
    (including padding) is exactly as long as one Y row (including padding).
*/
class Yuv12Image : public BasicImage<Yuv12>
{
    public:
        /** @brief Constructor.
        */
        Yuv12Image()
        : BasicImage( Yuv12() )
        { }
        
        /** @brief Constructor.
        */
        Yuv12Image(const Yuv12Image& image)
        : BasicImage( image )
        { }

        /** @brief Constructor.
        */
        Yuv12Image(Pt::ssize_t width, Pt::ssize_t height, size_t padding = 0)
        : BasicImage(width, height, padding, Yuv12())
        { }

        /** @brief Construct from external buffer.
        */
        Yuv12Image(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, 
                    size_t padding = 0)
        : BasicImage(data, width, height, padding, Yuv12())
        { }
};

/** @brief Yv-12 image.
*/
class Yuv12ConstImage : public BasicConstImage<Yuv12>
{
    public:
        /** @brief Constructor.
        */
        Yuv12ConstImage()
        : BasicConstImage( Yuv12() )
        { }
        
        /** @brief Constructor.
        */
        Yuv12ConstImage(const Yuv12ConstImage& image)
        : BasicConstImage( image )
        { }

        /** @brief Constructor.
        */
        Yuv12ConstImage(const Yuv12Image& image)
        : BasicConstImage( image )
        { }

        /** @brief Construct from external buffer.
        */
        Yuv12ConstImage(const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, 
                    size_t padding = 0)
        : BasicConstImage(data, width, height, padding, Yuv12())
        { }
};

/** @brief YV-12 pixel view.
*/
typedef BasicPixelView<Yuv12> Yuv12PixelView;

/** @brief YV-12 const pixel view.
*/
typedef BasicConstPixelView<Yuv12> Yuv12ConstPixelView;

} // namespace

} // namespace

#endif
