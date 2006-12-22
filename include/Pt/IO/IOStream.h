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
#ifndef Pt_IOStream_h
#define Pt_IOStream_h

#include <Pt/Api.h>
#include <Pt/IO/IODevice.h>
#include <Pt/IO/IOBuffer.h>

#include <iostream>
#include <memory>


namespace std
{
	class PT_API std::ios_base;
}


namespace Pt {

namespace IO {

	//! @brief An istream with peeking capability.
	template <typename CharT>
	class PT_API BasicIStream : public std::basic_istream<CharT> {
		public:
			~BasicIStream() throw()
			{ }

			//! @brief Access to the underlying buffer.
			BasicStreamBuffer<CharT>* rdbuf()
			{ return _buffer; }

			//! @brief Peeks bytes in the stream buffer.
			/**
			   The number of bytes that can be peeked depends on the current
			   stream buffer get area and maybe less than requested,
			   similar to istream::readsome().
			*/
			std::streamsize peek(char* buffer, std::streamsize size) throw(IOError)
			{ return _buffer->peek(buffer, size); }

		protected:
			explicit BasicIStream(BasicStreamBuffer<CharT>* buffer) throw(IOError)
			: std::basic_istream<CharT>( buffer ),
			  _buffer(buffer)
			{ }

		private:
			BasicStreamBuffer<CharT>* _buffer;
	};


	//! @brief An ostream with peeking capability.
	template <typename CharT>
	class PT_API BasicOStream : public std::basic_ostream<CharT> {
		public:
			~BasicOStream() throw()
			{}

			//! @brief Access to the underlying buffer.
			BasicStreamBuffer<CharT>* rdbuf()
			{ return _buffer; }

		protected:
			explicit BasicOStream(BasicStreamBuffer<CharT>* buffer) throw(IOError)
			: std::basic_ostream<CharT>( buffer ),
			  _buffer(buffer)
			{ }

		private:
			BasicStreamBuffer<CharT>* _buffer;
	};


	//! @brief An iostream with peeking capability.
	template <typename CharT>
	class PT_API BasicIOStream : public std::basic_iostream<CharT> {
		public:
			~BasicIOStream() throw()
			{ }

			//! @brief Access to the underlying buffer.
			BasicStreamBuffer<CharT>* rdbuf()
			{ return _buffer; }

			//! @brief Peeks bytes in the stream buffer.
			/**
			   The number of bytes that can be peeked depends on the current
			   stream buffer get area and maybe less than requested,
			   similar to istream::readsome().
			*/
			std::streamsize peek(char* buffer, std::streamsize size) throw(IOError)
			{ return _buffer->peek(buffer, size); }

		protected:
			explicit BasicIOStream(BasicIOBuffer<CharT>* buffer) throw(IOError)
			: std::basic_iostream<CharT>( buffer ),
			  _buffer(buffer)
			{ }

		private:
			BasicStreamBuffer<CharT>* _buffer;
};


	typedef BasicIStream<char> IStream;
	typedef BasicOStream<char> OStream;
	typedef BasicIOStream<char> IOStream;

} // namespace IO

} // !namespace Pt

#endif


