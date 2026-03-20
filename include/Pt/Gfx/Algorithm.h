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

#ifndef PT_GFX_ALGORITHM_H
#define PT_GFX_ALGORITHM_H

#include <Pt/Gfx/Api.h>
#include <Pt/TypeTraits.h>

namespace Pt {

namespace Gfx {

struct SameFormat
{};


struct DifferentFormat
{};


struct FromImageFormat 
{};


struct ToImageFormat 
{};


template <typename Fmt1, typename Fmt2>
struct MatchFormat
{
    typedef DifferentFormat Type;
};


template <typename Fmt>
struct MatchFormat<Fmt, Fmt>
{
    typedef SameFormat Type;
};


template <typename Fmt2>
struct MatchFormat<ImageFormat, Fmt2>
{
    typedef FromImageFormat Type;
};


template <typename Fmt1>
struct MatchFormat<Fmt1, ImageFormat>
{
    typedef ToImageFormat Type;
};


template <>
struct MatchFormat<ImageFormat, ImageFormat>
{
    typedef SameFormat Type;
};

///////////////////////////////////////////////////////////////////////
// copy single pixel
///////////////////////////////////////////////////////////////////////

template <typename P1, typename P2> 
void copyPixelImpl(const P1& p1, P2& p2, FalseType)
{
    p2 = p1.toColor();
}


template <typename P1, typename P2> 
void copyPixelImpl(const P1& p1, P2& p2, TrueType)
{
    p2.assign(p1);
}


template <typename P1, typename P2> 
void copyPixel(const P1& p1, P2& p2)
{
    typedef typename P1::FormatType Fmt1;
    typedef typename P2::FormatType Fmt2;

    copyPixelImpl(p1, p2, IsSame<Fmt1, Fmt2>());
}

///////////////////////////////////////////////////////////////////////
// copy lines
///////////////////////////////////////////////////////////////////////

template <typename P1, typename P2> 
void copyLineImpl(const P1& p1, P2& p2, std::size_t length, SameFormat)
{
    p2.assign(p1, length);
}


template <typename P1, typename P2> 
void copyLineImpl(const P1& p1, P2& p2, std::size_t length, DifferentFormat)
{
    typedef typename P2::ColorType ColorType;

    const std::size_t bufsize = 64;
    ColorType colors[bufsize];

    P1 from(p1);
    P2 to(p2);

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


template <typename P1, typename P2> 
void copyLineImpl(const P1& p1, P2& p2, std::size_t length, FromImageFormat)
{
    copyLineImpl(p1, p2, length, DifferentFormat());
}


template <typename P1, typename P2> 
void copyLineImpl(const P1& p1, P2& p2, std::size_t length, ToImageFormat)
{
    copyLineImpl(p1, p2, length, DifferentFormat());
}


template <typename P1, typename P2> 
void copyLine(const P1& p1, P2& p2, std::size_t length)
{
    typedef typename P1::FormatType Fmt1;
    typedef typename P2::FormatType Fmt2;

    copyLineImpl(p1, p2, length, typename MatchFormat<Fmt1, Fmt2>::Type());
}

///////////////////////////////////////////////////////////////////////
// copy area
///////////////////////////////////////////////////////////////////////

template <typename P1, typename P2> 
void copyArea(P1 from, P2 to, std::size_t width, std::size_t height)
{
    for(std::size_t y = 0; y < height; ++y)
    {
        copyLine(from, to, width);
        from.advanceLines(1);
        to.advanceLines(1);
    }
}

///////////////////////////////////////////////////////////////////////
// convert with multistage converter
///////////////////////////////////////////////////////////////////////

//template <typename T1, typename T2, int isDirect>
//struct DirectConverter 
//{
//    static void convert(const T1& p1, T2& p2) 
//    {
//        p2 = p1.toColor();
//    }
//};
//
//
//template <typename T1, typename T2>
//struct DirectConverter<T1, T2, 1> 
//{
//    static void convert(const T1& p1, T2& p2) 
//    {
//        p2 = p1;
//    }
//};
//
//
//template <typename P1, typename P2>
//struct Converter 
//    : public DirectConverter<P1, P2, 
//                             IsSame<typename P1::FormatType, 
//                                    typename P2::FormatType>::value> 
//{ };
//
//
//template <typename P1, typename P2> 
//void convert(const P1& p1, P2& p2)
//{
//    Converter<P1, P2>::convert(p1, p2);
//}

} // namespace

} // namespace

#endif
