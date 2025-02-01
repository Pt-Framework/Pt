/* Copyright (C) 2015-2016 Marc Boris Duerner
   Copyright (C) 2015-2016 Laurentiu-Gheorghe Crisan
   Copyright (C) 2017-2017 Aloysius Indrayanto

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

#include <Pt/Gfx/Argb32Format.h>
#include <Pt/Gfx/Argb32.h>
#include <cassert>

namespace Pt {

namespace Gfx {

Argb32Format::Argb32Format()
: ImageFormat(4)
{
}


std::size_t Argb32Format::onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                      std::size_t padding) const
{
    return Argb32::imageSize(width, height, padding);
}

//
// Get pixel color
//

Color Argb32Format::onGetColor(const View& view, const Pt::uint8_t* base, 
                               Pt::ssize_t x, Pt::ssize_t y) const
{
    return Argb32::getColor(base);
}

//
// Assign pixel
//

void Argb32Format::onSourceCopy(View& view, PixelBase& to, const Color& c) const
{
    Argb32::sourceCopy(to.base(), c);
}


void Argb32Format::onSourceOver(View& view, PixelBase& to, const Color& c) const
{
    Argb32::sourceOver(to.base(), c);
}


void Argb32Format::onSourceCopy(View& to, PixelBase& pos,
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y) const
{
    Argb32::sourceCopy(pos.base(), base);
}


void Argb32Format::onSourceOver(View& to, PixelBase& pos,
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y) const
{
    Argb32::sourceOver(pos.base(), base);
}

//
// Fill pixels
//

void Argb32Format::onSourceCopy(View& view, PixelBase& to, 
                                std::size_t n, const Color& c) const
{
    Argb32::sourceCopy(to.base(), n, c);
}


void Argb32Format::onSourceOver(View& view, PixelBase& to, 
                                std::size_t n, const Color& c) const
{
    Argb32::sourceOver(to.base(), n, c);
}


void Argb32Format::onSourceCopy(View& view, PixelBase& to, std::size_t n, 
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y) const
{
    Argb32::sourceCopy(to.base(), n, base);
}


void Argb32Format::onSourceOver(View& view, PixelBase& to, std::size_t n, 
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y) const
{
    Argb32::sourceOver(to.base(), n, base);
}

//
// Copy pixels
//

void Argb32Format::onSourceCopy(View& view, PixelBase& to, 
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y, std::size_t n) const
{
    Argb32::sourceCopy(to.base(), base, n);
}


void Argb32Format::onSourceOver(View& view, PixelBase& to, 
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y, std::size_t n) const
{
    Argb32::sourceOver(to.base(), base, n);
}


void Argb32Format::onSourceCopy(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                                const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                                Pt::ssize_t width, Pt::ssize_t height) const
{
    assert( toX >= 0 && toY >= 0 &&
            toX + width <= to.width() &&
            toY + height<= to.height() );

    Argb32::sourceCopy(to, toX, toY, from, fromX, fromY, width, height);
}


void Argb32Format::onSourceOver(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                                const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                                Pt::ssize_t width, Pt::ssize_t height) const
{

    assert( toX >= 0 && toY >= 0 &&
            toX + width <= to.width() &&
            toY + height<= to.height() );

    Argb32::sourceOver(to, toX, toY, from, fromX, fromY, width, height);
}

} // namespace

} // namespace
