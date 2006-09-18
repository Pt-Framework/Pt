/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Drner                               *
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
 **************************************************************************/

#ifndef Pt_Text_BasicTextStream_h
#define Pt_Text_BasicTextStream_h

#include <Pt/Api.h>
#include <Pt/Text/BasicTextBuffer.h>

#include <iostream>


namespace Pt {

namespace Text {

	template <typename I, typename E>
	class BasicTextIStream : public std::basic_istream<I> {
		public:
			typedef I InternT;
			typedef E ExternT;
			typedef TextCodec<InternT, ExternT> CodecT;

		public:
			BasicTextIStream(std::basic_istream<ExternT>& is, CodecT* codec)
			: std::basic_istream<InternT>( _buffer = new BasicTextBuffer<InternT, ExternT>( is.rdbuf() , codec) )
			{ }

			~BasicTextIStream()
			{ delete _buffer; }

		protected:
			BasicTextIStream(std::basic_streambuf<InternT>* buffer)
			: std::basic_istream<InternT>( buffer ),
			  _buffer(0)
			{ }

		private:
			std::basic_streambuf<InternT>* _buffer;
	};


	template <typename I, typename E>
	class BasicTextOStream : public std::basic_ostream<I> {
		public:
			typedef I InternT;
			typedef E ExternT;
			typedef TextCodec<InternT, ExternT> CodecT;

		public:
			BasicTextOStream(std::basic_ostream<ExternT>& is, CodecT* codec)
			: std::basic_ostream<InternT>( _buffer = new BasicTextBuffer<InternT, ExternT>( is.rdbuf() , codec) )
			{ }

			~BasicTextOStream()
			{ delete _buffer; }

		protected:
			BasicTextOStream(std::basic_streambuf<InternT>* buffer)
			: std::basic_ostream<InternT>( buffer ),
			  _buffer(0)
			{ }

		private:
			std::basic_streambuf<InternT>* _buffer;
	};


	template <typename I, typename E>
	class BasicTextStream : public std::basic_iostream<I> {
		public:
			typedef I InternT;
			typedef E ExternT;
			typedef TextCodec<InternT, ExternT> CodecT;

		public:
			BasicTextStream(std::basic_iostream<ExternT>& is, CodecT* codec)
			: std::basic_iostream<InternT>( _buffer = new BasicTextBuffer<InternT, ExternT>( is.rdbuf() , codec) )
			{ }

			~BasicTextStream()
			{ delete _buffer; }

		protected:
			BasicTextStream(std::basic_streambuf<InternT>* buffer)
			: std::basic_iostream<InternT>( buffer ),
			  _buffer(0)
			{ }

		private:
			std::basic_streambuf<InternT>* _buffer;
	};

} // namespace Text

} // namespace Pt

#endif

