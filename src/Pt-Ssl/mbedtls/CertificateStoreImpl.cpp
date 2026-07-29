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

#include "CertificateStoreImpl.h"
#include "CertificateImpl.h"
#include "MbedTls.h"
#include "Pkcs12Parser.h"
#include <Pt/Ssl/SslError.h>
#include <Pt/SmartPtr.h>
#include <Pt/System/Logger.h>
#include <mbedtls/x509_crt.h>
#include <cstring>
#include <vector>

PT_LOG_DEFINE("Pt.Ssl.CertificateStore")

namespace Pt {

namespace Ssl {

CertificateStoreImpl::CertificateStoreImpl()
{
}


CertificateStoreImpl::~CertificateStoreImpl()
{
    for(std::vector<Certificate*>::iterator it = _allCerts.begin();
        it != _allCerts.end(); ++it)
    {
        delete *it;
    }
}


void CertificateStoreImpl::loadPkcs12(const char* pkcs12,
                                      std::size_t len,
                                      const char* passwd)
{
    PT_LOG_DEBUG("loadPkcs12: " << passwd);

    mbedtls_pk_context* pkey = 0;
    mbedtls_x509_crt*   cert = 0;
    std::vector<mbedtls_x509_crt*> ca;

    if( ! parsePkcs12(reinterpret_cast<const unsigned char*>(pkcs12), len,
                      passwd, &pkey, &cert, ca))
        throw InvalidCertificate("invalid PKCS12 data");

    PkAutoPtr pkeyGuard(pkey);

    if(cert)
    {
        X509CrtAutoPtr certGuard(cert);
        AutoPtr<CertificateImpl> implGuard(new CertificateImpl(cert, pkey));
        certGuard.release();
        pkeyGuard.release();

        AutoPtr<Certificate> certObjGuard(new Certificate(implGuard.get()));
        implGuard.release();

        _allCerts.push_back(certObjGuard.get());
        certObjGuard.release();

        PT_LOG_DEBUG("imported certificate: " << _allCerts.back()->subject());
    }

    for(std::size_t i = 0; i < ca.size(); ++i)
    {
        X509CrtAutoPtr caGuard(ca[i]);
        AutoPtr<CertificateImpl> implGuard(new CertificateImpl(ca[i]));
        caGuard.release();

        AutoPtr<Certificate> certObjGuard(new Certificate(implGuard.get()));
        implGuard.release();

        _allCerts.push_back(certObjGuard.get());
        certObjGuard.release();

        PT_LOG_DEBUG("imported CA certificate: " << _allCerts.back()->subject());
    }
}


void CertificateStoreImpl::loadPem(const char* data,
                                   std::size_t len,
                                   const char* /*passwd*/)
{
    PT_LOG_DEBUG("loadPem");

    mbedtls_x509_crt* crt = new mbedtls_x509_crt();
    mbedtls_x509_crt_init(crt);

    X509CrtAutoPtr crtGuard(crt);

    if(mbedtls_x509_crt_parse(crt,
                               reinterpret_cast<const unsigned char*>(data),
                               len + 1) != 0)
        throw InvalidCertificate("invalid PEM certificate");

    AutoPtr<CertificateImpl> implGuard(new CertificateImpl(crt));
    crtGuard.release();

    AutoPtr<Certificate> certGuard(new Certificate(implGuard.get()));
    implGuard.release();

    _allCerts.push_back(certGuard.get());
    certGuard.release();

    PT_LOG_DEBUG("imported certificate: " << _allCerts.back()->subject());
}


const Certificate* CertificateStoreImpl::findCertificate(const std::string& subject)
{
    PT_LOG_TRACE("find certificate: " << subject);

    for(std::vector<Certificate*>::const_iterator it = _allCerts.begin();
        it != _allCerts.end(); ++it)
    {
        const Certificate* cert = *it;
        if( cert->subject().find(subject) != std::string::npos )
            return cert;
    }
    return 0;
}

} // namespace Ssl

} // namespace Pt
