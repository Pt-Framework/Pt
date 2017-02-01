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

#include <Pt/Gfx/Argb32Format.h>
#include <Pt/Gfx/Argb32Image.h>
#include <Pt/Gfx/ImageView.h>

namespace Pt {

namespace Gfx {

Argb32Format::Argb32Format()
: ImageFormat(4)
{
}


std::size_t Argb32Format::onImageSize(const Size& size, Pt::ssize_t padding) const
{
    std::size_t l = (size.width() * 4) + padding;
    std::size_t n = l * size.height();
    return n;
}


void Argb32Format::onSetPixel(Pixel& to, const Pixel& from,
                              CompositionMode mode) const
{
    Argb32Model::assign(to.base(), from.base(), mode);
}


void Argb32Format::onSetPixel(Pixel& to, const ConstPixel& from,
                                CompositionMode mode) const
{
    Argb32Model::assign(to.base(), from.base(), mode);
}


void Argb32Format::onSetPixel(Pixel& pixel, const Color& c,
                              CompositionMode mode) const
{
    Pt::uint8_t* data = pixel.base();
    Argb32Model::assign(data, c, mode);
}


Color Argb32Format::onGetColor(const Pixel& pixel) const
{
    return Argb32Model::toColor( pixel.base() );
}


Color Argb32Format::onGetColor(const ConstPixel& pixel) const
{
    return Argb32Model::toColor( pixel.base() );
}


void Argb32Format::onCopy(Pixel& to, const Pixel& from, size_t length,
                          CompositionMode mode) const
{
    Pt::uint8_t* dst = to.base();;
    const Pt::uint8_t* src = from.base();

    switch(mode)
    {
        default:
        case CompositionMode::SourceCopy:
            memcpy(dst, src, length * 4);
            break;

        case CompositionMode::SourceOver:
            for(size_t i = 0; i < length; ++i)
            {
                Argb32Model::sourceOver(dst, src);
                src += 4;
                dst += 4;
            }
            break;
    }
}


void Argb32Format::onCopy(Pixel& to, const ConstPixel& from, size_t length,
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

        case CompositionMode::SourceOver:
            for(size_t i = 0; i < length; ++i)
            {
                Argb32Model::sourceOver(dst, src);
                src += 4;
                dst += 4;
            }
            break;
    }
}

void Argb32Format::onCopy(Pixel& to, const Pt::uint16_t* alphas, size_t length,
                          const Color& color, CompositionMode mode) const
{
    /*
     *  Division of integers by constants
     *  June 5, 2009 by Nigel Jones
     *  http://embeddedgurus.com/stack-overflow/2009/06/division-of-integers-by-constants
     *
     *  Calculate the reciprocal multiplication factor and convert to binary using
     *  Calculate using http://www.easysurf.cc/cnver17.htm#b10tob2
     *     1 / 257 = 0.00389105058366
     *      => 0.0000000011111111000000001111111100000000111111111010111011011
     * or, directly using a browser, paste and execute in its URL
     *     javascript: (1.0 / 257.0).toString(2)
     *      => 0.00000000111111110000000011111111000000001111111100000001
     *
     *  Left shift until there is a 1 to the right of the binary point
     *      0.0000000011111111000000001111111100000000111111111010111011011
     *        ******** (8 shifts)
     *      => S = 8
     *      => 0.11111111000000001111111100000000111111111010111011011
     *
     *  Take the most significant 17 bits
     *      => 11111111000000001
     *
     *  Add 1 to it
     *      => 11111111000000010
     *
     *  Truncate to 16 bits
     *      => 1111111100000001
     *
     *  Express in 32-bit hexadecimal
     *      => M = 0x0000FF01
     *
     *  Calculate the result using
     *      result = ( ((uint32_t) input *          M) >> 16 ) >> S
     *             = ( ((uint32_t) input * 0x0000FF01) >> 16 ) >> 8
     *               ( ((uint32_t) input * 0x0000FF01) >> 24 )
     */

#define IDIV_BY_255(V) ( ((uint32_t)V * 0x00008081) >> 23 )
#define IDIV_BY_257(V) ( ((uint32_t)V * 0x0000FF01) >> 24 )

    Pt::uint8_t* dst = to.base();

    switch(mode) {
        default:
        case CompositionMode::SourceCopy:
            for(size_t i = 0; i < length; ++i) {
                Pt::uint32_t blendAlphaSrc = std::min<Pt::uint32_t>(255, *alphas++);
                Pt::uint32_t blendAlphaInv = 255 - blendAlphaSrc;
                if(blendAlphaSrc) {
                    dst[0] = (blendAlphaSrc * IDIV_BY_257(color.blue ()) + blendAlphaInv * dst[0] + 255) >> 8;
                    dst[1] = (blendAlphaSrc * IDIV_BY_257(color.green()) + blendAlphaInv * dst[1] + 255) >> 8;
                    dst[2] = (blendAlphaSrc * IDIV_BY_257(color.red  ()) + blendAlphaInv * dst[2] + 255) >> 8;
                    /*
                    dst[0] = (blendAlphaSrc * (Pt::uint32_t)(color.blue () / 257) + blendAlphaInv * dst[0] + 255) >> 8;
                    dst[1] = (blendAlphaSrc * (Pt::uint32_t)(color.green() / 257) + blendAlphaInv * dst[1] + 255) >> 8;
                    dst[2] = (blendAlphaSrc * (Pt::uint32_t)(color.red  () / 257) + blendAlphaInv * dst[2] + 255) >> 8;
                    */
                }
                dst[3] = IDIV_BY_257(color.alpha());
                dst += 4;
            }
            break;

        case CompositionMode::SourceOver:
            for(size_t i = 0; i < length; ++i) {
                Pt::uint32_t colorAlpha    = IDIV_BY_257(color.alpha());
                Pt::uint32_t blendAlpha    = IDIV_BY_255(colorAlpha * std::min<Pt::uint32_t>(255, *alphas++));
                Pt::uint32_t blendAlphaSrc = blendAlpha;
                Pt::uint32_t blendAlphaInv = 255 - blendAlphaSrc;
                if(blendAlphaSrc) {
                    dst[0] = (blendAlphaSrc * IDIV_BY_257(color.blue ()) + blendAlphaInv * dst[0] + 255) >> 8;
                    dst[1] = (blendAlphaSrc * IDIV_BY_257(color.green()) + blendAlphaInv * dst[1] + 255) >> 8;
                    dst[2] = (blendAlphaSrc * IDIV_BY_257(color.red  ()) + blendAlphaInv * dst[2] + 255) >> 8;
                    dst[3] = (blendAlphaSrc * colorAlpha                 + blendAlphaInv * dst[3] + 255) >> 8;
                }
                /*
                Pt::uint32_t colorAlpha    = color.alpha() / 257;
                Pt::uint32_t blendAlpha    = colorAlpha * std::min<Pt::uint32_t>(255, *alphas++) / 255;
                Pt::uint32_t blendAlphaSrc = blendAlpha;
                Pt::uint32_t blendAlphaInv = 255 - blendAlphaSrc;
                if(blendAlphaSrc) {
                    dst[0] = (blendAlphaSrc * (Pt::uint32_t)(color.blue () / 257) + blendAlphaInv * dst[0] + 255) >> 8;
                    dst[1] = (blendAlphaSrc * (Pt::uint32_t)(color.green() / 257) + blendAlphaInv * dst[1] + 255) >> 8;
                    dst[2] = (blendAlphaSrc * (Pt::uint32_t)(color.red  () / 257) + blendAlphaInv * dst[2] + 255) >> 8;
                    dst[3] = (blendAlphaSrc * colorAlpha                          + blendAlphaInv * dst[3] + 255) >> 8;
                }
                */
                dst += 4;
            }
            break;
    }
}

void Argb32Format::onCopy(ImageView& to, const Point& toPoint,
                            const ImageView& from, const Rect& fromRect,
                            CompositionMode mode) const
{
    Pt::ssize_t pixelSize = 4;

    // TODO: equals to toInfo.pitch()
    Pt::ssize_t toStride = (to.width() * pixelSize) + to.padding();
    Pt::ssize_t fromStride = (from.width() * pixelSize) + from.padding();

    Pt::ssize_t toBegin = (toPoint.y() * toStride) + (toPoint.x() * pixelSize);
    Pt::ssize_t fromBegin = (fromRect.y() * fromStride) + (fromRect.x() * pixelSize);

    Pt::uint8_t* toLine = to.data() + toBegin;
    const Pt::uint8_t* fromLine = from.data() + fromBegin;

    switch(mode)
    {
        default:
        case CompositionMode::SourceCopy:
        {
            Pt::ssize_t n = fromRect.width() * pixelSize;

            for(Pt::ssize_t y = 0; y < fromRect.height(); ++y)
            {
                memcpy(toLine, fromLine, n);

                toLine += toStride;
                fromLine += fromStride;
            }

            break;
        }

        case CompositionMode::SourceOver:
        {
            for(int y = 0; y < fromRect.height(); ++y)
            {
                Pt::uint8_t* to = toLine;
                const Pt::uint8_t* from = fromLine;

                for(int x = 0; x < fromRect.width() ; ++x )
                {
                    Argb32Model::sourceOver(to, from);
                    to += 4;
                    from += 4;
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
