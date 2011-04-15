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
        //! \brief Standard dtor.
        ~SSLPublicKey();

        //! \brief Check the signature of the given string with this public key.
        bool verifyStringSignature(const std::string& str, const std::string& sig, const char* digest = "SHA1") const;

        //! \brief Encrypt the given string with this public key.
        const std::string encryptString(const std::string& str) const;
        
        /// \internal Instantiate a public-key from the given OpenSSL raw private key handle.
        SSLPublicKey(evp_pkey_st* pkey);

        /// \internal Return the raw OpenSSL public key handle.
        evp_pkey_st* impl() const;

    private:
        class Impl;
        typedef SmartPtr<Impl> ImplPtr;

    private:
        // Copy ctor
        inline SSLPublicKey(ImplPtr ptr)
        : _impl(ptr)
        {}

        ImplPtr _impl;
};

//! \internal
class PT_SSL_API SSLPublicKey::Impl {
    public:
        Impl(evp_pkey_st* pkey);
        ~Impl();

        bool verifyStringSignature(const std::string& str, const std::string& sig, const char* digest) const;
        const std::string encryptString(const std::string& str) const;

        friend class SSLPublicKey;

    private:
        evp_pkey_st* _pkey;
};

} // namespace Pt
} // namespace Ssl

#endif
