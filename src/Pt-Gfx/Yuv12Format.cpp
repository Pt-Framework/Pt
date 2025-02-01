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
    return Yuv12::imageSize(width, height, padding);
}

//
// Get pixel color
//

Color Yuv12Format::onGetColor(const View& view, const Pt::uint8_t* base, 
                              Pt::ssize_t xpos, Pt::ssize_t ypos) const
{
    const Pt::uint8_t* y;
    const Pt::uint8_t* u;
    const Pt::uint8_t* v;

    const Pt::uint8_t* data = view.data();

    Yuv12::init(data, view.stride(), view.width(), view.height(),
                xpos, ypos, y, u, v);

    return Yuv12::getColor(*y, *u, *v);
}

//
// Assign pixel
//

void Yuv12Format::onSourceCopy(View& view, PixelBase& to, const Color& c) const
{
    Pt::uint8_t* y;
    Pt::uint8_t* u;
    Pt::uint8_t* v;

    Pt::uint8_t* data = view.data();

    Yuv12::init(data, view.stride(), view.width(), view.height(),
                to.x(), to.y(), y, u, v);

    Yuv12::fromColor(y, u, v, c);
}

    
void Yuv12Format::onSourceOver(View& view, PixelBase& to, const Color& c) const
{
    onSourceCopy(view, to, c);
}


void Yuv12Format::onSourceCopy(View& view, PixelBase& pos,
                               const View& from, const Pt::uint8_t* base,
                               Pt::ssize_t xpos, Pt::ssize_t ypos) const
{
    Pt::uint8_t* y;
    Pt::uint8_t* u;
    Pt::uint8_t* v;

    Pt::uint8_t* data = view.data();

    Yuv12::init(data, view.stride(), view.width(), view.height(),
                pos.x(), pos.y(), y, u, v);

    const Pt::uint8_t* cy;
    const Pt::uint8_t* cu;
    const Pt::uint8_t* cv;

    Yuv12::init(from.data(), from.stride(), from.width(), from.height(),
                xpos, ypos, cy, cu, cv);

    *y = *cy;
    *u = *cu;
    *v = *cv;
}


 void Yuv12Format::onSourceOver(View& to, PixelBase& pos,
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y) const
{
    onSourceCopy(to, pos, from, base, x ,y);
}

//
// Fill pixels
//

void Yuv12Format::onSourceCopy(View& view, PixelBase& to, 
                                std::size_t n, const Color& c) const
{

}


void Yuv12Format::onSourceOver(View& view, PixelBase& to, 
                                std::size_t n, const Color& c) const
{

}


void Yuv12Format::onSourceCopy(View& view, PixelBase& to, std::size_t n, 
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y) const
{

}


void Yuv12Format::onSourceOver(View& view, PixelBase& to, std::size_t n, 
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y) const
{

}

//
// Copy pixels
//

void Yuv12Format::onSourceCopy(View& view, PixelBase& to, 
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y, std::size_t n) const
{
}


void Yuv12Format::onSourceOver(View& view, PixelBase& to, 
                                const View& from, const Pt::uint8_t* base,
                                Pt::ssize_t x, Pt::ssize_t y, std::size_t n) const
{
}


void Yuv12Format::onSourceCopy(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                                const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                                Pt::ssize_t width, Pt::ssize_t height) const
{
}


void Yuv12Format::onSourceOver(View& to, Pt::ssize_t toX, Pt::ssize_t toY,
                                const View& from, Pt::ssize_t fromX, Pt::ssize_t fromY,
                                Pt::ssize_t width, Pt::ssize_t height) const
{
}

} // namespace

} // namespace
