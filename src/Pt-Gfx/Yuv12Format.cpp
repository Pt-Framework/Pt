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

#include <Pt/Gfx/Yuv12Format.h>
#include <Pt/Gfx/Yuv12Image.h>
#include <Pt/Gfx/ImageView.h>

namespace Pt {

namespace Gfx {

Yuv12Format::Yuv12Format()
: ImageFormat(1)
{
}


std::size_t Yuv12Format::onImageSize(Pt::ssize_t width, Pt::ssize_t height,
                                     std::size_t padding) const
{
    return Yuv12Model::imageSize(width, height, padding);
}


void Yuv12Format::onSetPixel(Pixel& to, const Pixel& from,
                             CompositionMode mode) const
{
    //Yuv12Model::Pixel toYuv( to.view(), to.base(), to.x(), to.y() );
    //Yuv12Model::ConstPixel fromYuv( from.view(), from.base(), from.x(), from.y() );

    //toYuv.assign(fromYuv, mode);
}


void Yuv12Format::onSetPixel(Pixel& to, const ConstPixel& from,
                             CompositionMode mode) const
{
    Pt::uint8_t* y;
    Pt::uint8_t* u;
    Pt::uint8_t* v;

    Yuv12Model::init(to.view().data(), to.view().stride(), 
                     to.view().width(), to.view().height(),
                     to.x(), to.y(), y, u, v);

    const Pt::uint8_t* cy;
    const Pt::uint8_t* cu;
    const Pt::uint8_t* cv;

    Yuv12Model::init(from.view().data(), from.view().stride(), 
                     from.view().width(), from.view().height(),
                     from.x(), from.y(), cy, cu, cv);

    *y = *cy;
    *u = *cu;
    *v = *cv;

    //Yuv12Model::Pixel toYuv( to.view(), to.base(), to.x(), to.y() );
    //Yuv12Model::ConstPixel fromYuv( from.view(), from.base(), from.x(), from.y() );

    //toYuv.assign(fromYuv, mode);
}


void Yuv12Format::onSetPixel(Pixel& p, const Color& c,
                             CompositionMode mode) const
{
    Pt::uint8_t* y;
    Pt::uint8_t* u;
    Pt::uint8_t* v;

    Yuv12Model::init(p.view().data(), p.view().stride(), 
                     p.view().width(), p.view().height(),
                     p.x(), p.y(), y, u, v);

    Yuv12Model::fromColor(*y, *u, *v, c);
}


void Yuv12Format::onSetPixel(Pixel& to, const Pixel& from,
                             CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    // ### !!! TODO !!! ###
}


void Yuv12Format::onSetPixel(Pixel& to, const ConstPixel& from,
                             CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    // ### !!! TODO !!! ###
}


void Yuv12Format::onSetPixel(Pixel& pixel, const Color& c,
                             CompositionMode mode, Pt::uint8_t blendingAlpha) const
{
    // ### !!! TODO !!! ###
}


void Yuv12Format::onSetPixels(Pixel& to, const Pixel& from, size_t length,
                              CompositionMode mode) const
{
    // ### !!! TODO !!! ###
}


void Yuv12Format::onSetPixels(Pixel& to, const ConstPixel& from, size_t length,
                              CompositionMode mode) const
{
    // ### !!! TODO !!! ###
}


void Yuv12Format::onSetPixels(Pixel& pixel, const Color& c, size_t length,
                              CompositionMode mode) const
{
    // ### !!! TODO !!! ###
}


Color Yuv12Format::onGetColor(const Pixel& p) const
{
    const Pt::uint8_t* y;
    const Pt::uint8_t* u;
    const Pt::uint8_t* v;

   const Pt::uint8_t* data = p.view().data();

    Yuv12Model::init(data, p.view().stride(), 
                     p.view().width(), p.view().height(),
                     p.x(), p.y(), y, u, v);

    return Yuv12Model::toColor(*y, *u, *v);
}


Color Yuv12Format::onGetColor(const ConstPixel& p) const
{
    const Pt::uint8_t* y;
    const Pt::uint8_t* u;
    const Pt::uint8_t* v;

   const Pt::uint8_t* data = p.view().data();

    Yuv12Model::init(data, p.view().stride(), 
                     p.view().width(), p.view().height(),
                     p.x(), p.y(), y, u, v);

    return Yuv12Model::toColor(*y, *u, *v);
}


void Yuv12Format::onCopy(Pixel& to, const Pixel& from, size_t length,
                         CompositionMode mode) const
{
}


void Yuv12Format::onCopy(Pixel& to, const ConstPixel& from, size_t length,
                         CompositionMode mode) const
{
}


void Yuv12Format::onCopy(ImageView& to, Pt::ssize_t toX, Pt::ssize_t toY,
                         const ImageView& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                         Pt::ssize_t width, Pt::ssize_t height, 
                         CompositionMode mode) const
{
}

} // namespace

} // namespace
