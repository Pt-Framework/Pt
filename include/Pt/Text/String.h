/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Duerner                         *
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

#ifndef Pt_Text_String_h
#define Pt_Text_String_h

#include <Pt/Text/Char.h>

#include <string>
#include <iterator>
#include <iostream>
#include <cassert>
#include <stdexcept>


namespace Pt {

typedef unsigned int AtomicInt;


class PT_EXPORT StringData {
	public:
		typedef size_t size_type;
		typedef Pt::Text::Char value_type;
		typedef std::char_traits<Pt::Text::Char> traits_type;
		typedef std::allocator<Pt::Char> allocator_type;

	public:
		StringData( const allocator_type& a = allocator_type() );

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

	private:
		// the allocated size is the capacity plus 1
		Pt::Char* _str;
		size_type _length;
		size_type _capacity;
		allocator_type _allocator;
		AtomicInt _n;
};

} // namespace Pt


namespace std {

template <>
class PT_EXPORT basic_string< Pt::Text::Char > {
	public:
		typedef Pt::Text::Char value_type;
		typedef size_t size_type;
		typedef char_traits< Pt::Text::Char > traits_type;
		typedef std::allocator<Pt::Text::Char> allocator_type;
		typedef allocator_type::difference_type difference_type;
		typedef allocator_type::reference reference;
		typedef allocator_type::const_reference const_reference;
		typedef allocator_type::pointer pointer;
		typedef allocator_type::const_pointer const_pointer;
		typedef value_type* iterator;
		typedef const value_type* const_iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef const std::reverse_iterator<const_iterator> const_reverse_iterator;

		static const size_type npos = static_cast<size_type>(-1);

	public:
		explicit basic_string( const allocator_type& a = allocator_type() );

		basic_string(const Pt::Char* str, const allocator_type& a = allocator_type());

		// untested
		basic_string(const wchar_t* str, const allocator_type& a = allocator_type());

		// untested
		basic_string(const wchar_t* str, size_type n, const allocator_type& a = allocator_type());

		basic_string(const Pt::Char* str, size_type n, const allocator_type& a = allocator_type());

		basic_string(size_type n, Pt::Char c);

		basic_string(const basic_string& str);

		basic_string(const basic_string& str, size_type pos);

		basic_string(const basic_string& str, size_type pos, size_type n);

		basic_string(const basic_string& str, size_type pos, size_type n, const allocator_type& a);

		~basic_string();

	public: 
		iterator begin();

		iterator end();

		const_iterator begin() const;

		const_iterator end() const;

		reverse_iterator rbegin()
		{ return reverse_iterator( this->end() ); }

		reverse_iterator rend()
		{ return reverse_iterator( this->begin() ); }

		const_reverse_iterator rbegin() const 
		{ return const_reverse_iterator( this->end() ); }

		const_reverse_iterator rend()   const 
		{ return const_reverse_iterator( this->begin() ); }

		reference operator[](size_type n)
		{
			this->detach( _data->length() );
			_data->setBusy();
			return *(_data->str() + n);
		}

		const_reference operator[](size_type n) const
		{ return *(_data->str() + n); }

		reference at(size_type n)
		{
			if( n > this->size() ) {
				throw std::out_of_range("The given at-value is out of range");
			}
			this->detach( _data->length() );
			_data->setBusy();
			return *(_data->str() + n);
		}

		const_reference at(size_type n) const
		{
			if( n > this->size() ) {
				throw std::out_of_range("The given at-value is out of range");
			}
			return *(_data->str() + n);
		}

	public: 
		void push_back(Pt::Char ch)
		{ this->append(1, ch); }

		// untested
		void resize( size_t n, Pt::Char ch = value_type() );

		// untested
		void reserve(size_t n = 0);

		// untested
		void swap(basic_string& str);

		allocator_type get_allocator() const
		{ return _data->get_allocator(); }

		// untested
		size_type copy(Pt::Char* a, size_type n, size_type pos = 0) const;

		basic_string substr(size_type pos, size_type n) const
		{ return basic_string(*this, pos, n); }

