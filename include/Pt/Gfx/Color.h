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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_COLOR_H
#define PT_GFX_COLOR_H

#include <Pt/Types.h>
#include <Pt/Gfx/Api.h>
#include <Pt/String.h>


namespace Pt {
namespace Gfx {


class Color
{
    public:
        Color()
        : _a(65535)
        , _r(0)
        , _g(0)
        , _b(0)
        { }

        Color(Pt::uint16_t a, Pt::uint16_t r, Pt::uint16_t g, Pt::uint16_t b)
        : _a(a)
        , _r(r)
        , _g(g)
        , _b(b)
        { }

        Color(Pt::uint16_t r, Pt::uint16_t g, Pt::uint16_t b)
        : _a(65535)
        , _r(r)
        , _g(g)
        , _b(b)
        { }

        Pt::uint16_t alpha() const
        {
              return _a;
        }

        Pt::uint16_t red() const
        {
              return _r;
        }

        Pt::uint16_t green() const
        {
              return _g;
        }

        Pt::uint16_t blue() const
        {
              return _b;
        }

        void setAlpha( Pt::uint16_t c)
        {
              _a = c;
        }

        void setRed( Pt::uint16_t c)
        {
              _r = c;
        }

        void setGreen( Pt::uint16_t c)
        {
              _g = c;
        }

        void setBlue( Pt::uint16_t c)
        {
              _b = c;
        }

        Color toGray() const
        {
            const Pt::uint32_t rf = 77;
            const Pt::uint32_t gf = 128;
            const Pt::uint32_t bf = 51;

            const Pt::uint32_t v = (_r * rf +
                                    _g * gf +
                                    _b * bf) >> 8;

            const Pt::uint16_t s = static_cast<Pt::uint16_t>(v);

            return Color(_a, s, s, s);
        }

        static Color fromRgb8(Pt::uint8_t r, Pt::uint8_t g,
                              Pt::uint8_t b, Pt::uint8_t a = 255)
        {
            return Color(a * 257, r * 257, g * 257, b * 257);
        }

        bool operator==(const Color& c) const
        {
            return _a == c._a && _r == c._r && _g == c._g && _b == c._b;
        }

    private:
        Pt::uint16_t _a;
        Pt::uint16_t _r;
        Pt::uint16_t _g;
        Pt::uint16_t _b;
};


} // namespace
} // namespace

#endif
