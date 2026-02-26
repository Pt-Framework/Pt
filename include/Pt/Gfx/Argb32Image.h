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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_ARGB32_IMAGE_H
#define PT_GFX_ARGB32_IMAGE_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/Argb32.h>
#include <Pt/Gfx/BasicPixelView.h>
#include <Pt/Gfx/BasicCursorView.h>
#include <Pt/Gfx/BasicLineView.h>
#include <Pt/Gfx/BasicImage.h>

namespace Pt {

namespace Gfx {

///** @brief ARGB-32 image.
//*/
//class Argb32Image : public BasicImage<Argb32>
//{
//    public:
//        /** @brief Constructor.
//        */
//        Argb32Image()
//        : BasicImage( Argb32() )
//        { }
//
//        /** @brief Constructor.
//        */
//        Argb32Image(const Argb32Image& image)
//        : BasicImage( image )
//        { }
//
//        /** @brief Constructor.
//        */
//        Argb32Image(Pt::ssize_t width, Pt::ssize_t height, size_t padding = 0)
//        : BasicImage(width, height, padding, Argb32())
//        { }
//
//        /** @brief Construct from external buffer.
//        */
//        Argb32Image(Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, 
//                    size_t padding = 0)
//        : BasicImage(data, width, height, padding, Argb32())
//        { }
//};
//
///** @brief ARGB-32 image.
//*/
//class Argb32ConstImage : public BasicConstImage<Argb32>
//{
//    public:
//        /** @brief Constructor.
//        */
//        Argb32ConstImage()
//        : BasicConstImage( Argb32() )
//        { }
//
//        /** @brief Constructor.
//        */
//        Argb32ConstImage(const Argb32ConstImage& image)
//        : BasicConstImage( image )
//        { }
//
//
//        /** @brief Constructor.
//        */
//        Argb32ConstImage(const Argb32Image& image)
//        : BasicConstImage( image )
//        { }
//
//        /** @brief Construct from external buffer.
//        */
//        Argb32ConstImage(const Pt::uint8_t* data, Pt::ssize_t width, Pt::ssize_t height, 
//                    size_t padding = 0)
//        : BasicConstImage(Argb32(), data, width, height, padding)
//        { }
//};
//
///** @brief ARGB-32 pixel view.
//*/
//typedef BasicPixelView<Argb32> Argb32PixelView;
//
///** @brief ARGB-32 const pixel view.
//*/
//typedef BasicConstPixelView<Argb32> Argb32ConstPixelView;
//
///** @brief ARGB-32 line view.
//*/
//typedef BasicLineView<Argb32> Argb32LineView;
//
///** @brief ARGB-32 const line view.
//*/
//typedef BasicConstLineView<Argb32> Argb32ConstLineView;

} // namespace

} // namespace

#endif
