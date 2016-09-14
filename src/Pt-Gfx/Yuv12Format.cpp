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
: ImageFormat(1, 3)
{
}


std::size_t Yuv12Format::imageSize(const Size& size, Pt::ssize_t padding) const
{
    Pt::ssize_t stride = size.width() + padding;
    Pt::ssize_t planeSize = stride * size.height();

    return planeSize + planeSize / 2;
}


void Yuv12Format::setPixel(Pixel& to, const Pixel& from,
                           CompositionMode mode) const
{
    Yuv12Pixel toYuv(to.view(), to.x(), to.y() );
    Yuv12Pixel fromYuv(from.view(), from.x(), from.y() );

    toYuv = fromYuv;
}


void Yuv12Format::setPixel(Pixel& p, const Color& c,
                           CompositionMode mode) const
{
    Yuv12Pixel yuv(p.view(), p.x(), p.y() );
    yuv.assign(c, mode);
}


Color Yuv12Format::getColor(const Pixel& p) const
{
    Yuv12Pixel yuv(p.view(), p.x(), p.y() );
    return yuv.toColor();
}


void Yuv12Format::copy(Pixel& to, const Pixel& from, size_t length, 
                       CompositionMode mode) const
{
}


void Yuv12Format::onCopy(const ImageView& to, const Point& toPoint,
                         const ImageView& from, const Rect& fromRect,
                         CompositionMode mode) const
{
}

} // namespace

} // namespace
