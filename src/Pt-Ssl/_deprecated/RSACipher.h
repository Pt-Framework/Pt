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
        RSACipher(std::iostream& ios);

        //! \brief Instantiate an RSA cipher object for data encryption.
        RSACipher(std::iostream& ios, const SSLPublicKey& pkey);

        //! \brief Instantiate an RSA cipher object for data decryption.
        RSACipher(std::iostream& ios, const SSLPrivateKey& pkey);

        //! \brief Standard dtor.
        virtual ~RSACipher();

        void setPadding(PaddingMode pmode);

        //! \brief Start a data encryption process.
        void setPublicKey(const SSLPublicKey& pkey);

        //! \brief Start a data decryption process.
        void setPrivateKey(const SSLPrivateKey& pkey);

        //! \brief Finish a data encryption/decryption process.
        void finish();

        /** \brief Returns the block (chunk) size.
            The system expect that upon calling encode() or decode(), the user ensure that the
            'to' pointer has at least 'block size' available space.
         */
        virtual size_t blockSize() const;

        /** \brief Encode bytes from the 'from' pointers to the 'to' pointers.
            Returns the number of written (encoded) bytes.
            Returns zero if there is not enough input bytes or the 'to' pointer does not have enough space.
            Returns -1 if EOF.
            Updates the 'from_next' and 'to_next' pointers as needed.
         */
        virtual int encode(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next);

        /** \brief Encode/decode any remaining bytes to the 'to' pointers.
            Returns the number of written (encoded/decoded) bytes.
            Returns zero if the 'to' pointer does not have enough space.
            Returns -1 if EOF.
            Updates the 'from_next' and 'to_next' pointers as needed.
         */
        virtual int finish(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next);
        
    protected:
        virtual int sync();
        virtual int_type underflow();
        virtual int_type overflow(int_type ch);

    private:
        rsa_st*           _rsa;          // RSA key
        int               _rsaSize;      // Size of the RSA

        rsa_st*           _rsaPriv;      // RSA key
        int               _rsaSizePriv;  // Size of the RSA

        int               _maxChunkSize; // Maximum data chunk size (encryption only)
        int               _pmode;        // Padding mode
        std::vector<char> _ioBuf;        // Input/output buffer
        std::vector<char> _cnvBuf;       // Conversion buffer

        int do_encrypt(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next, bool finish);
        int do_decrypt(const char* from, const char* from_end, const char*& from_next, char* to, char* to_end, char*& to_next, bool finish);
};

} // namespace Pt
} // namespace Ssl

#endif
