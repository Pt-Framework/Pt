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

Rgb565Format::Rgb565Format()
: ImageFormat(2, 3)
{
}


std::size_t Rgb565Format::onImageSize(const Size& size, Pt::ssize_t padding) const
{
    std::size_t l = (size.width() * 2) + padding;
    std::size_t n = l * size.height();
    return n;
}


void Rgb565Format::onSetPixel(Pixel& to, const Pixel& from, 
                              CompositionMode mode) const
{
    Pt::uint8_t* dst = to.base();
    const Pt::uint8_t* src = from.base();

    *((Pt::uint16_t*)dst) = *((const Pt::uint16_t*)src);
}


void Rgb565Format::onSetPixel(Pixel& to, const ConstPixel& from, 
                              CompositionMode mode) const
{
    Pt::uint8_t* dst = to.base();
    const Pt::uint8_t* src = from.base();

    *((Pt::uint16_t*)dst) = *((const Pt::uint16_t*)src);
}


void Rgb565Format::onSetPixel(Pixel& pixel, const Color& c,
                              CompositionMode mode) const
{
    Pt::uint32_t val =   uint32_t(c.red() & 0xF800) |
                       ( uint32_t(c.green() & 0xFC00) >> 5 ) |
                       ( uint32_t(c.blue () ) >> 11 );

    Pt::uint16_t* dst = reinterpret_cast<Pt::uint16_t*>( pixel.base() );
    *((Pt::uint16_t*)dst) = *((const Pt::uint16_t*)val);
}


Color Rgb565Format::onGetColor(const Pixel& pixel) const
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


Color Rgb565Format::onGetColor(const ConstPixel& pixel) const
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


void Rgb565Format::onCopy(Pixel& to, const Pixel& from, size_t length, 
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


void Rgb565Format::onCopy(Pixel& to, const ConstPixel& from, size_t length, 
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


void Rgb565Format::onCopy(ImageView& to, const Point& toPoint,
                          const ImageView& from, const Rect& fromRect,
                          CompositionMode mode) const
{
    Pt::ssize_t pixelSize = 2;

    // TODO: equals to toInfo.pitch()
    Pt::ssize_t toStride = (to.width() * pixelSize) + to.padding();
    Pt::ssize_t fromStride = (fromRect.width() * pixelSize) + from.padding();
    
    Pt::ssize_t toBegin = (toPoint.y() * toStride) + (toPoint.x() * pixelSize);
    Pt::ssize_t fromBegin = (fromRect.y() * fromStride) + (fromRect.x() * pixelSize);

    Pt::uint8_t* toLine = to.data() + toBegin;
    const Pt::uint8_t* fromLine = from.data() + fromBegin;

    Pt::ssize_t n = fromRect.width() * pixelSize;

    for(Pt::ssize_t y = 0; y < fromRect.height(); ++y)
    {
        memcpy(toLine, toLine, n);

        toLine += toStride;
        fromLine += fromStride;
    }
}

} // namespace

} // namespace
