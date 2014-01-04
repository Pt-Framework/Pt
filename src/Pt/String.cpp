/*
 * Copyright (C) 2004-2012 Marc Boris Duerner
 * Copyright (C) 2011 Tommi Maekitalo
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

#include <Pt/String.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <iostream>
#include <algorithm>

#ifndef INLINE
#define INLINE
#endif

#include "Unicode.h"

namespace Pt {

std::ctype_base::mask ctypeMask(const Char& ch)
{
    const uint32_t ucs = ch.value();
    return ctype_data[ ctype_lookup2[ ctype_lookup1[(ucs>>14)&127]+((ucs>>7)&127) ]+(ucs&127) ];
}


Char tolower(const Pt::Char& ch)
{
    const uint32_t ucs = ch.value();
    return ucs + lower_data[lower_lookup2[lower_lookup1[(ucs>>14)&127]+((ucs>>7)&127)]+(ucs&127)];
}


Char toupper(const Char& ch)
{
    const uint32_t ucs = ch.value();
    return ucs + upper_data[upper_lookup2[upper_lookup1[(ucs>>14)&127]+((ucs>>7)&127)]+(ucs&127)];
}

} //namespace Pt

#ifdef PT_WITH_STD_LOCALE
#include "Facets.cpp"
#endif

namespace std {

INLINE
void basic_string<Pt::Char>::resize(size_t n, Pt::Char ch)
{
    size_type size = this->size();
    if(size < n) {
        this->append(n - size, ch);
    }
    else if(n < size) {
        this->erase(n);
    }
}


INLINE
void basic_string<Pt::Char>::reserve(size_t n)
{
    if (capacity() < n)
    {
        // since capacity is always at least shortStringCapacity, we need to use long string
        // to ensure the requested capacity if the current is not enough
        Pt::Char* p = _d.allocate(n + 1);
        size_type l = length();
        const Pt::Char* oldData = privdata_ro();
        traits_type::copy(p, oldData, l);

        if (isShortString())
            markLongString();
        else
            _d.deallocate(longStringData(), longStringCapacity() + 1);

        _d._u._p._begin = p;
        _d._u._p._end = p + l;
        _d._u._p._capacity = p + n;
        *_d._u._p._end = 0;
    }
}


INLINE
void basic_string<Pt::Char>::privreserve(size_t n)
{
    if (capacity() < n)
    {
        size_type nn = 16;
        while (nn < n)
            nn += (nn >> 1);
        reserve(nn);
    }
}


INLINE
void basic_string<Pt::Char>::swap(basic_string& str)
{
    if (isShortString())
    {
        if (str.isShortString())
        {
            for (unsigned nn = 0; nn < _nN; ++nn)
                std::swap(shortStringData()[nn], str.shortStringData()[nn]);
        }
        else
        {
            Ptr p = str._d._u._p;
            for (unsigned nn = 0; nn < _nN; ++nn)
                str.shortStringData()[nn] = shortStringData()[nn];
            markLongString();
            _d._u._p = p;
        }
    }
    else
    {
        if (str.isShortString())
        {
            Ptr p = _d._u._p;
            for (unsigned nn = 0; nn < _nN; ++nn)
                shortStringData()[nn] = str.shortStringData()[nn];
            str.markLongString();
            str._d._u._p = p;
        }
        else
        {
            std::swap(_d._u._p, str._d._u._p);
        }
    }
}



INLINE
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::copy(Pt::Char* a, size_type n, size_type pos) const
{
    if( pos > this->size() ) {
        throw out_of_range("basic_string::copy");
    }

    if(n > this->size() - pos) {
        n = this->size() - pos;
    }

    traits_type::copy(a, privdata_ro() + pos, n);

    return n;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const wchar_t* str)
{
    size_type length = 0;

    while( str[length] )
        ++length;

    assign(str, length);
    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const wchar_t* str, size_type length)
{
    privreserve(length);
    Pt::Char* d = privdata_rw();

    for (unsigned n = 0; n < length; ++n)
    {
        d[n] = str[n];
    }

    setLength(length);
    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const char* str)
{
    size_type length = 0;

    while( str[length] )
        ++length;

    assign(str, length);
    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const char* str, size_type length)
{
    privreserve(length);
    Pt::Char* d = privdata_rw();

    for (unsigned n = 0; n < length; ++n)
    {
        d[n] = Pt::Char(str[n]);
    }

    setLength(length);
    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const Pt::Char* str)
{
    size_type length = traits_type::length(str);
    
    // self-assignment check
    if (str != privdata_ro())
    {
        privreserve(length);
        traits_type::copy(privdata_rw(), str, length);
    }

    setLength(length);
    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const Pt::Char* str, size_type length)
{
    // self-assignment check
    if (str != privdata_ro())
    {
        privreserve(length);
        traits_type::copy(privdata_rw(), str, length);
    }

    setLength(length);
    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::assign(size_type n, Pt::Char ch)
{
    privreserve(n);
    Pt::Char* p = privdata_rw();

    for (size_type nn = 0; nn < n; ++nn)
        p[nn] = ch;

    setLength(n);
    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::append(const Pt::Char* str, size_type n)
{
    size_type l = length();
    privreserve(l + n);
    
    traits_type::copy(privdata_rw() + l, str, n);
    
    setLength(l + n);
    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::append(size_type n, Pt::Char ch)
{
    size_type l = length();
    privreserve(l + n);
    Pt::Char* p = privdata_rw();
    
    for (size_type nn = 0; nn < n; ++nn)
        p[l + nn] = ch;
    
    setLength(l + n);
    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, const Pt::Char* str, size_type n)
{
    size_type l = length();

    if( pos > l )
        throw out_of_range("erase");

    privreserve(l + n);
    Pt::Char* p = privdata_rw() + pos;

    traits_type::move(p + n, p, l - pos);
    traits_type::copy(p, str, n);
    setLength(l + n);

    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, size_type n, Pt::Char ch)
{
    size_type l = length();
    
    if( pos > l )
        throw out_of_range("erase");

    privreserve(l + n);
    Pt::Char* p = privdata_rw() + pos;

    traits_type::move(p + n, p, l - pos);
    for (size_type nn = 0; nn < n; ++nn)
        p[nn] = ch;
    
    setLength(l + n);
    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::erase(size_type pos, size_type n)
{
    size_type l = length();

    if( pos > l )
        throw out_of_range("erase");
    
    Pt::Char* p = privdata_rw();

    if(n > l - pos)
        n = l - pos;

    traits_type::move(p + pos, p + pos + n, l - pos - n);
    
    setLength(l - n);
    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n, const Pt::Char* str, size_type n2)
{
    size_type l = length();

    if( pos > l )
        throw out_of_range("replace");
    
    if(n > l - pos)
        n = l - pos;

    Pt::Char* p;
    if(n != n2)
    {
        privreserve(l - n + n2);
        p = privdata_rw();
        traits_type::move(p + pos + n2, p + pos + n, l - pos - n);
        setLength(l - n + n2);
    }
    else
    {
        p = privdata_rw();
    }

    traits_type::copy(p + pos, str, n2);
    return *this;
}


INLINE
basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n, size_type n2, Pt::Char ch)
{
    size_type l = length();

    if( pos > l )
        throw out_of_range("replace");
    
    if(n > l - pos)
        n = l - pos;

    Pt::Char* p;
    if (n != n2)
    {
        privreserve(l - n + n2);
        p = privdata_rw();
        traits_type::move(p + pos + n2, p + pos + n, l - pos - n);
        setLength(l - n + n2);
    }
    else
    {
        p = privdata_rw();
    }

    for (size_type nn = 0; nn < n2; ++nn)
        p[pos + nn] = ch;

    return *this;
}


INLINE
int basic_string<Pt::Char>::compare(const Pt::Char* str) const
{
    const size_type size = this->size();
    size_type osize = traits_type::length(str);
    size_type n = min(size , osize);

    const int result = traits_type::compare(privdata_ro(), str, n);

    if (result == 0)
        return static_cast<int>(size - osize);

    return result;
}


INLINE
int basic_string<Pt::Char>::compare(const Pt::Char* str, size_type osize) const
{
    const size_type size = this->size();
    size_type n = min(size , osize);

    const int result = traits_type::compare(privdata_ro(), str, n);

    if (result == 0)
        return static_cast<int>(size - osize);

    return result;
}


INLINE
int basic_string<Pt::Char>::compare(const char* str) const
{
    size_type size = length();
    size_type n;
    const Pt::Char* p = privdata_ro();
    
    for (n = 0; n < size && str[n]; ++n)
    {
        Pt::Char ch(str[n]);
        if (p[n] != ch)
            return p[n] > ch ? 1 : -1;
    }

    return n < size ? 1 : str[n] ? -1 : 0;
}


INLINE
int basic_string<Pt::Char>::compare(const char* str, size_type len) const
{
    size_type size = length();
    size_type n;
    const Pt::Char* p = privdata_ro();
    
    for (n = 0; n < size && n < len; ++n)
    {
        Pt::Char ch(str[n]);
        if (p[n] != ch)
            return p[n] > ch ? 1 : -1;
    }

    return n < size ? 1 : n < len ? -1 : 0;
}


INLINE
int basic_string<Pt::Char>::compare(const wchar_t* str) const
{
    size_type size = length();
    size_type n;
    const Pt::Char* p = privdata_ro();
    
    for (n = 0; n < size && str[n]; ++n)
    {
        Pt::Char ch(str[n]);
        if (p[n] != ch)
            return p[n] > ch ? 1 : -1;
    }

    return n < size ? 1 : str[n] ? -1 : 0;
}


INLINE
int basic_string<Pt::Char>::compare(const wchar_t* str, size_type len) const
{
    size_type size = length();
    size_type n;
    const Pt::Char* p = privdata_ro();
    
    for (n = 0; n < size && n < len; ++n)
    {
        Pt::Char ch(str[n]);
        if (p[n] != ch)
            return p[n] > ch ? 1 : -1;
    }

    return n < size ? 1 : n < len ? -1 : 0;
}


INLINE
int basic_string<Pt::Char>::compare(size_type pos, size_type n, const Pt::Char* str, size_type n2) const
{
    size_type l = length();

    if( pos > l )
        throw out_of_range("compare");
    
    if(n > l - pos)
        n = l - pos;

    size_type len = min(n , n2);

    const int result = traits_type::compare(privdata_ro() + pos, str, len);

    if (result == 0)
        return static_cast<int>(n - n2);

    return result;
}


INLINE
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find(const Pt::Char* token, size_type pos, size_type n) const
{
    const size_type size = this->size();

    if( pos >= size )
        return npos;

    const Pt::Char* str = privdata_ro();

    for( ; n <= size - pos; ++pos) 
    {
        if( 0 == traits_type::compare( str + pos, token, n ) ) 
        {
            return pos;
        }
    }

    return npos;
}


INLINE
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find(Pt::Char ch, size_type pos) const
{
    const size_type size = this->size();

    if(pos >= size) 
        return npos;

    const Pt::Char* str = privdata_ro();
    const size_type n = size - pos;

    const Pt::Char* found = traits_type::find(str + pos, n, ch);
    if(found) 
        return found - str;

    return npos;
}


INLINE
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::rfind(const Pt::Char* token, size_type pos, size_type n) const
{
    const size_type size = this->size();

    if (n > size || size == 0)
        return npos;

    pos = min(size - n, pos);

    const Pt::Char* str = privdata_ro();
    do 
    {
        if (traits_type::compare(str + pos, token, n) == 0)
        return pos;
    }
    while (pos-- > 0);

    return npos;
}


INLINE
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::rfind(Pt::Char ch, size_type pos) const
{
    const Pt::Char* str = privdata_ro();
    size_type size = this->size();

    if(size == 0)
        return npos;

    if(--size > pos)
        size = pos;

    for(++size; size-- > 0; ) 
    {
        if( traits_type::eq(str[size], ch) )
            return size;
    }

    return npos;
}


INLINE
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find_first_of(const Pt::Char* s, size_type pos, size_type n) const
{
    const Pt::Char* str = privdata_ro();
    const size_type size = this->size();

    for (; pos < size; ++pos) 
    {
        if( traits_type::find(s, n, str[pos]) )
            return pos;
    }

    return npos;
}


INLINE
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find_last_of(const Pt::Char* s, size_type pos, size_type n) const
{
    size_type size = this->size();
    const Pt::Char* str = privdata_ro();

    if (size == 0 || n == 0)
        return npos;

    if (--size > pos)
        size = pos;

    do 
    {
        if( traits_type::find(s, n, str[size]) )
            return size;
    }
    while (size-- != 0);

    return npos;
}


INLINE
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find_first_not_of(const Pt::Char* tok, size_type pos, size_type n) const
{
    const Pt::Char* str = privdata_ro();
    const size_type size = this->size();

    for (; pos < size; ++pos) 
    {
        if ( ! traits_type::find(tok, n, str[pos]) )
            return pos;
    }

    return npos;
}


INLINE
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find_first_not_of(Pt::Char ch, size_type pos) const
{
    const Pt::Char* str = privdata_ro();
    const size_type size = this->size();

    for (; pos < size; ++pos) 
    {
        if ( ! traits_type::eq(str[pos], ch) ) {
            return pos;
        }
    }

    return npos;
}


INLINE
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find_last_not_of(const Pt::Char* tok, size_type pos, size_type n) const
{
    size_type size = this->size();
    const Pt::Char* str = privdata_ro();

    if (size == 0 || n == 0)
        return npos;

    if (--size > pos)
        size = pos;

    do 
    {
        if ( ! traits_type::find(tok, n, str[size]) )
            return size;
    }
    while(size-- != 0);

    return npos;
}


INLINE
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find_last_not_of(Pt::Char ch, size_type pos) const
{
    size_type size = this->size();
    const Pt::Char* str = privdata_ro();

    if (size == 0)
        return npos;

    if (--size > pos)
        size = pos;

    do 
    {
        if( ! traits_type::eq(str[size], ch) )
            return size;
    } 
    while (size--);

    return npos;
}


INLINE
std::string basic_string<Pt::Char>::narrow(char dfault) const
{
    std::string ret;
    size_type len = this->length();
    const Pt::Char* s = privdata_ro();

    ret.reserve(len);

    for (size_type n = 0; n < len; ++n)
        ret += s[n].narrow(dfault);

    return ret;
}


INLINE
basic_string<Pt::Char> basic_string<Pt::Char>::widen(const char* str)
{
    std::basic_string<Pt::Char> ret;

    size_type len = std::char_traits<char>::length(str);
    ret.privreserve(len);

    for (size_type n = 0; n < len; ++n)
        ret += Pt::Char( str[n] );

    return ret;
}


INLINE
basic_string<Pt::Char> basic_string<Pt::Char>::widen(const std::string& str)
{
    std::basic_string<Pt::Char> ret;

    size_type len = str.length();
    ret.privreserve(len);

    for (size_type n = 0; n < len; ++n)
        ret += Pt::Char( str[n] );

    return ret;
}


INLINE
ostream& operator<< (ostream& out, const basic_string<Pt::Char>& str)
{
    Pt::TextOStream tout(out, new Pt::Utf8Codec());
    tout << str;
    return out;
}

} // namespace std
