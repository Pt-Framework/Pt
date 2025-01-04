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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include <Pt/Gfx/Rgb32Format.h>
#include <Pt/Gfx/ImageView.h>

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


void Rgb32Format::onSetPixel(Pixel& to, const Pixel& from,
                             CompositionMode mode) const
{
    Pt::uint8_t* dst = to.base();
    const Pt::uint8_t* src = from.base();

    *((Pt::uint32_t*)dst) = *((const Pt::uint32_t*)src);
}


void Rgb32Format::onSetPixel(Pixel& to, const ConstPixel& from,
                             CompositionMode mode) const
{
    Pt::uint8_t* dst = to.base();
    const Pt::uint8_t* src = from.base();

    *((Pt::uint32_t*)dst) = *((const Pt::uint32_t*)src);
}


void Rgb32Format::onSetPixel(Pixel& pixel, const Color& c,
                             CompositionMode mode) const
{
    const uint32_t val = ( uint32_t(c.red  () & 0xFF) << 16 ) |
                         ( uint32_t(c.green() & 0xFF) << 8)  |
                         ( uint32_t(c.blue () & 0xFF) );

    Pt::uint16_t* dst = reinterpret_cast<Pt::uint16_t*>( pixel.base() );
    *((Pt::uint32_t*)dst) = *((const Pt::uint32_t*)&val);
}


void Rgb32Format::onSetPixel(Pixel& to, const Pixel& from,
                             CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    // ### !!! TODO !!! ###
}


void Rgb32Format::onSetPixel(Pixel& to, const ConstPixel& from,
                             CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    // ### !!! TODO !!! ###
}


void Rgb32Format::onSetPixel(Pixel& pixel, const Color& c,
                             CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    // ### !!! TODO !!! ###
}


void Rgb32Format::onSetPixels(Pixel& to, const Pixel& from, size_t length,
                              CompositionMode mode) const
{
    // ### !!! TODO !!! ###
}


void Rgb32Format::onSetPixels(Pixel& to, const ConstPixel& from, size_t length,
                              CompositionMode mode) const
{
    // ### !!! TODO !!! ###
}


void Rgb32Format::onSetPixels(Pixel& pixel, const Color& c, size_t length,
                              CompositionMode mode) const
{
    // ### !!! TODO !!! ###
}


Color Rgb32Format::onGetColor(const Pixel& pixel) const
{
    const Pt::uint16_t* p = (const Pt::uint16_t*) pixel.base();

    const uint16_t tr = (*p & 0x00FF0000) >> 16;
    const uint16_t tg = (*p & 0x0000FF00) >> 8;
    const uint16_t tb = *p & 0x000000FF;

    uint16_t a = 0xFFFF;
    uint16_t r = ((tr + !!tr) << 8) - !!tr;
    uint16_t g = ((tg + !!tg) << 8) - !!tg;
    uint16_t b = ((tb + !!tb) << 8) - !!tb;

    return Color(a, r, g, b);
}


Color Rgb32Format::onGetColor(const ConstPixel& pixel) const
{
    const Pt::uint16_t* p = (const Pt::uint16_t*) pixel.base();

    const uint16_t tr = (*p & 0x00FF0000) >> 16;
    const uint16_t tg = (*p & 0x0000FF00) >> 8;
    const uint16_t tb = *p & 0x000000FF;

    uint16_t a = 0xFFFF;
    uint16_t r = ((tr + !!tr) << 8) - !!tr;
    uint16_t g = ((tg + !!tg) << 8) - !!tg;
    uint16_t b = ((tb + !!tb) << 8) - !!tb;

    return Color(a, r, g, b);
}


void Rgb32Format::onCopy(Pixel& to, const Pixel& from, size_t length,
                          CompositionMode mode) const
{
    Pt::uint8_t* dst = to.base();
    const Pt::uint8_t* src = from.base();

    switch(mode)
    {
        default:
        case CompositionMode::SourceCopy:
            memcpy(dst, src, length * 4);
            break;
    }
}


void Rgb32Format::onCopy(Pixel& to, const ConstPixel& from, size_t length,
                          CompositionMode mode) const
{
    Pt::uint8_t* dst = to.base();
    const Pt::uint8_t* src = from.base();

    switch(mode)
    {
        default:
        case CompositionMode::SourceCopy:
            memcpy(dst, src, length * 4);
            break;
    }
}


void Rgb32Format::onCopy(ImageView& to, Pt::ssize_t toX, Pt::ssize_t toY,
                         const ImageView& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                         Pt::ssize_t width, Pt::ssize_t height, 
                         CompositionMode mode) const
{
    Pt::ssize_t pixelSize = 4;

    // TODO: equals to toInfo.pitch()
    Pt::ssize_t toStride = (to.width() * pixelSize) + to.padding();
    Pt::ssize_t fromStride = (from.width() * pixelSize) + from.padding();

    Pt::ssize_t toBegin = (toY * toStride) + (toX * pixelSize);
    Pt::ssize_t fromBegin = (fromY * fromStride) + (fromX * pixelSize);

    Pt::uint8_t* toLine = to.data() + toBegin;
    const Pt::uint8_t* fromLine = from.data() + fromBegin;

    Pt::ssize_t n = width * pixelSize;

    for(Pt::ssize_t y = 0; y < height; ++y)
    {
        memcpy(toLine, toLine, n);

        toLine += toStride;
        fromLine += fromStride;
    }
}

} // namespace

} // namespace
