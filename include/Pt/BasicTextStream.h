/***************************************************************************
 *   Copyright (C) 2004 Marc Boris D�rner                                  *
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

#ifndef Pt_BasicTextStream_h
#define Pt_BasicTextStream_h

#include <Pt/Api.h>
#include <Pt/String.h>
#include <Pt/BasicTextBuffer.h>
#include <iostream>

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

        @see std::basic_istream
    */
    template <typename I, typename E>
    class BasicTextIStream : public std::basic_istream<I>
    {
        public:
            typedef I InternT;
            typedef E ExternT;
            typedef TextCodec<InternT, ExternT> CodecT;

        public:
            /** @brief Construct by input stream and codec.

               The input stream @a is is used ro read a character sequence and
               convert it using the codec @a codec. The Codec object which is
               passed as pointer will afterwards be managed by this class and
               also be deleted on destruction
            */
            BasicTextIStream(std::basic_istream<ExternT>& is, CodecT* codec)
            : _buffer( is.rdbuf() , codec )
            {
                init(&_buffer);
            }

            //! @brief Deletes to codec.
            ~BasicTextIStream()
            {  }

            void attach(std::basic_iostream<ExternT>& ios)
            {
                _buffer.attach( ios.rdbuf() );
                this->clear();
            }

        private:
            BasicTextBuffer<InternT, ExternT> _buffer;
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

        @see std::basic_istream
    */
    template <typename I, typename E>
    class BasicTextOStream : public std::basic_ostream<I> {
        public:
            typedef I InternT;
            typedef E ExternT;
            typedef TextCodec<InternT, ExternT> CodecT;

        public:
            /** @brief Construct by output stream and codec.

               The output stream @a os is used ro write a character sequence
               which has been converted using the codec @a codec. The Codec
               object which is passed as pointer will afterwards be managed
               by this class and be deleted on destruction
            */
            BasicTextOStream(std::basic_ostream<ExternT>& os, CodecT* codec)
            : _buffer( os.rdbuf() , codec )
            { init(&_buffer); }

            //! @brief Deletes to codec.
            ~BasicTextOStream()
            {  }

            void attach(std::basic_iostream<ExternT>& ios)
            {
                _buffer.attach( ios.rdbuf() );
                this->clear();
            }

        private:
            BasicTextBuffer<InternT, ExternT> _buffer;
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

        @see std::basic_istream
    */
    template <typename I, typename E>
    class BasicTextStream : public std::basic_iostream<I> {
        public:
            typedef I InternT;
            typedef E ExternT;
            typedef TextCodec<InternT, ExternT> CodecT;

        public:
            /** @brief Construct by stream and codec.

               The stream @a ios is used to read a character sequences and
               convert is using the codec @a codec and write character
               sequences which have been converted using the codec @a codec.
               The codec object which is passed as pointer will afterwards
               be managed by this class and be deleted on destruction
            */
            BasicTextStream(std::basic_iostream<ExternT>& ios, CodecT* codec)
            : _buffer( ios.rdbuf() , codec)
            { init(&_buffer); }

            //! @brief Deletes to codec.
            ~BasicTextStream()
            { }

            void attach(std::basic_iostream<ExternT>& ios)
            {
                _buffer.attach( ios.rdbuf() );
                this->clear();
            }

        private:
            BasicTextBuffer<InternT, ExternT> _buffer;
    };

} // namespace Pt

#endif

