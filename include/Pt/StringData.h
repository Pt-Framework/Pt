/***************************************************************************
 *   Copyright (C) 2004-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
 
#ifndef PT_STRINGDATA_H
#define PT_STRINGDATA_H

#include <Pt/Api.h>
#include <Pt/Char.h>

#include <string>
#include <iterator>


namespace Pt {

typedef unsigned int AtomicInt;


class StringData {
    public:
        typedef size_t size_type;
        typedef Pt::Char value_type;
        typedef std::char_traits<Pt::Char> traits_type;
        typedef std::allocator<Pt::Char> allocator_type;

    public:
        StringData(const allocator_type& a = allocator_type());

        StringData(const Pt::Char* s, size_type length, const allocator_type& a = allocator_type());

        StringData(const wchar_t* wstr, size_type length, const allocator_type& a);

        StringData(size_type length, Pt::Char ch);

        ~StringData();

        allocator_type get_allocator() const
        { return _allocator; }

        const AtomicInt& refs() const;

        AtomicInt& ref();

        AtomicInt& unref();

        void setInitial()
        { _n = AtomicInt(1); }

        bool busy() const
        { return _n == AtomicInt(-1); }

        void setBusy()
        { _n = AtomicInt(-1); }

        bool shared() const
        { return (_n > 1) && ( _n != AtomicInt(-1) ); }

        Pt::Char* str();

        Pt::Char* end();

        size_type length() const;

        size_type capacity() const;

        void assign(const Pt::Char* s, size_type length);

        void assign(size_type length, Pt::Char ch);

        void append(size_type n, Pt::Char ch);

        void append(const Pt::Char* str, size_type n);

        void insert(size_type pos, const Pt::Char* str, size_type n);

        void insert(size_type pos, size_type n, Pt::Char ch);

        value_type* erase(value_type* pos, size_type n);

        void replace(size_type pos, size_type n, const Pt::Char* str, size_type n2);

        void replace(size_type pos, size_type n, size_type n2, Pt::Char ch);

        void reserve(size_type length);

    protected:
        void allocate(size_type length);
        void updateInternalStringData();
        

    public:
        // the allocated size is the capacity plus 1
        Pt::Char* _str;
        size_type _length;
        size_type _capacity;
        allocator_type _allocator;
        AtomicInt _n;

#ifndef NDEBUG
        wchar_t _wStr[100];
#endif
};

} // namespace Pt


// Include the implementation header
#include <Pt/StringData.tpp>

#endif
