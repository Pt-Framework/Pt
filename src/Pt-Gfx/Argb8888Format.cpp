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

#include <Pt/Gfx/Argb8888Format.h>

namespace Pt {

namespace Gfx {

Argb8888Format::Argb8888Format()
: ImageFormat(4, 4)
{
}


Color Argb8888Format::color(const Pt::uint8_t* pixel) const
{
    return Color( pixel[3] / 255.0f, 
                  pixel[2] / 255.0f, 
                  pixel[1] / 255.0f, 
                  pixel[0] / 255.0f );
}


void Argb8888Format::setColor(Pt::uint8_t* pixel, const Color& c) const
{
	  pixel[0] = (Pt::uint8_t) (c.blue() * 255.0f);	
	  pixel[1] = (Pt::uint8_t) (c.green() * 255.0f);
	  pixel[2] = (Pt::uint8_t) (c.red() * 255.0f);
	  pixel[3] = (Pt::uint8_t) (c.alpha() * 255.0f);
}


void Argb8888Format::copy(const ImageInfo& toInfo, const Point& toPoint,
                          const ImageInfo& fromInfo, const Rect& fromRect,
                          CompositionMode mode) const
{
    Pt::ssize_t toStride = (toInfo.width() * pixelSize()) + toInfo.stride();
    Pt::ssize_t fromStride = (fromRect.width() * pixelSize()) + fromInfo.stride();
    
    Pt::ssize_t toBegin = (toPoint.y() * toStride) + (toPoint.x() * pixelSize());
    Pt::ssize_t fromBegin = (fromRect.y() * fromStride) + (fromRect.x() * pixelSize());

    Pt::uint8_t* toLine = toInfo.data() + toBegin;
    const Pt::uint8_t* fromLine = fromInfo.data() + fromBegin;

    switch(mode)
    {
        default:
        case CompositionMode::SourceCopy:
        {
            Pt::ssize_t n = fromRect.width() * pixelSize();

            for(Pt::ssize_t y = 0; y < fromRect.height(); ++y)
            {
                memcpy(toLine, toLine, n);

                toLine += toStride;
                fromLine += fromStride;
            }

            break;
        }

        case CompositionMode::SourceOver:
        {
            for(int y = fromRect.y(); y < fromRect.height(); ++y)
            {
                Pt::uint8_t* to = toLine;
                const Pt::uint8_t* from = fromLine;
            
                for(int x = fromRect.x(); x < fromRect.width() ; ++x )
                {
                    Pt::uint8_t alpha = from[3];
                    Pt::uint32_t alphaSrc = alpha + 1;
                    Pt::uint32_t alphaInv = 256 - alpha;
                
                    to[0] = (unsigned char)((alphaSrc * from[0] + alphaInv * to[0]) >> 8);
                    to[1] = (unsigned char)((alphaSrc * from[1] + alphaInv * to[1]) >> 8);
                    to[2] = (unsigned char)((alphaSrc * from[2] + alphaInv * to[2]) >> 8);
                    to[3] = (unsigned char)((alphaSrc * from[3] + alphaInv * to[3]) >> 8);

                    to += pixelSize();
                    from += pixelSize();
                }

                toLine += toStride;
                fromLine += fromStride;
            }
            
            break;  
        }
    }
}

} // namespace

} // namespace
