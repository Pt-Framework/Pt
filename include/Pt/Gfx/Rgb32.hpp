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

#ifndef PT_GFX_RGB32_HPP
#define PT_GFX_RGB32_HPP

#include <Pt/Gfx/Rgb32.h>
#include <Pt/Gfx/View.h>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// Rgb32
///////////////////////////////////////////////////////////////////////

inline Rgb32Color Rgb32::getRgb32Color(const Pt::uint8_t* p)
{
    return Rgb32Color(p);
}


inline ColorF Rgb32::getColorF(const Pt::uint8_t* p)
{
    const Pt::uint32_t pixel = *reinterpret_cast<const Pt::uint32_t*>(p);
    const Pt::uint32_t a = pixel >> 24;

    if(a == 0)
        return ColorF(0, 0, 0, 0);

    const float invA = 1.f / static_cast<float>(a);
    return ColorF( ColorF::toChannelF(static_cast<Pt::uint8_t>(a)),
                   ((pixel & 0x00FF0000) >> 16) * invA,
                   ((pixel & 0x0000FF00) >>  8) * invA,
                    (pixel & 0x000000FF)        * invA );
}


inline Color Rgb32::getColor(const Pt::uint8_t* p)
{
    const Pt::uint32_t pixel = *reinterpret_cast<const Pt::uint32_t*>(p);
    const Pt::uint32_t a = pixel >> 24;

    if (a == 0)
        return Color(0, 0, 0, 0);

    const Pt::uint32_t r = ((pixel & 0x00FF0000) >> 16);
    const Pt::uint32_t g = ((pixel & 0x0000FF00) >>  8);
    const Pt::uint32_t b =  (pixel & 0x000000FF);

    return Color( Pt::uint8_t(a),
                        Pt::uint8_t((r * 255) / a),
                        Pt::uint8_t((g * 255) / a),
                        Pt::uint8_t((b * 255) / a) );
}


inline void Rgb32::getColors(const Pt::uint8_t* p, Rgb32Color* colors, std::size_t n)
{
    std::memcpy(colors, p, n * PixelWidth);
}


inline void Rgb32::getColors(const Pt::uint8_t* p, Color* colors, std::size_t n)
{
    for(std::size_t i = 0; i < n; ++i)
    {
        colors[i] = getColor(p);
        p += PixelWidth;
    }
}


inline void Rgb32::getColors(const Pt::uint8_t* p, ColorF* colors, std::size_t n)
{
    for(std::size_t i = 0; i < n; ++i)
    {
        colors[i] = getColorF(p);
        p += PixelWidth;
    }
}


inline void Rgb32::assign(Pt::uint8_t* to, const Color& from)
{
    const Pt::uint32_t value = premultiply(from.alpha(), from.red(), from.green(), from.blue());
    const Pt::uint8_t* p = reinterpret_cast<const Pt::uint8_t*>(&value);
    copy(to, p);
}


inline void Rgb32::assign(Pt::uint8_t* to, const ColorF& c)
{
    const Pt::uint32_t value = premultiply(c);
    const Pt::uint8_t* p = reinterpret_cast<const Pt::uint8_t*>(&value);
    copy(to, p);
}


inline void Rgb32::fill(Pt::uint8_t* to, std::size_t length, const Rgb32Color& c)
{
    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);
    const Pt::uint32_t value = c.value();

    for(std::size_t i = 0; i < length; ++i)
    {
        *dst++ = value;
    }
}


inline void Rgb32::fill(Pt::uint8_t* to, std::size_t length, const Color& c)
{
    const Pt::uint32_t value = premultiply(c.alpha(), c.red(), c.green(), c.blue());
    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);

    for(std::size_t i = 0; i < length; ++i)
    {
        *dst++ = value;
    }
}


inline void Rgb32::fill(Pt::uint8_t* to, std::size_t length, const ColorF& c)
{
    const Pt::uint32_t value = premultiply(c);
    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(to);

    for(std::size_t i = 0; i < length; ++i)
    {
        *dst++ = value;
    }
}


inline void Rgb32::assign(Pt::uint8_t* to, const Rgb32Color* colors, std::size_t length)
{
    static_assert( sizeof(Rgb32Color) == PixelWidth,
                   "invalid Rgb32Color size" );

    const Pt::uint8_t* from = reinterpret_cast<const Pt::uint8_t*>(colors);
    copy(to, from, length);
}


inline void Rgb32::assign(Pt::uint8_t* to, const Color* colors, std::size_t length)
{
    for(std::size_t i = 0; i < length; ++i)
    {
        assign(to, colors[i]);
        to += Rgb32::PixelWidth;
    }
}


inline void Rgb32::assign(Pt::uint8_t* to, const ColorF* colors, std::size_t length)
{
    for(std::size_t n = 0; n < length; ++n)
    {
        assign(to, colors[n]);
        to += Rgb32::PixelWidth;
    }
}


inline void Rgb32::copy(Pt::uint8_t* to, const Pt::uint8_t* from)
{
    std::memcpy(to, from, PixelWidth);
}


