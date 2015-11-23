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

#ifndef Pt_TextStream_h
#define Pt_TextStream_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/TextBuffer.h>
#include <Pt/IOStream.h>

namespace Pt {

/** @brief Converts character sequences using a codec.

    This stream decodes an external character sequence using a codec. Reading
    from the stream will convert from the the encoding of external characters.

    @ingroup Unicode
*/
template <typename CharT, typename ByteT>
class BasicTextIStream : public BasicIStream<CharT>
{
    public:
        //! @brief External character type
        typedef ByteT extern_type;
        
        //! @brief Internal character type
        typedef CharT intern_type;

        //! @brief Internal character type
        typedef CharT char_type;

        //! @brief Internal character traits
        typedef typename std::char_traits<CharT> traits_type;

        //! @brief Integer type
        typedef typename traits_type::int_type int_type;

        //! @brief Stream position type
        typedef typename traits_type::pos_type pos_type;

        //! @brief Stream offset type
        typedef typename traits_type::off_type off_type;

        //! @brief External stream type
        typedef std::basic_istream<extern_type> StreamType;

        //! @brief Codec type
        typedef TextCodec<char_type, extern_type> CodecType;

    public:
        /** @brief Construct with input stream and codec.

            The input stream @a is is used to read a character sequence and
            convert it using the codec @a codec. The codec object which is
            passed as pointer will be managed by this class and deleted if
            its reference count reaches 0.
        */
        BasicTextIStream(StreamType& is, CodecType* codec)
        : BasicIStream<intern_type>(0)
        , _tbuffer( is, codec )
        { 
            this->setBuffer(&_tbuffer); 
        }

        /** @brief Construct with codec.

            The codec object which is passed as pointer will be managed by
            this class and deleted if its reference count reaches 0.
        */
        explicit BasicTextIStream(CodecType* codec)
        : BasicIStream<intern_type>(0)
        , _tbuffer(codec )
        { 
            this->setBuffer(&_tbuffer); 
        }

        /** @brief Destructor.
        */
        ~BasicTextIStream()
        {  }

        /** @brief Returns the used code or a nullptr.
        */
        CodecType* codec()
        { 
            return _tbuffer.codec(); 
        }

        /** @brief Sets the text codec.
            
            The codec object which is passed as pointer will be managed by
            this class and deleted if its reference count reaches 0.
        */
        void setCodec(CodecType* codec)
        {           
            _tbuffer.setCodec(codec);
        }

        /** @brief Attach to external target.
        */
        void attach(StreamType& is)
        {
            _tbuffer.attach(is);
        }
        
        /** @brief Detach from external target.
        */
        void detach()
        {
            _tbuffer.detach();
        }

        /** @brief Discards the buffer.
        */
        void discard()
        {
            _tbuffer.discard();
        }

        /** @brief Resets the buffer and target.

            The target is detached and the buffer content is discarded.
            The codec is kept, if one was set previously.  
        */
        void reset()
        {
            _tbuffer.reset();
        }

        /** @brief Resets the buffer and target.

            Attaches to the new target and discards the buffer. The codec is
            kept, if one was set previously. 
        */
        void reset(StreamType& is)
        {
            _tbuffer.reset(is);
        }

        /** @brief Returns the stream buffer.
        */
        BasicTextBuffer<intern_type, extern_type>& textBuffer()
        { return _tbuffer; }

    private:
        BasicTextBuffer<intern_type, extern_type> _tbuffer;
};


/** @brief Converts character sequences using a codec.

    This stream encodes an external character sequence using a codec. Writing
    to the stream will convert the written characters to the external character
    types in the external encoding.

    @ingroup Unicode
*/
template <typename CharT, typename ByteT>
class BasicTextOStream : public BasicOStream<CharT>
{
    public:
        //! @brief External character type
        typedef ByteT extern_type;
        
        //! @brief Internal character type
        typedef CharT intern_type;

        //! @brief Internal character type
        typedef CharT char_type;

        //! @brief Internal character traits
        typedef typename std::char_traits<CharT> traits_type;

        //! @brief Integer type
        typedef typename traits_type::int_type int_type;

        //! @brief Stream position type
        typedef typename traits_type::pos_type pos_type;

        //! @brief Stream offset type
        typedef typename traits_type::off_type off_type;

        //! @brief External stream type
        typedef std::basic_ostream<extern_type> StreamType;

        //! @brief Codec type
        typedef TextCodec<char_type, extern_type> CodecType;

