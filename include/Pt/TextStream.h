/*
 * Copyright (C) 2004-2007 Marc Boris Duerner
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
#include <Pt/Char.h>
#include <Pt/BasicTextStream.h>

namespace Pt {

    /** @brief Converts 8-bit character sequences to unicode.
     *
     * The internal type is Pt::Char. The external type is $char$.
     *
     * See BasicTextBuffer for a more detailed description.
     */
    class PT_API TextBuffer : public BasicTextBuffer<Pt::Char, char>
    {
        public:
            typedef TextCodec<Pt::Char, char> Codec;

        public:
            /** @brief Constructs a new TextBuffer object using the given stream buffer as external device and
             * the codec for character conversion.
             *
             * See BasicTextBuffer::BasicTextBuffer() for a more detailed description.
             *
             * @param buffer The buffer (external device) which is wrapped by this object.
             * @param codec The codec which is used to convert data from and to the external device.
             */
            TextBuffer(std::ios* buffer, Codec* codec);
    };


    /** @brief Specialized class derived from BasicTextIStream using Pt::Char and $char$
     * as internal and external type.
     *
     * The internal type is Pt::Char. The external type is $char$.
     *
     * See BasicTextIStream for a more detailed description.
     */
    class PT_API TextIStream : public BasicTextIStream<Char, char>
    {
        public:
            typedef TextCodec<Pt::Char, char> Codec;

        public:
            /** @brief Constructs a new TextIStream object using the given input-stream as external device and
             * the codec for character conversion.
             *
             * See BasicTextIStream::BasicTextIStream() for a more detailed description.
             *
             * @param is The input-stream (external device) which is wrapped by this object.
             * @param codec The codec which is used to convert data from the external device.
             */
            TextIStream(std::istream& is, Codec* codec);

            TextIStream(Codec* codec);

            //! @brief Destructs this object freeing the internal buffer.
            ~TextIStream();
    };


    /** @brief Specialized class derived from BasicTextOStream using Pt::Char and $char$
     * as internal and external type.
     *
     * The internal type is Pt::Char. The external type is $char$.
     *
     * See BasicTextOStream for a more detailed description.
     */
    class PT_API TextOStream : public BasicTextOStream<Char, char>
    {
        public:
            typedef TextCodec<Pt::Char, char> Codec;

        public:
            /** @brief Constructs a new TextOStream object using the given output-stream as external device and
             * the codec for character conversion.
             *
             * See BasicTextOStream::BasicTextOStream() for a more detailed description.
             *
             * @param os The output-stream (external device) which is wrapped by this object.
             * @param codec The codec which is used to convert data to the external device.
             */
            TextOStream(std::ostream& os, Codec* codec);

            TextOStream(Codec* codec);

            //! @brief Destructs this object freeing the internal buffer.
            ~TextOStream();
    };


    /** @brief Specialized class derived from BasicTextStream using Pt::Char and $char$
     * as internal and external type.
     *
     * The internal type is Pt::Char. The external type is $char$.
     *
     * See BasicTextStream for a more detailed description.
     */
    class PT_API TextStream : public BasicTextStream<Char, char>
    {
        public:
            typedef TextCodec<Pt::Char, char> Codec;

        public:
            /** @brief Constructs a new TextStream object using the given I/O-stream as external device and
             * the codec for character conversion.
             *
             * See BasicTextStream::BasicTextStream() for a more detailed description.
             *
             * @param ios The I/O-stream (external device) which is wrapped by this object.
             * @param codec The codec which is used to convert data from or to the external device.
             */
            TextStream(std::iostream& ios, Codec* codec);

            TextStream(Codec* codec);

            //! @brief Destructs this object freeing the internal buffer.
            ~TextStream();
    };

} // namespace Pt

#endif

