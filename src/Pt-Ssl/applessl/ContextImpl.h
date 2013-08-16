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
#ifndef PT_SSL_CONTEXTIMPL_H
#define PT_SSL_CONTEXTIMPL_H

#include <Pt/Ssl/Api.h>
#include <Pt/Ssl/Context.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <vector>

namespace Pt {

namespace Ssl {

inline void SSLInitImpl()
{}

inline void SSLExitImpl()
{}

class ContextImpl
{
    public:
        ContextImpl(Context::Protocol protocol);

        ~ContextImpl();

        void assign(const ContextImpl& ctx);

        Context::Protocol protocol() const;

        void setProtocol(Context::Protocol protocol);

        void setVerifyDepth(int n);

        Context::VerifyMode verification() const
        { return _verify; }

        void setVerifyMode(Context::VerifyMode mode);

        void setCACertificates(const std::vector<Certificate>& trustedCert);

        void addCACertificate(const Certificate& trustedCert);

        void setCertificate(const Certificate& cert);

        void loadPkcs12(const char* data, size_t len, const char* passwd);

        void loadPkcs12(std::istream& is, const char* passwd);

        Certificate findCertificate(const std::string& subject);

        Certificate& certificate()
        { return _cert; }
        
        const std::vector<Certificate>& caCertificates()
        { return _caCerts; }
    
    private:
        Context::Protocol                 _protocol;
        std::vector<Certificate> _caCerts;
        Certificate              _cert;
        //CertificateList          _extraCerts;
        Context::VerifyMode               _verify;
        std::vector<Certificate> _certificates;
        void*                    _reserved;
};

} // namespace Ssl

} // namespace Pt

#endif
