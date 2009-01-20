/*
 * Copyright (C) 2004 Marc Boris D�rner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef Pt_TextBuffer_h
#define Pt_TextBuffer_h

#include <Pt/Api.h>
#include <Pt/TextCodec.h>
#include <iostream>
#include <vector>

namespace Pt {

/** @brief Converts character sequences with different encodings.

	This class derives from std::basic_streambuf which is the super-class of all stream buffer
	classes. Stream buffer classes are used to connect to an external device, transport characters
	from and to this external device and buffer the characters in an internal buffer.

	The internal character set can be specified using the template parameters 'char_type_', the
	external character set using 'ExternT_'. The external type is the input type and output
	type when reading from or writing to the external device. The internal type is the type
	which is used to internally store the data from the external device after the external
	format was converted using the Codec which is passed when constructing an object of this
	class.

	The Codec object which is passed as pointer to the constructor will afterwards be completely
	managed by this class and also be deleted by this class when it's destructed!

  @see std::basic_streambuf
*/
template <typename CharT, typename E>
class BasicTextBuffer : public std::basic_streambuf<CharT>
{
	public:
		typedef E ExternT;
		typedef CharT char_type;
		typedef typename std::char_traits<CharT> traits_type;
		typedef typename traits_type::int_type int_type;
		typedef typename traits_type::pos_type pos_type;
		typedef typename traits_type::off_type off_type;
		typedef TextCodec<char_type, ExternT> CodecT;

	private:
		//! The external device (stream buffer) from which data is read and to which data is written.
		std::basic_streambuf<ExternT>* _streambuf;

		//! Contains the state of conversion.
		MBState _state;

		//! The codec which is used to convert character data from or to the external device.
		CodecT* _codec;

		//! The size of the internal input and output buffer.
		size_t _bufferSize;

		static const int _pbmax = 4;

		//! Buffer for reading encoded chars
		//std::vector<ExternT> _readBuffer;

		static const int _ibufmax = 1024;
		ExternT _ibuf[_ibufmax];
		int _ibufsize;

		//! Input buffer for the decoded chars, used by the stream
		//std::vector<char_type> _inBuffer;

		static const int _gbufmax = 1024;
		char_type _gbuf[_gbufmax + _pbmax];

		//! Output buffer for the decoded chars, used by the stream
		std::vector<char_type> _outBuffer;

		//! Buffer for writing ecoded chars
		std::vector<ExternT> _writeBuffer;
		
	public:
		/** @brief Creates a BasicTextBuffer using the given stream buffer and codec.

			The given stream buffer @a buffer is used as external device,
			buffered by this Text buffer and all input from and output to
			the external device is converted using the codec @a codec.

			Note: The Codec object which is passed as pointer will be
			managed by this class and also be deleted by this class
			on destruction.
		*/
		BasicTextBuffer(std::basic_streambuf<ExternT>* buffer, CodecT* codec)
		: _streambuf(buffer)
		, _codec(codec)
		, _ibufsize(0)
		{
			_bufferSize = 1024;

			this->setg(0, 0, 0);
			//_readBuffer.reserve(_bufferSize);
			//_readBuffer.resize(0);

			// TODO: use size factor from TextCodec to determine
			// optimum buffer size ratios
			//_inBuffer.resize( _putbackMax + (_bufferSize*4) );
			//this->setg( &(_inBuffer[0]) + _putbackMax,
			//            &(_inBuffer[0]) + _putbackMax,
			//            &(_inBuffer[0]) + _putbackMax );

			_outBuffer.resize(_bufferSize);
			_writeBuffer.resize(_bufferSize * 4);
			this->setp(&_outBuffer[0], &_outBuffer[0] + _outBuffer.size());
		}

		~BasicTextBuffer() throw()
		{
			this->close();

			if(_codec->refs() == 0)
				delete _codec;
		}

		void attach(std::basic_streambuf<ExternT>* buffer)
		{
			_streambuf = buffer;
			this->setg(0, 0, 0);
			_ibufsize = 0;
		}

		void close()
		{
			bool ok = true;
			if( this->pbase() < this->pptr() )
			{
				const int_type n = this->overflow();
				if( traits_type::eq_int_type(n, traits_type::eof()) )
					ok = false;
			}

			if( ! _codec->always_noconv() && ok)
			{
				const std::size_t buflen = 128;
				ExternT buf[buflen];
				typename CodecT::result res = CodecT::error;
				std::streamsize len = 0;

				do
				{
					ExternT* next = 0;
					res = _codec->unshift(_state, buf, buf + buflen, next);

					if(res == CodecT::error)
					{
						ok = false;
					}
					else if (res == CodecT::ok || res == CodecT::partial)
					{
						len = next - buf;
						if(len > 0)
						{
							const std::streamsize n = _streambuf->sputn(buf, len);

							if (n != len)
								ok = false;
						}
					}
				}
				while (res == CodecT::partial && len > 0 && ok);
			}

			_state = MBState();
			_streambuf = 0;
		}

