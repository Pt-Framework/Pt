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

#include <Pt/Gfx/Rgb32Format.h>
#include <Pt/Gfx/ImageView.h>
#include <cstring>
#include <cassert>

namespace Pt {

namespace Gfx {

Rgb32Format::Rgb32Format()
: ImageFormat(4)
{
}


std::size_t Rgb32Format::onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                     std::size_t padding) const
{
    std::size_t l = (width * 4) + padding;
    std::size_t n = l * height;
    return n;
}

//
// Get pixel color
//

Color Rgb32Format::onGetColor(const View& view, const Pt::uint8_t* base, 
                              Pt::ssize_t x, Pt::ssize_t y) const
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

//
// Assign pixel
//

void Rgb32Format::onSourceCopy(View& view, PixelBase& to, const Color& c) const
{
    Pt::uint32_t* pixel = reinterpret_cast<Pt::uint32_t*>(to.base());

    *pixel = ( Pt::uint32_t(c.red() & 0xFF00)   <<  8 ) |
             ( Pt::uint32_t(c.green() & 0xFF00)       ) |
             ( Pt::uint32_t(c.blue() ) >>  8          );

/*
    const uint32_t val = ( uint32_t(c.red  () & 0xFF) << 16 ) |
                         ( uint32_t(c.green() & 0xFF) << 8)  |
                         ( uint32_t(c.blue () & 0xFF) );

    Pt::uint16_t* dst = reinterpret_cast<Pt::uint16_t*>( to.base() );
    *((Pt::uint32_t*)dst) = *((const Pt::uint32_t*)&val);
*/
}


void Rgb32Format::onSourceOver(View& view, PixelBase& to, const Color& c) const
{
    Pt::uint32_t* pixel = reinterpret_cast<Pt::uint32_t*>(to.base());

    *pixel = ( Pt::uint32_t(c.red() & 0xFF00)   <<  8 ) |
             ( Pt::uint32_t(c.green() & 0xFF00)       ) |
             ( Pt::uint32_t(c.blue() ) >>  8          );
}


void Rgb32Format::onSourceCopy(View& to, PixelBase& pos,
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y) const
{
    *((Pt::uint32_t*) pos.base()) = *((const Pt::uint32_t*) base);
}


void Rgb32Format::onSourceOver(View& to, PixelBase& pos,
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y) const
{
    *((Pt::uint32_t*) pos.base()) = *((const Pt::uint32_t*) base);
}

//
// Fill pixels
//

void Rgb32Format::onSourceCopy(View& view, PixelBase& to, 
                                std::size_t n, const Color& c) const
{
    // ### !!! TODO !!! ###
}


void Rgb32Format::onSourceOver(View& view, PixelBase& to, 
                                std::size_t n, const Color& c) const
{
    // ### !!! TODO !!! ###
}


void Rgb32Format::onSourceCopy(View& view, PixelBase& to, std::size_t n, 
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y) const
{
    // ### !!! TODO !!! ###
}


void Rgb32Format::onSourceOver(View& view, PixelBase& to, std::size_t n, 
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y) const
{
    // ### !!! TODO !!! ###
}

//
// Copy pixels
//

void Rgb32Format::onSourceCopy(View& view, PixelBase& to, 
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y, std::size_t n) const
{
    std::memcpy(to.base(), base, n * 4);
}


void Rgb32Format::onSourceOver(View& view, PixelBase& to, 
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y, std::size_t n) const
{
    std::memcpy(to.base(), base, n * 4);
}


void Rgb32Format::onSourceCopy(View& toView, Pt::ssize_t toX, Pt::ssize_t toY,
                               const View& fromView, Pt::ssize_t fromX, Pt::ssize_t fromY,
                               Pt::ssize_t width, Pt::ssize_t height) const
{
    assert( toX >= 0 && toY >= 0 &&
            toX + width <= toView.width() &&
            toY + height<= toView.height() );

    Pt::ssize_t bytesPerPixel = 4;
    Pt::ssize_t n = width * bytesPerPixel;

    Pt::uint8_t* to = toView.data() + (toY * toView.stride()) 
                                    + (toX * bytesPerPixel);
    
    const Pt::uint8_t* from = fromView.data() + (fromY * fromView.stride()) 
                                              + (fromX * bytesPerPixel);

    for(Pt::ssize_t y = 0; y < height; ++y)
    {
        memcpy(to, from, n);

        to += toView.stride();
        from += fromView.stride();
    }
}


void Rgb32Format::onSourceOver(View& toView, Pt::ssize_t toX, Pt::ssize_t toY,
                               const View& fromView, Pt::ssize_t fromX, Pt::ssize_t fromY,
                               Pt::ssize_t width, Pt::ssize_t height) const
{
    assert( toX >= 0 && toY >= 0 &&
            toX + width <= toView.width() &&
            toY + height<= toView.height() );

    Pt::ssize_t bytesPerPixel = 4;
    Pt::ssize_t n = width * bytesPerPixel;

    Pt::uint8_t* to = toView.data() + (toY * toView.stride()) 
                                    + (toX * bytesPerPixel);
    
    const Pt::uint8_t* from = fromView.data() + (fromY * fromView.stride()) 
                                              + (fromX * bytesPerPixel);

    for(Pt::ssize_t y = 0; y < height; ++y)
    {
        memcpy(to, from, n);

        to += toView.stride();
        from += fromView.stride();
    }
}

} // namespace

} // namespace
