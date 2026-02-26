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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef PT_GFX_IMAGE_H
#define PT_GFX_IMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ImageFormat.h>
#include <Pt/Gfx/BasicPixelView.h>
#include <Pt/Gfx/BasicCursorView.h>
#include <Pt/Gfx/BasicLineView.h>
#include <Pt/Gfx/BasicImage.h>

namespace Pt {

namespace Gfx {

/** @brief Generic image.
*/
class PT_GFX_API Image : public BasicImage<ImageFormat>
{
    public:
        Image();

        Image(const Image& image);

        explicit Image(const ImageFormat& format);

        Image(const ImageFormat& format, 
              Pt::ssize_t width, Pt::ssize_t height, size_t padding = 0);

        Image(const ImageFormat& format, Pt::uint8_t* buffer,
              Pt::ssize_t width, Pt::ssize_t height, size_t padding = 0);

        virtual ~Image();

        const Image& operator=(const Image& image);
};

///** @brief Generic const image.
//*/
//class PT_GFX_API ConstImage : public BasicConstImage<ImageFormat>
//{
//    public:
//        ConstImage();
//
//        ConstImage(const ConstImage& image);
//
//        explicit ConstImage(const Image& image);
//
//        explicit ConstImage(const ImageFormat& format);
//
//        ConstImage(const ImageFormat& format, const Pt::uint8_t* buffer,
//                   Pt::ssize_t width, Pt::ssize_t height, size_t padding = 0);
//
//        virtual ~ConstImage();
//};

} // namespace

} // namespace

#endif
