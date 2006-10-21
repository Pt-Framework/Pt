/***************************************************************************
 *   Copyright (C) 2005-2006 by Marc Boris Duerner                              *
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
#include "Pt/Text/String.h"
using namespace Pt;

#include <iostream>
using namespace std;


namespace Pt {

StringData::StringData( const allocator_type& a )
: _str(0), _length(0), _capacity(0), _allocator(a), _n(1)
{
	_str = _allocator.allocate(1, _str);
	_str[0] = Pt::Char::Null;
}


StringData::StringData(const Pt::Char* s, size_type length, const allocator_type& a)
: _str(0), _length(0), _capacity(0), _allocator(a), _n(1)
{
	_str = _allocator.allocate(length + 1, _str);
	_capacity = length;

	traits_type::copy(_str, s, length);
	_str[length] = Pt::Char::Null;
	_length = length;
}


StringData::StringData(const wchar_t* wstr, size_type length, const allocator_type& a)
: _str(0), _length(0), _capacity(0), _allocator(a), _n(1)
{
	_str = _allocator.allocate(length + 1, _str);
	_str[length] = Pt::Char::Null;
	_capacity = length;
	_length = length;

	Char* str = _str;
	for(size_t n = 0; n < length; ++n)
	{
		*str = *wstr;
		++wstr;
		++str;
	}
}


StringData::StringData(size_type length, Pt::Char ch)
: _str(0), _length(0), _capacity(0), _allocator(), _n(1)
{
	_str = _allocator.allocate(length + 1, _str);
	_capacity = length;

	traits_type::assign(_str, length, ch);
	_str[length] = Pt::Char::Null;
	_length = length;
}


StringData::~StringData()
{
	_allocator.deallocate(_str, _capacity + 1 );
}


const AtomicInt& StringData::refs() const
{
	return _n;
}


AtomicInt& StringData::ref()
{
	++_n;
	return _n;
}


AtomicInt& StringData::unref()
{
	--_n;
	return _n;
}


Pt::Char* StringData::str()
{
	return _str;
}


Pt::Char* StringData::end()
{
	return _str + _length;
}


StringData::size_type StringData::length() const
{
	return _length;
}


StringData::size_type StringData::capacity() const
{
	return _capacity;
}


void StringData::assign(const Pt::Char* s, size_type length)
{
	this->allocate(length);

	traits_type::copy(_str, s, length);

	_str[length] = Pt::Char::Null;
	_length = length;
}


void StringData::assign(size_type length, Pt::Char ch)
{
	this->allocate(length);

	traits_type::assign(_str, length, ch);

	_str[length] = Pt::Char::Null;
	_length = length;
}


void StringData::append(size_type n, Pt::Char ch)
{
	this->reserve( _length + n + 1);

	traits_type::assign( _str + _length, n, ch);

	_length += n;
	_str[_length] = Pt::Char::Null;
}


void StringData::append(const Pt::Char* str, size_type n)
{
	this->reserve( _length + n + 1);

	traits_type::copy( _str + _length, str, n);

	_length += n;
	_str[_length] = Pt::Char::Null;
}


void StringData::insert(size_type pos, const Pt::Char* str, size_type n)
{
	this->reserve( _length + n );

	traits_type::move(_str + pos + n,
	                  _str + pos,
	                  _length - pos);

	traits_type::copy(_str + pos, str, n);

	_length += n;
	_str[_length] = Pt::Char::Null;
}


void StringData::insert(size_type pos, size_type n, Pt::Char ch)
{
	this->reserve( _length + n );

	traits_type::move(_str + pos + n,
	                  _str + pos,
	                  _length - pos);

	traits_type::assign(_str + pos, n, ch);

	_length += n;
	_str[_length] = Pt::Char::Null;
}


// better make size_t pos based
StringData::value_type* StringData::erase(value_type* pos, size_type n)
{
	const size_type rpos = pos - _str;

	traits_type::move(pos,
	                  pos + n,
	                  _length - n - rpos);

	_length -= n;
	_str[_length] = Pt::Char::Null;

	return _str + rpos;
}


void StringData::replace(size_type pos, size_type n, const Pt::Char* str, size_type n2)
{
	this->reserve( _length - n + n2);

	if( n2 > n ) {
		traits_type::move(_str + pos + n2,
		                  _str + pos + n,
		                  _length - pos - n);
		//cerr << "moved a: " << (_length - pos - n) << endl;
	}
	else if(n2 < n) {
		traits_type::move(_str + pos + n2,
		                  _str + pos + n,
		                  _length - pos - n2);
		//cerr << "moved b: " << (_length - pos - n2) << endl;
	}

	traits_type::copy(_str + pos, str, n2);

	_length += (n2 - n);
	_str[_length] = Pt::Char::Null;
}


void StringData::replace(size_type pos, size_type n, size_type n2, Pt::Char ch)
{
	this->reserve( _length - n + n2);

	if( n2 > n ) {
		traits_type::move(_str + pos + n2,
		                  _str + pos + n,
		                  _length - pos - n);
		//cerr << "moved a: " << (_length - pos - n) << endl;
	}
	else if(n2 < n) {
		traits_type::move(_str + pos + n2,
		                  _str + pos + n,
		                  _length - pos - n2);
		//cerr << "moved b: " << (_length - pos - n2) << endl;
	}

	traits_type::assign(_str+ pos, n2, ch);

	_length += (n2 - n);
	_str[_length] = Pt::Char::Null;
}


void StringData::reserve(size_type n)
{
	// only resize if necessary
	if(n < _capacity)
		return;

	// make room for new string AND NULL TERMINATION CHAR
	Pt::Char* newStr = _allocator.allocate(n + 1, _str);

	// backup old string
	traits_type::copy(newStr, _str, _length);

	// dealloc old string
	if(_str) {
		_allocator.deallocate(_str, _capacity + 1 );
	}

	_str = newStr;
	_capacity = n;
}


void StringData::allocate(size_type n)
{
	if(n < _capacity)
		return;

	if(_str) {
		_allocator.deallocate(_str, _capacity + 1 );
	}

	_str = _allocator.allocate(n + 1, _str);
	_capacity = n;
}

} // namespace Pt




namespace std {

basic_string<Pt::Char>::basic_string(const allocator_type& a)
: _data( new StringData(a) )
{
	this->invariant();
}


basic_string<Pt::Char>::basic_string(const Pt::Char* str, const allocator_type& a)
: _data( new StringData( str, char_traits<Pt::Char>::length(str), a ) )
{
	this->invariant();
}


basic_string<Pt::Char>::basic_string(const wchar_t* str, const allocator_type& a)
: _data( new StringData( str, char_traits<wchar_t>::length(str), a ) )
{
	this->invariant();
}


basic_string<Pt::Char>::basic_string(const wchar_t* wstr, size_type length, const allocator_type& a)
: _data( new StringData( wstr, length, a ) )
{
	this->invariant();
}


basic_string<Pt::Char>::basic_string(const Pt::Char* str, size_type n, const allocator_type& a)
: _data( new StringData( str, n, a ) )
{
	this->invariant();
}


basic_string<Pt::Char>::basic_string(size_type n, Pt::Char c)
: _data( new StringData( n, c ) )
{
}


basic_string<Pt::Char>::basic_string(const basic_string& str)
: _data(0)
{
	if( str._data->busy() == false ) {
		_data = str._data;
		_data->ref();
	}
	else {
		_data = new StringData( str._data->str(), str._data->length() );
	}

	this->invariant();
}


basic_string<Pt::Char>::basic_string(const basic_string& str, size_type pos)
: _data( new StringData( str._data->str() + pos, str._data->length() - pos ) )
{
	this->invariant();
}


basic_string<Pt::Char>::basic_string(const basic_string& str, size_type pos, size_type n)
: _data( new StringData( str._data->str() + pos, n ) )
{
	this->invariant();
}


basic_string<Pt::Char>::basic_string(const basic_string& str, size_type pos, size_type n, const allocator_type& a)
: _data( new StringData( str._data->str() + pos, n, a ) )
{
	this->invariant();
}


basic_string<Pt::Char>::~basic_string()
{
	if( _data->busy() || _data->unref() < 1 ) {
		delete _data;
		_data = 0;
	}

	if(_data)
		this->invariant();
}


basic_string<Pt::Char>::iterator basic_string<Pt::Char>::begin()
{
	this->detach( _data->length() );
	_data->setBusy();
	return _data->str();
}


basic_string<Pt::Char>::iterator basic_string<Pt::Char>::end()
{
	this->detach( _data->length() );
	_data->setBusy();
	return _data->end();
}


basic_string<Pt::Char>::const_iterator basic_string<Pt::Char>::begin() const
{
	return _data->str();
}


basic_string<Pt::Char>::const_iterator basic_string<Pt::Char>::end() const
{
	return _data->end();
}


void basic_string<Pt::Char>::resize(size_t n, Pt::Char ch)
{
	size_type size = this->size();
	if(size < n) {
		this->append(n - size, ch);
	}
	else if(n < size) {
		this->erase(0, n);
	}

	// do nothing if n == size
}


void basic_string<Pt::Char>::reserve(size_t n)
{
	if( n == this->capacity() && _data->busy() )
		return;

	const size_type size = this->size();

	if(n > size)
		this->detach(n);
	else if(size > n)
		this->detach(size);

	// mutation ends busy mode
	_data->setInitial();
}


void basic_string<Pt::Char>::swap(basic_string& str)
{
	basic_string tmp1(this->c_str(), this->length(), this->get_allocator());
	basic_string tmp2(str.c_str(), str.length(), str.get_allocator());
	str = tmp1;
	*this = tmp2;
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::copy(Pt::Char* a, size_type n, size_type pos) const
{
	if( pos > this->size() ) {
		// throw out_of_range("basic_string::copy");
	}

	if(n > this->size() - pos) {
		n = this->size() - pos;
	}

	traits_type::copy(a, _data->str() + pos, n);
	return n;
}


basic_string<Pt::Char>::size_type basic_string<Pt::Char>::length() const
{
	return _data->length();
}


basic_string<Pt::Char>::size_type basic_string<Pt::Char>::size() const
{
	return _data->length();
}


bool basic_string<Pt::Char>::empty() const
{
	return _data->length() == 0;
}


basic_string<Pt::Char>::size_type basic_string<Pt::Char>::max_size() const
{
	return ( size_type(-1) / sizeof(Pt::Char) ) - 1;
}


basic_string<Pt::Char>::size_type basic_string<Pt::Char>::capacity() const
{
	return _data->capacity();
}


const Pt::Char* basic_string<Pt::Char>::c_str() const
{
	return _data->str();
}


basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const basic_string<Pt::Char>& str)
{
	if( str._data->busy() == false ) {
		_data = str._data;
		_data->ref();
	}
	else {
		_data->assign( str._data->str(), str._data->length() );
		// end busy mode
		_data->setInitial();
	}

	this->invariant();

	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const basic_string<Pt::Char>& str, size_type pos, size_type n)
{
	this->assign( str._data->str() + pos, n );

	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const Pt::Char* str)
{
	const size_type len = char_traits<Pt::Char>::length(str);
	this->assign(str, len);

	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const Pt::Char* str, size_type length)
{
	// copy if shared and not busy
	if( _data->shared() ) {
		StringData* newBuffer = new StringData( str, length );
		_data->unref();
		_data = newBuffer;
	}
	else { // unshared or busy, dont copy
		_data->assign(str, length);
		// mutate ends busy mode
		_data->setInitial();
	}

	this->invariant();

	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::assign(size_type n, Pt::Char ch)
{
	// copy if shared and not busy
	if( _data->shared() ) {
		StringData* newBuffer = new StringData( n, ch );
		_data->unref();
		_data = newBuffer;
	}
	else { // unshared or busy, dont copy
		_data->assign(n, ch);
		// mutate ends busy mode
		_data->setInitial();
	}

	this->invariant();

	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::append(const Pt::Char* str)
{
	return this->append( str, traits_type::length(str) );
}


/*
void basic_string<Pt::Char>::detach(size_type reserveSize)
{
	// shared, not busy - make copy
	if( _data->shared() ) {
		StringData* newBuffer = new StringData( reserveSize, _data->str(), _data->length() );
		_data->unref();
		_data = newBuffer;
	}
}
*/

