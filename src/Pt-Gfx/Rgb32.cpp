/* Copyright (C) 2015 Marc Boris Duerner
   Copyright (C) 2015 Laurentiu-Gheorghe Crisan

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

#include <Pt/Gfx/Rgb32.h>

namespace Pt {

namespace Gfx {

Rgb32::Rgb32()
: ImageFormat(4)
{
}


std::size_t Rgb32::onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                               std::size_t padding) const
{
    std::size_t l = (width * 4) + padding;
    std::size_t n = l * height;
    return n;
}


PixelBase* Rgb32::onCreatePixel(Pt::uint8_t* data, const ViewBase& view, 
                                  Pt::ssize_t x, Pt::ssize_t y, 
                                  PixelStorage& store) const
{ 
    return 0; 
}


Color Rgb32::onGetColor(const Pt::uint8_t* base)
{
    const Pt::uint32_t pixel = *reinterpret_cast<const Pt::uint32_t*>(base);

    const Pt::uint16_t tr = (pixel & 0x00FF0000) >> 16;
    const Pt::uint16_t tg = (pixel & 0x0000FF00) >>  8;
    const Pt::uint16_t tb =  pixel & 0x000000FF;

    Pt::uint16_t a = 0xFFFF;
    Pt::uint16_t r = (tr << 8) + tr;
    Pt::uint16_t g = (tg << 8) + tg;
    Pt::uint16_t b = (tb << 8) + tb;

    return Color(a, r, g, b);
}

} // namespace

} // namespace
