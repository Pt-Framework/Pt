/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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

#ifndef Pt_Text_TextBuffer_h
#define Pt_Text_TextBuffer_h

#include <Pt/Api.h>
#include <Pt/Text/TextCodec.h>

#include <iostream>
#include <vector>


namespace Pt {

namespace Text {

	/**
	 * @brief Generic text-buffer which wraps a streaming buffer and converts its character
	 * data on-the-fly using a Codec.
	 *
	 * This class derives from std::basic_streambuf which is the super-class of all stream buffer
	 * classes. Stream buffer classes are used to connect to an external device, transport characters
	 * from and to this external device and buffer the characters in an internal buffer.
	 *
	 * The internal character set can be specified using the template parameters 'InternT_', the
	 * external character set using 'ExternT_'. The external type is the input type and output
	 * type when reading from or writing to the external device. The internal type is the type
	 * which is used to internally store the data from the external device after the external
	 * format was converted using the Codec which is passed when constructing an object of this
	 * class.
	 *
	 * The Codec object which is passed as pointer to the constructor will afterwards be completely
	 * managed by this class and also be deleted by this class when it's destructed!
	 *
	 * @see std::basic_streambuf
	 */
	template <typename InternT_, typename ExternT_>
	class BasicTextBuffer : public std::basic_streambuf<InternT_> {
		public:
			typedef InternT_ InternT;
			typedef ExternT_ ExternT;
			typedef typename std::basic_streambuf<InternT>::int_type IntT;
			typedef typename std::basic_streambuf<InternT>::traits_type TraitsT;
			typedef TextCodec<InternT, ExternT> CodecT;

		public:
			/**
			 * @brief Creates a BasicTextBuffer using the given stream buffer and codec.
			 *
			 * The given stream buffer is used as external device, buffered by this text buffer
			 * and all input from and output to the external device is converted using the
			 * given codec class.
			 *
			 * Note: The Codec object which is passed as pointer will afterwards be completely
			 * managed by this class and also be deleted by this class when it's destructed!
			 *
			 * @param buffer The buffer (external device) which is wrapped by this object.
			 * @param codec The codec which is used to convert data from or to the external device.
			 */
			BasicTextBuffer(std::basic_streambuf<ExternT>* buffer, CodecT* codec)
			: _streambuf(buffer), _codec(codec)
			{
				this->btinit();
			}

			~BasicTextBuffer() throw()
			{ delete _codec; }

			InternT* begin()
			{ return this->gptr(); }

			const InternT* in_begin() const
			{ return this->gptr(); }

			void in_bump(size_t n)
			{ this->gbump(n); }

			const InternT* in_end() const
			{ return this->egptr(); }

		protected:
			//! @brief Initializes this text buffer by creating the internal buffer.
			void btinit();

			//! inheritdoc - reimplemented from basic_streambuf
			virtual int sync();

			//! inheritdoc - reimplemented from basic_streambuf
			virtual IntT overflow(IntT ch);

			//! inheritdoc - reimplemented from basic_streambuf
			virtual IntT underflow();

		private:
			//! The external device (stream buffer) from which data is read and to which data is written.
			std::basic_streambuf<ExternT>* _streambuf;

			//! Contains the state of conversion.
			mbstate_t _state;

			//! The codec which is used to convert character data from or to the external device.
			CodecT* _codec;

			//! The size of the internal input and output buffer.
			size_t _bufferSize;

			size_t _putbackMax;

			//! Buffer for reading encoded chars
			std::vector<ExternT> _readBuffer;

			//! Input buffer for the decoded chars, used by the stream
			std::vector<InternT> _inBuffer;

			//! Output buffer for the decoded chars, used by the stream
			std::vector<InternT> _outBuffer;

			//! Buffer for writing ecoded chars
			std::vector<ExternT> _writeBuffer;
	};


	template <typename I, typename E>
	void BasicTextBuffer<I, E>::btinit()
	{
		_bufferSize = 1024;
		_putbackMax = 4;

		_readBuffer.reserve(_bufferSize);

		// TODO: use size factor from TextCodec to determine
		// optimum buffer size ratios
		_inBuffer.resize( _putbackMax + (_bufferSize*4) );
		this->setg( &(_inBuffer[0]) + _putbackMax,
		            &(_inBuffer[0]) + _putbackMax,
		            &(_inBuffer[0]) + _putbackMax );

		_outBuffer.resize(_bufferSize);
		_writeBuffer.resize(_bufferSize * 4);
		this->setp(&_outBuffer[0], &_outBuffer[0] + _outBuffer.size());
	}


	template <typename I, typename E>
	int BasicTextBuffer<I, E>::sync()
	{
		if( this->overflow( TraitsT::eof() ) == TraitsT::eof() ) {
			return -1;
		}

		_streambuf->pubsync();

		return 0;
	}


