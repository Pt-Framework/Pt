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

#include <Pt/Gfx/Rgb16Format.h>
#include <Pt/Gfx/ImageView.h>

 namespace Pt {

 namespace Gfx {

Rgb16Format::Rgb16Format()
: ImageFormat(2)
{
}


std::size_t Rgb16Format::onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                     std::size_t padding) const
{
    std::size_t l = (width * 2) + padding;
    std::size_t n = l * height;
    return n;
}


void Rgb16Format::onSetPixel(Pixel& to, const Pixel& from,
                             CompositionMode mode) const
{
    Pt::uint8_t* dst = to.base();
    const Pt::uint8_t* src = from.base();

    *((Pt::uint16_t*)dst) = *((const Pt::uint16_t*)src);
}


void Rgb16Format::onSetPixel(Pixel& to, const ConstPixel& from,
                             CompositionMode mode) const
{
    Pt::uint8_t* dst = to.base();
    const Pt::uint8_t* src = from.base();

    *((Pt::uint16_t*)dst) = *((const Pt::uint16_t*)src);
}


void Rgb16Format::onSetPixel(Pixel& pixel, const Color& c,
                             CompositionMode mode) const
{
    Pt::uint32_t val =   uint32_t(c.red() & 0xF800) |
                       ( uint32_t(c.green() & 0xFC00) >> 5 ) |
                       ( uint32_t(c.blue () ) >> 11 );

    Pt::uint16_t* dst = reinterpret_cast<Pt::uint16_t*>( pixel.base() );
    *((Pt::uint16_t*)dst) = *((const Pt::uint16_t*)val);
}


void Rgb16Format::onSetPixel(Pixel& to, const Pixel& from,
                             CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    // ### !!! TODO !!! ###
}


void Rgb16Format::onSetPixel(Pixel& to, const ConstPixel& from,
                             CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    // ### !!! TODO !!! ###
}


void Rgb16Format::onSetPixel(Pixel& pixel, const Color& c,
                             CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    // ### !!! TODO !!! ###
}


void Rgb16Format::onSetPixels(Pixel& to, const Pixel& from, size_t length,
                              CompositionMode mode) const
{
    // ### !!! TODO !!! ###
}


void Rgb16Format::onSetPixels(Pixel& to, const ConstPixel& from, size_t length,
                              CompositionMode mode) const
{
    // ### !!! TODO !!! ###
}


void Rgb16Format::onSetPixels(Pixel& pixel, const Color& c, size_t length,
                              CompositionMode mode) const
{
    // ### !!! TODO !!! ###
}


Color Rgb16Format::onGetColor(const Pixel& pixel) const
{
    const Pt::uint16_t* p = (const Pt::uint16_t*) pixel.base();

    const uint16_t tr = (*p & 0xF800) >> 11;
    const uint16_t tg = (*p & 0x07E0) >> 5;
    const uint16_t tb = *p & 0x001F;

    uint16_t a = 0xFFFF;
    uint16_t r = ((tr + !!tr) << 11) - !!tr;
    uint16_t g = ((tg + !!tg) << 10) - !!tg;
    uint16_t b = ((tb + !!tb) << 11) - !!tb;

    return Color(a, r, g, b);
}


Color Rgb16Format::onGetColor(const ConstPixel& pixel) const
{
    const Pt::uint16_t* p = (const Pt::uint16_t*) pixel.base();

    const uint16_t tr = (*p & 0xF800) >> 11;
    const uint16_t tg = (*p & 0x07E0) >> 5;
    const uint16_t tb = *p & 0x001F;

    uint16_t a = 0xFFFF;
    uint16_t r = ((tr + !!tr) << 11) - !!tr;
    uint16_t g = ((tg + !!tg) << 10) - !!tg;
    uint16_t b = ((tb + !!tb) << 11) - !!tb;

    return Color(a, r, g, b);
}


void Rgb16Format::onCopy(Pixel& to, const Pixel& from, size_t length,
                          CompositionMode mode) const
{
    Pt::uint8_t* dst = to.base();
    const Pt::uint8_t* src = from.base();

    switch(mode)
    {
        default:
        case CompositionMode::SourceCopy:
            memcpy(dst, src, length * 2);
            break;
    }
}


void Rgb16Format::onCopy(Pixel& to, const ConstPixel& from, size_t length,
                          CompositionMode mode) const
{
    Pt::uint8_t* dst = to.base();
    const Pt::uint8_t* src = from.base();

    switch(mode)
    {
        default:
        case CompositionMode::SourceCopy:
            memcpy(dst, src, length * 2);
            break;
    }
}


void Rgb16Format::onCopy(ImageView& to, Pt::ssize_t toX, Pt::ssize_t toY,
                         const ImageView& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                         Pt::ssize_t width, Pt::ssize_t height, 
                         CompositionMode mode) const
{
    Pt::ssize_t pixelSize = 2;

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
