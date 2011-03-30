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
#ifndef PT_SSL_SSLTRUSTEDCERTIFICATE_H
#define PT_SSL_SSLTRUSTEDCERTIFICATE_H

#include <Pt/Ssl/SSLCipherInfo.h>
#include <vector>

namespace Pt {
namespace Ssl {

//! \brief Trusted CA certificate.
class PT_SSL_API SSLTrustedCertificate {
    public:
        //! \brief Instantiate an empty trusted-certificate-list.
        SSLTrustedCertificate();

        //! \brief Instantiate a trusted-certificate-list using the given certificate data.
        SSLTrustedCertificate(const std::string& certData);

        //! \brief Stanndard dtor.
        ~SSLTrustedCertificate();

        //! \brief Add certificate from the given data as a trusted-certificate.
        void addFromString(const std::string& certData);

        //! \brief Add certificate from the given file as a trusted-certificate.
        void addFromFile(const std::string& fileName);

        //! \brief Clear (delete) the list.
        void clear();
        
        friend class SSLContext;

    private:
        // Data
        std::vector<x509_st*> _trustedCert;

        // Helper functions
        void apply(ssl_ctx_st* ctx);
};

} // namespace Pt
} // namespace Ssl

#endif
