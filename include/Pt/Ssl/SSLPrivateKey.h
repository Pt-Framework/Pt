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
#ifndef PT_SSL_SSLPRIVATEKEY_H
#define PT_SSL_SSLPRIVATEKEY_H

#include <Pt/SmartPtr.h>
#include <Pt/Ssl/SSLCipherInfo.h>

namespace Pt {
namespace Ssl {

//! \brief Private key.
class PT_SSL_API SSLPrivateKey {
    public:
        //! \brief Instantiate an empty private-key.
        SSLPrivateKey();

        //! \brief Instantiate an empty private-key.
        SSLPrivateKey(const std::string& password);

        //! \brief Instantiate a private-key using the given key data.
        SSLPrivateKey(const std::string& keyData, const std::string& password);

        //! \brief Standard dtor.
        ~SSLPrivateKey();

        //! \brief Load private-key from the given data.
        void loadFromString(const std::string& keyData);

        //! \brief Load private-key from the given file.
        void loadFromFile(const std::string& fileName);

        //! \brief Clear (delete) any loaded key.
        void clear();

        //! \brief Sign the given string with this private key.
        const std::string signString(const std::string& str, const char* digest = "SHA1") const;

        /// \internal Return the raw OpenSSL private key handle.
        evp_pkey_st* impl() const;
        
    private:
        class Impl;
        typedef SmartPtr<Impl> ImplPtr;

        // Copy ctor
        inline SSLPrivateKey(ImplPtr ptr)
        : _impl(ptr)
        {}

    private:
        ImplPtr _impl;
};

//! \internal
class PT_SSL_API SSLPrivateKey::Impl {
    public:
        Impl();
        Impl(const std::string& password);
        ~Impl();

        void loadFromString(const std::string& keyData);
        void loadFromFile(const std::string& fileName);

        const std::string signString(const std::string& str, const char* digest) const;

        friend class SSLContext;
        friend class SSLPrivateKey;

    private:
        void clear();

        std::string  _pswd;
        evp_pkey_st* _pkey;
};


} // namespace Pt
} // namespace Ssl

#endif
