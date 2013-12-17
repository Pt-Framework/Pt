/*
 * Copyright (C) 2013 by Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PT_NUMERIC_CAST_H
#define PT_NUMERIC_CAST_H

#include <Pt/Api.h>
#include <Pt/ConversionError.h>
#include <limits>

namespace Pt {

template <bool signedX, bool signedY>
struct LessThanMin
{
    template <class X, class Y>
    static bool check(X x, Y y_min)
    { return x < y_min; }
};


template <>
struct LessThanMin<false, true>
{
    template <class X, class Y>
    static bool check(X, Y)
    { return false; }
};


template <>
struct LessThanMin<true, false>
{
    template <class X, class Y>
    static bool check(X x, Y)
    { return x < 0; }
};


template <bool signedX, bool signedY>
struct GreaterThanMax
{
    // both signed or both unsigned
    template <class X, class Y>
    static bool check(X x, Y ymax)
    { return x > ymax; }
};


template <>
struct GreaterThanMax<false, true>
{
    // x is unsigned, y is signed
    template <class X, class Y>
    static bool check(X x, Y ymax)
    { 
        return x > static_cast<X>(ymax);
    }
};


template <>
struct GreaterThanMax<true, false>
{
    // x is signed, y is unsigned
    template <class X, class Y>
    static bool check(X x, Y ymax)
    { 
        return x > 0 && static_cast<Y>(x) > ymax;
    }
};


template<typename Target, typename Source>
inline Target numeric_cast(Source arg)
{
    typedef std::numeric_limits<Source> SourceTraits;
    typedef std::numeric_limits<Target> ResultTraits;

    const bool sourceIsSigned = SourceTraits::is_signed;
    const bool resultIsSigned = ResultTraits::is_signed;
    const bool sameSign = sourceIsSigned == resultIsSigned;

    if (LessThanMin<sourceIsSigned, resultIsSigned>::check( arg, ResultTraits::min() )
     || GreaterThanMax<sourceIsSigned, resultIsSigned>::check( arg, ResultTraits::max() ) )
    {
        throw ConversionError("numeric conversion failed");
    }
    
    return static_cast<Target>(arg);
}

} // namespace Pt

#endif
