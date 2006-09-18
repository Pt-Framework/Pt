/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Dürner                                  *
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
#ifndef Pt_IO_StreamBuffer_h
#define Pt_IO_StreamBuffer_h

#include <Pt/Api.h>
#include <ios>
#include <streambuf>


namespace Pt {

namespace IO {

	template <typename CharT>
	class PT_EXPORT BasicStreamBuffer : public std::basic_streambuf<CharT> {
		public:
			typedef typename std::basic_streambuf<CharT>::int_type IntT;
			typedef typename std::basic_streambuf<CharT>::traits_type TraitsT;

		public:
			virtual std::streamsize peek(CharT* buffer, std::streamsize size)
			{ return this->_peek(buffer, size); }

		protected:
			virtual std::streamsize _peek(CharT* buffer, std::streamsize size)
			{ return 0; }
	};

	typedef BasicStreamBuffer<char> StreamBuffer;

} // namespace IO

} // namespace Pt

#endif


