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
#include <Pt/Gfx/PixelTraits.h>
#include <Pt/TypeTraits.h>

namespace Pt {

namespace Gfx {

template<typename InputIteratorT, typename OutputIteratorT>
void copy(InputIteratorT from, InputIteratorT fromEnd, OutputIteratorT to)
{
    for( ; from != fromEnd; ++from, ++to)
        *to = *from;
}


template<typename OutputIteratorT, typename T>
void fill(OutputIteratorT to, OutputIteratorT toEnd, const T& value)
{
    for (; to != toEnd; ++to)
        *to = value;
}


template<typename InputIteratorT, typename OutputIteratorT, typename OperationT>
void transform(InputIteratorT from, InputIteratorT fromEnd, 
               OutputIteratorT to, OperationT op)
{
    for( ; from != fromEnd; ++from, ++to)
        op(*to, *from);
}


template<typename IteratorT, typename OperationT>
void transform(IteratorT begin, IteratorT end, OperationT op)
{
    for( ; begin != end; ++begin) 
        op(*begin);
}

///////////////////////////////////////////////////////////////////////
// convert with multistage converter
///////////////////////////////////////////////////////////////////////

//
// TODO: GFX: converter or tag overload?
//

template <typename T1, typename T2, int isDirect>
struct DirectConverter 
{
    static void convert(const T1& p1, T2& p2) 
    {
        p2 = p1.toColor();
    }
};


template <typename T1, typename T2>
struct DirectConverter<T1, T2, 1> 
{
    static void convert(const T1& p1, T2& p2) 
    {
        p2 = p1;
    }
};


template <typename P1, typename P2>
struct Converter 
    : public DirectConverter<P1, P2, 
                             IsSame<typename PixelTraits<P1>::FormatType, 
                                    typename PixelTraits<P2>::FormatType>::value> 
{ };


template <typename P1, typename P2> 
void convert(const P1& p1, P2& p2)
{
    Converter<P1, P2>::convert(p1, p2);
}

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
    p2 = p1;
}


template <typename P1, typename P2> 
void copyPixel(const P1& p1, P2& p2)
{
    typedef typename PixelTraits<P1>::FormatType Fmt1;
    typedef typename PixelTraits<P2>::FormatType Fmt2;

    copyPixelImpl(p1, p2, IsSame<Fmt1, Fmt2>());
}

///////////////////////////////////////////////////////////////////////
// copy pixels
///////////////////////////////////////////////////////////////////////

template <typename P1, typename P2> 
void copyPixelsImpl(const P1& p1, P2& p2, std::size_t length, FalseType)
{
    typedef typename PixelTraits<P2>::ColorType ColorType;

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
void copyPixelsImpl(const P1& p1, P2& p2, std::size_t length, TrueType)
{
    p2.assign(p1, length);
}


template <typename P1, typename P2> 
void copyPixels(const P1& p1, P2& p2, std::size_t length)
{
    typedef typename PixelTraits<P1>::FormatType Fmt1;
    typedef typename PixelTraits<P2>::FormatType Fmt2;

    copyPixelsImpl(p1, p2, length, IsSame<Fmt1, Fmt2>());
}

} // namespace

} // namespace

#endif
