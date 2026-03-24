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
#include <Pt/Gfx/ImageView.h>
#include <Pt/Gfx/ImageFormat.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// Pixel
///////////////////////////////////////////////////////////////////////

template <typename ColorT>
template <typename Tr>
inline Pixel<ColorT>::Pixel(BasicImageView<ImageFormat, Tr>& view, Pt::ssize_t x, Pt::ssize_t y)
: _pixel(0)
, _format( &view.format() )
{ 
    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
}


template <typename ColorT>
inline Pixel<ColorT>::Pixel(const Pixel& p)
: _pixel(0)
, _format(p._format)
{ 
    _pixel = p._pixel->clone(_storage);
}


template <typename ColorT>
inline Pixel<ColorT>::~Pixel()
{
    if(_pixel)
        _pixel->~PixelBase();
}


template <typename ColorT>
template <typename Tr>
inline void Pixel<ColorT>::reset(BasicImageView<ImageFormat, Tr>& view, Pt::ssize_t x, Pt::ssize_t y)
{
    if(_pixel)
    {
        _pixel->~PixelBase();
        _pixel = 0;
    }

    _pixel = view.format().createPixel(view.data(), view, x, y, _storage);
    _format = &view.format();
}


template <typename ColorT>
inline void Pixel<ColorT>::reset(const Pixel& p)
{
    if(_pixel)
    {
        _pixel->~PixelBase();
        _pixel = 0;
    }

    _pixel = p._pixel->clone(_storage);
    _format = p._format;
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const ColorF& color)
{
    _pixel->assign(color);
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const Argb32Color& color)
{
    _pixel->assign(color);
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const Pixel<ColorF>& p)
{
   
    _pixel->assign( p.toColor() );
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const Pixel<Argb32Color>& p)
{ 
    _pixel->assign( p.toColor() );
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const ConstPixel<ColorF>& p)
{
    _pixel->assign( p.toColor() );
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const ConstPixel<Argb32Color>& p)
{
    _pixel->assign( p.toColor() );
}


template <typename ColorT>
template <typename PixelT>
void Pixel<ColorT>::assignPixels(const PixelT& p, std::size_t length)
{
    bool isCompatible = _pixel->assign(*p._pixel, length);
    if( isCompatible )
        return;

    isCompatible = p._pixel->copy(*_pixel, length);
    if( isCompatible )
        return;

    //
    // TODO GFX: use the precise color type only if both pixels require it
    //
    const std::size_t bufsize = 64;
    ColorT colors[bufsize];

    PixelT from(p);
    Pixel<ColorT> to(*this);

    while(length > 0)
    {
        std::size_t n = std::min(length, bufsize);
        
        from.getColors(colors, n);
        to.assign(colors, n);
        
        from.advance(n);
        to.advance(n);

        length -= n;
    }
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const Pixel<ColorF>& p, std::size_t length)
{
    assignPixels(p, length);
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const Pixel<Argb32Color>& p, std::size_t length)
{
    assignPixels(p, length);
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const ConstPixel<ColorF>& p, std::size_t length)
{
    assignPixels(p, length);
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const ConstPixel<Argb32Color>& p, std::size_t length)
{
    assignPixels(p, length);
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
template <typename Tr>
inline ConstPixel<ColorT>::ConstPixel(const BasicConstImageView<ImageFormat, Tr>& view, 
                                      Pt::ssize_t x, Pt::ssize_t y)
: _pixel(0)
, _format( &view.format() )
{ 
    Pt::uint8_t* data = const_cast<Pt::uint8_t*>( view.data() );
    _pixel = view.format().createPixel(data, view, x, y, _storage);
}


template <typename ColorT>
template <typename Tr>
inline ConstPixel<ColorT>::ConstPixel(const BasicImageView<ImageFormat, Tr>& view, 
                                      Pt::ssize_t x, Pt::ssize_t y)
: _pixel(0)
, _format( &view.format() )
{ 
    Pt::uint8_t* data = const_cast<Pt::uint8_t*>( view.data() );
    _pixel = view.format().createPixel(data, view, x, y, _storage);
}


template <typename ColorT>
inline ConstPixel<ColorT>::ConstPixel(const ConstPixel& p)
: _pixel(0)
, _format(p._format)
{ 
    _pixel = p._pixel->clone(_storage);
}


template <typename ColorT>
inline ConstPixel<ColorT>::ConstPixel(const Pixel<ColorT>& p)
: _pixel(0)
, _format(p._format)
{ 
    _pixel = p._pixel->clone(_storage);
}


template <typename ColorT>
inline ConstPixel<ColorT>::~ConstPixel()
{
    if(_pixel)
        _pixel->~PixelBase();
}


template <typename ColorT>
template <typename Tr>
inline void ConstPixel<ColorT>::reset(const BasicConstImageView<ImageFormat, Tr>& view, 
                                      Pt::ssize_t x, Pt::ssize_t y)
{
    if(_pixel)
    {
        _pixel->~PixelBase();
        _pixel = 0;
    }

    Pt::uint8_t* data = const_cast<Pt::uint8_t*>(view.data());
    _pixel = view.format().createPixel(data, view, x, y, _storage);
    _format = &view.format();
}


template <typename ColorT>
template <typename Tr>
inline void ConstPixel<ColorT>::reset(const BasicImageView<ImageFormat, Tr>& view, 
                                      Pt::ssize_t x, Pt::ssize_t y)
{
    if(_pixel)
    {
        _pixel->~PixelBase();
        _pixel = 0;
    }

    Pt::uint8_t* data = const_cast<Pt::uint8_t*>(view.data());
    _pixel = view.format().createPixel(data, view, x, y, _storage);
    _format = &view.format();
}


template <typename ColorT>
inline void ConstPixel<ColorT>::reset(const ConstPixel& p)
{
    if(_pixel)
    {
        _pixel->~PixelBase();
        _pixel = 0;
    }

    _pixel = p._pixel->clone(_storage);
    _format = p._format;
}


template <typename ColorT>
inline void ConstPixel<ColorT>::reset(const Pixel<ColorT>& p)
{
    if(_pixel)
    {
        _pixel->~PixelBase();
        _pixel = 0;
    }

    _pixel = p._pixel->clone(_storage);
    _format = p._format;
}

} // namespace

} // namespace

#endif
