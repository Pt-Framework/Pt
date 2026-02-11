/* Copyright (C) 2015 Marc Boris Duerner

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

#ifndef PT_GFX_PIXEL_HPP
#define PT_GFX_PIXEL_HPP

#include <Pt/Gfx/Api.h>

#include <Pt/Gfx/ImageFormat.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// Pixel
///////////////////////////////////////////////////////////////////////

inline Pixel::Pixel(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
, _format( &view.format() )
, _pixel(0)
, _data( view.data() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


inline Pixel::Pixel(const Pixel& p)
: _view(p._view)
, _x(p._x)
, _y(p._y)
, _format(p._format)
, _pixel(0)
, _data(p._data)
{ 
    if(p._pixel)
    {
        _pixel = _format->createPixel(_data, *_view, p.xpos(), p.ypos(), _storage);
    }
}


inline void Pixel::reset(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
{
    if(_pixel)
    {
        _pixel->~PixelBase();
        _pixel = 0;
    }

    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
    _data = view.data();
    _view = &view;
    _x = x;
    _y = y;
    _format = &view.format();
}


inline void Pixel::reset(const Pixel& p)
{
    if(_pixel)
    {
        _pixel->~PixelBase();
        
        _pixel = 0;
        _data = 0;
        _view = 0;
        _x = 0;
        _y = 0;
        _format = 0;
    }

    _pixel = p._format->createPixel(p._data, *p._view, p.xpos(), p.ypos(), _storage);
    _data = p._data;
    _view = p._view;
    _x = p._x;
    _y = p._y;
    _format = p._format;
}


inline Pixel& Pixel::operator=(const Argb32Color& color)
{
    _pixel->assign(color);
    return *this;
}


inline Pixel& Pixel::operator=(const Color& color)
{
    _pixel->assign(color);
    return *this;
}


inline Pixel& Pixel::operator=(const Pixel& p)
{
    _pixel->assign( p.toColor() );
    return *this;
}


inline Pixel& Pixel::operator=(const ConstPixel& p)
{
    _pixel->assign( p.toColor() );
    return *this;
}


inline void Pixel::assign(const ConstPixel& p, std::size_t length)
{
    bool isCompatible = _pixel->assign(*p._pixel, length);
    if( isCompatible )
        return;

    isCompatible = p._pixel->copy(*_pixel, length);
    if( isCompatible )
        return;

    const std::size_t colorsLength = 64;
    Argb32Color colors[colorsLength];

    std::size_t avail = length;

    while(avail > 0)
    {
        std::size_t n = std::min(colorsLength, avail);
        p.getColors(colors, n);
        this->assign(colors, n);
        avail -= n;
    }
}


inline bool Pixel::equals(const ConstPixel& p) const
{ 
    return _pixel->base() == p.base();
}

///////////////////////////////////////////////////////////////////////
// ConstPixel
///////////////////////////////////////////////////////////////////////

inline ConstPixel::ConstPixel(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
, _format( &view.format() )
, _pixel(0)
, _data( view.data() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


inline ConstPixel::ConstPixel(const BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
, _format( &view.format() )
, _pixel(0)
, _data( view.data() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


inline ConstPixel::ConstPixel(const ConstPixel& p)
: _view(p._view)
, _x(p._x)
, _y(p._y)
, _format(p._format)
, _pixel(0)
, _data(p._data)
{ 
    if(p._pixel)
    {
        _pixel = _format->createPixel(_data, *_view, p.xpos(), p.ypos(), _storage);
    }
}


inline ConstPixel::ConstPixel(const Pixel& p)
: _view(p._view)
, _x(p._x)
, _y(p._y)
, _format(p._format)
, _pixel(0)
, _data(p._data)
{ 
    _pixel = _format->createPixel(_data, *_view, p.xpos(), p.ypos(), _storage);
}


inline void ConstPixel::reset(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
{
    if(_pixel)
    {
        _pixel->~ConstPixelBase();
        _pixel = 0;
    }

    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
    _data = view.data();
    _view = &view;
    _x = x;
    _y = y;
    _format = &view.format();
}


inline void ConstPixel::reset(const ConstPixel& p)
{
    if(_pixel)
    {
        _pixel->~ConstPixelBase();
        
        _pixel = 0;
        _data = 0;
        _view = 0;
        _x = 0;
        _y = 0;
        _format = 0;
    }

    if(p._pixel)
    {
        _pixel = p._format->createPixel(p._data, *p._view, p.xpos(), p.ypos(), _storage);
        _data = p._data;
        _view = p._view;
        _x = p._x;
        _y = p._y;
        _format = p._format;
    }
}

} // namespace

} // namespace

#endif
