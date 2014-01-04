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

/** @brief Converts character sequences using a Codec.

    This generic stream, which only supports input, wraps another input-stream and converts the
    character data which is received from the stream on-the-fly using a Codec. The data which is
    received from the wrapped input-stream is buffered.

    This class derives from std::basic_istream which is the super-class of input-stream classes.
    Stream classes are used to connect to an external device and transport characters
    from this external device.

    The internal character set can be specified using the template parameters 'I', the
    external character set using 'E'. The external type is the input type and output
    type when reading from or writing to the external device. The internal type is the type
    which is used to internally store the data from the external device after the external
    format was converted using the Codec which is passed when constructing an object of this
    class.

    The Codec object which is passed as pointer to the constructor will afterwards be
    managed by this class and also be deleted by this class when it's destructed!

    @ingroup Unicode
*/
template <typename CharT, typename ByteT>
class BasicTextIStream : public BasicIStream<CharT>
{
    public:
        typedef ByteT extern_type;
        typedef CharT intern_type;
        typedef CharT char_type;
        typedef typename std::char_traits<CharT> traits_type;
        typedef typename traits_type::int_type int_type;
        typedef typename traits_type::pos_type pos_type;
        typedef typename traits_type::off_type off_type;
        typedef std::basic_istream<extern_type> StreamType;
        typedef TextCodec<char_type, extern_type> CodecType;

    public:
        /** @brief Construct by input stream and codec.

            The input stream @a is is used ro read a character sequence and
            convert it using the codec @a codec. The Codec object which is
            passed as pointer will afterwards be managed by this class and
            also be deleted on destruction
        */
        BasicTextIStream(StreamType& is, CodecType* codec)
        : BasicIStream<intern_type>(0)
        , _tbuffer( is, codec )
        {
            this->init(&_tbuffer);
        }

        explicit BasicTextIStream(CodecType* codec)
        : BasicIStream<intern_type>(0)
        , _tbuffer(codec )
        {
            this->init(&_tbuffer);
        }

        ~BasicTextIStream()
        {  }

        CodecType* codec()
        { 
            return _tbuffer.codec(); 
        }

        void setCodec(CodecType* codec)
        {           
            _tbuffer.setCodec(codec);
        }

        void attach(StreamType& is)
        {
            _tbuffer.attach(is);
        }

        void detach()
        {
            _tbuffer.detach();
        }

        void discard()
        {
            _tbuffer.discard();
        }

        void reset()
        {
            _tbuffer.reset();
        }

        void reset(StreamType& is)
        {
            _tbuffer.reset(is);
        }

        void terminate()
        {
            _tbuffer.terminate();
        }

        BasicTextBuffer<intern_type, extern_type>& textBuffer()
        { return _tbuffer; }

    private:
        BasicTextBuffer<intern_type, extern_type> _tbuffer;
};


/** @brief Converts character sequences using a Codec.

    This generic stream, which only supports output, wraps another input-stream and converts the
    character data which is received from the stream on-the-fly using a Codec. The data which is
    received from the wrapped input-stream is buffered.

    This class derives from std::basic_istream which is the super-class of input-stream classes.
    Stream classes are used to connect to an external device and transport characters
    from this external device.

    The internal character set can be specified using the template parameters 'I', the
    external character set using 'E'. The external type is the input type and output
    type when reading from or writing to the external device. The internal type is the type
    which is used to internally store the data from the external device after the external
    format was converted using the Codec which is passed when constructing an object of this
    class.

    The Codec object which is passed as pointer to the constructor will afterwards be
    managed by this class and also be deleted by this class when it's destructed!

    @ingroup Unicode
*/
template <typename CharT, typename ByteT>
class BasicTextOStream : public BasicOStream<CharT>
{
    public:
        typedef ByteT extern_type;
        typedef CharT intern_type;
        typedef CharT char_type;
        typedef typename std::char_traits<CharT> traits_type;
        typedef typename traits_type::int_type int_type;
        typedef typename traits_type::pos_type pos_type;
        typedef typename traits_type::off_type off_type;
        typedef std::basic_ostream<extern_type> StreamType;
        typedef TextCodec<char_type, extern_type> CodecType;

