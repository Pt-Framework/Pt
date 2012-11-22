/*
 * Copyright (C) 2012 Marc Boris Duerner
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
#ifndef Pt_IOStream_h
#define Pt_IOStream_h

#include <Pt/Api.h>
#include <Pt/StreamBuffer.h>
#include <iostream>
#include <algorithm>

namespace Pt {

template <typename CharT>
class BasicIStream : public std::basic_istream<CharT>
{
    public:
        explicit BasicIStream(BasicStreamBuffer<CharT>* sb = 0);
        
        ~BasicIStream()
        {}

        //! @brief Peeks bytes in the stream buffer.
        /**
            The number of bytes that can be peeked depends on the current
            stream buffer get area and maybe less than requested,
            similar to istream::readsome().
        */
        std::streamsize peeksome(CharT* buffer, std::streamsize n);

        BasicStreamBuffer<CharT>* buffer()
        { return _buffer; }

        void setBuffer(BasicStreamBuffer<CharT>* sb)
        { 
            _buffer = sb; 
            this->rdbuf(sb);
        }

    private:
        BasicStreamBuffer<CharT>* _buffer;
};


template <typename CharT>
class BasicOStream : public std::basic_ostream<CharT>
{
    public:
        explicit BasicOStream(BasicStreamBuffer<CharT>* sb = 0);

        ~BasicOStream()
        {}

        std::streamsize writesome(CharT* buffer, std::streamsize n);
    
        BasicStreamBuffer<CharT>* buffer()
        { return _buffer; }

        void setBuffer(BasicStreamBuffer<CharT>* sb)
        { 
            _buffer = sb; 
            this->rdbuf(sb);
        }

    private:
        BasicStreamBuffer<CharT>* _buffer;
};


template <typename CharT>
class BasicIOStream : public std::basic_iostream<CharT>
{
    public:
        explicit BasicIOStream(BasicStreamBuffer<CharT>* sb = 0);

        ~BasicIOStream()
        {}

        //! @brief Peeks bytes in the stream buffer.
        /**
            The number of bytes that can be peeked depends on the current
            stream buffer get area and maybe less than requested,
            similar to istream::readsome().
        */
        std::streamsize peeksome(CharT* buffer, std::streamsize n);
    
        std::streamsize writesome(CharT* buffer, std::streamsize n);
    
        BasicStreamBuffer<CharT>* buffer()
        { return _buffer; }

        void setBuffer(BasicStreamBuffer<CharT>* sb)
        { 
            _buffer = sb; 
            this->rdbuf(sb);
        }

    private:
        BasicStreamBuffer<CharT>* _buffer;
};


template <typename CharT>
inline BasicIStream<CharT>::BasicIStream(BasicStreamBuffer<CharT>* sb)
: std::basic_istream<CharT>(sb)
, _buffer(sb)
{
}


template <typename CharT>
inline std::streamsize BasicIStream<CharT>::peeksome(CharT* buffer, std::streamsize n)
{
    if(_buffer && this->rdbuf() == _buffer)
        return _buffer->speekn(buffer, n);

    if(n > 0)
    {
        buffer[0] = this->peek();
        return 1;
    }

    return 0;
}


template <typename CharT>
inline BasicOStream<CharT>::BasicOStream(BasicStreamBuffer<CharT>* sb)
: std::basic_ostream<CharT>(sb)
, _buffer(sb)
{
}


template <typename CharT>
inline std::streamsize BasicOStream<CharT>::writesome(CharT* buffer, std::streamsize n)
{
    if( ! _buffer || this->rdbuf() != _buffer )
        return 0;

    std::streamsize avail = _buffer->out_avail();
    if(avail == 0)
    {
        return 0;
    }

    n = std::min(avail, n);
    return _buffer->sputn(buffer, n);
}


template <typename CharT>
inline BasicIOStream<CharT>::BasicIOStream(BasicStreamBuffer<CharT>* sb)
: std::basic_iostream<CharT>(sb)
, _buffer(sb)
{
}


template <typename CharT>
inline std::streamsize BasicIOStream<CharT>::peeksome(CharT* buffer, std::streamsize n)
{
    if(_buffer && this->rdbuf() == _buffer)
        return _buffer->speekn(buffer, n);

    if(n > 0)
    {
        buffer[0] = this->peek();
        return 1;
    }

    return 0;
}


template <typename CharT>
inline std::streamsize BasicIOStream<CharT>::writesome(CharT* buffer, std::streamsize n)
{
    if( ! _buffer || this->rdbuf() != _buffer )
        return 0;

    std::streamsize avail = _buffer->out_avail();
    if(avail == 0)
    {
        return 0;
    }

    n = std::min(avail, n);
    return _buffer->sputn(buffer, n);
}

} // !namespace Pt

#endif

