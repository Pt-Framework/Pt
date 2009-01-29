/*
 * Copyright (C) 2004 Marc Boris Duerner
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

namespace Pt {

/** @brief Converts character sequences with different encodings.

	This class derives from std::basic_streambuf which is the super-class of all stream buffer
	classes. Stream buffer classes are used to connect to an external device, transport characters
	from and to this external device and buffer the characters in an internal buffer.

	The internal character set can be specified using the template parameters 'char_type_', the
	external character set using 'extern_type_'. The external type is the input type and output
	type when reading from or writing to the external device. The internal type is the type
	which is used to internally store the data from the external device after the external
	format was converted using the Codec which is passed when constructing an object of this
	class.

	The Codec object which is passed as pointer to the constructor will afterwards be completely
	managed by this class and also be deleted by this class when it's destructed!

  @see std::basic_streambuf
*/
template <typename CharT, typename ByteT>
class BasicTextBuffer : public std::basic_streambuf<CharT>
{
    public:
        typedef ByteT extern_type;
        typedef CharT intern_type;
        typedef CharT char_type;
        typedef typename std::char_traits<CharT> traits_type;
        typedef typename traits_type::int_type int_type;
        typedef typename traits_type::pos_type pos_type;
        typedef typename traits_type::off_type off_type;
        typedef TextCodec<char_type, extern_type> CodecType;

    private:
        //! The external device (stream buffer) from which data is read and to which data is written.
        std::basic_streambuf<extern_type>* _streambuf;

        //! Contains the state of conversion.
        MBState _state;

        //! The codec which is used to convert character data from or to the external device.
        CodecType* _codec;

        static const int _pbmax = 4;

        static const int _ebufmax = 256;
        extern_type _ebuf[_ebufmax];
        int _ebufsize;

        static const int _ibufmax = 256;
        intern_type _ibuf[_ibufmax];
        int _ibufsize;

    public:
        /** @brief Creates a BasicTextBuffer using the given stream buffer and codec.

            The given stream buffer @a buffer is used as external device,
            buffered by this Text buffer and all input from and output to
            the external device is converted using the codec @a codec.

            Note: The Codec object which is passed as pointer will be
            managed by this class and also be deleted by this class
            on destruction.
        */
        BasicTextBuffer(std::basic_streambuf<extern_type>* buffer, CodecType* codec)
        : _streambuf(buffer)
        , _codec(codec)
        , _ebufsize(0)
        {
            this->setg(0, 0, 0);
            this->setp(0, 0);
        }

        ~BasicTextBuffer() throw()
        {
            try
            {
                this->terminate();
            }
            catch(...) {}

            if(_codec->refs() == 0)
                delete _codec;
        }

        void attach(std::basic_streambuf<extern_type>* buffer)
        {
            // TODO error handling
            this->terminate();
            _streambuf = buffer;
        }

        void detach()
        {
            // TODO error handling
            this->terminate();
            _streambuf = 0;
        }

        int terminate()
        {
            if( this->pptr() )
            {
                if( -1 == this->sync() )
                    return -1;

                if( ! _codec->always_noconv() )
                {
                    typename CodecType::result res = CodecType::error;
                    do
                    {
                        extern_type* next = 0;
                        res = _codec->unshift(_state, _ebuf, _ebuf + _ebufmax, next);
                        _ebufsize = next - _ebuf;

                        if(res == CodecType::error)
                        {
                            return -1;
                        }
                        else if(res == CodecType::ok || res == CodecType::partial)
                        {
                            if(_ebufsize > 0)
                            {
                                const std::streamsize n = _streambuf->sputn(_ebuf, _ebufsize);
                                _ebufsize -= n;
                                if(_ebufsize)
                                {
                                    if(_ebufsize < _ebufmax)
                                        std::char_traits<extern_type>::move(_ebuf, _ebuf + n, _ebufsize);

                                    return -1;
                                }
                            }
                        }
                    }
                    while(res == CodecType::partial);
                }
            }

            this->setp(0, 0);
            this->setg(0, 0, 0);
            _ebufsize = 0;
            _state = MBState();
            return 0;
        }

    protected:
        // inheritdoc
        virtual int sync()
        {
            if( this->pptr() )
            {
                std::streamsize n = this->pptr() - this->pbase();
                while( n > 0 )
                {
                    if( this->overflow( traits_type::eof() ) == traits_type::eof() )
                        return -1;

                    if( n == this->pptr() - this->pbase() )
                        return -1;

                    n = this->pptr() - this->pbase();
                }

                if(_ebufsize)
                {
                    n = _streambuf->sputn(_ebuf, _ebufsize);
                    _ebufsize -= n;
                    if(_ebufsize > 0)
                    {
                        if(_ebufsize < _ebufmax)
                            std::char_traits<extern_type>::move(_ebuf, _ebuf + n, _ebufsize);

                        return -1;
                    }
                }
            }

            return 0;
        }


