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
#include "OpenSsl.h"
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Logger.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>
#include <openssl/pem.h>
#include <cstdio>

log_define("Pt.Ssl.CertificateStore")

namespace Pt {

namespace Ssl {

CertificateStoreImpl::CertificateStoreImpl()
{
}


CertificateStoreImpl::~CertificateStoreImpl()
{
    for(std::vector<Certificate*>::iterator it = _allCerts.begin(); it != _allCerts.end(); ++it)
    {
        delete *it;
    }
}


void CertificateStoreImpl::loadPkcs12(const char* data, size_t len, const char* passwd)
{
    EVP_PKEY* pkey = NULL;
    X509* cert = NULL;
    STACK_OF(X509)* ca = NULL;

    BioAutoPtr in( BIO_new_mem_buf( (void*) data, len ) );

    PKCS12* p12 = d2i_PKCS12_bio(in.get(), NULL);
    if( ! p12)
        throw InvalidCertificate("invalid PKCS12 data");

    int status = PKCS12_parse(p12, passwd, &pkey, &cert, &ca);
    PKCS12_free(p12);

    if( ! status )
        throw InvalidCertificate("invalid PKCS12 content");

    // TODO: use smart pointers later...

    if(cert) 
    {
        X509AutoPtr x509(cert);
        Certificate* c = new Certificate( new CertificateImpl(x509.get(), pkey) );
        _allCerts.push_back(c);
        x509.release();
    }
    
    if(ca)
    {
        for(int i = 0; i < sk_X509_num(ca); i++)
        {
            X509AutoPtr x509( sk_X509_pop(ca) );
            Certificate* c = new Certificate( new CertificateImpl(x509.get()) );
            _allCerts.push_back(c);
            x509.release();
        }

        sk_X509_pop_free(ca, X509_free);
        ca = NULL;
    }  
}


const Certificate* CertificateStoreImpl::findCertificate(const std::string& subject)
{
    for(std::vector<Certificate*>::const_iterator it = _allCerts.begin(); it != _allCerts.end(); ++it) 
    {
        if( (*it)->subject().find(subject) != std::string::npos )
            return *it;
    }

    return 0;
}

//        //For PEM we use:
//        //   PEM_read_PUBKEY
//        //   PEM_read_bio_PrivateKey
//        //
//        //For reading  ASN1 (DER) we use:
//        //  d2i_PUBKEY_bio
//        //  d2i_PrivateKey_bio
//        //
//        //For writing, I believe the functions are:
//        //   PEM_write_bio_PUBKEY
//        //   PEM_write_bio_PrivateKey
//        //   i2d_PUBKEY_bio
//        //   i2d_PrivateKey_bio
//        
//        void fromPem(const char* data, size_t len)
//        {
//            BioAutoPtr in( BIO_new_mem_buf( (void*) data, len ) );
//
//            // Try to read/parse the CA X509 certificates
//            while(true) 
//            {
//                // Read the certificate
//                X509AutoPtr x509 ( PEM_read_bio_X509_AUX(in.get(), 0, 0, 0) );
//                if( ! x509) 
//                  break;
//
//                Certificate cert( new CertificateImpl(x509.get()) );
//                this->push_back(cert);
//                
//                x509.release();
//            }
//        }

} // namespace Ssl

} // namespace Pt
