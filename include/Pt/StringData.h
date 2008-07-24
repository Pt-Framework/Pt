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
#include <algorithm>

namespace std {

    template<>
    class allocator<Pt::Char>
    {
        public:
            typedef size_t          size_type;
            typedef ptrdiff_t       difference_type;
            typedef Pt::Char*       pointer;
            typedef const Pt::Char* const_pointer;
            typedef Pt::Char&       reference;
            typedef const Pt::Char& const_reference;
            typedef Pt::Char        value_type;

            template<typename U>
            struct rebind
            { typedef allocator<U> other; };

            allocator()
            { }

            template<typename U>
            allocator(const allocator<U>&)
            { }

            ~allocator()
            { }

            pointer address(reference x) const
            { return &x; }

            const_pointer address(const_reference x) const
            { return &x; }

            pointer allocate(size_type n, const void* = 0)
            {
                return static_cast<value_type*>(::operator new(n * sizeof(value_type)));
            }

            void deallocate(pointer p, size_type)
            { ::operator delete(p); }

            size_type max_size() const throw()
            { return size_t(-1) / sizeof(value_type); }

            void construct(pointer p, const value_type& val)
            { ::new(p) value_type(val); }

            void destroy(pointer p)
            { p->~value_type(); }
    };

} // namespace std

namespace Pt {

/** @internal
*/
class StringData {
    public:
        typedef unsigned int atomic_type;

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

        const atomic_type& refs() const;

        atomic_type& ref();

        atomic_type& unref();

        void setInitial()
        { _n = atomic_type(1); }

        /** @brief Check if in busy state

            Busy mode means that a modifying action that reuires more than one
            calls by the caller was started. This is the case if a non-const
            iterator was aquired or the index operator is used. Busy mode will
            be ended if the client calls a modifying action which would also
            invalidate an iterator even if the class was not shared.
        */
        bool busy() const
        { return _n == atomic_type(-1); }

        /** @brief Enter busy state

            For example a mutating iterator is aquired, so we can not allow any
            data sharing until the caller calls a modifying method that would
            normally invalidate any iterator as well.
        */
        void setBusy()
        { _n = atomic_type(-1); }

        bool shared() const
        { return (_n > 1) && ( _n != atomic_type(-1) ); }

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
        atomic_type _n;

#ifndef NDEBUG
        static const Pt::size_t _wstrSize = 100;
        wchar_t _wStr[_wstrSize];
#endif
};

static StringData EmptyStringData;

} // namespace Pt


// Include the implementation header
#include <Pt/StringData.tpp>

#endif
