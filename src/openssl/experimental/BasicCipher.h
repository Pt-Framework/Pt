/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
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
#ifndef PT_SSL_BASICCIPHER_H
#define PT_SSL_BASICCIPHER_H

#include <Pt/NonCopyable.h>
#include <Pt/Ssl/Api.h>

#include <streambuf>

namespace Pt {
namespace Ssl {

//! \brief The base of all cipher classes.
//! Not all cipher will need password. Therefore we do not specify a method
//! to set a password in this class.
class PT_SSL_API BasicCipher : public NonCopyable {
    public:
        //! \brief Instantiate an empty basic-cipher object.
        BasicCipher();

        //! \brief Standard dtor.
        virtual ~BasicCipher();

        /** \brief Returns the expected input block (chunk) size for encoding data.
            For maximum efficiency, upon calling encode() the user must ensure that the
            'from' pointer has the minimum available data (unless of course at the end of
            the stream).
         */
        virtual size_t encodingInputBlockSize() const = 0;

        /** \brief Returns the minimum output block (chunk) size for encoding data.
            Upon calling encode(), the user must ensure that the 'to' pointer has
            the minimum available space or the process will fail.
         */
        virtual size_t encodingOutputBlockSize() const = 0;

        /** \brief Encode bytes from the 'from' pointers to the 'to' pointers.
            Returns -1 if the 'to' pointer does not have enough space.
            <br/>
            Returns  0 if there is not enough input data.
            <br/>
            Returns  1 if success.
            <br/>
            Updates the 'from_next' and 'to_next' pointers as needed.
         */
        virtual int encode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next) = 0;

        /** \brief Get the any left-over encoded bytes.
            Returns true if there is no more byte left.
         */
        virtual bool finishEncode(char* to, char* to_end, char*& to_next) = 0;
        
        /** \brief Returns the expected input block (chunk) size for decoding data.
            Upon calling decode() the user must ensure that the 'from' pointer
            has the minimum available data or the decoding process may fail
            (unless of course at the end of the stream).
         */
        virtual size_t decodingInputBlockSize() const = 0;

        /** \brief Returns the minimum output block (chunk) size for decoding data.
            Upon calling decode(), the user must ensure that the 'to' pointer has
            the minimum available space or the process will fail.
         */
        virtual size_t decodingOutputBlockSize() const = 0;

        /** \brief Decode bytes from the 'from' pointers to the 'to' pointers.
            Returns -1 if the 'to' pointer does not have enough space.
            <br/>
            Returns  0 if there is not enough input data.
            <br/>
            Returns  1 if success.
            <br/>
            Updates the 'from_next' and 'to_next' pointers as needed.
         */
        virtual int decode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next) = 0;

        /** \brief Get the any left-over decoded bytes.
            Returns true if there is no more byte left.
         */
        virtual bool finishDecode(char* to, char* to_end, char*& to_next) = 0;
};

} // namespace Ssl
} // namespace Pt

#endif
