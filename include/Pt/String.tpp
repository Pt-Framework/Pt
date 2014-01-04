/*
 * Copyright (C) 2004-2007 Marc Boris Duerner
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

namespace std {

inline basic_string<Pt::Char>::basic_string(const allocator_type& a)
: _d(a)
{
}


inline basic_string<Pt::Char>::basic_string(const Pt::Char* str, const allocator_type& a)
: _d(a)
{
    assign(str);
}


inline basic_string<Pt::Char>::basic_string(const wchar_t* str, const allocator_type& a)
: _d(a)
{
    assign(str);
}


inline basic_string<Pt::Char>::basic_string(const wchar_t* str, size_type length, const allocator_type& a)
: _d(a)
{
    assign(str, length);
}


inline basic_string<Pt::Char>::basic_string(const basic_string& str)
: _d(str.get_allocator())
{
    assign(str);
}


inline basic_string<Pt::Char>::basic_string(const char* str, const allocator_type& a)
: _d(a)
{
    assign(str);
}


inline basic_string<Pt::Char>::basic_string(const char* str, size_type length, const allocator_type& a)
: _d(a)
{
    assign(str, length);
}


inline basic_string<Pt::Char>::basic_string(const Pt::Char* str, size_type n, const allocator_type& a)
: _d(a)
{
    assign(str, n);
}


inline basic_string<Pt::Char>::basic_string(size_type n, Pt::Char c, const allocator_type& a)
: _d(a)
{
    assign(n, c);
}


inline basic_string<Pt::Char>::basic_string(const basic_string& str, const allocator_type& a)
: _d(a)
{
    assign(str);
}


inline basic_string<Pt::Char>::basic_string(const basic_string& str, size_type pos, size_type n, const allocator_type& a)
: _d(a)
{
    assign(str, pos, n);
}


inline basic_string<Pt::Char>::basic_string(const Pt::Char* begin, const Pt::Char* end, const allocator_type& a)
: _d(a)
{
    assign(begin, end);
}


template <typename InputIterator>
basic_string<Pt::Char>::basic_string(InputIterator begin, InputIterator end, const allocator_type& a)
: _d(a)
{
    assign(begin, end);
}


inline basic_string<Pt::Char>::~basic_string()
{
    if( ! isShortString() )
    {
        _d.deallocate(longStringData(), longStringCapacity() + 1);
    }
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const basic_string<Pt::Char>& str)
{
    return assign( str.c_str(), str.size() );
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const basic_string<Pt::Char>& str, size_type pos, size_type n)
{
    const Pt::Char* from = &str.at(pos);

    if( str.size() - pos < n )
        n = str.size() - pos; 

    return this->assign( from, n );  
}


template <typename InputIterator>
basic_string<Pt::Char>& basic_string<Pt::Char>::assign(InputIterator begin, InputIterator end)
{
    clear();
    append(begin, end);
    return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::append(const Pt::Char* str)
{
    return append( str, traits_type::length(str) );
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::append(const basic_string& str)
{
    return this->append( str.data(), str.length() );
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::append(const basic_string& str, size_type pos, size_type n)
{
    const Pt::Char* from = &str.at(pos);

    if( str.size() - pos < n )
        n = str.size() - pos; 

    return this->append( from, n );
}


template <typename InputIterator>
basic_string<Pt::Char>& basic_string<Pt::Char>::append(InputIterator begin, InputIterator end)
{
    while (begin != end)
    {
        push_back(*begin++);
    }
    
    return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::append(const Pt::Char* begin, const Pt::Char* end)
{
    return this->append( begin, end-begin );
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, const Pt::Char* str)
{
    return this->insert( pos, str, traits_type::length(str) );
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, const basic_string& str)
{
    return insert(pos, str.privdata_ro(), str.length());
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, const basic_string& str, size_type pos2, size_type n)
{
    if( pos2 > str.size() )
        throw out_of_range("insert");

    const Pt::Char* from = &str[pos2];

    if( str.size() - pos2 < n )
        n = str.size() - pos2; 

    return insert(pos, from, n);
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::insert(iterator p, Pt::Char ch)
{
    return insert(p - begin(), 1, ch);
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::insert(iterator p, size_type n, Pt::Char ch)
{
    return insert(p - begin(), n, ch);
}


inline
basic_string<Pt::Char>::iterator
basic_string<Pt::Char>::erase(iterator it)
{
    size_type pos = it - begin();
    erase(pos, 1);
    return begin() + pos;
}


inline
basic_string<Pt::Char>::iterator
basic_string<Pt::Char>::erase(iterator first, iterator last)
{
    size_type pos = first - begin();
    erase(pos, last - first);
    return begin() + pos;
}


inline
basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n, const Pt::Char* str)
{
    return replace(pos, n, str, traits_type::length(str));
}


inline
basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n, const basic_string& str)
{
    return replace(pos, n, str.privdata_ro(), str.length());
}


inline
basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n,
                                                        const basic_string& str, size_type pos2, size_type n2)
{
    if( pos2 > str.size() )
        throw out_of_range("replace");

    if( str.size() - pos2 < n2 )
        n2 = str.size() - pos2; 

    return replace(pos, n, str.privdata_ro() + pos2, n2);
}


inline
basic_string<Pt::Char>& basic_string<Pt::Char>::replace(iterator i1, iterator i2, const Pt::Char* str)
{
    size_type pos = i1 - begin();
    size_type n = i2 - i1;
    return replace(pos, n, str);
}


inline
basic_string<Pt::Char>& basic_string<Pt::Char>::replace(iterator i1, iterator i2, const Pt::Char* str, size_type n)
{
    size_type pos = i1 - begin();
    size_type n1 = i2 - i1;
    return replace(pos, n1, str, n);
}


inline
basic_string<Pt::Char>& basic_string<Pt::Char>::replace(iterator i1, iterator i2, size_type n, Pt::Char ch)
{
    size_type pos = i1 - begin();
    size_type n1 = i2 - i1;
    return replace(pos, n1, n, ch);
}


inline
basic_string<Pt::Char>& basic_string<Pt::Char>::replace(iterator i1, iterator i2, const basic_string& str)
{
    size_type pos = i1 - begin();
    size_type n = i2 - i1;
    return replace(pos, n, str);
}


inline int basic_string<Pt::Char>::compare(const basic_string& str) const
{
    return compare( str.data(), str.size() );
}


inline int basic_string<Pt::Char>::compare(size_type pos, size_type n, const basic_string& str) const
{
    return compare(pos, n, str, 0, str.length());
}


inline int basic_string<Pt::Char>::compare(size_type pos, size_type n, const basic_string& str, size_type pos2, size_type n2) const
{
    if( pos2 > str.size() )
        throw out_of_range("compare");

    if( str.size() - pos2 < n2 )
        n2 = str.size() - pos2; 

    return compare(pos, n, str.privdata_ro() + pos2, n2);
}


inline int basic_string<Pt::Char>::compare(size_type pos, size_type n, const Pt::Char* str) const
{
    return compare(pos, n, str, traits_type::length(str));
}


inline
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find(const basic_string& str, size_type pos) const
{
    return this->find( str.privdata_ro(), pos, str.size() );
}


inline
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::rfind(const basic_string& str, size_type pos) const
{
    return this->rfind( str.privdata_ro(), pos, str.size() );
}


inline
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find(const Pt::Char* str, size_type pos) const
{
    return this->find( str, pos, traits_type::length(str) );
}


inline
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::rfind(const Pt::Char* str, size_type pos) const
{
    return this->rfind( str, pos, traits_type::length(str) );
}


inline 
basic_string<Pt::Char>& basic_string<Pt::Char>::operator+=(Pt::Char c)
{
    size_type len = 0;
    size_type cap = 0;

    if( isShortString() )
    {
        len = shortStringLength();
        cap = shortStringCapacity();
    }
    else
    {
        len = longStringLength();
        cap = longStringCapacity();
    }
    
    size_type newLen = len + 1;
    if( newLen > cap )
        privreserve(newLen);

    if( isShortString() )
    {
        Pt::Char* p = shortStringData();
        p[len] = c;

        setShortStringLength(newLen);
    }
    else
    {
        Pt::Char* p = longStringData();
        p[len] = c;
    
        _d._u._p._end = _d._u._p._begin + newLen;
        _d._u._p._begin[newLen] = 0;
    }

    return *this;
}


template <typename InIterT>
basic_string<Pt::Char> basic_string<Pt::Char>::fromUtf16(InIterT from, InIterT fromEnd)
{
    std::basic_string<Pt::Char> ret;

    for( ; from != fromEnd; ++from)
    {
        unsigned ch = *from;

        // high surrogate
        if (ch >= 0xD800 && ch <= 0xDBFF) 
        {
            // invalid or missing low surrogate
            if(++from == fromEnd || *from < 0xDC00 || *from > 0xDFFF) 
            {
                ret += Pt::Char(0xFFFD);
                break;
            }

            const unsigned lo = *from;
            ch = ((ch - 0xD800) << 10) + (lo - 0xDC00) + 0x0010000U;
            ret += Pt::Char(ch);
        }
        // not a surrogate
        else if(ch < 0xDC00 || ch > 0xDFFF)
        {
            ret += Pt::Char(ch);
        }
        // not a valid unicode point
        else
        {
            ret += Pt::Char(0xFFFD);
        }
    }

    return ret;
}

template <typename OutIterT>
OutIterT basic_string<Pt::Char>::toUtf16(OutIterT to) const
{
    const_iterator from = this->begin();
    const_iterator fromEnd = this->end();

    for( ; from != fromEnd; ++from)
    {
        const int ch = *from;

        if( ch < 0xD800 ||
           (ch > 0xDFFF && ch <= 0xFFFF) )
        {
            *to++ = *from;
        }
        else if(ch > 0xFFFF && ch <= 0x0010FFFF)
        {
            const int n = (ch - 0x0010000UL);
            *to++ = ((n >> 10) + 0xD800);
            *to++ = ((n & 0x3FFU) + 0xDC00);
        }
        else
        {
            *to++ = 0xFFFD;
        }
    }

    return to;
}

}
