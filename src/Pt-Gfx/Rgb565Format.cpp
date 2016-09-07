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

 #include <Pt/Gfx/Rgb565Format.h>
 #include <Pt/Gfx/ImageInfo.h>

 namespace Pt {

 namespace Gfx {

Rgb565Format::Rgb565Format()
: ImageFormat(2, 3)
{
}


void Rgb565Format::setColor(Pt::uint8_t* pixel, const Color& c,
                            CompositionMode mode) const
{
    Pt::uint16_t* val = (Pt::uint16_t*) pixel;
    *val  =  (Pt::uint16_t) (c.red() * 32.0f);
    *val  |=  ((Pt::uint16_t) (c.green() * 64.0f))  << 5;
    *val  |=  ((Pt::uint16_t) (c.blue() * 32.0f))  << 11;
}


Color Rgb565Format::color(const Pt::uint8_t* pixel) const
{
    const Pt::uint16_t* val = (const Pt::uint16_t*) pixel;

    const float r = ((*val & 0xF800) >> 11) / 32.0f;
    const float g = ((*val & 0x07E0) >> 5) / 64.0f;
    const float b = (*val & 0x001F) / 32.0f;

    return Color(1, r, g, b );
}


void Rgb565Format::setPixel(Pt::uint8_t* dst, const Pt::uint8_t* src,
                            CompositionMode mode) const
{
}


void Rgb565Format::setSpan(Pt::uint8_t* dst, const Pt::uint8_t* src, 
                           size_t length, CompositionMode mode) const
{
}


void Rgb565Format::onCopy(const ImageInfo& toInfo, const Point& toPoint,
                          const ImageInfo& fromInfo, const Rect& fromRect,
                          CompositionMode mode) const
{
    // TODO: equals to toInfo.pitch()
    Pt::ssize_t toStride = (toInfo.width() * pixelSize()) + toInfo.padding();
    Pt::ssize_t fromStride = (fromRect.width() * pixelSize()) + fromInfo.padding();
    
    Pt::ssize_t toBegin = (toPoint.y() * toStride) + (toPoint.x() * pixelSize());
    Pt::ssize_t fromBegin = (fromRect.y() * fromStride) + (fromRect.x() * pixelSize());

    Pt::uint8_t* toLine = toInfo.data() + toBegin;
    const Pt::uint8_t* fromLine = fromInfo.data() + fromBegin;

    Pt::ssize_t n = fromRect.width() * pixelSize();

    for(Pt::ssize_t y = 0; y < fromRect.height(); ++y)
    {
        memcpy(toLine, toLine, n);

        toLine += toStride;
        fromLine += fromStride;
    }
}

} // namespace

} // namespace
