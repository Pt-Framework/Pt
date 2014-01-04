/*
 * Copyright (C) 2004-2013 Marc Boris Duerner
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
#include <Pt/String.h>
#include <Pt/TextCodec.h>
#include <Pt/StreamBuffer.h>
#include <Pt/ConversionError.h>

namespace Pt {

/** @brief Converts character sequences with different encodings.

    @ingroup Unicode
*/
template <typename CharT, typename ByteT>
class BasicTextBuffer : public BasicStreamBuffer<CharT>
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
        typedef typename CodecType::result CodecResult;
        typedef MBState state_type;

    public:
        /** @brief Creates a BasicTextBuffer using the given stream buffer and codec.

            The given stream buffer @a target is used as external device,
            buffered by this Text buffer and all input from and output to
            the external device is converted using the codec @a codec.

            Note: The Codec object which is passed as pointer will be
            managed by this class and also be deleted by this class
            on destruction.
        */
        explicit BasicTextBuffer(std::basic_ios<extern_type>& target, CodecType* codec = 0)
        : _ebufsize(0)
        , _codec(codec)
        , _codecRefs( codec ? codec->refs() : 0 )
        , _target(&target)
        { 
            this->setg(0, 0, 0);
            this->setp(0, 0);
        }

        explicit BasicTextBuffer(CodecType* codec = 0)
        : _ebufsize(0)
        , _codec(codec)
        , _codecRefs( codec ? codec->refs() : 0 )
        , _target(0)
        {
            this->setg(0, 0, 0);
            this->setp(0, 0);
        }

        ~BasicTextBuffer() throw()
        {
            // if _codecRefs is greater than 0, the codec might have been
            // destroyed before the text buffer, therefore we work with a
            // separate refcount to mark owned codecs
            
            if(_codecRefs == 0)
                delete _codec;
        }

        CodecType* codec()
        { return _codec; }

        /** @brief Sets the text codec.
        
            The caller must ensure that the codec does not run out of scope
            before the text buffer is destructed. 
        */
        void setCodec(CodecType* codec)
        {           
            _state = state_type();

            if(_codecRefs == 0)
                delete _codec;

            _codec = codec;
            _codecRefs = codec ? codec->refs() : 0;
        }

        void attach(std::basic_ios<extern_type>& target)
        {
            _target = &target;
        }

        void detach()
        {
            _target = 0;
        }

        /** @brief Resets the buffer and target.

            The target is detached and the buffer content is discarded.
            The codec is kept, if one was set previously.  
        */
        void reset()
        {
            discard();
            detach();
        }

        /** @brief Resets the buffer and target.

            The buffer is discarded and the buffer is attached to the new
            target. The codec is kept, if one was set previously. 
        */
        void reset(std::basic_ios<extern_type>& target)
        {
            discard();
            attach(target);
        }

        void discard()
        {
            _ebufsize = 0;
            this->setp(0, 0);
            this->setg(0, 0, 0);
            
            _state = state_type();
        }

        void import(std::streamsize size = 0)
        {
            if( this->pptr() )
            {
                if( -1 == this->terminate() )
                    return;
            }

            if( ! this->gptr() )
            {
                this->setg(_ibuf, _ibuf, _ibuf);
            }

            std::basic_streambuf<extern_type>* rdbuf = _target ? _target->rdbuf()
                                                               : 0;
            
            // special case: read available input
            if(size == 0 && rdbuf)
                size = rdbuf->in_avail();

            // not more than input buffer size
            const std::streamsize ebufavail = _ebufmax - _ebufsize;
            size = ebufavail < size ? ebufavail : size;

            if(size > 0 && rdbuf)
            {
                std::streamsize n = rdbuf->sgetn( _ebuf + _ebufsize,  size );
                _ebufsize += static_cast<int>(n);
                if(n <= 0)
                {
                    // rdbuf == 0 means "at end of input"
                    rdbuf = 0;
                }  
            }

            if( this->gptr() - this->eback() > _pbmax)
            {
                std::streamsize movelen = this->egptr() - this->gptr() + _pbmax;
                std::char_traits<char_type>::move( _ibuf,
                                                   this->gptr() - _pbmax,
                                                   static_cast<std::size_t>(movelen));
                this->setg(_ibuf, _ibuf + _pbmax, _ibuf + movelen);
            }

            typename CodecType::result r = decode();

            // fail if partial conversion at the end of input
            // rdbuf == 0 means "at end of input"
            if( rdbuf == 0 && _ebufsize == 0 && r == CodecType::partial)
                throw ConversionError("character encoding");
        }

        std::streamsize import(const extern_type* buf, std::streamsize size)
        {
            if( this->pptr() )
            {
                if( -1 == this->terminate() )
                    return 0;
            }

            if( ! this->gptr() )
            {
                this->setg(_ibuf, _ibuf, _ibuf);
            }

            const std::streamsize bufavail = _ebufmax - _ebufsize;
            size = bufavail < size ? bufavail : size;
            if(size > 0)
            {
                std::char_traits<extern_type>::copy( _ebuf + _ebufsize, buf, static_cast<std::size_t>(size) );
                _ebufsize += static_cast<int>(size);
            }

            if( this->gptr() - this->eback() > _pbmax)
            {
                std::streamsize movelen = this->egptr() - this->gptr() + _pbmax;
                std::char_traits<char_type>::move( _ibuf,
                                                   this->gptr() - _pbmax,
                                                   static_cast<std::size_t>(movelen));
                this->setg(_ibuf, _ibuf + _pbmax, _ibuf + movelen);
            }

            decode();

            return size;
        }

        int terminate()
        {
            if( this->pptr() )
            {
                if( -1 == this->sync() )
                    return -1;

                if( _target && _target->rdbuf() && _codec && ! _codec->always_noconv() )
                {
                    typename CodecType::result res = CodecType::error;
                    do
                    {
                        extern_type* next = 0;
                        res = _codec->unshift(_state, _ebuf, _ebuf + _ebufmax, next);
                        _ebufsize = static_cast<int>(next - _ebuf);

                        if(res == CodecType::error)
                        {
                            throw ConversionError("character encoding");
                        }
                        else if(res == CodecType::ok || res == CodecType::partial)
                        {
                            if(_ebufsize > 0)
                            {
                                _ebufsize -= static_cast<int>(_target->rdbuf()->sputn(_ebuf, _ebufsize));
                                if(_ebufsize)
                                    return -1;
                            }
                        }
                    }
                    while(res == CodecType::partial);
                }
            }

            discard();
            
            return 0;
        }

    protected:
        // inheritdoc
        virtual int sync()
        {
            if( this->pptr() )
            {
                // Try to write out the whole buffer to the underlying stream.
                while( this->pptr() > this->pbase() )
                {
                    if( this->overflow( traits_type::eof() ) == traits_type::eof() )
                        return -1;
                }
            }

            return 0;
        }

        // inheritdoc
        virtual std::streamsize showmanyc()
        {
            return _target && _target->rdbuf() ? _target->rdbuf()->in_avail() 
                                               : -1;
        }

        // inheritdoc
        virtual int_type overflow( int_type ch = traits_type::eof() )
        {
            if( ! _target || ! _target->rdbuf() || this->gptr() )
                return traits_type::eof();

            if( ! this->pptr() )
            {
                this->setp( _ibuf, _ibuf + _ibufmax );
            }

            while( this->pptr() > this->pbase() )
            {
                const char_type* fromBegin = _ibuf;
                const char_type* fromEnd   = this->pptr();
                const char_type* fromNext  = fromBegin;
                extern_type* toBegin       = _ebuf + _ebufsize;
                extern_type* toEnd         = _ebuf + _ebufmax;
                extern_type* toNext        = toBegin;

                typename CodecType::result res = CodecType::noconv;
                if(_codec)
                    res = _codec->out(_state, fromBegin, fromEnd, fromNext, toBegin, toEnd, toNext);

                if(res == CodecType::noconv)
                {
                    std::size_t fromSize = fromEnd - fromBegin;
                    std::size_t toSize   = toEnd - toBegin;
                    std::size_t size     = toSize < fromSize ? toSize : fromSize;

                    this->copyChars( toBegin, fromBegin, size );

                    fromNext += size;
                    toNext += size;
                }

                _ebufsize += static_cast<int>(toNext - toBegin);
                std::size_t leftover = fromEnd - fromNext;
                if(leftover && fromNext > fromBegin)
                {
                    std::char_traits<char_type>::move(_ibuf, fromNext, leftover);
                }

                this->setp( _ibuf, _ibuf + _ibufmax );
                this->pbump( static_cast<int>(leftover) );

                if(res == CodecType::error)
                    throw ConversionError("character encoding");

                if(res == CodecType::partial && _ebufsize == 0)
                    break;

                _ebufsize -= static_cast<int>(_target->rdbuf()->sputn(_ebuf, _ebufsize));
                if(_ebufsize)
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
            if( this->gptr() < this->egptr() )
                return traits_type::to_int_type( *this->gptr() );

            import(_ebufmax);

            return this->gptr() < this->egptr() ? traits_type::to_int_type( *this->gptr() )
                                                : traits_type::eof();
        }

    private:
        CodecResult decode()
        {
            const extern_type* fromBegin = _ebuf;
            const extern_type* fromEnd   = _ebuf + _ebufsize;
            const extern_type* fromNext  = fromBegin;
            char_type* toBegin           = this->egptr();
            char_type* toEnd             = _ibuf + _ibufmax;
            char_type* toNext            = toBegin;

            typename CodecType::result r = CodecType::noconv;
            if(_codec)
                r = _codec->in(_state, fromBegin, fromEnd, fromNext, toBegin, toEnd, toNext);

            if(r == CodecType::noconv)
            {
                // copy characters and advance fromNext and toNext
                int n =_ebufsize > _ibufmax ? _ibufmax : _ebufsize ;
                this->copyChars(toBegin, fromBegin, n);
                fromNext += n;
                toNext += n;
            }

            std::streamsize consumed = fromNext - fromBegin;
            if(consumed > 0)
            {
                _ebufsize -= static_cast<int>(consumed);
                std::char_traits<extern_type>::move( _ebuf, _ebuf + consumed, _ebufsize);
            }

            std::streamsize generated = toNext - toBegin;
            if(generated)
            {
                this->setg(this->eback(),               // start of read buffer
                           this->gptr(),                // gptr position
                           this->egptr() + generated ); // end of read buffer
            }

            if(r == CodecType::error)
                throw ConversionError("character encoding");

            return r;
        }

        template <typename T>
        void copyChars(T* s1, const T* s2, std::size_t n)
        {
            std::char_traits<T>::copy(s1, s2, n);
        }

        template <typename A, typename B>
        void copyChars(A* s1, const B* s2, std::size_t n)
        {
            while(n-- > 0)
            {
                *s1 = *s2;
                ++s1;
                ++s2;
            }
        }

        template <typename A>
        void copyChars(A* s1, const Char* s2, std::size_t n)
        {
            while(n-- > 0)
            {
                *s1 = s2->value();
                ++s1;
                ++s2;
            }
        }

    private:
        static const int _pbmax = 4;

        static const int _ebufmax = 256;
        extern_type _ebuf[_ebufmax];
        int _ebufsize;

        static const int _ibufmax = 256;
        intern_type _ibuf[_ibufmax];

        state_type _state;
        CodecType* _codec;
        std::size_t _codecRefs;
        std::basic_ios<extern_type>* _target;
};

typedef BasicTextBuffer<Pt::Char, char> TextBuffer;

///** @brief Converts 8-bit character sequences to unicode.
//*/
//class PT_API TextBuffer : public BasicTextBuffer<Pt::Char, char>
//{
//    public:
//        typedef TextCodec<Pt::Char, char> Codec;
//
//    public:
//        /** @brief Constructs a new TextBuffer
//        */
//        explicit TextBuffer(std::ios& buffer, Codec* codec = 0);
//
//        explicit TextBuffer(Codec* codec = 0);
//};

} // namespace Pt

#endif // Pt_TextBuffer_h