basic_string<Pt::Char>& basic_string<Pt::Char>::append(const Pt::Char* str, size_type n)
{
	// shared, not busy - work on copy
	if( _data->shared() ) {
		StringData* newBuffer = new StringData( _data->str(), _data->length() );
		_data->unref();
		_data = newBuffer; // refs == 1
	}

	// mutation ends busy mode
	_data->setInitial();

	_data->append(str, n);
	this->invariant();
	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::append(size_type n, Pt::Char ch)
{
	// shared, not busy - work on copy
	if( _data->shared() ) {
		StringData* newBuffer = new StringData();
		newBuffer->reserve(_data->length() + n);
		newBuffer->assign( _data->str(), _data->length() );

		_data->unref();
		_data = newBuffer; // refs == 1
	}

	// mutate ends busy mode
	_data->setInitial();
	_data->append(n, ch);

	this->invariant();
	return *this;
}



basic_string<Pt::Char>& basic_string<Pt::Char>::append(const basic_string& str)
{
	return this->append( str._data->str(), str._data->length() );
}


basic_string<Pt::Char>& basic_string<Pt::Char>::append(const basic_string& str, size_type pos, size_type n)
{
	return this->append( str._data->str() + pos, n );
}


basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, const Pt::Char* str)
{
	return this->insert( pos, str, traits_type::length(str) );
}


basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, const Pt::Char* str, size_type n)
{
	// detach to new size
	this->detach( _data->length() + n );

	// mutation ends busy mode
	_data->setInitial();

	_data->insert(pos, str, n);

	this->invariant();
	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, size_type n, Pt::Char ch)
{
	this->detach( _data->length() + n );

	_data->setInitial();

	_data->insert(pos, n, ch);

	this->invariant();
	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, const basic_string& str)
{
	this->detach( _data->length() + str.length() );

	_data->setInitial();

	_data->insert(pos, str._data->str(), str._data->length());

	this->invariant();

	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, const basic_string& str, size_type pos2, size_type n)
{
	this->detach( _data->length() + n );

	_data->setInitial();

	_data->insert(pos, str._data->str() + pos2, n);

	this->invariant();
	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::insert(iterator p, Pt::Char ch)
{
	return this->insert(p, 1, ch);
}


basic_string<Pt::Char>& basic_string<Pt::Char>::insert(iterator p, size_type n, Pt::Char ch)
{
	const size_type pos = p - _data->str();

	this->detach( _data->length() + n );

	_data->setInitial();

	_data->insert(pos, n, ch);

	this->invariant();
	return *this;
}


void basic_string<Pt::Char>::clear()
{
	if( _data->busy() || _data->unref() < 1 ) {
		delete _data;
		_data = 0;
	}

	_data = new StringData();
}


basic_string<Pt::Char>& basic_string<Pt::Char>::erase(size_type pos, size_type n)
{
	this->detach( _data->length() );
	_data->setInitial();

	const size_type len = this->size() - pos;
	if(n > len) {
		n = len;
	}

	_data->erase(_data->str() + pos, n);
	return *this;
}


basic_string<Pt::Char>::iterator
basic_string<Pt::Char>::erase(iterator it)
{
	const size_type pos = it - _data->str();
	this->detach( _data->length() );
	_data->setInitial();
	return _data->erase(_data->str() + pos, 1);
}


basic_string<Pt::Char>::iterator
basic_string<Pt::Char>::erase(iterator first, iterator last)
{
	const size_type pos = first - _data->str();
	const size_type n = last - first;
	this->detach( _data->length() );
	_data->setInitial();
	return _data->erase(_data->str() + pos, n);
}


basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n, const Pt::Char* str)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( pos, n, str, traits_type::length(str) );
	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n, const Pt::Char* str, size_type n2)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( pos, n, str, n2 );
	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n, size_type n2, Pt::Char ch)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace(pos, n, n2, ch);
	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n, const basic_string& str)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( pos, n, str._data->str(), str._data->length() );
	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n,
                                                        const basic_string& str, size_type pos2, size_type n2)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( pos, n, str._data->str() + pos2, n2 );
	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::replace(iterator i1, iterator i2, const Pt::Char* str)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( i1 - _data->str(), i2 - i1, str, traits_type::length(str) );
	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::replace(iterator i1, iterator i2, const Pt::Char* str, size_type n)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( i1 - _data->str(), i2 - i1, str, n );
	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::replace(iterator i1, iterator i2, size_type n, Pt::Char ch)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( i1 - _data->str(), i2 - i1, n, ch );
	return *this;
}


