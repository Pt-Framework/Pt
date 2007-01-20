/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Dürner                                  *
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

#ifndef PT_Text_BasicTextStream_h
#define PT_Text_BasicTextStream_h

#include <Pt/Text/Api.h>
#include <Pt/Text/String.h>
#include <Pt/Text/BasicTextBuffer.h>

#include <iostream>


namespace Pt {

namespace Text {

	/**
	 * @brief Generic input-stream which converts the data from the external device using a Codec.
	 *
	 * This generic stream, which only supports input, wraps another input-stream and converts the
	 * character data which is received from the stream on-the-fly using a Codec. The data which is
	 * received from the wrapped input-stream is buffered.
	 *
	 * This class derives from std::basic_istream which is the super-class of input-stream classes.
	 * Stream classes are used to connect to an external device and transport characters
	 * from this external device.
	 *
	 * The internal character set can be specified using the template parameters 'I', the
	 * external character set using 'E'. The external type is the input type and output
	 * type when reading from or writing to the external device. The internal type is the type
	 * which is used to internally store the data from the external device after the external
	 * format was converted using the Codec which is passed when constructing an object of this
	 * class.
	 *
	 * The Codec object which is passed as pointer to the constructor will afterwards be completely
	 * managed by this class and also be deleted by this class when it's destructed!
	 *
	 * @see std::basic_istream
	 */
	template <typename I, typename E>
	class BasicTextIStream : public std::basic_istream<I> {
		public:
			typedef I InternT;
			typedef E ExternT;
			typedef TextCodec<InternT, ExternT> CodecT;

		public:
			/**
			 * @brief Creates a BasicTextOStream using the given input-stream and codec.
			 *
			 * The given stream is used as external device and all data from the device
			 * is converted using the given codec class.
			 *
			 * Note: The Codec object which is passed as pointer will afterwards be completely
			 * managed by this class and also be deleted by this class when it's destructed!
			 *
			 * @param is The output-stream (external device) which is wrapped by this object.
			 * @param codec The codec which is used to convert data from the external device.
			 */
			BasicTextIStream(std::basic_istream<ExternT>& is, CodecT* codec)
			: std::basic_istream<InternT>( _buffer = new BasicTextBuffer<InternT, ExternT>( is.rdbuf() , codec) )
			{ }

			//! @brief Destructs this object freeing the internal buffer.
			~BasicTextIStream()
			{ delete _buffer; }

		protected:
			BasicTextIStream(std::basic_streambuf<InternT>* buffer)
			: std::basic_istream<InternT>( buffer ),
			  _buffer(0)
			{ }

		private:
			//! @brief Internal buffer using the internal data type (InternT)
			std::basic_streambuf<InternT>* _buffer;
	};


	/**
	 * @brief Generic output-stream which converts the data for the external device using a Codec.
	 *
	 * This generic stream, which only supports output, wraps another output-stream and converts the
	 * character data which is sent to the stream on-the-fly using a Codec. The data which is sent
	 * to the wrapped output-stream is buffered.
	 *
	 * This class derives from std::basic_ostream which is the super-class of output-stream classes.
	 * Stream classes are used to connect to an external device and transport characters
	 * to this external device.
	 *
	 * The internal character set can be specified using the template parameters 'I', the
	 * external character set using 'E'. The external type is the input type and output
	 * type when reading from or writing to the external device. The internal type is the type
	 * which is used to internally store the data from the external device after the external
	 * format was converted using the Codec which is passed when constructing an object of this
	 * class.
	 *
	 * The Codec object which is passed as pointer to the constructor will afterwards be completely
	 * managed by this class and also be deleted by this class when it's destructed!
	 *
	 * @see std::basic_ostream
	 */
	template <typename I, typename E>
	class BasicTextOStream : public std::basic_ostream<I> {
		public:
			typedef I InternT;
			typedef E ExternT;
			typedef TextCodec<InternT, ExternT> CodecT;

		public:
			/**
			 * @brief Creates a BasicTextOStream using the given output-stream and codec.
			 *
			 * The given stream is used as external device and all data to the device
			 * is converted using the given codec class.
			 *
			 * Note: The Codec object which is passed as pointer will afterwards be completely
			 * managed by this class and also be deleted by this class when it's destructed!
			 *
			 * @param is The output-stream (external device) which is wrapped by this object.
			 * @param codec The codec which is used to convert data to the external device.
			 */
			BasicTextOStream(std::basic_ostream<ExternT>& is, CodecT* codec)
			: std::basic_ostream<InternT>( _buffer = new BasicTextBuffer<InternT, ExternT>( is.rdbuf() , codec) )
			{ }

			//! @brief Destructs this object freeing the internal buffer.
			~BasicTextOStream()
			{ delete _buffer; }

		protected:
			BasicTextOStream(std::basic_streambuf<InternT>* buffer)
			: std::basic_ostream<InternT>( buffer ),
			  _buffer(0)
			{ }

		private:
			//! @brief Internal buffer using the internal data type (InternT)
			std::basic_streambuf<InternT>* _buffer;
	};


	/**
	 * @brief Generic I/O-stream which converts the data from or to the external device using a Codec.
	 *
	 * This generic stream, which supports both input and output, wraps another I/O-stream and converts its
	 * character data on-the-fly using a Codec. The data from the wrapped stream is buffered.
	 *
	 * This class derives from std::basic_iostream which is the super-class of I/O stream classes.
	 * Stream classes are used to connect to an external device and transport characters
	 * from and to this external device.
	 *
	 * The internal character set can be specified using the template parameters 'I', the
	 * external character set using 'E'. The external type is the input type and output
	 * type when reading from or writing to the external device. The internal type is the type
	 * which is used to internally store the data from the external device after the external
	 * format was converted using the Codec which is passed when constructing an object of this
	 * class.
	 *
	 * The Codec object which is passed as pointer to the constructor will afterwards be completely
	 * managed by this class and also be deleted by this class when it's destructed!
	 *
	 * @see std::basic_iostream
	 */
	template <typename I, typename E>
	class BasicTextStream : public std::basic_iostream<I> {
		public:
			typedef I InternT;
			typedef E ExternT;
			typedef TextCodec<InternT, ExternT> CodecT;

		public:
			/**
			 * @brief Creates a BasicTextStream using the given I/O-stream and codec.
			 *
			 * The given stream is used as external device and all data from and to the device
			 * is converted using the given codec class.
			 *
			 * Note: The Codec object which is passed as pointer will afterwards be completely
			 * managed by this class and also be deleted by this class when it's destructed!
			 *
			 * @param is The I/O-stream (external device) which is wrapped by this object.
			 * @param codec The codec which is used to convert data from or to the external device.
			 */
			BasicTextStream(std::basic_iostream<ExternT>& is, CodecT* codec)
			: std::basic_iostream<InternT>( _buffer = new BasicTextBuffer<InternT, ExternT>( is.rdbuf() , codec) )
			{ }

			//! @brief Destructs this object freeing the internal buffer.
			~BasicTextStream()
			{ delete _buffer; }

		protected:
			BasicTextStream(std::basic_streambuf<InternT>* buffer)
			: std::basic_iostream<InternT>( buffer ),
			  _buffer(0)
			{ }

		private:
			//! @brief Internal buffer using the internal data type (InternT)
			std::basic_streambuf<InternT>* _buffer;
	};

} // namespace Text

} // namespace Pt

#endif