inline void Rgb32::copy(Pt::uint8_t* to, const Pt::uint8_t* from, std::size_t length)
{
    std::memcpy(to, from, length * PixelWidth);
}

//
// Blend (SourceOver)
//

inline void sourceOver(Rgb32Pixel& to, std::size_t length, const Rgb32Color& from)
{
    const Pt::uint8_t* src = reinterpret_cast<const Pt::uint8_t*>(&from.value());
    const Pt::uint32_t alphaInv = 255 - src[3];

    Pt::uint8_t* dst = to.base();

    for(std::size_t i = 0; i < length; ++i) {
        dst[0] = (Pt::uint8_t)(src[0] + ((alphaInv * dst[0]) >> 8));
        dst[1] = (Pt::uint8_t)(src[1] + ((alphaInv * dst[1]) >> 8));
        dst[2] = (Pt::uint8_t)(src[2] + ((alphaInv * dst[2]) >> 8));
        dst[3] = (Pt::uint8_t)(src[3] + ((alphaInv * dst[3]) >> 8));
        dst += 4;
    }
}


inline void sourceCopy(Rgb32Pixel& to, const Rgb32ConstPixel& from, std::size_t length)
{
    Rgb32::copy( to.base(), from.base(), length );
}


inline void sourceOver(Rgb32Pixel& to, const Rgb32ConstPixel& from, std::size_t length)
{
    const Pt::uint8_t* src = from.base();
          Pt::uint8_t* dst = to.base();

    for(std::size_t i = 0; i < length; ++i)
    {
        const Pt::uint32_t alphaInv = 255 - src[3];
        dst[0] = (Pt::uint8_t) ( src[0] + ((alphaInv * dst[0]) >> 8) );
        dst[1] = (Pt::uint8_t) ( src[1] + ((alphaInv * dst[1]) >> 8) );
        dst[2] = (Pt::uint8_t) ( src[2] + ((alphaInv * dst[2]) >> 8) );
        dst[3] = (Pt::uint8_t) ( src[3] + ((alphaInv * dst[3]) >> 8) );
        src += 4;
        dst += 4;
    }
}

template <typename T>
inline Rgb32Pixel::Rgb32Pixel(T& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Rgb32::getPixel(view, view.data(), x, y) )
{
}


template <typename T>
inline void Rgb32Pixel::reset(T& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Rgb32::getPixel(view, view.data(), x, y);
}


inline void Rgb32Pixel::reset(const Rgb32Pixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Rgb32Pixel::advance()
{
    _base = Rgb32::advance(*_view, _base);
}


inline void Rgb32Pixel::skipPadding()
{
    _base = Rgb32::skipPadding(*_view, _base);
}


inline void Rgb32Pixel::advance(Pt::ssize_t n)
{
    _base = Rgb32::advance(*_view, _base, n);
}


inline void Rgb32Pixel::advanceLines(Pt::ssize_t n)
{
    _base = Rgb32::advanceLines(*_view, _base, n);
}


inline Rgb32Pixel& Rgb32Pixel::operator=(const Color& color)
{
    Rgb32::assign(base(), color);
    return *this;
}


inline Rgb32Pixel& Rgb32Pixel::operator=(const Rgb32Color& color)
{
    Rgb32::copy( base(), reinterpret_cast<const Pt::uint8_t*>(&color.value()) );
    return *this;
}


inline void Rgb32Pixel::assign(const Rgb32Pixel& p)
{
    Rgb32::copy( base(), p.base() );
}


inline void Rgb32Pixel::assign(const Rgb32ConstPixel& p)
{
    Rgb32::copy( base(), p.base() );
}


inline void Rgb32Pixel::assign(const Rgb32Pixel& p, std::size_t length)
{
    Rgb32::copy(base(), p.base(), length);
}


inline void Rgb32Pixel::assign(const Rgb32ConstPixel& p, std::size_t length)
{
    Rgb32::copy(base(), p.base(), length);
}


inline void Rgb32Pixel::assign(const Color* colors, std::size_t length)
{
    Rgb32::assign(base(), colors, length);
}


inline void Rgb32Pixel::assign(const Rgb32Color* colors, std::size_t length)
{
    Rgb32::assign(base(), colors, length);
}


inline void Rgb32Pixel::getColors(Color* colors, std::size_t length) const
{
  Rgb32::getColors(base(), colors, length);
}


inline void Rgb32Pixel::getColors(Rgb32Color* colors, std::size_t length) const
{
    Rgb32::getColors(base(), colors, length);
}


inline void Rgb32Pixel::fill(std::size_t n, const Color& color)
{
    Rgb32::fill(base(), n, color);
}


inline void Rgb32Pixel::fill(std::size_t n, const Rgb32Color& color)
{
    Rgb32::fill(base(), n, color);
}


inline bool Rgb32Pixel::equals(const Rgb32Pixel& p) const
{
    return base() == p.base();
}


inline bool Rgb32Pixel::equals(const Rgb32ConstPixel& p) const
{
    return base() == p.base();
}


inline Pt::uint8_t Rgb32Pixel::alpha() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val >> 24;
}


