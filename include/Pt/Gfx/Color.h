/* Copyright (C) 2016 Marc Boris Duerner
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

#ifndef PT_GFX_COLOR_H
#define PT_GFX_COLOR_H

#include <Pt/Gfx/Api.h>
#include <Pt/Types.h>
#include <cstddef>
#include <cstring>

namespace Pt {

namespace Gfx {

class ColorF;

/** @brief Standard color type.
*/
class Color
{
    public:
        Color()
        : _value(0)
        { }

        Color(const Color& color) = default;

        explicit Color(uint32_t value)
        : _value(value)
        { }

        explicit Color(const uint8_t* base)
        : _value()
        {
            std::memcpy( &_value, base, sizeof(uint32_t) );
        }

        Color(Pt::uint8_t r, Pt::uint8_t g, Pt::uint8_t b)
        : _value( (uint32_t(255) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b) )
        { }

        Color(Pt::uint8_t a, Pt::uint8_t r, Pt::uint8_t g, Pt::uint8_t b)
        : _value( (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b) )
        { }

        explicit Color(const ColorF& c);

        Color& operator=(const Color& color) = default;

        Color& operator=(const ColorF& c);

        Color& operator=(uint32_t value)
        {
            _value = value;
            return *this;
        }

        Pt::uint8_t alpha() const
        {
            return _value >> 24;
        }

        void setAlpha(Pt::uint8_t a)
        {
            _value = (_value & 0x00FFFFFF) | (uint32_t(a) << 24);
        }

        Pt::uint8_t red() const
        {
            return (_value & 0x00FF0000) >> 16;
        }

        void setRed(Pt::uint8_t r)
        {
            _value = (_value & 0xFF00FFFF) | (uint32_t(r) << 16);
        }

        Pt::uint8_t green() const
        {
            return (_value & 0x0000FF00) >> 8;
        }

        void setGreen(Pt::uint8_t g)
        {
            _value = (_value & 0xFFFF00FF) | (uint32_t(g) << 8);
        }

        Pt::uint8_t blue() const
        {
            return _value & 0x000000FF;
        }

        void setBlue(Pt::uint8_t b)
        {
            _value = (_value & 0xFFFFFF00) | uint32_t(b);
        }

        const uint32_t& value() const
        {
            return _value;
        }

        bool operator==(const Color& other) const
        { return _value == other._value; }

        bool operator!=(const Color& other) const
        { return _value != other._value; }

    private:
        Pt::uint32_t _value;
};

/** @brief RGBA float32 working color.

    Four native floats in RGBA memory order. Channels use the range
    [0, 1]. Values greater than 1 are allowed for HDR. Alpha is
    straight. ColorF is a working type, not a pixel buffer format.
    Memory order matches RGBA32F so a later ArgbF32 format can copy
    a ColorF as 16 bytes.
*/
class ColorF
{
    public:
        ColorF()
        : _r(0)
        , _g(0)
        , _b(0)
        , _a(0)
        { }

        ColorF(const ColorF&) = default;

        explicit ColorF(const Color& c);

        ColorF& operator=(const ColorF&) = default;

        ColorF& operator=(const Color& c);

        ColorF(float a, float r, float g, float b)
        : _r(r)
        , _g(g)
        , _b(b)
        , _a(a)
        { }

        ColorF(float r, float g, float b)
        : _r(r)
        , _g(g)
        , _b(b)
        , _a(1.f)
        { }

        float alpha() const
        {
            return _a;
        }

        void setAlpha(float c)
        {
            _a = c;
        }

        float red() const
        {
            return _r;
        }

        void setRed(float c)
        {
            _r = c;
        }

        float green() const
        {
            return _g;
        }

        void setGreen(float c)
        {
            _g = c;
        }

        float blue() const
        {
            return _b;
        }

        void setBlue(float c)
        {
            _b = c;
        }

        ColorF toGray() const
        {
            const float s = 0.299f * _r + 0.587f * _g + 0.114f * _b;
            return ColorF(_a, s, s, s);
        }

        /** @brief Converts an 8-bit channel to the unit interval.
        */
        static float toChannelF(Pt::uint8_t c)
        {
            return c * (1.f / 255.f);
        }

        /** @brief Converts a unit-interval channel to 8-bit.

            Values below 0 become 0. Values of 1 or greater become 255.
        */
        static Pt::uint8_t toChannel8(float c)
        {
            if(c <= 0.f)
                return 0;
            if(c >= 1.f)
                return 255;

            return static_cast<Pt::uint8_t>(c * 255.f + 0.5f);
        }

        static ColorF fromRgb8(Pt::uint8_t r, Pt::uint8_t g,
                               Pt::uint8_t b, Pt::uint8_t a = 255)
        {
            return ColorF(toChannelF(a), toChannelF(r),
                          toChannelF(g), toChannelF(b));
        }

    private:
        float _r;
        float _g;
        float _b;
        float _a;
};


inline Color::Color(const ColorF& c)
: _value( (Pt::uint32_t(ColorF::toChannel8(c.alpha())) << 24) |
          (Pt::uint32_t(ColorF::toChannel8(c.red()))   << 16) |
          (Pt::uint32_t(ColorF::toChannel8(c.green())) <<  8) |
           Pt::uint32_t(ColorF::toChannel8(c.blue())) )
{
}


inline Color& Color::operator=(const ColorF& c)
{
    _value = (Pt::uint32_t(ColorF::toChannel8(c.alpha())) << 24) |
             (Pt::uint32_t(ColorF::toChannel8(c.red()))   << 16) |
             (Pt::uint32_t(ColorF::toChannel8(c.green())) <<  8) |
              Pt::uint32_t(ColorF::toChannel8(c.blue()));
    return *this;
}


inline ColorF::ColorF(const Color& c)
: _r(toChannelF(c.red()))
, _g(toChannelF(c.green()))
, _b(toChannelF(c.blue()))
, _a(toChannelF(c.alpha()))
{
}


inline ColorF& ColorF::operator=(const Color& c)
{
    _r = toChannelF(c.red());
    _g = toChannelF(c.green());
    _b = toChannelF(c.blue());
    _a = toChannelF(c.alpha());
    return *this;
}

} // namespace

} // namespace

#endif
