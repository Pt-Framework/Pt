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

#if __cplusplus >= 201103L
#include <utility>
#endif

namespace Pt {

static std::size_t expandToCapacity(Pt::Char* /*p*/, std::size_t n)
{
    return n;
}


StringBuffer::StringBuffer(std::ios::openmode mode)
: BasicStreamBuffer<Char>()
, _mode(mode)
, _hwm(0)
{
    this->setg(0, 0, 0);
    this->setp(0, 0);

    str(_str);
}


StringBuffer::StringBuffer(const Pt::String& s, std::ios::openmode mode)
: BasicStreamBuffer<Char>()
, _mode(mode)
, _hwm(0)
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
    if( (_mode & std::ios::out) && this->pptr() )
    {
        std::streamsize n = this->pptr() - this->pbase();
        if(n < _hwm)
            n = _hwm;
        return Pt::String(this->pbase(), static_cast<std::size_t>(n));
    }

    return _str;
}


void StringBuffer::str(const Pt::String& str)
{
    _str = str;

    std::streamsize used = static_cast<std::streamsize>( _str.size() );
    _hwm = used;

    if(_mode & std::ios::out)
        _str.resize_and_overwrite(_str.capacity(), expandToCapacity);

    char_type* base = const_cast<char_type*>( _str.data() );
    std::streamsize avail  = static_cast<std::streamsize>( _str.size() );

    if(_mode & std::ios::in)
        this->setg(base, base, base + used);

    if(_mode & std::ios::out)
    {
        this->setp(base, base + avail);
        this->pbump(static_cast<int>(used));
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
    if( this->pptr() > this->egptr() )
        this->setg(this->eback(), this->gptr(), this->pptr());

    return 0;
}


StringBuffer::int_type StringBuffer::pbackfail(int_type ch)
{
    if( this->gptr() == this->eback() )
        return traits_type::eof();

    if( traits_type::eq_int_type(ch, traits_type::eof()) )
    {
        this->gbump(-1);
        return traits_type::not_eof(ch);
    }

    char_type* prev = this->gptr() - 1;
    if( traits_type::eq( traits_type::to_char_type(ch), *prev) )
    {
        this->gbump(-1);
        return ch;
    }

    if( ! (_mode & std::ios_base::out) )
        return traits_type::eof();

    this->gbump(-1);
    *this->gptr() = traits_type::to_char_type(ch);
    return ch;
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
        // pptr() == epptr() == base + _str.size() by invariant
        std::streamsize used = this->pptr() - this->pbase();
        std::streamsize goff = (this->eback() != 0) ? (this->gptr() - this->eback()) : 0;

        _str.push_back( traits_type::to_char_type(ch) );
        _str.resize_and_overwrite(_str.capacity(), expandToCapacity);

        _hwm = used + 1;

        char_type* base = const_cast<char_type*>( _str.data() );
        std::streamsize avail = static_cast<std::streamsize>( _str.size() );

        if(_mode & std::ios::in)
            this->setg(base, base + goff, base + _hwm);

        this->setp(base, base + avail);
        this->pbump(static_cast<int>(_hwm));
    }

    return ch;
}


std::streamsize StringBuffer::xsgetn(char_type* s, std::streamsize n)
{
    if( ! this->gptr() )
        return 0;

    std::streamsize avail = this->egptr() - this->gptr();
    if(avail <= 0)
        return 0;

    std::streamsize count = (n < avail) ? n : avail;
    traits_type::copy(s, this->gptr(), static_cast<std::size_t>(count));
    this->gbump(static_cast<int>(count));
    return count;
}


std::streamsize StringBuffer::xsputn(const char_type* s, std::streamsize n)
{
    if( ! this->pptr() || n <= 0 )
        return 0;

    std::streamsize avail = this->epptr() - this->pptr();
    if(n <= avail)
    {
        traits_type::copy(this->pptr(), s, static_cast<std::size_t>(n));
        this->pbump(static_cast<int>(n));
        return n;
    }

    // Compute HWM after this write; save get position before reallocation
    std::streamsize used = (this->pptr() - this->pbase()) + n;
    std::streamsize goff = (this->eback() != 0) ? (this->gptr() - this->eback()) : 0;

    // Fill available space...
    if(avail > 0)
        traits_type::copy( this->pptr(), s, static_cast<std::size_t>(avail) );

    // ...then append the rest
    _str.append(s + avail, static_cast<std::size_t>(n - avail));
    _str.resize_and_overwrite(_str.capacity(), expandToCapacity);

    _hwm = used;

    char_type* base = const_cast<char_type*>( _str.data() );
    std::streamsize cap = static_cast<std::streamsize>( _str.size() );

    if(_mode & std::ios::in)
        this->setg(base, base + goff, base + _hwm);

    this->setp(base, base + cap);
    this->pbump(static_cast<int>(_hwm));

    return n;
}


