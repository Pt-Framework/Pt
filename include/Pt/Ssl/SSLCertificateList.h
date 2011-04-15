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
#ifndef PT_SSL_SSLCERTIFICATELIST_H
#define PT_SSL_SSLCERTIFICATELIST_H

#include <Pt/Ssl/SSLPublicKey.h>
#include <vector>

namespace Pt {
namespace Ssl {

//! \brief Certificate list.
class PT_SSL_API SSLCertificateList {
    public:
        //! \brief Instantiate an empty certificate-list.
        SSLCertificateList();

        //! \brief Instantiate a certificate-list using the given certificate data.
        SSLCertificateList(const std::string& certData);

        //! \brief Stanndard dtor.
        ~SSLCertificateList();

        //! \brief Load certificate from the given data.
        void loadFromString(const std::string& certData);

        //! \brief Load certificate from the given file.
        void loadFromFile(const std::string& fileName);

        //! \brief Clear (delete) any loaded certificate.
        void clear();

        //! \brief Returns a list of certificates' informations.
        const std::vector<SSLCertificateInfo>& certInfo() const;

        //! \brief Get the public key of the main (first) certificate.
        const SSLPublicKey getPublicKey() const;

        /// \internal Return a list of raw OpenSSL X509 certificate handle.
        const std::vector<x509_st*>& impl() const;
        
    private:
        class Impl;
        typedef SmartPtr<Impl> ImplPtr;

        // Copy ctor
        inline SSLCertificateList(ImplPtr ptr)
        : _impl(ptr)
        {}

    private:
        ImplPtr _impl;
};

//! \internal
class PT_SSL_API SSLCertificateList::Impl {
    public:
        Impl();
        ~Impl();

        void loadFromString(const std::string& certData);
        void loadFromFile(const std::string& fileName);

        inline const std::vector<SSLCertificateInfo>& certInfo() const
        { return _certInfo; }
        const SSLPublicKey getPublicKey() const;

        friend class SSLCertificateList;

    private:
        void clear();

        std::vector<x509_st*>           _cert;
        std::vector<SSLCertificateInfo> _certInfo;
};

} // namespace Pt
} // namespace Ssl

#endif