		basic_string substr(size_type pos = 0) const
		{ return basic_string(*this, pos); }

	public:
		size_type length() const;

		size_type size() const;

		bool empty() const;

		size_type max_size() const;

		size_type capacity() const;

		const Pt::Char* data() const
		{ return this->c_str(); }

		const Pt::Char* c_str() const;

		basic_string& assign(const basic_string& str);

		basic_string& assign(const basic_string& str, size_type pos, size_type n);

		basic_string& assign(const Pt::Char* str);

		basic_string& assign(const Pt::Char* str, size_type length);

		basic_string& assign(size_type n, Pt::Char c);

		basic_string& append(const Pt::Char* str);

		basic_string& append(const Pt::Char* str, size_type n);

		basic_string& append(size_type n, Pt::Char ch);

		basic_string& append(const basic_string& str);

		basic_string& append(const basic_string& str, size_type pos, size_type n);

		basic_string& insert(size_type pos, const Pt::Char* str);

		basic_string& insert(size_type pos, const Pt::Char* str, size_type n);

		basic_string& insert(size_type pos, size_type n, Pt::Char ch);

		basic_string& insert(size_type pos, const basic_string& str);

		basic_string& insert(size_type pos, const basic_string& str, size_type pos2, size_type n);

		basic_string& insert(iterator p, Pt::Char ch);

		basic_string& insert(iterator p, size_type n, Pt::Char ch);

		// unimplemented
		//template <typename InputIterator>
		//basic_string& insert(iterator p, InputIterator first, InputIterator last);

		// check MT
		void clear();

		basic_string& erase(size_type pos = 0, size_type n = npos);

		iterator erase(iterator pos);

		// untested
		iterator erase(iterator first, iterator last);

		// untested
		basic_string& replace(size_type pos, size_type n, const Pt::Char* str);

		// untested
		basic_string& replace(size_type pos, size_type n, const Pt::Char* str, size_type n2);

		// untested
		basic_string& replace(size_type pos, size_type n, size_type n2, Pt::Char ch);

		// untested
		basic_string& replace(size_type pos, size_type n, const basic_string& str);

		// untested
		basic_string& replace(size_type pos, size_type n, const basic_string& str, size_type pos2, size_type n2);

		// untested
		basic_string& replace(iterator i1, iterator i2, const Pt::Char* str);
		// untested
		basic_string& replace(iterator i1, iterator i2, const Pt::Char* str, size_type n);

		// untested
		basic_string& replace(iterator i1, iterator i2, size_type n, Pt::Char ch);

		// untested
		basic_string& replace(iterator i1, iterator i2, const basic_string& str);

		//template<InputIterator>
		//basic_string& replace(iterator i1, iterator i2, InputIterator j1, InputIterator j2);

		int compare(const basic_string& str) const;

		int compare(const Pt::Char* str) const;

		int compare(size_type pos, size_type n, const basic_string& str) const;

		int compare(size_type pos, size_type n, const basic_string& str, size_type pos2, size_type n2) const;

		int compare(size_type pos, size_type n, const Pt::Char* str) const;

		int compare(size_type pos, size_type n, const Pt::Char* str, size_type n2) const;

		size_type find(const basic_string& str, size_type pos = 0) const;

		size_type find(const Pt::Char* str, size_type pos, size_type n) const;

		size_type find(const Pt::Char* str, size_type pos = 0) const;

		size_type find(Pt::Char ch, size_type pos = 0) const;

		// untested
		size_type rfind(const basic_string& str, size_type pos = npos) const;

		// untested
		size_type rfind(const Pt::Char* str, size_type pos, size_type n) const;

		// untested
		size_type rfind(const Pt::Char* str, size_type pos = npos) const;

		// untested
		size_type rfind(Pt::Char ch, size_type pos = npos) const;

		// untested
		size_type find_first_of(const basic_string& str, size_type pos = 0) const
		{ return this->find_first_of( str.data(), pos, str.size() ); }

		// untested
		size_type find_first_of(const Pt::Char* s, size_type pos, size_type n) const;

