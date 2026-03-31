/* Copyright (C) 2016-2016 Marc Boris Duerner
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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef PT_GFX_ARGB32_HPP
#define PT_GFX_ARGB32_HPP

#include <Pt/Gfx/Argb32.h>
#include <Pt/Gfx/View.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// Argb32
///////////////////////////////////////////////////////////////////////

inline ColorF Argb32::getColorF(const Pt::uint8_t* p)
{
    const Pt::uint32_t pixel = *reinterpret_cast<const Pt::uint32_t*>(p);

    const Pt::uint16_t ta =  pixel               >> 24;
    const Pt::uint16_t tr = (pixel & 0x00FF0000) >> 16;
    const Pt::uint16_t tg = (pixel & 0x0000FF00) >>  8;
    const Pt::uint16_t tb =  pixel & 0x000000FF;

    Pt::uint16_t a = (ta << 8) + ta;
    Pt::uint16_t r = (tr << 8) + tr;
    Pt::uint16_t g = (tg << 8) + tg;
    Pt::uint16_t b = (tb << 8) + tb;

    return ColorF(a, r, g, b);
}


inline Color Argb32::getColor(const Pt::uint8_t* p)
{
    return Color(p);
}


inline void Argb32::getColors(const Pt::uint8_t* p, ColorF* colors, std::size_t n)
{
    for(std::size_t i = 0; i < n; ++i)
    {
        colors[i] = getColorF(p);
        p += PixelWidth;
    }
}


inline void Argb32::getColors(const Pt::uint8_t* p, Color* colors, std::size_t n)
{
    void* cp = reinterpret_cast<void*>(colors); 
    std::memcpy(cp, p, n * PixelWidth);
}

inline void Argb32::assign(Pt::uint8_t* to, const Color& from)
{
    std::memcpy(to, &from.value(), PixelWidth);
}


inline void Argb32::assign(Pt::uint8_t* to, const ColorF& c)
{
    Pt::uint32_t* pixel = reinterpret_cast<Pt::uint32_t*>(to);

    *pixel = ( Pt::uint32_t(c.alpha() & 0xFF00) << 16 ) |
             ( Pt::uint32_t(c.red  () & 0xFF00) <<  8 ) |
               Pt::uint32_t(c.green() & 0xFF00)         |
             ( Pt::uint32_t(c.blue ()         ) >>  8 );
}


inline void Argb32::fill(Pt::uint8_t* to, std::size_t length, const Color& c)
{
    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);
    const Pt::uint32_t value = c.value();

    for(std::size_t i = 0; i < length; ++i)
        *dst++ = value;
}


inline void Argb32::fill(Pt::uint8_t* to, std::size_t length, const ColorF& c)
{
    const Pt::uint32_t value = ( Pt::uint32_t(c.alpha() & 0xFF00) << 16 ) |
                               ( Pt::uint32_t(c.red  () & 0xFF00) <<  8 ) |
                               ( Pt::uint32_t(c.green() & 0xFF00)       ) |
                               ( Pt::uint32_t(c.blue ()         ) >>  8 );

    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);
    for(std::size_t i = 0; i < length; ++i) 
        *dst++ = value;
}


inline void Argb32::assign(Pt::uint8_t* to, const Color* colors, std::size_t length)
{
    const Pt::uint8_t* from = reinterpret_cast<const Pt::uint8_t*>(colors);
    copy(to, from, length);
}


inline void Argb32::assign(Pt::uint8_t* to, const ColorF* colors, std::size_t length)
{          
    for(std::size_t n = 0; n < length; ++n)
    {
        assign(to, colors[n]);
        to += Argb32::PixelWidth;
    }
}


inline void Argb32::copy(Pt::uint8_t* to, const Pt::uint8_t* from)
{
    std::memcpy(to, from, PixelWidth);
}


inline void Argb32::copy(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length)
{
    std::memcpy(to, from, length * PixelWidth);
}

///////////////////////////////////////////////////////////////////////
// Argb32Pixel
///////////////////////////////////////////////////////////////////////

template <typename T>
inline Argb32Pixel::Argb32Pixel(T& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Argb32::getPixel(view, view.data(), x, y) )
{
}


template <typename T>
inline void Argb32Pixel::reset(T& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Argb32::getPixel(view, view.data(), x, y);
    
}


inline void Argb32Pixel::reset(const Argb32Pixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Argb32Pixel::advance()
{
    _base = Argb32::advance(*_view, _base);
}


inline void Argb32Pixel::skipPadding()
{
    _base = Argb32::skipPadding(*_view, _base);
}


inline void Argb32Pixel::advance(Pt::ssize_t n)
{
    _base = Argb32::advance(*_view, _base, n);
}


inline void Argb32Pixel::advanceLines(Pt::ssize_t n)
{
    _base = Argb32::advanceLines(*_view, _base, n);
}


inline Argb32Pixel& Argb32Pixel::operator=(const Color& color)
{ 
    Argb32::assign(base(), color);
    return *this;
}


inline void Argb32Pixel::assign(const Argb32Pixel& p)
{
    Argb32::copy( base(), p.base() );
}


inline void Argb32Pixel::assign(const Argb32ConstPixel& p)
{
    Argb32::copy( base(), p.base() );
}


inline void Argb32Pixel::assign(const Argb32Pixel& p, std::size_t length)
{
    Argb32::copy(base(), p.base(), length);
}


inline void Argb32Pixel::assign(const Argb32ConstPixel& p, std::size_t length)
{
    Argb32::copy(base(), p.base(), length);
}


inline void Argb32Pixel::getColors(Color* colors, std::size_t length) const
{
    Argb32::getColors(base(), colors, length);
}


inline void Argb32Pixel::assign(const Color* colors, std::size_t length)
{
    Argb32::assign(base(), colors, length);
}


inline void Argb32Pixel::fill(std::size_t n, const Color& color)
{
    Argb32::fill(base(), n, color);
}


inline bool Argb32Pixel::equals(const Argb32Pixel& p) const
{
    return base() == p.base();
}


inline bool Argb32Pixel::equals(const Argb32ConstPixel& p) const
{
    return base() == p.base();
}


inline Pt::uint8_t Argb32Pixel::alpha() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val >> 24;
}


inline void Argb32Pixel::setAlpha(Pt::uint8_t a)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0x00FFFFFF) | (uint32_t(a) << 24);
}


inline Pt::uint8_t Argb32Pixel::red() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x00FF0000) >> 16;
}


inline void Argb32Pixel::setRed(Pt::uint8_t r)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0xFF00FFFF) | (uint32_t(r) << 16);
}


inline Pt::uint8_t Argb32Pixel::green() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x0000FF00) >> 8;
}


inline void Argb32Pixel::setGreen(Pt::uint8_t g)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0xFFFF00FF) | (uint32_t(g) << 8);
}


inline Pt::uint8_t Argb32Pixel::blue() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val & 0x000000FF;
}


inline void Argb32Pixel::setBlue(Pt::uint8_t b)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0xFFFFFF00) | uint32_t(b);
}


inline Color Argb32Pixel::getColor() const
{
    return Color( base() );
}


///////////////////////////////////////////////////////////////////////
// Argb32ConstPixel
///////////////////////////////////////////////////////////////////////

inline Argb32ConstPixel::Argb32ConstPixel(const Pt::uint8_t* data, const ViewBase& view, 
                                          Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Argb32::getPixel(view, data, x, y) )
{
}


template <typename T>
inline Argb32ConstPixel::Argb32ConstPixel(const T& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Argb32::getPixel(view, view.data(), x, y) )
{
}


template <typename T>
inline Argb32ConstPixel::Argb32ConstPixel(T& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Argb32::getPixel(view, view.data(), x, y) )
{
}


inline Argb32ConstPixel::Argb32ConstPixel(const Argb32Pixel& p)
: _view(p._view)
, _base(p._base)
{ 
}


template <typename T>
inline void Argb32ConstPixel::reset(const T& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Argb32::getPixel(view, view.data(), x, y);
}


template <typename T>
inline void Argb32ConstPixel::reset(T& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Argb32::getPixel(view, view.data(), x, y);
}


inline void Argb32ConstPixel::reset(const Argb32ConstPixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Argb32ConstPixel::reset(const Argb32Pixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Argb32ConstPixel::advance()
{
    _base = Argb32::advance(*_view, _base);
}


inline void Argb32ConstPixel::skipPadding()
{
    _base = Argb32::skipPadding(*_view, _base);
}


inline void Argb32ConstPixel::advance(Pt::ssize_t n)
{
    _base = Argb32::advance(*_view, _base, n);
}


inline void Argb32ConstPixel::advanceLines(Pt::ssize_t n)
{
    _base = Argb32::advanceLines(*_view, _base, n);
}


inline Pt::uint8_t Argb32ConstPixel::alpha() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val >> 24;
}


inline Pt::uint8_t Argb32ConstPixel::red() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x00FF0000) >> 16;
}


inline Pt::uint8_t Argb32ConstPixel::green() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x0000FF00) >> 8;
}


inline Pt::uint8_t Argb32ConstPixel::blue() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val & 0x000000FF;
}


inline Color Argb32ConstPixel::getColor() const
{
    return Color( base() );
}


inline void Argb32ConstPixel::getColors(Color* colors, std::size_t length) const
{
    Argb32::getColors(base(), colors, length);
}


inline bool Argb32ConstPixel::equals(const Argb32ConstPixel& p) const
{
    return base() == p.base();
}


inline bool Argb32ConstPixel::equals(const Argb32Pixel& p) const
{
    return base() == p.base();
}

///////////////////////////////////////////////////////////////////////
// Free compositing functions
///////////////////////////////////////////////////////////////////////

inline void sourceCopy(Argb32Pixel& to, std::size_t length, const Color& from)
{
    Argb32::fill( to.base(), length, from );
}


inline void sourceOver(Argb32Pixel& to, std::size_t length, const Color& from)
{
    const Pt::uint8_t* src = reinterpret_cast<const Pt::uint8_t*>(&from.value());
    const Pt::uint32_t alphaSrc = src[3];
    const Pt::uint32_t alphaInv = 255 - alphaSrc;

    Pt::uint8_t* dst = to.base();

    for(std::size_t i = 0; i < length; ++i) {
        dst[0] = (Pt::uint8_t)((alphaSrc * src[0]   + alphaInv * dst[0]) >> 8);
        dst[1] = (Pt::uint8_t)((alphaSrc * src[1]   + alphaInv * dst[1]) >> 8);
        dst[2] = (Pt::uint8_t)((alphaSrc * src[2]   + alphaInv * dst[2]) >> 8);
        dst[3] = (Pt::uint8_t)((alphaSrc * alphaSrc + alphaInv * dst[3]) >> 8);
        dst += 4;
    }
}


inline void sourceCopy(Argb32Pixel& to, const Argb32ConstPixel& from, std::size_t length)
{
    Argb32::copy( to.base(), from.base(), length );
}


inline void sourceOver(Argb32Pixel& to, const Argb32ConstPixel& from, std::size_t length)
{
    const Pt::uint8_t* src = from.base();
          Pt::uint8_t* dst = to.base();

    for(std::size_t i = 0; i < length; ++i)
    {
        const Pt::uint32_t alphaSrc = src[3];
        const Pt::uint32_t alphaInv = 255 - alphaSrc;
        dst[0] = (Pt::uint8_t)((alphaSrc * src[0]   + alphaInv * dst[0]) >> 8);
        dst[1] = (Pt::uint8_t)((alphaSrc * src[1]   + alphaInv * dst[1]) >> 8);
        dst[2] = (Pt::uint8_t)((alphaSrc * src[2]   + alphaInv * dst[2]) >> 8);
        dst[3] = (Pt::uint8_t)((alphaSrc * alphaSrc + alphaInv * dst[3]) >> 8);
        src += 4;
        dst += 4;
    }
}

} // namespace

} // namespace

#endif