basic_string<Pt::Char>& basic_string<Pt::Char>::replace(iterator i1, iterator i2, const basic_string& str)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( i1 - _data->str(), i2 - i1, str._data->str(), str.length() );
	return *this;
}


int basic_string<Pt::Char>::compare(const basic_string& str) const
{
	const size_type size = this->size();
	const size_type osize = str.size();
	size_type n = std::min(size , osize);

	const int result = traits_type::compare(_data->str(), str._data->str(), n);

	// unlike real life, size only matters when the quality is equal
	if (result == 0) {
		return size - osize;
	}

	return result;
}


int basic_string<Pt::Char>::compare(const Pt::Char* str) const
{
	const size_type size = this->size();
	const size_type osize = traits_type::length(str);
	size_type n = std::min(size , osize);

	const int result = traits_type::compare(_data->str(), str, n);

	// unlike real life, size only matters when the quality is equal
	if (result == 0) {
		return size - osize;
	}

	return result;
}


int basic_string<Pt::Char>::compare(size_type pos, size_type n, const basic_string& str) const
{
	const size_type size = n;
	const size_type osize = str.size();
	size_type len = std::min(size , osize);

	const int result = traits_type::compare(_data->str() + pos, str._data->str(), len);

	// unlike real life, size only matters when the quality is equal
	if (result == 0) {
		return size - osize;
	}

	return result;
}


