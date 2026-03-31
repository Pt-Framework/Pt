/* Copyright (C) 2010-2016 Marc Boris Duerner 
   Copyright (C) 2006-2010 by Aloysius Indrayanto

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

#ifndef PT_GFX_COPYPIXEL_H
#define PT_GFX_COPYPIXEL_H

#include <Pt/Gfx/Api.h>
#include <Pt/Gfx/ConvertColor.h>
#include <Pt/Gfx/ImageTraits.h>
#include <Pt/TypeTraits.h>

#include <algorithm>
#include <cstddef>

namespace Pt {

namespace Gfx {

///////////////////////////////////////////////////////////////////////
// PixelConverter
///////////////////////////////////////////////////////////////////////

template <typename Fmt1, typename Fmt2>
struct PixelConverter
{
    template <typename P1, typename P2>
    static void convert(const P1& from, P2& to)
    {
        typedef typename P2::ColorType ToColorType;

        to = convertColor<ToColorType>(from.toColor());
    }

    template <typename P1, typename P2>
    static void convert(const P1& from, P2& to, std::size_t length)
    {
        typedef typename P1::ColorType SourceColorType;
        typedef typename P2::ColorType TargetColorType;

        convertImpl(from, to, length, IsSame<SourceColorType, TargetColorType>());
    }

    template <typename P1, typename P2>
    static void convertImpl(const P1& from, P2& to, std::size_t length, TrueType)
    {
        typedef typename P1::ColorType ColorType;

        const std::size_t bufsize = 64;
        ColorType colors[bufsize];

        P1 f(from);
        P2 t(to);

        while(length > 0)
        {
            std::size_t n = std::min(length, bufsize);

            f.getColors(colors, n);
            t.assign(colors, n);

            f.advance(n);
            t.advance(n);

            length -= n;
        }
    }

    template <typename P1, typename P2>
    static void convertImpl(const P1& from, P2& to, std::size_t length, FalseType)
    {
        typedef typename P1::ColorType SourceColorType;
        typedef typename P2::ColorType TargetColorType;

        const std::size_t bufsize = 64;
        SourceColorType sourceColors[bufsize];
        TargetColorType targetColors[bufsize];

        P1 f(from);
        P2 t(to);

        while(length > 0)
        {
            std::size_t n = std::min(length, bufsize);

            f.getColors(sourceColors, n);
            convertColors(targetColors, sourceColors, n);
            t.assign(targetColors, n);

            f.advance(n);
            t.advance(n);

            length -= n;
        }
    }
};

template <typename Fmt>
struct PixelConverter<Fmt, Fmt>
{
    template <typename P1, typename P2>
    static void convert(const P1& from, P2& to)
    {
        to.assign(from);
    }

    template <typename P1, typename P2>
    static void convert(const P1& from, P2& to, std::size_t length)
    {
        to.assign(from, length);
    }
};

///////////////////////////////////////////////////////////////////////
// copy pixels
///////////////////////////////////////////////////////////////////////

/** @brief Copies a single pixel.
 */
template <typename P1, typename P2>
void copyPixel(const P1& from, P2& to)
{
    typedef typename P1::FormatType FromFmt;
    typedef typename P2::FormatType ToFmt;

    PixelConverter<FromFmt, ToFmt>::convert(from, to);
}

/** @brief Copies a span of pixels.
 */
template <typename P1, typename P2>
void copyPixel(const P1& from, P2& to, std::size_t length)
{
    typedef typename P1::FormatType FromFmt;
    typedef typename P2::FormatType ToFmt;

    PixelConverter<FromFmt, ToFmt>::convert(from, to, length);
}

} // namespace Gfx

} // namespace Pt

#endif
