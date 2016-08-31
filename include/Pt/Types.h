/*
 * Copyright (C) 2004-2013 Marc Boris Duerner
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

#ifndef Pt_Types_h
#define Pt_Types_h

#include <Pt/Api.h>
#include <climits>
#include <cstddef>

namespace Pt {

    typedef unsigned char uchar;
    typedef unsigned short ushort;
    typedef unsigned int uint;
    typedef unsigned long ulong;

    typedef std::size_t size_t;
    typedef std::ptrdiff_t ssize_t;

    typedef signed char int8_t;
    typedef unsigned char uint8_t;

#if USHRT_MAX == 0xffff

    typedef short int16_t;
    typedef unsigned short uint16_t;

#elif UINT_MAX == 0xffff

    typedef int int16_t;
    typedef unsigned int uint16_t;

#endif

#if UINT_MAX == 0xffffffffUL

    typedef int int32_t;
    typedef unsigned int uint32_t;

#elif ULONG_MAX == 0xffffffffUL

    typedef long int32_t;
    typedef unsigned long uint32_t;

#endif

#if ULONG_MAX == 18446744073709551615ULL
    #define PT_WITH_INT64 1

    typedef long int64_t;
    typedef unsigned long uint64_t;

#elif ULLONG_MAX == 18446744073709551615ULL
    #define PT_WITH_INT64 1

    typedef long long int64_t;
    typedef unsigned long long uint64_t;

#elif defined(__GNUC__) || defined(__MWERKS_SYMBIAN__)
    #define PT_WITH_INT64 1

    typedef long long int64_t;
    typedef unsigned long long uint64_t;

#endif

#ifdef ULLONG_MAX
    #define PT_WITH_LONG_LONG 1
#endif

//! @internal
union varint_t
{
    void* ptr;
    bool b;
    int i;
    unsigned u;
    long l;
    unsigned long ul;
    std::size_t s;
    Pt::uint64_t u64;
};

} // namespace Pt

#endif // Pt_Types_h
