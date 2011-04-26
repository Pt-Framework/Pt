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
#ifndef PT_SSL_SSLPUBLICKEY_H
#define PT_SSL_SSLPUBLICKEY_H

#include <Pt/SmartPtr.h>
#include <Pt/Ssl/SSLCipherInfo.h>

namespace Pt {
namespace Ssl {

//! \brief Public key.
class PT_SSL_API SSLPublicKey {
    public:
        //! \brief Padding mode for string encryption.
        enum PaddingMode {
            RSA_PKCS1,      //!< The most widely used mode. 
            RSA_PKCS1_OAEP  //!< Recommended for new applications.
        };

    public:
        //! \brief Copy ctor.
        SSLPublicKey(const SSLPublicKey& pkey);
        
        //! \brief Standard dtor.
        ~SSLPublicKey();

        //! \brief Begin string verification.
        void beginVerifyString(const char* digest = "SHA1");

        //! \brief Add a string chunk to be verified.
        void addStringToVerify(const std::string& str);

        //! \brief End string verification.
        const bool endVerifyString(const std::string& sig);
        
        //! \brief Begin string encryption.
        void beginEncryptString(PaddingMode pmode);

        //! \brief Try to encrypt the given string; may return an empty string if there is too few input data.
        const std::string tryEncryptString(const std::string& str);

        //! \brief End string encryption.
        const std::string endEncryptString();
        
        /// \internal Instantiate a public-key from the given OpenSSL raw private key handle.
        SSLPublicKey(evp_pkey_st* pkey);

        /// \internal Return the raw OpenSSL public key handle.
        evp_pkey_st* impl() const;
        
    private:
        // Foward declaration of the shared implementation class
        class Impl;
        typedef SmartPtr<Impl> ImplPtr;

    private:
        // Shared implementation of the class (COW)
        ImplPtr _impl;

        // Non-shared data for string verification
        bio_st*        _sbio;
        env_md_ctx_st* _mctx;

        // Non-shared data for string encryption
        rsa_st*                    _rsa;
        int                        _epmode;
        int                        _rsaSize;
        int                        _maxChunkSize;
        std::string                _eibuf;
        std::vector<unsigned char> _eobuf;
};

//! \internal
class PT_SSL_API SSLPublicKey::Impl {
    public:
        Impl(evp_pkey_st* pkey);
        ~Impl();

        friend class SSLPublicKey;

    private:
        evp_pkey_st* _pkey;
};

} // namespace Pt
} // namespace Ssl

#endif