        // inheritdoc
        virtual int_type overflow( int_type ch = traits_type::eof() )
        {
            if( ! _codec || ! _streambuf || this->gptr() )
                return traits_type::eof();

            if( this->pptr() <= this->pbase() ) // no put area
            {
                this->setp( _ibuf, _ibuf + _ibufmax );
				if( ! traits_type::eq_int_type(ch, traits_type::eof()) )
				{
					*( this->pptr() ) = traits_type::to_char_type(ch);
					this->pbump(1);
				}

				return traits_type::not_eof(ch);
            }

		typename CodecType::result res = CodecType::ok;
		while( this->pptr() > this->pbase() )
		{
			if(_ebufsize)
			{
				std::streamsize n = _streambuf->sputn(_ebuf, _ebufsize);
				_ebufsize -= n;
				if( _ebufsize )
				{
					if(_ebufsize < _ebufmax)
						std::char_traits<extern_type>::move(_ebuf, _ebuf + n, _ebufsize);

					return traits_type::eof();
				}
			}

			const char_type* fromBegin = _ibuf;
			const char_type* fromEnd   = this->pptr();
			const char_type* fromNext  = _ibuf;
			extern_type* toBegin       = _ebuf + _ebufsize;
			extern_type* toEnd         = _ebuf + _ebufmax;
			extern_type* toNext        = _ebuf + _ebufsize;

			res = _codec->out(_state, fromBegin, fromEnd, fromNext, toBegin, toEnd, toNext);
			if(res == CodecType::noconv)
			{
				// If no conversion is required, fromNext is set to fromBegin
				// and toNext is set to toBegin.
				size_t fromSize = fromEnd - fromBegin;
				size_t toSize   = toEnd - toBegin;
				size_t size     = toSize < fromSize ? toSize : fromSize;

				this->copyChars( toBegin, fromBegin, size );
				fromNext += size;
				toNext += size;
			}

			_ebufsize += toNext - toBegin;
			size_t leftover = fromEnd - fromNext;
			if(leftover)
			{
				std::char_traits<char_type>::move(_ibuf, fromNext, leftover);
			}

			this->setp( _ibuf + leftover, _ibuf + _ibufmax );

			if(res == CodecType::error)
				return traits_type::eof();
		}


            if( ! traits_type::eq_int_type(ch, traits_type::eof()) )
            {
                *( this->pptr() ) = traits_type::to_char_type(ch);
                this->pbump(1);
            }

            return traits_type::not_eof(ch);
        }


        // inheritdoc
        virtual int_type underflow()
        {
            if( ! _codec || ! _streambuf )
                return traits_type::eof();

            if( this->gptr() < this->egptr() )
                return traits_type::to_int_type( *this->gptr() );

            if( this->pptr() )
            {
                if(-1 == this->terminate())
                    return traits_type::eof();
            }

            size_t putback = _pbmax;

            if( this->gptr() )
            {
                putback = std::min<size_t>(this->gptr() - this->eback(), _pbmax);
                std::char_traits<char_type>::move( _ibuf + _pbmax - putback,
                                                   this->gptr() - putback,
                                                   putback );
                this->setg(_ibuf + _pbmax - putback, _ibuf + _pbmax, _ibuf + _pbmax);
            }

			bool atEof = false;
			if( _ebufsize < _ebufmax)
			{
				std::streamsize n = _streambuf->sgetn( _ebuf + _ebufsize, _ebufmax - _ebufsize );
				_ebufsize += n;
				if(n == 0)
					atEof = true;
			}

			const extern_type* fromBegin = _ebuf;
			const extern_type* fromEnd   = _ebuf + _ebufsize;
			const extern_type* fromNext  = _ebuf;
			char_type* toBegin           = _ibuf + _pbmax;
			char_type* toEnd             = _ibuf + _pbmax + _ibufmax;
			char_type* toNext            = _ibuf;

			typename CodecType::result r = CodecType::error;
			r = _codec->in(_state, fromBegin, fromEnd, fromNext, toBegin, toEnd, toNext);

			if(r == CodecType::noconv)
			{
				// copy characters and advance fromNext and toNext
				int n =_ebufsize > _ibufmax ? _ibufmax : _ebufsize ;
				this->copyChars(toBegin, fromBegin, n);
				_ebufsize -= n;
				fromNext += n;
				toNext += n;
			}

			std::streamsize consumed = fromNext - fromBegin;
			if(consumed)
			{
				std::char_traits<extern_type>::move( _ebuf, _ebuf + consumed, _ebufsize );
				_ebufsize -= consumed;
			}

			std::streamsize generated = toNext - toBegin;
			if(generated)
			{
				this->setg(_ibuf + _pbmax - putback,     // start of read buffer
				           _ibuf + _pbmax,               // gptr position
				           _ibuf + _pbmax + generated ); // end of read buffer
			}

			if(r == CodecType::error)
				return traits_type::eof();

			if( this->gptr() < this->egptr() )
				return traits_type::to_int_type( *this->gptr() );

			if(r == CodecType::partial && atEof)
				return traits_type::eof();

            return traits_type::eof();
        }

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

