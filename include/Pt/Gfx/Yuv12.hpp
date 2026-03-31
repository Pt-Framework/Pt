/* Copyright (C) 2016 Marc Boris Duerner

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

#ifndef PT_GFX_YUV12_HPP
#define PT_GFX_YUV12_HPP

#include <Pt/Gfx/Yuv12.h>
#include <Pt/Gfx/View.h>
#include <Pt/Gfx/Argb32.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// Yuv12Pixel
///////////////////////////////////////////////////////////////////////

template <typename T>
inline Yuv12Pixel::Yuv12Pixel(T& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
: _view(view)
, _xpos(xpos)
, _ypos(ypos)
, _subStride(0)
, _y(0)
, _u(0)
, _v(0)
{
    _subStride = Yuv12::init(view.data(), _view.stride(), 
                             _view.width(), _view.height(),
                             xpos,  ypos, _y, _u, _v);
}


inline Yuv12Pixel::Yuv12Pixel(Pt::uint8_t* data, const ViewBase& view, 
                              Pt::ssize_t xpos, Pt::ssize_t ypos)
: _view(view)
, _xpos(xpos)
, _ypos(ypos)
, _subStride(0)
, _y(0)
, _u(0)
, _v(0)
{
    _subStride = Yuv12::init(data, view.stride(), 
                             view.width(), view.height(),
                             xpos,  ypos, _y, _u, _v);
}


inline bool Yuv12Pixel::equals(const Yuv12Pixel& p) const
{ 
    return _y == p._y; 
}


inline bool Yuv12Pixel::equals(const Yuv12ConstPixel& p) const
{ 
    return _y == p._y; 
}


inline ColorF Yuv12Pixel::getColor() const
{ 
    return Yuv12::getColorF(*_y, *_u, *_v);
}


inline Yuv12Pixel& Yuv12Pixel::operator=(const ColorF& color)
{
    Yuv12::fromColor(_y, _u, _v, color);
    return *this;
}


inline void Yuv12Pixel::advance()
{
    Yuv12::advance(_y, _u, _v, _xpos, _ypos,
                   _view.width(), _view.stride() - _view.width(), _subStride);
}


inline void Yuv12Pixel::skipPadding()
{
}


inline void Yuv12Pixel::advance(Pt::ssize_t n)
{
    Yuv12::advanceBy(_y, _u, _v, n, _xpos, _ypos,
                     _view.width(), _view.stride(), _subStride);
}


inline void Yuv12Pixel::advanceLines(Pt::ssize_t n)
{
    Pt::ssize_t m = n * _view.width();

    Yuv12::advanceBy(_y, _u, _v, m, _xpos, _ypos,
                     _view.width(), _view.stride(), _subStride);
}

///////////////////////////////////////////////////////////////////////
// Yuv12ConstPixel
///////////////////////////////////////////////////////////////////////

template <typename T>
inline Yuv12ConstPixel::Yuv12ConstPixel(const T& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
: _view( view )
, _xpos(xpos)
, _ypos(ypos)
, _subStride(0)
, _y(0)
, _u(0)
, _v(0)
{
    _subStride = Yuv12::init(view.data(), view.stride(), 
                             view.width(), view.height(), 
                             xpos,  ypos, _y, _u, _v);
}


template <typename T>
inline Yuv12ConstPixel::Yuv12ConstPixel(T& view, Pt::ssize_t xpos, Pt::ssize_t ypos)
: _view( view )
, _xpos(xpos)
, _ypos(ypos)
, _subStride(0)
, _y(0)
, _u(0)
, _v(0)
{
    _subStride = Yuv12::init(view.data(), view.stride(), 
                             view.width(), view.height(), 
                             xpos,  ypos, _y, _u, _v);
}


inline Yuv12ConstPixel::Yuv12ConstPixel(const Pt::uint8_t* data, const ViewBase& view,
                                        Pt::ssize_t xpos, Pt::ssize_t ypos)
: _view(view)
, _xpos(xpos)
, _ypos(ypos)
, _subStride(0)
, _y(0)
, _u(0)
, _v(0)
{
    _subStride = Yuv12::init(data, view.stride(), 
                             view.width(), view.height(), 
                             xpos,  ypos, _y, _u, _v);
}


inline ColorF Yuv12ConstPixel::getColor() const
{ 
    return Yuv12::getColorF(*_y, *_u, *_v);
}


inline void Yuv12ConstPixel::advance()
{
    Yuv12::advance(_y, _u, _v, _xpos, _ypos,
                   _view.width(), _view.stride() - _view.width(), _subStride);
}


inline void Yuv12ConstPixel::skipPadding()
{
}


inline void Yuv12ConstPixel::advance(Pt::ssize_t n)
{
    Yuv12::advanceBy(_y, _u, _v, n, _xpos, _ypos,
                     _view.width(), _view.stride(), _subStride);
}


inline void Yuv12ConstPixel::advanceLines(Pt::ssize_t n)
{
    Pt::ssize_t m = n * _view.width();

    Yuv12::advanceBy(_y, _u, _v, m, _xpos, _ypos,
                     _view.width(), _view.stride(), _subStride);
}

} // namespace

} // namespace

#endif