	template <typename I, typename E>
	typename BasicTextBuffer<I, E>::IntT BasicTextBuffer<I, E>::underflow()
	{
		if( !_codec )
			return TraitsT::eof();

		// return if input buffer is not empty.
		if( this->gptr() != 0 && this->gptr() < this->egptr() ) {
			return TraitsT::to_int_type( *(this->gptr()) );
		}

		// keep chars for putback
		size_t putbackSize = std::min<size_t>(this->gptr() - this->eback(), _putbackMax);
		std::char_traits<InternT>::copy( &(_inBuffer[0]) + (_putbackMax - putbackSize),
		                                 this->gptr() - putbackSize,
		                                 putbackSize );

		// get encoded characters
		size_t currentSize = _readBuffer.size();
		_readBuffer.resize(_bufferSize);
		size_t readSize = _streambuf->sgetn( &(_readBuffer[0]) + currentSize,
		                                     _readBuffer.size() - currentSize );
		_readBuffer.resize(currentSize + readSize);
		if(readSize <= 0) {
			return TraitsT::eof();
		}

		// set pointers to source and destination
		const ExternT* fromBegin = &(_readBuffer[0]);
		const ExternT* fromEnd = fromBegin + (currentSize + readSize);
		const ExternT* fromNext = fromBegin;
		InternT* toBegin = &(_inBuffer[0]) + _putbackMax;
		InternT* toEnd = &(_inBuffer[0]) + _inBuffer.capacity();
		InternT* toNext = toBegin;

		typename CodecT::result result = _codec->in(_state, fromBegin, fromEnd, fromNext, toBegin, toEnd, toNext);
		switch(result) {
			case CodecT::ok: {
				_readBuffer.resize(0);
				break;
			}
			case CodecT::partial: {
				// move converted chars
				const size_t leftover = fromEnd - fromNext;
				std::char_traits<ExternT>::move( &(_readBuffer[0]), fromNext, leftover );
				_readBuffer.resize(leftover);
				break;
			}
			case CodecT::noconv: {
				//std::copy(fromBegin, fromEnd, toBegin);
				//_readBuffer.resize(0);
				break;
			}
			case CodecT::error: {
				return TraitsT::eof();
				break;
			}
		}

		this->setg(&(_inBuffer[0]) + (_putbackMax - putbackSize), // start of read buffer
		           &(_inBuffer[0]) + _putbackMax,                     // gptr position
		           &(_inBuffer[0]) + _putbackMax + (toNext - toBegin) ); // end of read buffer

		return TraitsT::to_int_type( *(this->gptr()) );
	}


	template <typename I, typename E>
	typename BasicTextBuffer<I, E>::IntT BasicTextBuffer<I, E>::overflow(IntT ch)
	{
		if(!_codec)
			return TraitsT::eof();

		// set pointers for codec to source and destination
		const InternT* fromBegin = &(_outBuffer[0]);
		const InternT* fromEnd = fromBegin + (this->pptr() - this->pbase());
		const InternT* fromNext = fromBegin;
		ExternT* toBegin = &(_writeBuffer[0]);
		ExternT* toEnd = &(_writeBuffer[0]) + _writeBuffer.capacity();
		ExternT* toNext = toBegin;

		// encode chars
		typename CodecT::result result = _codec->out(_state, fromNext, fromEnd, fromNext, toNext, toEnd, toNext);
		if(result == CodecT::partial) {
			// move leftover to front
			const size_t leftover = fromEnd - fromNext;
			std::char_traits<InternT>::move( &(_outBuffer[0]), fromNext, leftover);
		}
		else if(result == CodecT::noconv) {
			//std::copy(fromBegin, fromEnd, toBegin);
			//fromNext = fromEnd;
		}
		else if(result == CodecT::error) {
			return TraitsT::eof();
		}

		// unshift if EOF is reached
		if( TraitsT::eq_int_type(ch, TraitsT::eof()) ) {
			//typename CodecT::result r = 
			_codec->unshift(_state, toNext, toEnd, toNext);
		}

		//write encoded chars
		const size_t writeSize = _streambuf->sputn(toBegin, toNext - toBegin);
		if( writeSize == 0 ) {
			return TraitsT::eof();
		}

		this->setp( &_outBuffer[0] + (fromEnd - fromNext),
		            &_outBuffer[0] + _outBuffer.size() );

		if( !TraitsT::eq_int_type(ch, TraitsT::eof()) ) {
			*( this->pptr() ) = TraitsT::to_char_type(ch);
			this->pbump(1);
		}

		return TraitsT::not_eof(ch);
	}

} // namespace Text

} // namespace Pt

#endif