    public:
        /** @brief Construct with output stream and codec.

            The character sequence converted by the @a codec is written to
            the output stream @a os. The codec object which is passed as
            a pointer will be managed by this class and deleted if its
            reference count reaches 0.
        */
        BasicTextOStream(StreamType& os, CodecType* codec)
        : BasicOStream<intern_type>(0)
        , _tbuffer( os , codec )
        { 
            this->setBuffer(&_tbuffer); 
        }

        /** @brief Construct with codec.

            The codec object which is passed as pointer will be managed by
            this class and deleted if its reference count reaches 0.
        */
        explicit BasicTextOStream(CodecType* codec)
        : BasicOStream<intern_type>(0)
        , _tbuffer( codec )
        { 
            this->setBuffer(&_tbuffer); 
        }

        /** @brief Destructor.
        */
        ~BasicTextOStream()
        {  }

        /** @brief Returns the used code or a nullptr.
        */
        CodecType* codec()
        { 
            return _tbuffer.codec(); 
        }

        /** @brief Sets the text codec.
            
            The codec object which is passed as pointer will be managed by
            this class and deleted if its reference count reaches 0.
        */
        void setCodec(CodecType* codec)
        {           
            _tbuffer.setCodec(codec);
        }

        /** @brief Attach to external target.
        */
        void attach(StreamType& os)
        {
            _tbuffer.attach(os);
        }

        /** @brief Detach from external target.
        */
        void detach()
        {
            _tbuffer.detach();
        }

        /** @brief Discards the buffer.
        */
        void discard()
        {
            _tbuffer.discard();
        }

        /** @brief Resets the buffer and target.

            The target is detached and the buffer content is discarded.
            The codec is kept, if one was set previously.  
        */
        void reset()
        {
            _tbuffer.reset();
        }

        /** @brief Resets the buffer and target.

            Attaches to the new target and discards the buffer. The codec is
            kept, if one was set previously. 
        */
        void reset(StreamType& os)
        {
            _tbuffer.reset(os);
        }

        /** @brief Returns the stream buffer.
        */
        BasicTextBuffer<intern_type, extern_type>& textBuffer()
        { return _tbuffer; }

    private:
        BasicTextBuffer<intern_type, extern_type> _tbuffer;
};

/** @brief Converts character sequences using a codec.

    This stream encodes and decodes an external character sequence using a 
    codec. Writing to the stream will convert the written characters to the
    external character types in the external encoding. Reading from the stream
    will convert from the the encoding of external characters.

    @ingroup Unicode
*/
template <typename CharT, typename ByteT>
class BasicTextStream : public BasicIOStream<CharT>
{
    public:
        //! @brief External character type
        typedef ByteT extern_type;
        
        //! @brief Internal character type
        typedef CharT intern_type;

        //! @brief Internal character type
        typedef CharT char_type;

        //! @brief Internal character traits
        typedef typename std::char_traits<CharT> traits_type;

        //! @brief Integer type
        typedef typename traits_type::int_type int_type;

        //! @brief Stream position type
        typedef typename traits_type::pos_type pos_type;

        //! @brief Stream offset type
        typedef typename traits_type::off_type off_type;

        //! @brief External stream type
        typedef std::basic_iostream<extern_type> StreamType;

        //! @brief Codec type
        typedef TextCodec<char_type, extern_type> CodecType;

    public:
        /** @brief Construct by stream and codec.

            The stream @a ios is used to read and write a character sequence 
            and convert it using a @a codec. The codec object which is passed
            as a pointer will be managed by this class and deleted if its
            reference count reaches 0.
        */
        BasicTextStream(StreamType& ios, CodecType* codec)
        : BasicIOStream<intern_type>(0)
        , _tbuffer( ios, codec)
        { 
            this->setBuffer(&_tbuffer); 
        }

        /** @brief Construct with codec.

            The codec object which is passed as pointer will be managed by
            this class and deleted if its reference count reaches 0.
        */
        explicit BasicTextStream(CodecType* codec)
        : BasicIOStream<intern_type>(0)
        , _tbuffer(codec)
        { 
            this->setBuffer(&_tbuffer); 
        }

        /** @brief Destructor.
        */
        ~BasicTextStream()
        { }

        /** @brief Returns the used code or a nullptr.
        */
        CodecType* codec()
        { 
            return _tbuffer.codec(); 
        }

