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
#include <Pt/Gfx/BasicView.h>
#include <Pt/Gfx/BasicSpan.h>
#include <Pt/Gfx/PixelBase.h>

namespace Pt {

namespace Gfx {

template <typename FormatT1, typename FormatT2, 
          typename TraitsT1, typename TraitsT2>
BasicPixelIterator<FormatT2, TraitsT2> copyColors(const BasicConstSpan<FormatT1, TraitsT1>& fromSpan, 
                                                  BasicPixelIterator<FormatT2, TraitsT2> to)
{
    typedef typename TraitsT2::ColorType ColorType;

    const std::size_t bufsize = 64;
    ColorType colors[bufsize];

    auto from = fromSpan.cbegin();
    std::size_t length = fromSpan.length();

    while(length > 0)
    {
        std::size_t n = std::min(length, bufsize);
        
        from->getColors(colors, n);
        to->assign(colors, n);
        
        from += n;
        to += n;

        length -= n;
    }

    return to;
}


template <typename TraitsT1, typename TraitsT2>
inline BasicPixelIterator<ImageFormat, TraitsT2> copy(const BasicConstSpan<ImageFormat, TraitsT1>& from, 
                                                      BasicPixelIterator<ImageFormat, TraitsT2> to)
{    
    to->assign(from.front(), from.length());
    return to += from.length();
}


template <typename FormatT, typename TraitsT1, typename TraitsT2>
BasicPixelIterator<FormatT, TraitsT2> copy(const BasicConstSpan<ImageFormat, TraitsT1>& from, 
                                           BasicPixelIterator<FormatT, TraitsT2> to)
{
    return copyColors(from, to);
}


template <typename FormatT, typename TraitsT1, typename TraitsT2>
BasicPixelIterator<ImageFormat, TraitsT2> copy(const BasicConstSpan<FormatT, TraitsT1>& from, 
                                                BasicPixelIterator<ImageFormat, TraitsT2> to)
{
    return copyColors(from, to);
}

///////////////////////////////////////////////////////////////////////
// Pixel
///////////////////////////////////////////////////////////////////////

template <typename ColorT>
inline Pixel<ColorT>::Pixel(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
, _format( &view.format() )
, _pixel(0)
, _data( view.data() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


template <typename ColorT>
inline Pixel<ColorT>::Pixel(const Pixel& p)
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


template <typename ColorT>
inline Pixel<ColorT>::~Pixel()
{
    if(_pixel)
        _pixel->~PixelBase();
}


template <typename ColorT>
inline void Pixel<ColorT>::reset(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
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


template <typename ColorT>
inline void Pixel<ColorT>::reset(const Pixel& p)
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


template <typename ColorT>
inline Pixel<ColorT>& Pixel<ColorT>::operator=(const Argb32Color& color)
{
    _pixel->assign(color);
    return *this;
}


template <typename ColorT>
inline Pixel<ColorT>& Pixel<ColorT>::operator=(const Color& color)
{
    _pixel->assign(color);
    return *this;
}


template <typename ColorT>
inline Pixel<ColorT>& Pixel<ColorT>::operator=(const Pixel<Color>& p)
{
   
    _pixel->assign( p.toColor() );
    return *this;
}


template <typename ColorT>
inline Pixel<ColorT>& Pixel<ColorT>::operator=(const Pixel<Argb32Color>& p)
{ 
    _pixel->assign( p.toColor() );
    return *this;
}


template <typename ColorT>
inline Pixel<ColorT>& Pixel<ColorT>::operator=(const ConstPixel<Color>& p)
{
    _pixel->assign( p.toColor() );
    return *this;
}


template <typename ColorT>
inline Pixel<ColorT>& Pixel<ColorT>::operator=(const ConstPixel<Argb32Color>& p)
{
    _pixel->assign( p.toColor() );
    return *this;
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const ConstPixel<Color>& p, std::size_t length)
{
    bool isCompatible = _pixel->assign(*p._pixel, length);
    if( isCompatible )
        return;

    isCompatible = p._pixel->copy(*_pixel, length);
    if( isCompatible )
        return;

    if(p.format().quality() == ImageFormat::Quality::Normal ||
       this->format().quality() == ImageFormat::Quality::Normal)
    {
        const std::size_t bufsize = 64;
        Argb32Color colors[bufsize];

        while(length > 0)
        {
            std::size_t n = std::min(length, bufsize);
            p.getColors(colors, n);
            this->assign(colors, n);
            length -= n;
        }

        return;
    }

    const std::size_t bufsize = 64;
    ColorT colors[bufsize];

    while(length > 0)
    {
        std::size_t n = std::min(length, bufsize);
        p.getColors(colors, n);
        this->assign(colors, n);
        length -= n;
    }
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const ConstPixel<Argb32Color>& p, std::size_t length)
{
    bool isCompatible = _pixel->assign(*p._pixel, length);
    if( isCompatible )
        return;

    isCompatible = p._pixel->copy(*_pixel, length);
    if( isCompatible )
        return;

    if(p.format().quality() == ImageFormat::Quality::Normal ||
       this->format().quality() == ImageFormat::Quality::Normal)
    {
        const std::size_t bufsize = 64;
        Argb32Color colors[bufsize];

        while(length > 0)
        {
            std::size_t n = std::min(length, bufsize);
            p.getColors(colors, n);
            this->assign(colors, n);
            length -= n;
        }

        return;
    }

    const std::size_t bufsize = 64;
    Gfx::Color colors[bufsize];

    while(length > 0)
    {
        std::size_t n = std::min(length, bufsize);
        p.getColors(colors, n);
        this->assign(colors, n);
        length -= n;
    }
}


template <typename ColorT>
inline bool Pixel<ColorT>::equals(const ConstPixel<ColorT>& p) const
{ 
    return _pixel->base() == p.base();
}

///////////////////////////////////////////////////////////////////////
// ConstPixel
///////////////////////////////////////////////////////////////////////

template <typename ColorT>
inline ConstPixel<ColorT>::ConstPixel(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
, _format( &view.format() )
, _pixel(0)
, _data( view.data() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


template <typename ColorT>
inline ConstPixel<ColorT>::ConstPixel(const BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _x(x)
, _y(y)
, _format( &view.format() )
, _pixel(0)
, _data( view.data() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


template <typename ColorT>
inline ConstPixel<ColorT>::ConstPixel(const ConstPixel& p)
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


template <typename ColorT>
inline ConstPixel<ColorT>::ConstPixel(const Pixel<ColorT>& p)
: _view(p._view)
, _x(p._x)
, _y(p._y)
, _format(p._format)
, _pixel(0)
, _data(p._data)
{ 
    _pixel = _format->createPixel(_data, *_view, p.xpos(), p.ypos(), _storage);
}


template <typename ColorT>
inline ConstPixel<ColorT>::~ConstPixel()
{
    if(_pixel)
        _pixel->~ConstPixelBase();
}


template <typename ColorT>
inline void ConstPixel<ColorT>::reset(const BasicConstView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
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


template <typename ColorT>
inline void ConstPixel<ColorT>::reset(const BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
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


template <typename ColorT>
inline void ConstPixel<ColorT>::reset(const ConstPixel& p)
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


template <typename ColorT>
inline void ConstPixel<ColorT>::reset(const Pixel<ColorT>& p)
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
        const Pt::uint8_t* data = p._data;
        _pixel = p._format->createPixel(data, *p._view, p.xpos(), p.ypos(), _storage);
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