    public:
        /** @brief Construct with output stream and codec.

            The output stream @a os is used to write a character sequence
            which has been converted using the codec @a codec. The Codec
            object which is passed as pointer will afterwards be managed
            by this class and be deleted on destruction
        */
        BasicTextOStream(StreamType& os, CodecType* codec)
        : BasicOStream<intern_type>(0)
        , _tbuffer( os , codec )
        { this->init(&_tbuffer); }

        explicit BasicTextOStream(CodecType* codec)
        : BasicOStream<intern_type>(0)
        , _tbuffer( codec )
        { this->init(&_tbuffer); }

        ~BasicTextOStream()
        {  }

        CodecType* codec()
        { 
            return _tbuffer.codec(); 
        }

        void setCodec(CodecType* codec)
        {           
            _tbuffer.setCodec(codec);
        }

        void attach(StreamType& os)
        {
            _tbuffer.attach(os);
        }

        void detach()
        {
            _tbuffer.detach();
        }

        void discard()
        {
            _tbuffer.discard();
        }

        void reset()
        {
            _tbuffer.reset();
        }

        void reset(StreamType& os)
        {
            _tbuffer.reset(os);
        }

        void terminate()
        {
            _tbuffer.terminate();
        }

        BasicTextBuffer<intern_type, extern_type>& textBuffer()
        { return _tbuffer; }

    private:
        BasicTextBuffer<intern_type, extern_type> _tbuffer;
};

/** @brief Converts character sequences using a Codec.

    This generic stream, which only supports input and output, wraps another input-stream and converts the
    character data which is received from the stream on-the-fly using a Codec. The data which is
    received from the wrapped input-stream is buffered.

    This class derives from std::basic_istream which is the super-class of input-stream classes.
    Stream classes are used to connect to an external device and transport characters
    from this external device.

    The internal character set can be specified using the template parameters 'I', the
    external character set using 'E'. The external type is the input type and output
    type when reading from or writing to the external device. The internal type is the type
    which is used to internally store the data from the external device after the external
    format was converted using the Codec which is passed when constructing an object of this
    class.

    The Codec object which is passed as pointer to the constructor will afterwards be
    managed by this class and also be deleted by this class when it's destructed!

    @ingroup Unicode
*/
template <typename CharT, typename ByteT>
class BasicTextStream : public BasicIOStream<CharT>
{
    public:
        typedef ByteT extern_type;
        typedef CharT intern_type;
        typedef CharT char_type;
        typedef typename std::char_traits<CharT> traits_type;
        typedef typename traits_type::int_type int_type;
        typedef typename traits_type::pos_type pos_type;
        typedef typename traits_type::off_type off_type;
        typedef std::basic_iostream<extern_type> StreamType;
        typedef TextCodec<char_type, extern_type> CodecType;

    public:
        /** @brief Construct by stream and codec.

            The stream @a ios is used to read a character sequences and
            convert is using the codec @a codec and write character
            sequences which have been converted using the codec @a codec.
            The codec object which is passed as pointer will afterwards
            be managed by this class and be deleted on destruction
        */
        BasicTextStream(StreamType& ios, CodecType* codec)
        : BasicIOStream<intern_type>(0)
        , _tbuffer( ios, codec)
        { this->init(&_tbuffer); }

        explicit BasicTextStream(CodecType* codec)
        : BasicIOStream<intern_type>(0)
        , _tbuffer(codec)
        { this->init(&_tbuffer); }

        ~BasicTextStream()
        { }

        CodecType* codec()
        { 
            return _tbuffer.codec(); 
        }

        void setCodec(CodecType* codec)
        {           
            _tbuffer.setCodec(codec);
        }

        void attach(StreamType& ios)
        {
            _tbuffer.attach(ios);
        }

        void detach()
        {
            _tbuffer.detach();
        }

        void discard()
        {
            _tbuffer.discard();
        }

        void reset()
        {
            _tbuffer.reset();
        }

        void reset(StreamType& ios)
        {
            _tbuffer.reset(ios);
        }

        void terminate()
        {
            _tbuffer.terminate();
        }

        BasicTextBuffer<intern_type, extern_type>& textBuffer()
        { return _tbuffer; }

    private:
        BasicTextBuffer<intern_type, extern_type> _tbuffer;
};

typedef BasicTextIStream<Char, char>  TextIStream;
typedef BasicTextOStream<Char, char>  TextOStream;
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
