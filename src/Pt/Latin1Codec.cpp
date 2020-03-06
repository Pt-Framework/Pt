/*
 * Copyright (C) 2020 by Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
 * MA 02110-1301 USA
 */

#include <Pt/Latin1Codec.h>
#include <Pt/Types.h>

namespace Pt {

Latin1Codec::Latin1Codec(std::size_t ref)
: Pt::TextCodec<Pt::Char, char>(ref)
{
}


Latin1Codec::~Latin1Codec()
{
}


Latin1Codec::result Latin1Codec::do_in(Pt::MBState& s,
                                       const char* fromBegin, 
                                       const char* fromEnd, 
                                       const char*& fromNext,
                                       Pt::Char* toBegin, 
                                       Pt::Char* toEnd, 
                                       Pt::Char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    while( fromNext != fromEnd && toNext != toEnd )
    {
        *toNext++ = Char(*fromNext++);
    }

    return fromNext != fromEnd ? std::codecvt_base::partial 
                               : std::codecvt_base::ok;
}


Latin1Codec::result Latin1Codec::do_out(Pt::MBState& s,
                                        const Pt::Char* fromBegin,
                                        const Pt::Char* fromEnd, 
                                        const Pt::Char*& fromNext,
                                        char* toBegin, 
                                        char* toEnd, 
                                        char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    while(fromNext != fromEnd && toNext != toEnd)
    {
        const Pt::Char& ch = *fromNext++;
        *toNext++ = ch > 0xFF ? 0x3F : ch.narrow();
    }

    return fromBegin == fromNext ? std::codecvt_base::partial 
                                 : std::codecvt_base::ok;
}

} // namespace
