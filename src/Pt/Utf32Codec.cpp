/*
 * Copyright (C) 2004 Marc Boris Duerner
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
#include "Pt/Text/Utf32Codec.h"

namespace Pt {

Utf32Codec::Utf32Codec(size_t ref)
: Pt::TextCodec<Char, char>(ref)
{
}


Utf32Codec::~Utf32Codec()
{
}


Utf32Codec::result Utf32Codec::do_in(std::mbstate_t& s, const char* fromBegin,
                                    const char* fromEnd, const char*& fromNext,
                                    Char* toBegin, Char* toEnd, Char*& toNext) const
{
    return Utf32Codec::noconv;
}


Utf32Codec::result Utf32Codec::do_out(std::mbstate_t& s, const Char* fromBegin,
                                     const Char* fromEnd, const Char*& fromNext,
                                     char* toBegin, char* toEnd, char*& toNext) const
{
    return Utf32Codec::noconv;
}


int Utf32Codec::do_length(std::mbstate_t& s, const char* fromBegin, const char* fromEnd, size_t max) const
{
    return (fromEnd - fromBegin)/4;
}


int Utf32Codec::do_max_length() const throw()
{
    return 4;
}


bool Utf32Codec::do_always_no_conv() const throw()
{
    return true;
}

}