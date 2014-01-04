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

#include <Pt/Utf32Codec.h>
#include <algorithm>

const Pt::Char MaxUtf16 = 0x0010FFFF;

namespace Pt {

Utf32BECodec::Utf32BECodec(std::size_t ref)
: Pt::TextCodec<Char, char>(ref)
{}


Utf32BECodec::~Utf32BECodec()
{}


Utf32BECodec::result Utf32BECodec::do_in(MBState& s, 
                                         const char* from, const char* fromEnd, const char*& fromNext,
                                         Char* to, Char* toEnd, Char*& toNext) const
{
    fromNext = from;
    toNext = to;

    while( 4 <= fromEnd - fromNext && toNext != toEnd )
    {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(fromNext);
        Pt::Char ch = static_cast<Pt::uint32_t>( p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3] );
        if(MaxUtf16 < ch)
            return std::codecvt_base::error;

        *toNext++ = ch;
        fromNext += 4;
    }

    return fromNext != fromEnd ? std::codecvt_base::partial 
                               : std::codecvt_base::ok;
}


Utf32BECodec::result Utf32BECodec::do_out(MBState& s, const Char* fromBegin,
                                          const Char* fromEnd, const Char*& fromNext,
                                          char* toBegin, char* toEnd, char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    while(fromNext != fromEnd && 4 <= toEnd - toNext)
    {
        Pt::uint32_t ch = fromNext->value();
        fromNext++;

        if(0x0010FFFF < ch)
            return std::codecvt_base::error;

        *toNext++ = (char)(ch >> 24);
        *toNext++ = (char)(ch >> 16);
        *toNext++ = (char)(ch >> 8);
        *toNext++ = (char)ch;
    }

    return fromBegin == fromNext ? std::codecvt_base::partial 
                                 : std::codecvt_base::ok;
}


int Utf32BECodec::do_length(MBState& s, const char* fromBegin, const char* fromEnd, std::size_t max) const
{
    std::size_t n = (fromEnd - fromBegin);
    std::size_t count = std::min(max, n/4);

    return static_cast<int>(count);
}


bool Utf32BECodec::do_always_noconv() const throw() 
{ return false; }


int Utf32BECodec::do_max_length() const throw()  
{ return 4; }


std::codecvt_base::result Utf32BECodec::do_unshift(Pt::MBState&, char*, char*, char*&) const
{ return std::codecvt_base::noconv; }


int Utf32BECodec::do_encoding() const throw()
{ return 4; }




Utf32LECodec::Utf32LECodec(std::size_t ref)
: Pt::TextCodec<Char, char>(ref)
{}


Utf32LECodec::~Utf32LECodec()
{}


Utf32LECodec::result Utf32LECodec::do_in(MBState& s, 
                                         const char* from, const char* fromEnd, const char*& fromNext,
                                         Char* to, Char* toEnd, Char*& toNext) const
{
    fromNext = from;
    toNext = to;

    while( 4 <= fromEnd - fromNext && toNext != toEnd )
    {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(fromNext);

        Pt::Char ch = static_cast<Pt::uint32_t>( p[3] << 24 | p[2] << 16 | p[1] << 8 | p[0] );

        if(MaxUtf16 < ch)
            return std::codecvt_base::error;

        *toNext++ = ch;
        fromNext += 4;
    }

    return fromNext != fromEnd ? std::codecvt_base::partial 
                               : std::codecvt_base::ok;
}


Utf32LECodec::result Utf32LECodec::do_out(MBState& s, const Char* fromBegin,
                                          const Char* fromEnd, const Char*& fromNext,
                                          char* toBegin, char* toEnd, char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    while(fromNext != fromEnd && 4 <= toEnd - toNext)
    {
        Pt::uint32_t ch = fromNext->value();
        fromNext++;

        if(0x0010FFFF < ch)
            return std::codecvt_base::error;

        *toNext++ = (char)ch;
        *toNext++ = (char)(ch >> 8);
        *toNext++ = (char)(ch >> 16);
        *toNext++ = (char)(ch >> 24);
    }

    return fromBegin == fromNext ? std::codecvt_base::partial 
                                 : std::codecvt_base::ok;
}


int Utf32LECodec::do_length(MBState& s, const char* fromBegin, const char* fromEnd, std::size_t max) const
{
    std::size_t n = (fromEnd - fromBegin);
    std::size_t count = std::min(max, n/4);

    return static_cast<int>(count);
}


bool Utf32LECodec::do_always_noconv() const throw() 
{ return false; }


int Utf32LECodec::do_max_length() const throw()  
{ return 4; }


std::codecvt_base::result Utf32LECodec::do_unshift(Pt::MBState&, char*, char*, char*&) const
{ return std::codecvt_base::noconv; }


int Utf32LECodec::do_encoding() const throw()
{ return 4; }

} // namespace Pt
