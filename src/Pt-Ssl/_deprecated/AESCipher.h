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
#ifndef PT_SSL_AESCIPHER_H
#define PT_SSL_AESCIPHER_H

#include <Pt/Ssl/BasicSymmetricCipher.h>

namespace Pt {
namespace Ssl {

//! \brief AES (Advanced Encryption Standard) symmetric-block-cipher classes.
class PT_SSL_API AESCipher  : public BasicSymmetricCipher {
    public:
        using BasicSymmetricCipher::SaltType;
        using BasicSymmetricCipher::OperationMode;
        using BasicSymmetricCipher::KeySize;

    public:
        //! \brief Instantiate a symmetric-block-cipher with the given password, key size, and mode of operation.
        AESCipher(const std::string& password, KeySize keySize = K128, OperationMode operMode = CBC);

        //! \brief Standard dtor.
        virtual ~AESCipher();

        //! \brief Set the key size of the cipher.
        void setKeySize(KeySize keySize);
        
        /** \brief Returns the expected input block (chunk) size for encoding data.
            For maximum efficiency, upon calling encode() the user must ensure that the
            'from' pointer has the minimum available data (unless of course at the end of
            the stream).
         */
        virtual size_t encodingInputBlockSize() const;

        /** \brief Returns the minimum output block (chunk) size for encoding data.
            Upon calling encode(), the user must ensure that the 'to' pointer has
            the minimum available space or the process will fail.
         */
        virtual size_t encodingOutputBlockSize() const;

        /** \brief Returns the expected input block (chunk) size for decoding data.
            Upon calling decode() the user must ensure that the 'from' pointer
            has the minimum available data or the decoding process may fail
            (unless of course at the end of the stream).
         */
        virtual size_t decodingInputBlockSize() const;

        /** \brief Returns the minimum output block (chunk) size for decoding data.
            Upon calling decode(), the user must ensure that the 'to' pointer has
            the minimum available space or the process will fail.
         */
        virtual size_t decodingOutputBlockSize() const ;

    protected:
        virtual const char* getOpenSSLCipherName() const;

    private:
        KeySize _keySize;
};

} // namespace Pt
} // namespace Ssl

#endif
