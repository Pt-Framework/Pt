/***************************************************************************
 *   Copyright (C) 2004 Marc Boris Duerner                                 *
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

#ifndef PT_Text_TextStream_h
#define PT_Text_TextStream_h

#include <Pt/Text/Api.h>
#include <Pt/Char_ctype.h>
#include <Pt/Char_numpunct.h>
#include <Pt/Char.h>
#include <Pt/Text/BasicTextStream.h>


namespace std {
    class PT_TEXT_API ios_base;
}


namespace Pt {

namespace Text {

    /**
     * @brief Specialized class derived from BasicTextBuffer using Pt::Char and $char$
     * as internal and external type.
     *
     * The internal type is Pt::Char. The external type is $char$.
     *
     * See BasicTextBuffer for a more detailed description.
     */
    class PT_TEXT_API TextBuffer : public BasicTextBuffer<Pt::Char, char> {
        public:
            typedef TextCodec<Pt::Char, char> Codec;

        public:
            /**
             * @brief Constructs a new TextBuffer object using the given stream buffer as external device and
             * the codec for character conversion.
             *
             * See BasicTextBuffer::BasicTextBuffer() for a more detailed description.
             *
             * @param buffer The buffer (external device) which is wrapped by this object.
             * @param codec The codec which is used to convert data from and to the external device.
             */
            TextBuffer(std::streambuf* buffer, Codec* codec);
    };


    /**
     * @brief Specialized class derived from BasicTextIStream using Pt::Char and $char$
     * as internal and external type.
     *
     * The internal type is Pt::Char. The external type is $char$.
     *
     * See BasicTextIStream for a more detailed description.
     */
    class PT_TEXT_API TextIStream : public BasicTextIStream<Char, char> {
        public:
            typedef TextCodec<Pt::Char, char> CodecT;

        public:
            /**
             * @brief Constructs a new TextIStream object using the given input-stream as external device and
             * the codec for character conversion.
             *
             * See BasicTextIStream::BasicTextIStream() for a more detailed description.
             *
             * @param is The input-stream (external device) which is wrapped by this object.
             * @param codec The codec which is used to convert data from the external device.
             */
            TextIStream(std::istream& is, CodecT* codec);

            //! @brief Destructs this object freeing the internal buffer.
            ~TextIStream();

        protected:
            TextIStream(TextBuffer* buffer);

        private:
            TextBuffer* _buffer;
    };


    /**
     * @brief Specialized class derived from BasicTextOStream using Pt::Char and $char$
     * as internal and external type.
     *
     * The internal type is Pt::Char. The external type is $char$.
     *
     * See BasicTextOStream for a more detailed description.
     */
    class PT_TEXT_API TextOStream : public BasicTextOStream<Char, char> {
        public:
            typedef TextCodec<Pt::Char, char> CodecT;

        public:
            /**
             * @brief Constructs a new TextOStream object using the given output-stream as external device and
             * the codec for character conversion.
             *
             * See BasicTextOStream::BasicTextOStream() for a more detailed description.
             *
             * @param is The output-stream (external device) which is wrapped by this object.
             * @param codec The codec which is used to convert data to the external device.
             */
            TextOStream(std::ostream& os, CodecT* codec);

            //! @brief Destructs this object freeing the internal buffer.
            ~TextOStream();
    };


    /**
     * @brief Specialized class derived from BasicTextStream using Pt::Char and $char$
     * as internal and external type.
     *
     * The internal type is Pt::Char. The external type is $char$.
     *
     * See BasicTextStream for a more detailed description.
     */
    class PT_TEXT_API TextStream : public BasicTextStream<Char, char> {
        public:
            typedef TextCodec<Pt::Char, char> CodecT;

        public:
            /**
             * @brief Constructs a new TextStream object using the given I/O-stream as external device and
             * the codec for character conversion.
             *
             * See BasicTextStream::BasicTextStream() for a more detailed description.
             *
             * @param is The I/O-stream (external device) which is wrapped by this object.
             * @param codec The codec which is used to convert data from or to the external device.
             */
            TextStream(std::iostream& ios, CodecT* codec);

            //! @brief Destructs this object freeing the internal buffer.
            ~TextStream();
    };

} // namespace Text

} // namespace Pt

#endif

