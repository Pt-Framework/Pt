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

#include<Pt/Utf16Codec.h>

const Pt::Char MaxUtf16 = 0x0010FFFF;

namespace Pt {

Utf16BECodec::Utf16BECodec(std::size_t ref)
: Pt::TextCodec<Char, char>(ref)
{}


Utf16BECodec::~Utf16BECodec()
{}


Utf16BECodec::result Utf16BECodec::do_in(MBState& s, 
                                         const char* from, const char* fromEnd, const char*& fromNext,
                                         Char* to, Char* toEnd, Char*& toNext) const
{
    fromNext = from;
    toNext = to;

    while( 2 <= fromEnd - fromNext && toNext != toEnd )
    {
        Pt::Char ch;
        const unsigned char* p = reinterpret_cast<const unsigned char*>(fromNext);
        Pt::uint16_t ch0 = static_cast<unsigned short>(p[0] << 8 | p[1]);

        if (ch0 < 0xd800 || 0xdc00 <= ch0)
        {
            // decode from one 16-bit word
            fromNext += 2;
            ch = ch0;
        }
        else if (fromEnd - fromNext >= 4)
        {
            // decode from two 16-bit words
            Pt::uint16_t ch1 = (Pt::uint16_t)(p[2] << 8 | p[3]);

            if (ch1 < 0xdc00 || 0xe000 <= ch1)
                return std::codecvt_base::error;

            fromNext += 4;
            ch = static_cast<Pt::uint32_t>(ch0 - 0xd800 + 0x0040) << 10 | (ch1 - 0xdc00);
        }
        else
        {
            break;
        }

        if(MaxUtf16 < ch)
            return std::codecvt_base::error;

        *toNext++ = ch;
    }

    return fromNext != fromEnd ? std::codecvt_base::partial 
                               : std::codecvt_base::ok;
}


Utf16BECodec::result Utf16BECodec::do_out(MBState& s, const Char* fromBegin,
                                          const Char* fromEnd, const Char*& fromNext,
                                          char* toBegin, char* toEnd, char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    while(fromNext != fromEnd && 2 <= toEnd - toNext)
    {
        bool multi = false;
        Pt::uint32_t ch = fromNext->value();
        fromNext++;

        if(0x0010FFFF < ch)
            return std::codecvt_base::error;

        if(ch <= 0xffff)
        {	
            // encode one 16-bit word
            if (0xd800 <= ch && ch < 0xdc00)
                return std::codecvt_base::error;
        }
        else if (toEnd - toNext < 4)
        {	
            // not enough space
            --fromNext;
            return std::codecvt_base::partial;
        }
        else
        {
            multi = true;
        }

        if( ! multi )
        {
            *toNext++ = (char)(ch >> 8);
            *toNext++ = (char)ch;
        }
        else
        {
            Pt::uint16_t ch0 = (Pt::uint16_t)(0xd800 | ((Pt::uint16_t)(ch >> 10) - 0x0040));
            *toNext++ = (char)(ch0 >> 8);
            *toNext++ = (char)ch0;

            ch0 = (Pt::uint16_t)(0xdc00 | ((Pt::uint16_t)ch & 0x03ff));
            *toNext++ = (char)(ch0 >> 8);
            *toNext++ = (char)ch0;
        }
    }

    return fromBegin == fromNext ? std::codecvt_base::partial 
                                 : std::codecvt_base::ok;
}


int Utf16BECodec::do_length(MBState& s, const char* fromBegin, const char* fromEnd, std::size_t max) const
{
    std::size_t count = 0;
    MBState state = s;

    while(count < max && fromBegin != fromEnd)
    {
        const char* fromNext;
        Pt::Char* toNext;
        Pt::Char ch;

        Utf16BECodec::result r = do_in(state, fromBegin, fromEnd, fromNext, &ch, &ch + 1, toNext);
        switch(r)
        {
            case std::codecvt_base::noconv:
                return static_cast<int>(count + (fromEnd - fromBegin));

            case  std::codecvt_base::ok:
                // do_in may swallow the char into state
                if (toNext == &ch + 1)
                    ++count;	
                
                fromBegin = fromNext;
                break;

            default: // error or partial
                return static_cast<int>(count);	
        }
    }
  
    return static_cast<int>(count);
}


bool Utf16BECodec::do_always_noconv() const throw() 
{ 
    return false; 
}


int Utf16BECodec::do_max_length() const throw()  
{ 
    return 12; 
}


std::codecvt_base::result Utf16BECodec::do_unshift(Pt::MBState&, char*, char*, char*&) const
{ 
    return std::codecvt_base::noconv; 
}


int Utf16BECodec::do_encoding() const throw()
{ 
    return 0; 
}




Utf16LECodec::Utf16LECodec(std::size_t ref)
: Pt::TextCodec<Char, char>(ref)
{}


Utf16LECodec::~Utf16LECodec()
{}


Utf16LECodec::result Utf16LECodec::do_in(MBState& s, 
                                         const char* from, const char* fromEnd, const char*& fromNext,
                                         Char* to, Char* toEnd, Char*& toNext) const
{
    fromNext = from;
    toNext = to;

    for (; 2 <= fromEnd - fromNext && toNext != toEnd; )
    {
        unsigned char *Ptr = (unsigned char*)fromNext;
        Pt::Char ch;
        Pt::uint16_t ch0, ch1;

        ch0 = (unsigned short)(Ptr[1] << 8 | Ptr[0]);

        if (ch0 < 0xd800 || 0xdc00 <= ch0)
        {
            fromNext += 2;
            ch = ch0;
        }
        else if (fromEnd - fromNext >= 4)
        {
          ch1 = (Pt::uint16_t)(Ptr[3] << 8 | Ptr[2]);

          if (ch1 < 0xdc00 || 0xe000 <= ch1)
              return std::codecvt_base::error;

          fromNext += 4;

          ch = (Pt::uint32_t)(ch0 - 0xd800 + 0x0040) << 10 | (ch1 - 0xdc00);
        }
        else
        {
            break;
        }

        if (MaxUtf16 < ch)
            return std::codecvt_base::error;

        *toNext++ = ch;
    }
   
    return fromNext != fromEnd ? std::codecvt_base::partial 
                               : std::codecvt_base::ok;
 }

Utf16LECodec::result Utf16LECodec::do_out(MBState& s, const Char* fromBegin,
                                          const Char* fromEnd, const Char*& fromNext,
                                          char* toBegin, char* toEnd, char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;
    
    while( fromNext != fromEnd && 2 <= toEnd - toNext )
    {
        bool multi = false;
        Pt::uint32_t ch = fromNext->value();
        fromNext++;

        if (0x0010FFFF  < ch)
            return std::codecvt_base::error;

        if (ch <= 0xffff)
        {
            if (0xd800 <= ch && ch < 0xdc00)
                return std::codecvt_base::error;
        }
        else if (toEnd - toNext < 4)
        {
            --fromNext;
            return std::codecvt_base::partial;
        }
        else
            multi = true;

        if( ! multi )
        {
            *toNext++ = (char)ch;
            *toNext++ = (char)(ch >> 8);
        }
        else
        {
            Pt::uint16_t ch0 = (Pt::uint16_t)(0xd800 | ((Pt::uint16_t)(ch >> 10) - 0x0040));
            *toNext++ = (char)ch0;
            *toNext++ = (char)(ch0 >> 8);

            ch0 = (Pt::uint16_t)(0xdc00 | ((Pt::uint16_t)ch & 0x03ff));
            *toNext++ = (char)ch0;
            *toNext++ = (char)(ch0 >> 8);
        }
    }
		
    return fromBegin == fromNext ? std::codecvt_base::partial 
                                 : std::codecvt_base::ok;
}


int Utf16LECodec::do_length(MBState& s, const char* fromBegin, const char* fromEnd, std::size_t max) const
{
    std::size_t count = 0;
    MBState state = s;

    while( count < max && fromBegin != fromEnd )
    {
        const char *fromNext;
        Pt::Char *toNext;
        Pt::Char ch;

        Utf16LECodec::result r = do_in(state, fromBegin, fromEnd, fromNext, &ch, &ch + 1, toNext);
        switch(r)
        {
            case std::codecvt_base::noconv:
                return static_cast<int>(count + (fromEnd - fromBegin));

            case  std::codecvt_base::ok:
                if (toNext == &ch + 1)
                    ++count;
            
                fromBegin = fromNext;
                break;

            default:
                return static_cast<int>(count);
        }
    }

    return static_cast<int>(count);
}


bool Utf16LECodec::do_always_noconv() const throw() 
{ 
    return false; 
}


int Utf16LECodec::do_max_length() const throw()  
{ 
    return 12; 
}


std::codecvt_base::result Utf16LECodec::do_unshift(Pt::MBState&, char*, char*, char*&) const
{ 
    return std::codecvt_base::noconv; 
}


int Utf16LECodec::do_encoding() const throw()
{ 
    return 0; 
}

} // namespace Pt
