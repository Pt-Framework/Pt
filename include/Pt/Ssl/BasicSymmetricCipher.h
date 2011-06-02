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
#ifndef PT_SSL_BASICSYMMETRICCIPHER_H
#define PT_SSL_BASICSYMMETRICCIPHER_H

#include <Pt/Ssl/BasicCipher.h>

namespace Pt {
namespace Ssl {

//! \brief RSA cipher classes.
class PT_SSL_API BasicSymmetricCipher : public BasicCipher {
    public:
        //! \brief Salt type.
        enum SaltType {
            NoSalt,     //!< No salt.
            NormalSalt, //!< Use pseudo-random bytes as salt.
            StrongSalt  //!< Use strong pseudo-random bytes as salt.
        };

        /** \brief Cipher mode of operations.
            Please refer to http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation for more details.
            Please note that not all ciphers will support all the modes.
         */
        enum OperationMode {
            ECB,  //!< Electronic Codebook Book; not secure for long message that with many redundancies.
            CBC,  //!< Cipher-Block Chaining; standard operational mode for block symmetric ciphers.
            CFB1, //!< Cipher Feedback 1 bit; encrypting data as if it is a stream cipher.
            CFB8, //!< Cipher Feedback 8 bit; encrypting data as if it is a stream cipher.
            CFB,  //!< Cipher Feedback 64 bit; encrypting data as if it is a stream cipher.
            OFB   //!< Output Feedback; encrypting data as if it is a stream cipher.
        };

        /** \brief Cipher strength (key size).
            Please note that not all ciphers allow us to specify the key size.
         */
        enum KeySize {
            K128, //!< 128 bit.
            K192, //!< 192 bit.
            K256  //!< 256 bit.
        };

    public:
        //! \brief Instantiate a symmetric-block-cipher with the given password and mode of operation.
        BasicSymmetricCipher(const std::string& password, OperationMode operMode = CBC);

        //! \brief Standard dtor.
        virtual ~BasicSymmetricCipher();

        /** \brief Set the mode of operation of the cipher.
            Override this function if the specific cipher does not support all the modes.
         */
        virtual void setMode(OperationMode operMode);

        //! \brief Set the password of the cipher.
        void setPassword(const std::string& password);

        //! \brief Return the length of the salt.
        size_t saltLength() const;
        
        /** \brief Set the salt.
            Except the salt empty, its length must be exactly saltLength() bytes.
         */
        void setSalt(const std::string& salt);
        
        //! \brief Get the salt.
        const std::string& getSalt() const;

        //! \brief Generate a new random salt.
        void genSalt(SaltType saltType = NormalSalt);

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
        
    protected:
        //! \brief Override this to return the OpenSSL cipher string ID.
        virtual const char* getOpenSSLCipherName() const = 0;

        // Data
        OperationMode _operMode;
        std::string   _password;
        std::string   _salt;
};

} // namespace Ssl
} // namespace Pt

#endif
