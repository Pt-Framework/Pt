/* Copyright (C) 2016 Marc Boris Duerner 

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA  02110-1301  USA
*/

#include <Pt/Utf8.h>
#include <Pt/ConversionError.h>
#include <algorithm>

namespace Pt {

//
// Utf8Iterator
//

void Utf8Iterator::decode()
{
    const char* from = _utf8;
    const char* fromEnd = from + _n;
    const char* fromNext = from;

    Char* to = &_value;
    Char* toEnd = to + 1;
    Char* toNext = to;

    MBState s;
    Utf8Codec::result r = _codec.in(s, from, fromEnd, fromNext,
                                        to,   toEnd,   toNext);
            
    std::size_t count = fromNext - from;
    if(toEnd != toNext || count == 0 || count > _n)
    {
        // TODO: error handling throw ConversionError
        _utf8 = 0;
        _n = 0;
    }

    _n -= count;
    _utf8 += count;
}

//
// Utf8Appender
//

void Utf8Appender::encode(const Char& ch)
{
    MBState s;

    const Char* from = &ch;
    const Char* fromEnd = from + 1;
    const Char* fromNext = from;

    char buf[6] = {0};

    char* to = buf;
    char* toEnd = buf + sizeof(buf);
    char* toNext = to;

    Utf8Codec::result r = _codec.out(s, from, fromEnd, fromNext, 
                                        to, toEnd, toNext);
    
    if (r == Utf8Codec::error)
    {
        // TODO: error handling throw ConversionError
        return;
    }

    std::size_t count = toNext - to;
    _str->append(to, count);
}

//
// Utf8Convert
//

Utf8Convert::Utf8Convert(TextCodec<Char, char>* codec)
: _codec(codec)
{
}
      
        
Utf8Convert::~Utf8Convert()
{
    // _codec->refs() == 0 indicates ownership
    if(_codec && _codec->refs() == 0)
        delete _codec;
}


std::string Utf8Convert::toBytes(const char* utf8, std::size_t n)
{
    std::string ret;
    Utf8Iterator it(utf8, n);
    Utf8Iterator end;

    Char* ibufEnd = _ibuf + _ibufSize;
    
    if( ! _codec)
        return ret;

    MBState s;
    CodecType::result r = CodecType::noconv;

    while(it != end)
    {
        Char* fromBegin       = _ibuf;
        Char* fromEnd         = fromBegin;
        const Char* fromNext  = fromBegin;

        while(it != end && fromEnd != ibufEnd)
            *fromEnd++ = *it++;

        do
        {
            char* toBegin = _ebuf;
            char* toEnd   = _ebuf + _ebufSize;
            char* toNext  = toBegin;

            if(_codec)
                r = _codec->out(s, fromBegin, fromEnd, fromNext, 
                                   toBegin, toEnd, toNext);

            if(r == CodecType::noconv)
            {
                int n = std::min(fromEnd - fromBegin, toEnd - toBegin);
        
                // copy characters and advance fromNext and toNext
                while(n-- > 0)
                    *toNext++ = *fromNext++;

                r = fromNext == fromEnd ? CodecType::ok
                                        : CodecType::partial;
            }
            
            if(r == CodecType::error)
            {
                // TODO: error handling throw ConversionError
                return ret;
            }
            
            ret.append(toBegin, toNext - toBegin);
        }
        while(r == CodecType::partial);
    }

    return ret;
}

        
std::string Utf8Convert::fromBytes(const char* bytes, std::size_t n)
{ 
    std::string ret;
    Utf8Appender appender(ret);

    const char* fromBegin = bytes;
    const char* fromEnd   = bytes + n;
    const char* fromNext  = fromBegin;
    Char* toBegin         = _ibuf;
    Char* toEnd           = _ibuf + _ibufSize;
    Char* toNext          = toBegin;

    if( ! _codec)
        return ret;

    MBState s;
    CodecType::result r = CodecType::noconv;

    do
    { 
        r = _codec->in(s, fromBegin, fromEnd, fromNext, 
                          toBegin, toEnd, toNext);

        if(r == CodecType::noconv)
        {
            int n = std::min(fromEnd - fromBegin, toEnd - toBegin);
        
            // copy characters and advance fromNext and toNext
            while(n-- > 0)
                *toNext++ = *fromNext++;

            r = fromNext == fromEnd ? CodecType::ok
                                    : CodecType::partial;
        }
        
        if(r == CodecType::error)
        {
            // TODO: error handling throw ConversionError
            return ret;
        }
        
        std::copy(toBegin, toNext, appender);
    }
    while (r == CodecType::partial);

    return ret; 
}

} // namespace Pt
