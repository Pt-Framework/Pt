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

namespace Pt {

namespace Gfx {

// Gfx TODO: use ColorType defined by the non-ImageFormat as conversion color
//           or preferred quality

//template <typename SpanT, typename Fmt, typename Tr>
//void copySpanColors(const SpanT& fromSpan, BasicPixelIterator<Fmt, Tr> to)
//{
//    typedef typename Tr::ColorType ColorType;
//
//    const std::size_t bufsize = 64;
//    ColorType colors[bufsize];
//
//    auto from = fromSpan.cbegin();
//    std::size_t length = fromSpan.length();
//
//    while(length > 0)
//    {
//        std::size_t n = std::min(length, bufsize);
//        
//        from->getColors(colors, n);
//        to->assign(colors, n);
//        
//        from += n;
//        to += n;
//
//        length -= n;
//    }
//}
//
//
//template <typename Tr1, typename Tr2>
//void copySpan(const BasicSpan<ImageFormat, Tr1>& from, 
//              BasicPixelIterator<ImageFormat, Tr2>& to)
//{    
//    to->assign(from.front(), from.length());
//}
//
//
//template <typename Tr1, typename Tr2>
//void copySpan(const BasicConstSpan<ImageFormat, Tr1>& from, 
//              BasicPixelIterator<ImageFormat, Tr2>& to)
//{    
//    to->assign(from.front(), from.length());
//}
//
//
//template <typename Fmt, typename Tr1, typename Tr2>
//void copySpan(const BasicSpan<ImageFormat, Tr1>& from, 
//              BasicPixelIterator<Fmt, Tr2>& to)
//{
//    copySpanColors(from, to);
//}
//
//
//template <typename Fmt, typename Tr1, typename Tr2>
//void copySpan(const BasicConstSpan<ImageFormat, Tr1>& from, 
//              BasicPixelIterator<Fmt, Tr2>& to)
//{
//    copySpanColors(from, to);
//}
//
//
//template <typename Fmt, typename Tr1, typename Tr2>
//void copySpan(const BasicSpan<Fmt, Tr1>& from, 
//              BasicPixelIterator<ImageFormat, Tr2>& to)
//{
//    copySpanColors(from, to);
//}
//
//
//template <typename Fmt, typename Tr1, typename Tr2>
//void copySpan(const BasicConstSpan<Fmt, Tr1>& from, 
//              BasicPixelIterator<ImageFormat, Tr2>& to)
//{
//    copySpanColors(from, to);
//}

///////////////////////////////////////////////////////////////////////
// Pixel
///////////////////////////////////////////////////////////////////////

template <typename ColorT>
inline Pixel<ColorT>::Pixel(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
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
inline void Pixel<ColorT>::reset(BasicView<ImageFormat>& view, Pt::ssize_t x, Pt::ssize_t y)
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
inline Pixel<ColorT>& Pixel<ColorT>::operator=(const Color& color)
{
    _pixel->assign(color);
    return *this;
}


template <typename ColorT>
inline Pixel<ColorT>& Pixel<ColorT>::operator=(const Argb32Color& color)
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
template <typename PixelT>
void Pixel<ColorT>::assignPixels(const PixelT& p, std::size_t length)
{
    bool isCompatible = _pixel->assign(*p._pixel, length);
    if( isCompatible )
        return;

    isCompatible = p._pixel->copy(*_pixel, length);
    if( isCompatible )
        return;

    // TODO: use the precise color type only if both pixels require it
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
inline void Pixel<ColorT>::assign(const Pixel<Color>& p, std::size_t length)
{
    assignPixels(p, length);
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const Pixel<Argb32Color>& p, std::size_t length)
{
    assignPixels(p, length);
}


template <typename ColorT>
inline void Pixel<ColorT>::assign(const ConstPixel<Color>& p, std::size_t length)
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
inline ConstPixel<ColorT>::ConstPixel(const BasicConstView<ImageFormat>& view, 
                                      Pt::ssize_t x, Pt::ssize_t y)
: _pixel(0)
, _format( &view.format() )
{ 
    Pt::uint8_t* data = const_cast<Pt::uint8_t*>( view.data() );
    _pixel = view.format().createPixel(data, view, x, y, _storage);
}


template <typename ColorT>
inline ConstPixel<ColorT>::ConstPixel(const BasicView<ImageFormat>& view, 
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
inline void ConstPixel<ColorT>::reset(const BasicConstView<ImageFormat>& view, 
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
inline void ConstPixel<ColorT>::reset(const BasicView<ImageFormat>& view, 
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