inline void Rgb32Pixel::setAlpha(Pt::uint8_t a)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0x00FFFFFF) | (uint32_t(a) << 24);
}


inline Pt::uint8_t Rgb32Pixel::red() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x00FF0000) >> 16;
}


inline void Rgb32Pixel::setRed(Pt::uint8_t r)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0xFF00FFFF) | (uint32_t(r) << 16);
}


inline Pt::uint8_t Rgb32Pixel::green() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x0000FF00) >> 8;
}


inline void Rgb32Pixel::setGreen(Pt::uint8_t g)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0xFFFF00FF) | (uint32_t(g) << 8);
}


inline Pt::uint8_t Rgb32Pixel::blue() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val & 0x000000FF;
}


inline void Rgb32Pixel::setBlue(Pt::uint8_t b)
{
    Pt::uint32_t* val = reinterpret_cast<Pt::uint32_t*>(base());
    *val = (*val & 0xFFFFFF00) | uint32_t(b);
}


inline Rgb32Color Rgb32Pixel::color() const
{
    return Rgb32::getRgb32Color( base() );
}


inline Color Rgb32Pixel::getColor() const
{
    return Rgb32::getColor( base() );
}

///////////////////////////////////////////////////////////////////////
// Rgb32ConstPixel
///////////////////////////////////////////////////////////////////////

inline Rgb32ConstPixel::Rgb32ConstPixel(const Pt::uint8_t* data, const ViewBase& view,
                                        Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Rgb32::getPixel(view, data, x, y) )
{
}


template <typename T>
inline Rgb32ConstPixel::Rgb32ConstPixel(const T& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Rgb32::getPixel(view, view.data(), x, y) )
{
}


template <typename T>
inline Rgb32ConstPixel::Rgb32ConstPixel(T& view, Pt::ssize_t x, Pt::ssize_t y)
: _view(&view)
, _base( Rgb32::getPixel(view, view.data(), x, y) )
{
}


inline Rgb32ConstPixel::Rgb32ConstPixel(const Rgb32Pixel& p)
: _view(p._view)
, _base(p._base)
{
}


template <typename T>
inline void Rgb32ConstPixel::reset(const T& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Rgb32::getPixel(view, view.data(), x, y);
}


template <typename T>
inline void Rgb32ConstPixel::reset(T& view, Pt::ssize_t x, Pt::ssize_t y)
{
    _view = &view;
    _base = Rgb32::getPixel(view, view.data(), x, y);
}


inline void Rgb32ConstPixel::reset(const Rgb32ConstPixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Rgb32ConstPixel::reset(const Rgb32Pixel& p)
{
    _view = p._view;
    _base = p._base;
}


inline void Rgb32ConstPixel::advance()
{
    _base = Rgb32::advance(*_view, _base);
}


inline void Rgb32ConstPixel::skipPadding()
{
    _base = Rgb32::skipPadding(*_view, _base);
}


inline void Rgb32ConstPixel::advance(Pt::ssize_t n)
{
    _base = Rgb32::advance(*_view, _base, n);
}


inline void Rgb32ConstPixel::advanceLines(Pt::ssize_t n)
{
    _base = Rgb32::advanceLines(*_view, _base, n);
}


inline Pt::uint8_t Rgb32ConstPixel::alpha() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val >> 24;
}


inline Pt::uint8_t Rgb32ConstPixel::red() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x00FF0000) >> 16;
}


inline Pt::uint8_t Rgb32ConstPixel::green() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return (*val & 0x0000FF00) >> 8;
}


inline Pt::uint8_t Rgb32ConstPixel::blue() const
{
    const Pt::uint32_t* val = reinterpret_cast<const Pt::uint32_t*>(base());
    return *val & 0x000000FF;
}


inline Rgb32Color Rgb32ConstPixel::color() const
{
    return Rgb32::getRgb32Color( base() );
}


inline Color Rgb32ConstPixel::getColor() const
{
    return Rgb32::getColor( base() );
}


inline void Rgb32ConstPixel::getColors(Color* colors, std::size_t length) const
{
    Rgb32::getColors(base(), colors, length);
}


inline void Rgb32ConstPixel::getColors(Rgb32Color* colors, std::size_t length) const
{
  Rgb32::getColors(base(), colors, length);
}


inline bool Rgb32ConstPixel::equals(const Rgb32ConstPixel& p) const
{
    return base() == p.base();
}


inline bool Rgb32ConstPixel::equals(const Rgb32Pixel& p) const
{
    return base() == p.base();
}

///////////////////////////////////////////////////////////////////////
// Free compositing functions
///////////////////////////////////////////////////////////////////////

inline void sourceCopy(Rgb32Pixel& to, std::size_t length, const Rgb32Color& from)
{
    Rgb32::fill( to.base(), length, from );
}


///////////////////////////////////////////////////////////////////////
// Rgb32Pixel
///////////////////////////////////////////////////////////////////////

} // namespace

} // namespace

#endif
