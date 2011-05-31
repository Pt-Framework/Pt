/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
 * Copyright (C) 2010-2010 by Marc Boris Duerner
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
#ifndef PT_SSL_RSACIPHER_H
#define PT_SSL_RSACIPHER_H

#include <Pt/Ssl/BasicCipher.h>
#include <Pt/Ssl/SSLPublicKey.h>
#include <Pt/Ssl/SSLPrivateKey.h>

namespace Pt {
namespace Ssl {

//! \brief RSA cipher classes.
class PT_SSL_API RSACipher : public BasicCipher {
    public:
        //! \brief Padding mode for data encryption.
        enum PaddingMode {
            RSA_PKCS1,      //!< The most widely used mode.
            RSA_PKCS1_OAEP  //!< Recommended for new applications.
        };

    public:
        //! \brief Instantiate an empty RSA cipher object.
        RSACipher();

        //! \brief Instantiate an RSA cipher object that can be used for data encryption.
        RSACipher(const SSLPublicKey& pubKey, PaddingMode pmode = RSA_PKCS1);

        //! \brief Instantiate an RSA cipher object that can be used for data decryption.
        RSACipher(const SSLPrivateKey& prvKey, PaddingMode pmode = RSA_PKCS1);

        //! \brief Instantiate an RSA cipher object that can be used for both data encryption and decryption.
        RSACipher(const SSLPublicKey& pubKey, const SSLPrivateKey& prvKey, PaddingMode pmode = RSA_PKCS1);

        //! \brief Standard dtor.
        virtual ~RSACipher();

        //! \brief Set the public key
        void setPublicKey(const SSLPublicKey& pkey);

        //! \brief Set the private key
        void setPrivateKey(const SSLPrivateKey& pkey);

        //! \brief Set the padding mode.
        void setPadding(PaddingMode pmode);

        /** \brief Returns the expected input block (chunk) size for encoding and decoding data.
            For maximum efficiency, upon calling encode() the user must ensure that the
            'from' pointer has the minimum available data (unless of course at the end of
            the stream).
            </br>
            Upon calling decode() the user must ensure that the 'from' pointer
            has the minimum available data or the decoding process may fail
            (unless of course at the end of the stream).
         */
        virtual size_t inputBlockSize() const;

        /** \brief Returns the minimum output block (chunk) size for encoding and decoding data.
            Upon calling encode() or decode(), the user must ensure that the
            'to' pointer can has the minimum available space or the process will fail.
         */
        virtual size_t outputBlockSize() const;

        /** \brief Encode bytes from the 'from' pointers to the 'to' pointers.
            Returns -1 if failed (the 'to' pointer does not have enough space or there is no data to be encoded at all).
            <br/>
            Returns  1 if success.
            <br/>
            Updates the 'from_next' and 'to_next' pointers as needed.
         */
        virtual int encode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next);

        /** \brief Encode bytes from the 'from' pointers to the 'to' pointers.
            Returns -1 if failed (the 'to' pointer does not have enough space or there is no data to be decoded at all).
            <br/>
            Returns  1 if success.
            <br/>
            Updates the 'from_next' and 'to_next' pointers as needed.
         */
        virtual int decode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next);

    private:
        // Common data
        int _pmode; // Padding mode

        // Encryption
        rsa_st* _rsaPub;     // RSA key
        size_t  _rsaPubSize; // Size of the RSA
        size_t  _encCSize;   // Maximum data chunk size for encryption

        // Decryption
        rsa_st* _rsaPrv;      // RSA key
        size_t  _rsaPrvSize;  // Size of the RSA
};

} // namespace Ssl
} // namespace Pt

#endif
