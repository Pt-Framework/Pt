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
#ifndef PT_SSL_BASICSYMETRICCIPHER_H
#define PT_SSL_BASICSYMETRICCIPHER_H

#include <Pt/Ssl/BasicCipher.h>
#include <Pt/Ssl/Exception.h>

#include <vector>

namespace Pt {
namespace Ssl {

//! \brief The base of all symmetric-cipher classes.
//! Symmetric-cipher needs password. Therefore we specify a method
//! to set a password in this class.
class PT_SSL_API BasicSymmetricCipher : public BasicCipher {
    public:
        //! \brief Salt type
        enum SaltType {
            NoSalt,     //!< No salt.
            NormalSalt, //!< Use pseudo-random bytes as salt.
            StrongSalt  //!< Use strong pseudo-random bytes as salt.
        };
        
    public:
        //! \brief Instantiate an empty symmetric-cipher object.
        BasicSymmetricCipher(std::iostream& ios);

        //! \brief Standard dtor.
        virtual ~BasicSymmetricCipher();

        /** \brief Start a data encryption process.
            If the useSalt parameter is set to NormalSalt or StrongSalt, the system will generate a random salt
            to improve the security of the encryption. The salt will be returned as a string.
         */ 
        virtual const std::string startEncrypt(const std::string& password, SaltType saltType = NoSalt);

        /** \brief Start a data decryption process.
            If the salt parameter is not an empty string, it will be used in the decryption process.
            It must be the same salt that was returned by startEncrypt().
         */
        virtual void startDecrypt(const std::string& password, const std::string& saltStr = "");

        //! \brief Finish a data encryption/decryption process.
        void finish();

    protected:
        virtual int sync();
        virtual int_type underflow();
        virtual int_type overflow(int_type ch);

        //! \brief Override this to return the OpenSSL cipher string ID.
        virtual const char* getOpenSSLCipherName() const = 0;

        // Helper function to store the encrypted data to the attached iostream
        void storeEncryptedData();

    protected:
        bio_st*           _bioEnc; // Encryption BIO
        bio_st*           _bioDec; // Decryption BIO
        bio_st*           _bioIO;  // Input/output BIO
        
        std::vector<char> _ioBuf;  // Input/output buffer
        std::vector<char> _cnvBuf; // Conversion buffer
};

} // namespace Pt
} // namespace Ssl

#endif