        /** @brief Sets the text codec.
            
            The codec object which is passed as pointer will be managed by
            this class and deleted if its reference count reaches 0.
        */
        void setCodec(CodecType* codec)
        {           
            _tbuffer.setCodec(codec);
        }

        /** @brief Attach to external target.
        */
        void attach(StreamType& ios)
        {
            _tbuffer.attach(ios);
        }

        /** @brief Detach from external target.
        */
        void detach()
        {
            _tbuffer.detach();
        }

        /** @brief Discards the buffer.
        */
        void discard()
        {
            _tbuffer.discard();
        }

        /** @brief Resets the buffer and target.

            The target is detached and the buffer content is discarded.
            The codec is kept, if one was set previously.  
        */
        void reset()
        {
            _tbuffer.reset();
        }

        /** @brief Resets the buffer and target.

            Attaches to the new target and discards the buffer. The codec is
            kept, if one was set previously. 
        */
        void reset(StreamType& ios)
        {
            _tbuffer.reset(ios);
        }

        /** @brief Returns the stream buffer.
        */
        BasicTextBuffer<intern_type, extern_type>& textBuffer()
        { return _tbuffer; }

    private:
        BasicTextBuffer<intern_type, extern_type> _tbuffer;
};

/** @class Pt::TextIStream TextStream.h "Pt/TextStream.h"
    @brief Text input stream for unicode character conversion.

    This class is a typedef of the BasicTextIStream template for
    the unicode character type Pt::Char:

    @code
    typedef BasicTextIStream<Pt::Char, char> TextBuffer;
    @endcode

    @ingroup Unicode
*/
typedef BasicTextIStream<Char, char>  TextIStream;

/** @class Pt::TextOStream TextStream.h "Pt/TextStream.h"
    @brief Text output stream for unicode character conversion.

    This class is a typedef of the BasicTextOStream template for
    the unicode character type Pt::Char:

    @code
    typedef BasicTextOStream<Pt::Char, char> TextBuffer;
    @endcode

    @ingroup Unicode
*/
typedef BasicTextOStream<Char, char>  TextOStream;

/** @class Pt::TextStream TextStream.h "Pt/TextStream.h"
    @brief Text stream for unicode character conversion.

    This class is a typedef of the BasicTextStream template for
    the unicode character type Pt::Char:

    @code
    typedef BasicTextStream<Pt::Char, char> TextBuffer;
    @endcode

    @ingroup Unicode
*/
typedef BasicTextStream<Char, char> TextStream;


///** @brief Text Input Stream for Character conversion
//
//    @ingroup Unicode
//*/
//class PT_API TextIStream : public BasicTextIStream<Char, char>
//{
//    public:
//        typedef TextCodec<Pt::Char, char> Codec;
//
//    public:
//        /** @brief Constructor
//
//            The stream will read bytes from \a is and use the codec \a codec
//            for character conversion. The codec will be destroyed by the
//            buffer of this stream if the codec was constructed with a
//            refcount of 0.
//        */
//        TextIStream(std::istream& is, Codec* codec);
//
//        explicit TextIStream(Codec* codec);
//
//        ~TextIStream();
//};
//
//
///** @brief Text Output Stream for Character conversion
//
//    @ingroup Unicode
//*/
//class PT_API TextOStream : public BasicTextOStream<Char, char>
//{
//    public:
//        typedef TextCodec<Pt::Char, char> Codec;
//
//    public:
//        /** @brief Constructor
//
//            The stream will write bytes to \a os and use the codec \a codec
//            for character conversion. The codec will be destroyed by the
//            buffer of this stream if the codec was constructed with a
//            refcount of 0.
//        */
//        TextOStream(std::ostream& os, Codec* codec);
//
//        explicit TextOStream(Codec* codec);
//
//        ~TextOStream();
//};
//
//
///** @brief Text Stream for Character conversion
//
//    @ingroup Unicode
//*/
//class PT_API TextStream : public BasicTextStream<Char, char>
//{
//    public:
//        typedef TextCodec<Pt::Char, char> Codec;
//
//    public:
//        /** @brief Constructor
//
//            The stream will write or write bytes to \a ios and use the codec
//            \a codec for character conversion. The codec will be destroyed
//            by the buffer of this stream if the codec was constructed with a
//            refcount of 0.
//        */
//        TextStream(std::iostream& ios, Codec* codec);
//
//        explicit TextStream(Codec* codec);
//
//        ~TextStream();
//};

} // namespace Pt

#endif // Pt_TextStream_h
