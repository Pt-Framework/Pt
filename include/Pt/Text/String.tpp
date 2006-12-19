/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Dürner                                  *
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
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


using namespace Pt;

using namespace std;



namespace std {

inline basic_string<Pt::Char>::basic_string(const allocator_type& a)
: _data( new StringData(a) )
{
}


inline basic_string<Pt::Char>::basic_string(const Pt::Char* str, const allocator_type& a)
: _data( new StringData( str, char_traits<Pt::Char>::length(str), a ) )
{
}


inline basic_string<Pt::Char>::basic_string(const wchar_t* str, const allocator_type& a)
: _data( new StringData( str, char_traits<wchar_t>::length(str), a ) )
{
}


inline basic_string<Pt::Char>::basic_string(const wchar_t* wstr, size_type length, const allocator_type& a)
: _data( new StringData( wstr, length, a ) )
{
}


inline basic_string<Pt::Char>::basic_string(const Pt::Char* str, size_type n, const allocator_type& a)
: _data( new StringData( str, n, a ) )
{
}


inline basic_string<Pt::Char>::basic_string(size_type n, Pt::Char c)
: _data( new StringData( n, c ) )
{
}


inline basic_string<Pt::Char>::basic_string(const basic_string& str)
: _data(0)
{
	if( str._data->busy() == false ) {
		_data = str._data;
		_data->ref();
	}
	else {
		_data = new StringData( str._data->str(), str._data->length() );
	}
}


inline basic_string<Pt::Char>::basic_string(const basic_string& str, size_type pos)
: _data( new StringData( str._data->str() + pos, str._data->length() - pos ) )
{
}


inline basic_string<Pt::Char>::basic_string(const basic_string& str, size_type pos, size_type n)
: _data( new StringData( str._data->str() + pos, n ) )
{
}


inline basic_string<Pt::Char>::basic_string(const basic_string& str, size_type pos, size_type n, const allocator_type& a)
: _data( new StringData( str._data->str() + pos, n, a ) )
{
}


inline basic_string<Pt::Char>::~basic_string()
{
	if( _data->busy() || _data->unref() < 1 ) {
		delete _data;
		_data = 0;
	}
}


inline basic_string<Pt::Char>::iterator basic_string<Pt::Char>::begin()
{
	this->detach( _data->length() );
	_data->setBusy();
	return _data->str();
}


inline basic_string<Pt::Char>::iterator basic_string<Pt::Char>::end()
{
	this->detach( _data->length() );
	_data->setBusy();
	return _data->end();
}


inline basic_string<Pt::Char>::const_iterator basic_string<Pt::Char>::begin() const
{
	return _data->str();
}


inline basic_string<Pt::Char>::const_iterator basic_string<Pt::Char>::end() const
{
	return _data->end();
}


inline void basic_string<Pt::Char>::resize(size_t n, Pt::Char ch)
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


inline void basic_string<Pt::Char>::reserve(size_t n)
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


inline void basic_string<Pt::Char>::swap(basic_string& str)
{
	basic_string tmp1(this->c_str(), this->length(), this->get_allocator());
	basic_string tmp2(str.c_str(), str.length(), str.get_allocator());
	str = tmp1;
	*this = tmp2;
}


inline 
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


inline basic_string<Pt::Char>::size_type basic_string<Pt::Char>::length() const
{
	return _data->length();
}


inline basic_string<Pt::Char>::size_type basic_string<Pt::Char>::size() const
{
	return _data->length();
}


inline bool basic_string<Pt::Char>::empty() const
{
	return _data->length() == 0;
}


inline basic_string<Pt::Char>::size_type basic_string<Pt::Char>::max_size() const
{
	return ( size_type(-1) / sizeof(Pt::Char) ) - 1;
}


inline basic_string<Pt::Char>::size_type basic_string<Pt::Char>::capacity() const
{
	return _data->capacity();
}


inline const Pt::Char* basic_string<Pt::Char>::c_str() const
{
	return _data->str();
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const basic_string<Pt::Char>& str)
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

	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const basic_string<Pt::Char>& str, size_type pos, size_type n)
{
	this->assign( str._data->str() + pos, n );

	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const Pt::Char* str)
{
	const size_type len = char_traits<Pt::Char>::length(str);
	this->assign(str, len);

	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::assign(const Pt::Char* str, size_type length)
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

	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::assign(size_type n, Pt::Char ch)
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

	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::append(const Pt::Char* str)
{
	return this->append( str, traits_type::length(str) );
}


/*
inline void basic_string<Pt::Char>::detach(size_type reserveSize)
{
	// shared, not busy - make copy
	if( _data->shared() ) {
		StringData* newBuffer = new StringData( reserveSize, _data->str(), _data->length() );
		_data->unref();
		_data = newBuffer;
	}
}
*/

inline basic_string<Pt::Char>& basic_string<Pt::Char>::append(const Pt::Char* str, size_type n)
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
	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::append(size_type n, Pt::Char ch)
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

	return *this;
}