	protected:
		// inheritdoc - reimplemented from basic_streambuf
		virtual int sync()
		{
			if( this->pptr() )
			{
				while( this->pptr() > this->pbase() )
				{
					if( this->overflow( traits_type::eof() ) == traits_type::eof() ) {
						return -1;
					}

					_streambuf->pubsync();
				}
			}

			return 0;
		}

		// inheritdoc - reimplemented from basic_streambuf
		virtual int_type overflow( int_type ch = traits_type::eof() )
		{
			if( ! _codec )
				return traits_type::eof();

			// set pointers for codec to source and destination
			const char_type* fromBegin = &(_outBuffer[0]);
			const char_type* fromEnd = fromBegin + (this->pptr() - this->pbase());
			const char_type* fromNext = fromBegin;
			ExternT* toBegin = &(_writeBuffer[0]);
			ExternT* toEnd = &(_writeBuffer[0]) + _writeBuffer.capacity();
			ExternT* toNext = toBegin;

			// encode chars
			typename CodecT::result r;
			r = _codec->out(_state, fromNext, fromEnd, fromNext, toNext, toEnd, toNext);
			if(r == CodecT::partial)
			{
				// move leftover to front
				const size_t leftover = fromEnd - fromNext;
				std::char_traits<char_type>::move( &(_outBuffer[0]), fromNext, leftover);
			}
			else if(r == CodecT::noconv)
			{
				// If no conversion is required, fromNext is set to fromBegin
				// and toNext is set to toBegin. codecvt::max_length ensures
				// that the buffer to keep the converted chars is large enough
				size_t sz = fromEnd - fromBegin;
				this->copyChars( toBegin, fromBegin, fromEnd - fromBegin );
				fromNext += sz;
				toNext += sz;
			}
			else if(r == CodecT::error)
			{
				return traits_type::eof(); // TODO: throw exception
			}

			//write encoded chars
			const size_t writeSize = _streambuf->sputn(toBegin, toNext - toBegin);
			if( writeSize == 0 )
			{
				return traits_type::eof();
			}

			this->setp( &_outBuffer[0] + (fromEnd - fromNext),
						&_outBuffer[0] + _outBuffer.size() );

			if( !traits_type::eq_int_type(ch, traits_type::eof()) )
			{
				*( this->pptr() ) = traits_type::to_char_type(ch);
				this->pbump(1);
			}

			return traits_type::not_eof(ch);
		}

		// inheritdoc - reimplemented from basic_streambuf
		virtual int_type underflow()
		{
			if( ! _codec || ! _streambuf )
				return traits_type::eof();

			if( this->gptr() < this->egptr() )
				return traits_type::to_int_type( *this->gptr() );

			if(_ibufsize == 0)
			{
				_ibufsize = _streambuf->sgetn( _ibuf, _ibufmax );
				if(_ibufsize <= 0)
					return traits_type::eof();
			}

			size_t putback = _pbmax;

			if( this->gptr() )
			{
				putback = std::min<size_t>(this->gptr() - this->eback(), _pbmax);
				std::char_traits<char_type>::move( _gbuf + _pbmax - putback,
												   this->gptr() - putback,
												   putback );
			}

			const ExternT* fromBegin = _ibuf;
			const ExternT* fromEnd   = _ibuf + _ibufsize;
			const ExternT* fromNext  = _ibuf;
			char_type* toBegin       = _gbuf + _pbmax;
			char_type* toEnd         = _gbuf + _pbmax + _gbufmax;
			char_type* toNext        = _gbuf;

			typename CodecT::result r;
			r = _codec->in(_state, fromBegin, fromEnd, fromNext, toBegin, toEnd, toNext);
			switch(r)
			{
				case CodecT::ok:
				{
					_ibufsize = 0;
					break;
				}
				case CodecT::partial:
				{
					// move converted chars
					_ibufsize = fromEnd - fromNext;
					std::char_traits<ExternT>::move( _ibuf, fromNext, _ibufsize );
					break;
				}
				case CodecT::noconv:
				{
					// If no conversion is required, we simply need to copy
					// fromNext is set to fromBegin and toNext is set to toBegin.
					int n =_ibufsize > _gbufmax ? _gbufmax : _ibufsize ;
					this->copyChars(toBegin, fromBegin, n);

					_ibufsize -= n;
					if(_ibufsize > 0)
					{
						std::char_traits<ExternT>::move( _ibuf, _ibuf + n, _ibufsize );
					}

					break;
				}
				case CodecT::error:
				{
					return traits_type::eof(); // TODO: throw exception
					break;
				}
			}

			this->setg(_gbuf + _pbmax - putback,              // start of read buffer
					   _gbuf + _pbmax,                        // gptr position
					   _gbuf + _pbmax + (toNext - toBegin) ); // end of read buffer

			return traits_type::to_int_type( *this->gptr() );
		}

		//TODO: signature like codecvt with ptr refs
		template <typename T>
		void copyChars(T* s1, const T* s2, size_t n)
		{
			std::char_traits<T>::copy(s1, s2, n);
		}

		//TODO: signature like codecvt with ptr refs
		template <typename A, typename B>
		void copyChars(A* s1, const B* s2, size_t n)
		{
			while(n-- > 0)
			{
				*s1 = *s2;
				++s1;
				++s2;
			}
		}
};

} // namespace Pt

#endif