int basic_string<Pt::Char>::compare(size_type pos, size_type n, const basic_string& str, size_type pos2, size_type n2) const
{
	const size_type size = n;
	const size_type osize = n2;
	size_type len = std::min(size , osize);

	const int result = traits_type::compare(_data->str() + pos,
	                                        str._data->str() + pos2,
	                                        len);

	// unlike real life, size only matters when the quality is equal
	if (result == 0) {
		return size - osize;
	}

	return result;
}


int basic_string<Pt::Char>::compare(size_type pos, size_type n, const Pt::Char* str) const
{
	const size_type size = n;
	const size_type osize = traits_type::length(str);
	size_type len = std::min(size , osize);

	const int result = traits_type::compare(_data->str() + pos,
	                                        str,
	                                        len);

	// unlike real life, size only matters when the quality is equal
	if (result == 0) {
		return size - osize;
	}

	return result;
}


int basic_string<Pt::Char>::compare(size_type pos, size_type n, const Pt::Char* str, size_type n2) const
{
	const size_type size = n;
	const size_type osize = n2;
	size_type len = std::min(size , osize);

	const int result = traits_type::compare(_data->str() + pos,
	                                        str,
	                                        len);

	// unlike real life, size only matters when the quality is equal
	if (result == 0) {
		return size - osize;
	}

	return result;
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find(const basic_string& str, size_type pos) const
{
	return this->find( str.data(), pos, str.size() );
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find(const Pt::Char* token, size_type pos, size_type n) const
{
	const size_type size = this->size();
	const Char* str = _data->str();

	for( ; pos + n <= size; ++pos) {
		if( 0 == traits_type::compare( str + pos, token, n ) ) {
			return pos;
		}
	}

	return npos;
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find(const Pt::Char* str, size_type pos) const
{
	return this->find( str, pos, traits_type::length(str) );
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find(Pt::Char ch, size_type pos) const
{
	const size_type size = this->size();
	if(pos > size) {
		return npos;
	}

	const Char* str = _data->str();
	const size_type n = size - pos;

	const Pt::Char* found = traits_type::find(str + pos, n, ch);
	if(found) {
		return found - str;
	}

	return npos;
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::rfind(const basic_string& str, size_type pos) const
{
	return this->rfind( str.data(), pos, str.size() );
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::rfind(const Pt::Char* token, size_type pos, size_type n) const
{
	// FIXME: check length
	const size_type size = this->size();

	if (n > size) {
		return npos;
	}

	pos = std::min(size_type(size - n), pos);
	const Char* str = _data->str();
	do {
		if (traits_type::compare(str + pos, token, n) == 0)
		return pos;
	}
	while (pos-- > 0);

	return npos;
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::rfind(const Pt::Char* str, size_type pos) const
{
	return this->rfind( str, pos, traits_type::length(str) );
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::rfind(Pt::Char ch, size_type pos) const
{
	const Char* str = _data->str();
	size_type size = this->size();

	if(size == 0)
		return npos;

	if(--size > pos)
		size = pos;

	for(++size; size-- > 0; ) {
		if( traits_type::eq(str[size], ch) )
			return size;
	}

	return npos;
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find_first_of(const Pt::Char* s, size_type pos, size_type n) const
{
	// check length os s against n
	const Char* str = _data->str();
	const size_type size = this->size();

	for (; n && pos < size; ++pos) {
		if( traits_type::find(s, n, str[pos]) )
			return pos;
	}

	return npos;
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find_last_of(const Pt::Char* s, size_type pos, size_type n) const
{
	// check length os s against n
	size_type size = this->size();
	const Char* str = _data->str();

	if (size == 0 ||  n == 0) {
		return npos;
	}

	if (--size > pos) {
		size = pos;
	}

	do {
		if( traits_type::find(s, n, str[size]) )
			return size;
	}
	while (size-- != 0);


	return npos;
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find_first_not_of(const Pt::Char* tok, size_type pos, size_type n) const
{
	// pt_requires_string_len(str, n);
	const Char* str = _data->str();

	for (; pos < this->size(); ++pos) {
		if ( !traits_type::find(tok, n, str[pos]) )
			return pos;
	}
	return npos;
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find_first_not_of(Pt::Char ch, size_type pos) const
{
	const Char* str = _data->str();

	for (; pos < this->size(); ++pos) {
		if ( !traits_type::eq(str[pos], ch) ) {
			return pos;
		}
	}

	return npos;
}


basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find_last_not_of(const Pt::Char* tok, size_type pos, size_type n) const
{
	//requires_string_len(__s, __n);
	size_type size = this->size();
	const Char* str = _data->str();

	if(size) {
		if (--size > pos)
			size = pos;

		do {
			if ( !traits_type::find(tok, n, str[size]) ) {
				return size;
			}
		}
		while(size--);
	}

	return npos;
}

basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find_last_not_of(Pt::Char ch, size_type pos) const
{
	size_type size = this->size();
	const Char* str = _data->str();

	if (size) {
		if (--size > pos)
			size = pos;
		do {
			if( !traits_type::eq(str[size], ch) ) {
				return size;
			}
		} while (size--);
	}

	return npos;
}


void basic_string<Pt::Char>::detach(size_type reserveSize)
{
	// shared, not busy - make copy
	if( _data->shared() ) {
		StringData* newBuffer = new StringData();
		newBuffer->reserve( reserveSize );
		newBuffer->assign( _data->str(), _data->length() );

		_data->unref();
		_data = newBuffer;
	}
}


std::string basic_string<Pt::Char>::narrow() const
{
	std::string ret;
	size_type len = this->length();
	const Pt::Char* s = _data->str();

	for(size_t n = 0; n < len; ++n){
		ret.append( 1, s->narrow('_') );
		++s;
	}

	return ret;
}


void basic_string<Pt::Char>::invariant()
{
	assert( _data != 0 );
	assert( this->capacity() >= this->length() );
	assert( this->size() == this->length() );
	assert( traits_type::length( _data->str() ) == this->length() );
	assert( _data->refs() >= 1);
}

}


