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

#if defined(_MSC_VER) && defined(_WIN32_WCE) 
    // alternatively compile with /FORCE:multiple
    template class PT_EXPORT std::basic_ios<char>;
    template class PT_EXPORT std::basic_istream<char>;
    template class PT_EXPORT std::basic_ostream<char>;
    template class PT_EXPORT std::basic_iostream<char>; 
#endif

#if defined(_MSC_VER)
    template class PT_EXPORT std::basic_ios<Pt::Char>;
    template class PT_EXPORT std::basic_istream<Pt::Char>;
    template class PT_EXPORT std::basic_ostream<Pt::Char>;
    template class PT_EXPORT std::basic_iostream<Pt::Char>; 
#endif

namespace Pt {

template <typename CharT, typename TraitsT = std::char_traits<CharT> >
class BasicIStream : public std::basic_istream<CharT, TraitsT>
{
    public:
        typedef CharT char_type;
        typedef TraitsT traits_type;
        typedef typename TraitsT::int_type int_type;
        typedef typename TraitsT::pos_type pos_type;
        typedef typename TraitsT::off_type off_type;

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


template <typename CharT, typename TraitsT = std::char_traits<CharT> >
class BasicOStream : public std::basic_ostream<CharT, TraitsT>
{
    public:
        typedef CharT char_type;
        typedef TraitsT traits_type;
        typedef typename TraitsT::int_type int_type;
        typedef typename TraitsT::pos_type pos_type;
        typedef typename TraitsT::off_type off_type;

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


template <typename CharT, typename TraitsT = std::char_traits<CharT> >
class BasicIOStream : public std::basic_iostream<CharT, TraitsT>
{
    public:
        typedef CharT char_type;
        typedef TraitsT traits_type;
        typedef typename TraitsT::int_type int_type;
        typedef typename TraitsT::pos_type pos_type;
        typedef typename TraitsT::off_type off_type;

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


template <typename CharT, typename TraitsT>
inline BasicIStream<CharT, TraitsT>::BasicIStream(BasicStreamBuffer<CharT>* sb)
: std::basic_istream<CharT>(sb)
, _buffer(sb)
{
}


template <typename CharT, typename TraitsT>
inline std::streamsize BasicIStream<CharT, TraitsT>::peeksome(CharT* buffer, std::streamsize n)
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


template <typename CharT, typename TraitsT>
inline BasicOStream<CharT, TraitsT>::BasicOStream(BasicStreamBuffer<CharT>* sb)
: std::basic_ostream<CharT>(sb)
, _buffer(sb)
{
}


template <typename CharT, typename TraitsT>
inline std::streamsize BasicOStream<CharT, TraitsT>::writesome(CharT* buffer, std::streamsize n)
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


template <typename CharT, typename TraitsT>
inline BasicIOStream<CharT, TraitsT>::BasicIOStream(BasicStreamBuffer<CharT>* sb)
: std::basic_iostream<CharT>(sb)
, _buffer(sb)
{
}


template <typename CharT, typename TraitsT>
inline std::streamsize BasicIOStream<CharT, TraitsT>::peeksome(CharT* buffer, std::streamsize n)
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


template <typename CharT, typename TraitsT>
inline std::streamsize BasicIOStream<CharT, TraitsT>::writesome(CharT* buffer, std::streamsize n)
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

} // namespace Pt

#endif