		// untested
		size_type find_first_of(const Pt::Char* str, size_type pos = 0) const
		{ return this->find_first_of( str, pos, traits_type::length(str) ); }

		// untested
		size_type find_first_of(const Pt::Char ch, size_type pos = 0) const
		{ return this->find(ch, pos); }

		// untested
		size_type find_last_of(const basic_string& str, size_type pos = npos) const
		{ return this->find_last_of( str.data(), pos, str.size() ); }

		// untested
		size_type find_last_of(const Pt::Char* s, size_type pos, size_type n) const;

		// untested
		size_type find_last_of(const Pt::Char* str, size_type pos = npos) const
		{ return this->find_last_of( str, pos, traits_type::length(str) ); }

		// untested
		size_type find_last_of(const Pt::Char ch, size_type pos = npos) const
		{ return this->rfind(ch, pos); }

		// untested
		size_type find_first_not_of(const basic_string& str, size_type pos = 0) const
		{ return this->find_first_not_of( str.data(), pos, str.size() ); }

		// untested
		size_type find_first_not_of(const Pt::Char* s, size_type pos, size_type n) const;

		// untested
		size_type find_first_not_of(const Pt::Char* str, size_type pos = 0) const
		{
			// requires_string(str);
			return this->find_first_not_of( str, pos, traits_type::length(str) );
		}

		// untested
		size_type find_first_not_of(const Pt::Char ch, size_type pos = 0) const;

		// untested
		size_type find_last_not_of(const basic_string& str, size_type pos = npos) const
		{ return this->find_last_not_of( str.data(), pos, str.size() ); }

		// untested
		size_type find_last_not_of(const Pt::Char* tok, size_type pos, size_type n) const;

		// untested
		size_type find_last_not_of(const Pt::Char* str, size_type pos = npos) const
		{
			//requires_string(s);
			return this->find_last_not_of( str, pos, traits_type::length(str) );
		}

		// untested
		size_type find_last_not_of(Pt::Char ch, size_type pos = npos) const;

	public:
		void detach(size_type reserveSize);

		std::string narrow() const;

	public:
		basic_string& operator=(const basic_string& str)
		{return this->assign(str);}

		basic_string& operator=(const Pt::Char* str)
		{return this->assign(str);}

		basic_string& operator=(Pt::Char c)
		{return this->assign(1, c);}

		basic_string& operator+=(const basic_string& str)
		{ return this->append(str); }

		basic_string& operator+=(const Pt::Char* str)
		{ return this->append(str); }

		basic_string& operator+=(Pt::Text::Char c)
		{ return this->append(1, c); }

	public:
		void invariant();

	private:
		Pt::StringData* _data;
	};

	inline basic_string<Pt::Text::Char>& operator+(const basic_string<Pt::Text::Char>& a, const basic_string<Pt::Text::Char>& b);

	inline basic_string<Pt::Text::Char>& operator+(const basic_string<Pt::Text::Char>& a, const Pt::Text::Char* b);

	inline basic_string<Pt::Text::Char>& operator+(const Pt::Text::Char* a, const basic_string<Pt::Text::Char>& b);

	inline basic_string<Pt::Text::Char>& operator+(const basic_string<Pt::Text::Char>& a, Pt::Text::Char b);

	inline basic_string<Pt::Text::Char>& operator+(Pt::Text::Char a, const basic_string<Pt::Text::Char>& b);

	inline bool operator==(const basic_string<Pt::Text::Char>& a, const basic_string<Pt::Text::Char>& b)
	{ return a.compare(b) == 0; }

	inline bool operator==(const Pt::Char* a, const basic_string<Pt::Text::Char>& b)
	{ return b.compare(a) == 0; }

	inline bool operator==(const basic_string<Pt::Text::Char>& a, const Pt::Char* b)
	{ return a.compare(b) == 0; }

} // namespace std


namespace Pt {

	namespace Text {

		typedef std::basic_string<Pt::Char> String;

	} //namespace Text

	using Pt::Text::String;
} // namespace Pt

#endif
