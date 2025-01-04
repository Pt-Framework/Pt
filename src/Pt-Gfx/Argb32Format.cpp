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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include "Argb32SIMDOps.h"
#include <Pt/Gfx/Argb32Format.h>
#include <Pt/Gfx/Argb32Image.h>
#include <Pt/Gfx/ImageView.h>

namespace Pt {

namespace Gfx {

Argb32Format::Argb32Format()
: ImageFormat(4)
{
}


std::size_t Argb32Format::onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                      std::size_t padding) const
{
    std::size_t l = (width * 4) + padding;
    std::size_t n = l * height;
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
    Argb32Model::assign(pixel.base(), c, mode);
}


void Argb32Format::onSetPixel(Pixel& to, const Pixel& from,
                              CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    Argb32Model::assign(to.base(), from.base(), mode, blendingAlpha);
}


void Argb32Format::onSetPixel(Pixel& to, const ConstPixel& from,
                              CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    Argb32Model::assign(to.base(), from.base(), mode, blendingAlpha);
}


void Argb32Format::onSetPixel(Pixel& pixel, const Color& c,
                              CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    Argb32Model::assign(pixel.base(), c, mode, blendingAlpha);
}


void Argb32Format::onSetPixels(Pixel& to, const Pixel& from, size_t length,
                               CompositionMode mode) const
{
    Argb32Model::assign(to.base(), from.base(), length, mode);
}


void Argb32Format::onSetPixels(Pixel& to, const ConstPixel& from, size_t length,
                               CompositionMode mode) const
{
    Argb32Model::assign(to.base(), from.base(), length, mode);
}


void Argb32Format::onSetPixels(Pixel& pixel, const Color& c, size_t length,
                               CompositionMode mode) const
{
    Argb32Model::assign(pixel.base(), c, length, mode);
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
    switch(mode) {
        default:
        case CompositionMode::SourceCopy:
            Argb32::pixelOps_SourceCopy(to.base(), from.base(), length);
            break;

        case CompositionMode::SourceOver:
            Argb32::pixelOps_SourceOver(to.base(), from.base(), length);
            break;
    }
}


void Argb32Format::onCopy(Pixel& to, const ConstPixel& from, size_t length,
                          CompositionMode mode) const
{
    switch(mode) {
        default:
        case CompositionMode::SourceCopy:
            Argb32::pixelOps_SourceCopy(to.base(), from.base(), length);
            break;

        case CompositionMode::SourceOver:
            Argb32::pixelOps_SourceOver(to.base(), from.base(), length);
            break;
    }
}


void Argb32Format::onCopy(ImageView& to, Pt::ssize_t toX, Pt::ssize_t toY,
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

    switch(mode)
    {
        default:
        case CompositionMode::SourceCopy:
        {
            Pt::ssize_t n = width * pixelSize;

            for(Pt::ssize_t y = 0; y < height; ++y)
            {
                memcpy(toLine, fromLine, n);

                toLine += toStride;
                fromLine += fromStride;
            }

            break;
        }

        case CompositionMode::SourceOver:
        {
            for(int y = 0; y < height; ++y)
            {
                Pt::uint8_t* to = toLine;
                const Pt::uint8_t* from = fromLine;

                for(int x = 0; x < width ; ++x )
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
