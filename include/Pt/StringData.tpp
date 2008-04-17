/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <algorithm>

namespace Pt {

inline StringData::StringData( const allocator_type& a )
: _str(0), _length(0), _capacity(0), _allocator(a), _n(1)
{
    _str = (Pt::Char*) _allocator.allocate(1, _str);
    _str[0] = Pt::Char::null();
    this->updateInternalStringData();
}


inline StringData::StringData(const Pt::Char* s, size_type length, const allocator_type& a)
: _str(0), _length(0), _capacity(0), _allocator(a), _n(1)
{
    _str = (Pt::Char*) _allocator.allocate(length + 1, _str);
    _capacity = length;

    traits_type::copy(_str, s, length);
    _str[length] = Pt::Char::null();
    _length = length;
    this->updateInternalStringData();
}


inline StringData::StringData(const wchar_t* wstr, size_type length, const allocator_type& a)
: _str(0), _length(0), _capacity(0), _allocator(a), _n(1)
{
    _str = (Pt::Char*) _allocator.allocate(length + 1, _str);
    _str[length] = Pt::Char::null();
    _capacity = length;
    _length = length;

    Char* str = _str;
    for(size_t n = 0; n < length; ++n)
    {
        *str = *wstr;
        ++wstr;
        ++str;
    }
    this->updateInternalStringData();
}


inline StringData::StringData(size_type length, Pt::Char ch)
: _str(0), _length(0), _capacity(0), _allocator(), _n(1)
{
    _str = (Pt::Char*) _allocator.allocate(length + 1, _str);
    _capacity = length;

    traits_type::assign(_str, length, ch);
    _str[length] = Pt::Char::null();
    _length = length;
    this->updateInternalStringData();
}


inline StringData::~StringData()
{
    _allocator.deallocate(_str, _capacity + 1 );
}


inline const StringData::atomic_type& StringData::refs() const
{
    return _n;
}


inline StringData::atomic_type& StringData::ref()
{
    ++_n;
    return _n;
}


inline StringData::atomic_type& StringData::unref()
{
    --_n;
    return _n;
}


inline Pt::Char* StringData::str()
{
    return _str;
}


inline Pt::Char* StringData::end()
{
    return _str + _length;
}


inline StringData::size_type StringData::length() const
{
    return _length;
}


inline StringData::size_type StringData::capacity() const
{
    return _capacity;
}


inline void StringData::assign(const Pt::Char* s, size_type length)
{
    this->allocate(length);

    traits_type::copy(_str, s, length);

    _str[length] = Pt::Char::null();
    _length = length;
    this->updateInternalStringData();
}


inline void StringData::assign(size_type length, Pt::Char ch)
{
    this->allocate(length);

    traits_type::assign(_str, length, ch);

    _str[length] = Pt::Char::null();
    _length = length;
    this->updateInternalStringData();
}


inline void StringData::append(size_type n, Pt::Char ch)
{
    this->reserve( _length + n + 1);

    traits_type::assign( _str + _length, n, ch);

    _length += n;
    _str[_length] = Pt::Char::null();
    this->updateInternalStringData();
}


inline void StringData::append(const Pt::Char* str, size_type n)
{
    this->reserve( _length + n + 1);

    traits_type::copy( _str + _length, str, n);

    _length += n;
    _str[_length] = Pt::Char::null();
    this->updateInternalStringData();
}


inline void StringData::insert(size_type pos, const Pt::Char* str, size_type n)
{
    this->reserve( _length + n );

    traits_type::move(_str + pos + n,
                      _str + pos,
                      static_cast<std::char_traits<Pt::Char>::int_type>(_length - pos));

    traits_type::copy(_str + pos, str, n);

    _length += n;
    _str[_length] = Pt::Char::null();
    this->updateInternalStringData();
}


inline void StringData::insert(size_type pos, size_type n, Pt::Char ch)
{
    this->reserve( _length + n );

    traits_type::move(_str + pos + n,
                      _str + pos,
                      static_cast<std::char_traits<Pt::Char>::int_type>(_length - pos));

    traits_type::assign(_str + pos, n, ch);

    _length += n;
    _str[_length] = Pt::Char::null();
    this->updateInternalStringData();
}


// better make size_t pos based
inline StringData::value_type* StringData::erase(value_type* pos, size_type n)
{
    const size_type rpos = pos - _str;

    traits_type::move(pos,
                      pos + n,
                      static_cast<std::char_traits<Pt::Char>::int_type>(_length - n - rpos));

    _length -= n;
    _str[_length] = Pt::Char::null();

    this->updateInternalStringData();
    return _str + rpos;
}


inline void StringData::replace(size_type pos, size_type n, const Pt::Char* str, size_type n2)
{
    this->reserve( _length - n + n2);

    if( n2 > n ) {
        traits_type::move(_str + pos + n2,
                          _str + pos + n,
                          static_cast<std::char_traits<Pt::Char>::int_type>(_length - pos - n));
        //cerr << "moved a: " << (_length - pos - n) << endl;
    }
    else if(n2 < n) {
        traits_type::move(_str + pos + n2,
                          _str + pos + n,
                          static_cast<std::char_traits<Pt::Char>::int_type>(_length - pos - n2));
        //cerr << "moved b: " << (_length - pos - n2) << endl;
    }

    traits_type::copy(_str + pos, str, n2);

    _length += (n2 - n);
    _str[_length] = Pt::Char::null();
    this->updateInternalStringData();
}


inline void StringData::replace(size_type pos, size_type n, size_type n2, Pt::Char ch)
{
    this->reserve( _length - n + n2);

    if( n2 > n ) {
        traits_type::move(_str + pos + n2,
                          _str + pos + n,
                          static_cast<std::char_traits<Pt::Char>::int_type>(_length - pos - n));
        //cerr << "moved a: " << (_length - pos - n) << endl;
    }
    else if(n2 < n) {
        traits_type::move(_str + pos + n2,
                          _str + pos + n,
                          static_cast<std::char_traits<Pt::Char>::int_type>(_length - pos - n2));
        //cerr << "moved b: " << (_length - pos - n2) << endl;
    }

    traits_type::assign(_str+ pos, n2, ch);

    _length += (n2 - n);
    _str[_length] = Pt::Char::null();
    this->updateInternalStringData();
}


inline void StringData::reserve(size_type n)
{
    // only resize if necessary
    if(n <= _capacity)
        return;

    // make room for new string AND NULL TERMINATION CHAR
    Pt::Char* newStr = (Pt::Char*) _allocator.allocate(n + 1, _str);

    // backup old string
    traits_type::copy(newStr, _str, _length);

    // dealloc old string
    if(_str) {
        _allocator.deallocate(_str, _capacity + 1 );
    }

    _str = newStr;
    _capacity = n;
    this->updateInternalStringData();
}


inline void StringData::updateInternalStringData()
{
#ifndef NDEBUG
/*
    Pt::Char* str = _str;

    for (Pt::size_t charCount = 0; ((charCount < _capacity) && (charCount < 100)); ++charCount)
    {
        this->_wStr[charCount] = (wchar_t)(*str);
        ++str;
    }

    _wStr[99] = '0';
*/

    Pt::size_t size = std::min<size_t>( _wstrSize - 1, _length );
    wchar_t* end = std::copy(_str, _str + size, _wStr);
    *end = 0;
#endif
}


inline void StringData::allocate(size_type n)
{
    if(n <= _capacity)
        return;

    if(_str) {
        _allocator.deallocate(_str, _capacity + 1 );
    }

    _str = (Pt::Char*) _allocator.allocate(n + 1, _str);
    _capacity = n;
}

} // namespace Pt

