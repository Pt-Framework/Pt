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

#include <string>

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
        inline RSACipher(std::ostream& out)
        : BasicCipher(out)
        {}

        //! \brief Instantiate an RSA cipher object for data encryption.
        inline RSACipher(std::ostream& out, const SSLPublicKey& pkey, PaddingMode pmode = RSA_PKCS1)
        : BasicCipher(out)
        { startEncrypt(pkey, pmode); }

        //! \brief Instantiate an RSA cipher object for data decryption.
        inline RSACipher(std::ostream& out, const SSLPrivateKey& pkey, PaddingMode pmode = RSA_PKCS1)
        : BasicCipher(out)
        { startDecrypt(pkey, pmode); }

        //! \brief Standard dtor.
        virtual ~RSACipher()
        {}

        //! \brief Start a data encryption process.
        void startEncrypt(const SSLPublicKey& pkey, PaddingMode pmode = RSA_PKCS1);

        //! \brief Start a data decryption process.
        void startDecrypt(const SSLPrivateKey& pkey, PaddingMode pmode = RSA_PKCS1);

        //! \brief Add data (update the state of this cipher object).
        void update(const char* str, int len);

        //! \brief Add data (update the state of this cipher object).
        void update(const std::string& str);

        //! \brief Add data (update the state of this ciphert object).
        void update(std::istream& is);

        //! \brief Finish a data encryption/decryption process.
        void finish();
};

} // namespace Pt
} // namespace Ssl

#endif
