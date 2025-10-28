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

#include <Pt/StringStream.h>

namespace Pt {

StringBuffer::StringBuffer(std::ios::openmode mode)
: BasicStreamBuffer<Char>()
, _mode(mode)
{
    this->setg(0, 0, 0);
    this->setp(0, 0);

    str(_str);
}


StringBuffer::StringBuffer(const Pt::String& s, std::ios::openmode mode)
: BasicStreamBuffer<Char>()
, _mode(mode)
, _str()
{
    this->setg(0, 0, 0);
    this->setp(0, 0);

    str(s);
}


StringBuffer::~StringBuffer()
{
}


Pt::String StringBuffer::str() const
{
    return _str;
}


void StringBuffer::str(const Pt::String& str)
{
    _str = str;

    char_type* base = const_cast<char_type*>( _str.data() );
    std::streamsize size = static_cast<std::streamsize>( _str.size() );

    if (_mode & std::ios::in)
    {
        this->setg(base, base, base + size);
    }

    if(_mode & std::ios::out) 
    {
        this->setp(base + size, base + size);
    } 
}


std::streamsize StringBuffer::showfull()
{
    return Base::showfull();
}


std::streamsize StringBuffer::showmanyc()
{
    return Base::showmanyc();
}


int StringBuffer::sync()
{   
    if (this->pptr() > this->egptr())
        this->setg(this->eback(), this->gptr(), this->pptr());
    
    return 0;
}


StringBuffer::int_type StringBuffer::pbackfail(int_type ch)
{
    return Base::pbackfail(ch);
}


StringBuffer::int_type StringBuffer::underflow() 
{
    if( this->pptr() > this->egptr() )
        this->setg(this->eback(), this->gptr(), this->pptr());

    if( this->gptr() < this->egptr() ) 
        return traits_type::to_int_type(*this->gptr());

    return traits_type::eof();
}


StringBuffer::int_type StringBuffer::overflow(int_type ch) 
{
    if( ! this->pptr() )
        return traits_type::eof();

    if( traits_type::eq_int_type(ch, traits_type::eof()) ) 
        return traits_type::not_eof(ch);

    if( this->pptr() == this->epptr() ) 
    {
        _str.push_back( traits_type::to_char_type(ch) );

        char_type* base = const_cast<char_type*>( _str.data() );
        this->setp(base + _str.size(), base + _str.size());
    }

    return ch;
}


std::streamsize StringBuffer::xsgetn(char_type* s, std::streamsize n)
{
    return Base::xsgetn(s, n);
}


std::streamsize StringBuffer::xsputn(const char_type* s, std::streamsize n)
{
    return Base::xsputn(s, n);
}


StringBuffer::pos_type StringBuffer::seekoff(off_type off, std::ios_base::seekdir way, 
                                             std::ios_base::openmode m) 
{
    return Base::seekoff(off, way, m);
}


StringBuffer::pos_type StringBuffer::seekpos(pos_type sp, 
                                             std::ios_base::openmode m) 
{
  return seekoff(sp, std::ios_base::beg, m);
}

} // namespace Pt


#if defined(_MSC_VER) && __cplusplus >= 202002L

namespace Pt {

IStringStream::IStringStream(ios_base::openmode mode)
: basic_istream<Pt::Char>(0)
, _buffer(mode)
{
    init(&_buffer);
}


IStringStream::IStringStream(const Pt::String& str, std::ios_base::openmode mode)
: basic_istream<Pt::Char>(0)
, _buffer(str, mode)
{
    init(&_buffer);
}


OStringStream::OStringStream(ios_base::openmode mode)
: basic_ostream<Pt::Char>(0)
, _buffer(mode)
{
    init(&_buffer);
}


OStringStream::OStringStream(const Pt::String& str, std::ios_base::openmode mode)
: basic_ostream<Pt::Char>(0)
, _buffer(str, mode)
{
    init(&_buffer);
}


StringStream::StringStream(ios_base::openmode mode)
: basic_iostream<Pt::Char>(0)
, _buffer(mode)
{
    init(&_buffer);
}


StringStream::StringStream(const Pt::String& str, std::ios_base::openmode mode)
: basic_iostream<Pt::Char>(0)
, _buffer(str, mode)
{
    init(&_buffer);
}

} // namespace std

#endif

