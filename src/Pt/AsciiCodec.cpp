/*
 * Copyright (C) 2005 by Marc Boris Drner
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
#include <Pt/AsciiCodec.h>

namespace Pt {

AsciiCodec::AsciiCodec(size_t ref)
: Pt::TextCodec<Char, char>(ref)
{}


AsciiCodec::~AsciiCodec()
{}


AsciiCodec::result AsciiCodec::do_in(MBState& s, const char* fromBegin,
                                     const char* fromEnd, const char*& fromNext,
                                     Pt::Char* toBegin, Pt::Char* toEnd, Pt::Char*& toNext) const
{
    toNext = toBegin;
    for(fromNext = fromBegin; fromNext < fromEnd; ++fromNext) {
        if(toNext == toEnd)
            return AsciiCodec::partial;

        *toNext = Char( *(fromNext) );
        ++toNext;
    }

    return AsciiCodec::ok;
}


AsciiCodec::result AsciiCodec::do_out(MBState& s, const Pt::Char* fromBegin,
                                      const Pt::Char* fromEnd, const Pt::Char*& fromNext,
                                      char* toBegin, char* toEnd, char*& toNext) const
{
    toNext = toBegin;
    for(fromNext = fromBegin; fromNext < fromEnd; ++fromNext) {
        if(toNext == toEnd)
            return AsciiCodec::partial;

        *toNext = fromNext->narrow('*');
        if( *toNext < 0x00 ) {
            *toNext = '*';
        }

        ++toNext;
    }

    return AsciiCodec::ok;
}


int AsciiCodec::do_length(MBState& s, const char* fromBegin, const char* fromEnd, size_t max) const
{
    return fromEnd-fromBegin;
}


int AsciiCodec::do_max_length() const throw()
{
    return 1;
}


bool AsciiCodec::do_always_no_conv() const throw()
{
    return false;
}

} // namespace Pt