StringBuffer::pos_type StringBuffer::seekoff(off_type off, std::ios_base::seekdir way,
                                             std::ios_base::openmode which)
{
    char_type* base = const_cast<char_type*>( _str.data() );

    // End of content: HWM for output mode, string size for input-only
    std::streamsize used = 0;
    if( (_mode & std::ios_base::out) && this->pptr() )
    {
        used = this->pptr() - this->pbase();
        if(used < _hwm)
            used = _hwm;
    }
    else
    {
        used = static_cast<std::streamsize>( _str.size() );
    }

    // Mask out directions not supported by open mode
    if( ! (_mode & std::ios_base::in) )
        which &= ~std::ios_base::in;
    if( ! (_mode & std::ios_base::out) )
        which &= ~std::ios_base::out;

    if(which == 0)
        return pos_type(off_type(-1));

    // seekdir::cur with both in and out is not supported
    if(way == std::ios_base::cur && (which & std::ios_base::in) &&
                                    (which & std::ios_base::out))
        return pos_type(off_type(-1));

    std::streamsize newpos;
    if(way == std::ios_base::beg)
    {
        newpos = static_cast<std::streamsize>(off);
    }
    else if(way == std::ios_base::end)
    {
        newpos = used + static_cast<std::streamsize>(off);
    }
    else // cur
    {
        if(which & std::ios_base::in)
            newpos = (this->gptr() - this->eback()) + static_cast<std::streamsize>(off);
        else
            newpos = (this->pptr() - this->pbase()) + static_cast<std::streamsize>(off);
    }

    if(newpos < 0 || newpos > used)
        return pos_type(off_type(-1));

    if(which & std::ios_base::in)
        this->setg(base, base + newpos, base + used);

    if(which & std::ios_base::out)
    {
        // Re-arm with epptr at capacity; _hwm is preserved (not updated on seek)
        std::streamsize cap = static_cast<std::streamsize>( _str.size() );
        this->setp(base, base + cap);
        this->pbump(static_cast<int>(newpos));
    }

    return pos_type(newpos);
}


StringBuffer::pos_type StringBuffer::seekpos(pos_type sp,
                                             std::ios_base::openmode m)
{
  return seekoff(sp, std::ios_base::beg, m);
}

#if __cplusplus >= 201103L

void StringBuffer::str(Pt::String&& s)
{
    _str = std::move(s);

    std::streamsize used = static_cast<std::streamsize>( _str.size() );
    _hwm = used;

    if(_mode & std::ios::out)
        _str.resize_and_overwrite(_str.capacity(), expandToCapacity);

    char_type* base = const_cast<char_type*>( _str.data() );
    std::streamsize cap  = static_cast<std::streamsize>( _str.size() );

    if(_mode & std::ios::in)
        this->setg(base, base, base + used);

    if(_mode & std::ios::out)
    {
        this->setp(base, base + cap);
        this->pbump(static_cast<int>(used));
    }
}


void StringBuffer::swap(StringBuffer& other)
{
    // Capture actual HWMs (including inline sputc writes past _hwm)
    std::streamsize this_hwm = _hwm;
    if( (_mode & std::ios_base::out) && this->pbase() )
    {
        std::streamsize p = this->pptr() - this->pbase();
        if(p > this_hwm)
            this_hwm = p;
    }

    std::streamsize other_hwm = other._hwm;
    if( (other._mode & std::ios_base::out) && other.pbase() )
    {
        std::streamsize p = other.pptr() - other.pbase();
        if(p > other_hwm)
            other_hwm = p;
    }

    // Save cursor positions
    std::streamsize this_goff = ((_mode & std::ios_base::in) && this->eback()) ?
                                (this->gptr() - this->eback()) : 0;
    std::streamsize this_poff = ((_mode & std::ios_base::out) && this->pbase()) ?
                                (this->pptr() - this->pbase()) : 0;

    std::streamsize other_goff = ((other._mode & std::ios_base::in) && other.eback()) ?
                                 (other.gptr() - other.eback()) : 0;
    std::streamsize other_poff = ((other._mode & std::ios_base::out) && other.pbase()) ?
                                 (other.pptr() - other.pbase()) : 0;

    std::swap(_str, other._str);
    std::swap(_mode, other._mode);
    _hwm = other_hwm;
    other._hwm = this_hwm;

    // Re-init this with other's string and positions
    {
        if(_mode & std::ios_base::out)
            _str.resize_and_overwrite(_str.capacity(), expandToCapacity);

        char_type* base = const_cast<char_type*>(_str.data());
        std::streamsize cap  = static_cast<std::streamsize>(_str.size());

        if(_mode & std::ios_base::in)
            this->setg(base, base + other_goff, base + _hwm);
        else
            this->setg(0, 0, 0);

        if(_mode & std::ios_base::out)
        {
            this->setp(base, base + cap);
            this->pbump(static_cast<int>(other_poff));
        }
        else
            this->setp(0, 0);
    }

    // Re-init other with this's string and positions
    {
        if(other._mode & std::ios_base::out)
            other._str.resize_and_overwrite(other._str.capacity(), expandToCapacity);

        char_type* base = const_cast<char_type*>(other._str.data());
        std::streamsize cap  = static_cast<std::streamsize>(other._str.size());

        if(other._mode & std::ios_base::in)
            other.setg(base, base + this_goff, base + other._hwm);
        else
            other.setg(0, 0, 0);

        if(other._mode & std::ios_base::out)
        {
            other.setp(base, base + cap);
            other.pbump(static_cast<int>(this_poff));
        }
        else
            other.setp(0, 0);
    }
}

#endif // __cplusplus >= 201103L

} // namespace Pt


#if ! defined(PT_WITH_STD_STRINGSTREAM)

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

#endif // PT_WITH_STD_STRINGSTREAM
