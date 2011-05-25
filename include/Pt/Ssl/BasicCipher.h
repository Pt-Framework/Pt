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
class PT_SSL_API BasicCipher : public NonCopyable, public std::streambuf {
    public:
        //! \brief Instantiate an empty basic-cipher object.
        BasicCipher(std::iostream& ios);

        //! \brief Standard dtor.
        ~BasicCipher();

        //! \brief Set the output stream.
        void setIOStream(std::iostream& out);

        /** @brief Reads data from the underlying stream
            Returns the number bytes in the data.
        */
        virtual std::streamsize import();

        /** \brief Returns the block (chunk) size.
            The system expect that upon calling encode() or decode(), the user ensure that the
            'to' pointer has at least 'block size' available space.
         */
        virtual size_t blockSize() const = 0;

        /** \brief Encode bytes from the 'from' pointers to the 'to' pointers.
            Returns the number of written (encoded) bytes.
            Returns zero if there is not enough input bytes or the 'to' pointer does not have enough space.
            Returns -1 if EOF.
            Updates the 'from_next' and 'to_next' pointers as needed.
         */
        virtual int encode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next) = 0;

        /** \brief Decode bytes from the 'from' pointers to the 'to' pointers.
            Returns the number of written (decoded) bytes.
            Returns zero if there is not enough input bytes or the 'to' pointer does not have enough space.
            Returns -1 if EOF.
            Updates the 'from_next' and 'to_next' pointers as needed.
         */
        virtual int decode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next) = 0;

        /** \brief Encode/decode any remaining bytes to the 'to' pointers.
            Returns the number of written (encoded) bytes.
            Returns zero the 'to' pointer does not have enough space.
            Returns -1 if EOF.
            Updates the and 'to_next' pointer as needed.
         */
        virtual int finish(char* to, char* to_end, char*& to_next) = 0;
        
    protected:
        std::iostream* _ios;
};

} // namespace Pt
} // namespace Ssl

#endif