inline basic_string<Pt::Char>& basic_string<Pt::Char>::append(const basic_string& str)
{
	return this->append( str._data->str(), str._data->length() );
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::append(const basic_string& str, size_type pos, size_type n)
{
	return this->append( str._data->str() + pos, n );
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, const Pt::Char* str)
{
	return this->insert( pos, str, traits_type::length(str) );
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, const Pt::Char* str, size_type n)
{
	// detach to new size
	this->detach( _data->length() + n );

	// mutation ends busy mode
	_data->setInitial();

	_data->insert(pos, str, n);

	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, size_type n, Pt::Char ch)
{
	this->detach( _data->length() + n );

	_data->setInitial();

	_data->insert(pos, n, ch);

	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, const basic_string& str)
{
	this->detach( _data->length() + str.length() );

	_data->setInitial();

	_data->insert(pos, str._data->str(), str._data->length());

	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::insert(size_type pos, const basic_string& str, size_type pos2, size_type n)
{
	this->detach( _data->length() + n );

	_data->setInitial();

	_data->insert(pos, str._data->str() + pos2, n);

	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::insert(iterator p, Pt::Char ch)
{
	return this->insert(p, 1, ch);
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::insert(iterator p, size_type n, Pt::Char ch)
{
	const size_type pos = p - _data->str();

	this->detach( _data->length() + n );

	_data->setInitial();

	_data->insert(pos, n, ch);

	return *this;
}


inline void basic_string<Pt::Char>::clear()
{
	if( _data->busy() || _data->unref() < 1 ) {
		delete _data;
		_data = 0;
	}

	_data = new StringData();
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::erase(size_type pos, size_type n)
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


inline
basic_string<Pt::Char>::iterator
basic_string<Pt::Char>::erase(iterator it)
{
	const size_type pos = it - _data->str();
	this->detach( _data->length() );
	_data->setInitial();
	return _data->erase(_data->str() + pos, 1);
}


inline
basic_string<Pt::Char>::iterator
basic_string<Pt::Char>::erase(iterator first, iterator last)
{
	const size_type pos = first - _data->str();
	const size_type n = last - first;
	this->detach( _data->length() );
	_data->setInitial();
	return _data->erase(_data->str() + pos, n);
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n, const Pt::Char* str)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( pos, n, str, traits_type::length(str) );
	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n, const Pt::Char* str, size_type n2)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( pos, n, str, n2 );
	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n, size_type n2, Pt::Char ch)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace(pos, n, n2, ch);
	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n, const basic_string& str)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( pos, n, str._data->str(), str._data->length() );
	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::replace(size_type pos, size_type n,
                                                        const basic_string& str, size_type pos2, size_type n2)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( pos, n, str._data->str() + pos2, n2 );
	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::replace(iterator i1, iterator i2, const Pt::Char* str)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( i1 - _data->str(), i2 - i1, str, traits_type::length(str) );
	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::replace(iterator i1, iterator i2, const Pt::Char* str, size_type n)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( i1 - _data->str(), i2 - i1, str, n );
	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::replace(iterator i1, iterator i2, size_type n, Pt::Char ch)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( i1 - _data->str(), i2 - i1, n, ch );
	return *this;
}


inline basic_string<Pt::Char>& basic_string<Pt::Char>::replace(iterator i1, iterator i2, const basic_string& str)
{
	this->detach( _data->length() );
	_data->setInitial();
	_data->replace( i1 - _data->str(), i2 - i1, str._data->str(), str.length() );
	return *this;
}


inline int basic_string<Pt::Char>::compare(const basic_string& str) const
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


inline int basic_string<Pt::Char>::compare(const Pt::Char* str) const
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


inline int basic_string<Pt::Char>::compare(size_type pos, size_type n, const basic_string& str) const
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


inline int basic_string<Pt::Char>::compare(size_type pos, size_type n, const basic_string& str, size_type pos2, size_type n2) const
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


inline int basic_string<Pt::Char>::compare(size_type pos, size_type n, const Pt::Char* str) const
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


inline int basic_string<Pt::Char>::compare(size_type pos, size_type n, const Pt::Char* str, size_type n2) const
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


inline
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find(const basic_string& str, size_type pos) const
{
	return this->find( str.data(), pos, str.size() );
}


inline
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


inline
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::find(const Pt::Char* str, size_type pos) const
{
	return this->find( str, pos, traits_type::length(str) );
}


inline
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


inline
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::rfind(const basic_string& str, size_type pos) const
{
	return this->rfind( str.data(), pos, str.size() );
}


inline
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


inline
basic_string<Pt::Char>::size_type
basic_string<Pt::Char>::rfind(const Pt::Char* str, size_type pos) const
{
	return this->rfind( str, pos, traits_type::length(str) );
}


inline
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

inline
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

inline
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

inline
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

inline
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


inline
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

inline
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


inline void basic_string<Pt::Char>::detach(size_type reserveSize)
{
	// shared, not busy - make copy
	if( _data->shared() ) {
		StringData* newBuffer = new StringData();
		newBuffer->reserve( reserveSize );
		newBuffer->assign( _data->str(), _data->length() );

		_data->unref();
		_data = newBuffer;
	}
	else
	{
		_data->reserve( reserveSize );
	}
}


inline std::string basic_string<Pt::Char>::narrow() const
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

}

