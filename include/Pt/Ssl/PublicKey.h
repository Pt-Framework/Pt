/*
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
 * Copyright (C) 2010-2012 by Marc Duerner
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
#ifndef PT_SSL_PUBLICKEY_H
#define PT_SSL_PUBLICKEY_H

#include <Pt/Ssl/Api.h>
#include <Pt/Ssl/SslError.h>
#include <Pt/SmartPtr.h>

namespace Pt {

namespace Ssl {

//! \brief Public key.
class PT_SSL_API PublicKey 
{
    public:
        //! \brief Padding mode for string encryption.
        enum PaddingMode {
            RSA_PKCS1,      //!< The most widely used mode. 
            RSA_PKCS1_OAEP  //!< Recommended for new applications.
        };

    public:
        //! \brief Instantiate an empty public-key.
        PublicKey();

        //! \brief Copy ctor.
        PublicKey(const PublicKey& pkey);

        //! \brief Instantiate a public-key using the given key data.
        PublicKey(const std::string& keyData);
        
        //! \brief Standard dtor.
        ~PublicKey();

        //! \brief Load public-key from the given data.
        void loadFromString(const std::string& keyData);

        //! \brief Load public-key from the given file.
        void loadFromFile(const std::string& fileName);

        void toPem(std::string& keyData) const;

        //! \brief Clear (delete) any loaded key.
        void clear();

    public:
        //! \internal Instantiate a public-key from the given OpenSSL raw private key handle.
        PublicKey(evp_pkey_st* pkey);

        //! \internal Return the raw OpenSSL public key handle.
        evp_pkey_st* impl() const;
        
    private:
        // Foward declaration of the shared implementation class
        class Impl;
        typedef SmartPtr<Impl> ImplPtr;

    private:
        // Shared implementation of the class (COW)
        ImplPtr _impl;
};

//! @internal
class PT_SSL_API PublicKey::Impl 
{
    friend class PublicKey;

    public:
        Impl();

        Impl(evp_pkey_st* pkey);

        ~Impl();

        void loadFromString(const std::string& keyData);

        void loadFromFile(const std::string& fileName);

        void toPem(std::string& fileName) const;

        void clear();

    private:
        evp_pkey_st* _pkey;
};

} // namespace Ssl

} // namespace Pt

#endif // PT_SSL_PUBLICKEY_H
