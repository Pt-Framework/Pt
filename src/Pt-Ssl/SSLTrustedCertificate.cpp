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

#include <Pt/Ssl/SSLTrustedCertificate.h>
#include <fstream>

#include "Utils.h"

namespace Pt {
namespace Ssl {

SSLTrustedCertificate::SSLTrustedCertificate()
{}

SSLTrustedCertificate::SSLTrustedCertificate(const std::string& certData)
{ addFromString(certData); }

SSLTrustedCertificate::~SSLTrustedCertificate()
{ clear(); }

void SSLTrustedCertificate::addFromString(const std::string& certData)
{
    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) certData.c_str(), certData.length() ) );

    // For calculating the fingerprint hash of the certificates
    const EVP_MD* fdig = EVP_sha1();
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int  n;
    
    // Try to read/parse the CA X509 certificates
    int count = 0;
    while(true) {
        // Read the certificate
        X509AutoPtr ca ( PEM_read_bio_X509_AUX(in.get(), 0, 0, 0) );
        if(!ca) break;

        // Calculate the fingerprint hash of the certificate
        X509* ptrCA = ca.get();
        if(!X509_digest(ptrCA, fdig, md, &n))
            throw SSLRuntimeError("Could not calculate the certificate's fingerprint hash!", PT_SOURCEINFO);

        // Store the certificate
        _trustedCert.push_back(ptrCA);
        ca.release();
        ++count;

        // Store some information about the certificate
        _caCertInfo.push_back(SSLCertificateInfo(
                              X509_get_version      (ptrCA),
            ASN1_INTEGER_get( X509_get_serialNumber (ptrCA) ),
            x509nam2string  ( X509_get_issuer_name  (ptrCA) ),
            sslhash2string  ( X509_issuer_name_hash (ptrCA) ),
            x509nam2string  ( X509_get_subject_name (ptrCA) ),
            sslhash2string  ( X509_subject_name_hash(ptrCA) ),
            asn1tim2string  ( X509_get_notBefore    (ptrCA) ),
            asn1tim2string  ( X509_get_notAfter     (ptrCA) ),
            OBJ_nid2sn(EVP_MD_type(fdig)),
            sslhash2string(md, n)
        ));
    }

    if(!count)
        throw SSLRuntimeError("Could not read/parse any CA certificate data!", PT_SOURCEINFO);
}

void SSLTrustedCertificate::addFromFile(const std::string& fileName)
{
    std::string   data;
    std::ifstream ifs;
    char          rbuf[4096];

    ifs.open(fileName.c_str(), std::ios::binary);
    while(ifs) {
        ifs.read( rbuf, sizeof(rbuf) );
        data += std::string( rbuf, ifs.gcount() );
    }

    addFromString(data);
}

void SSLTrustedCertificate::clear()
{
    for(std::vector<X509*>::const_iterator it = _trustedCert.begin(); it != _trustedCert.end(); ++it) {
        X509_free(*it);
    }
    _trustedCert.clear();
}

void SSLTrustedCertificate::apply(SSL_CTX* ctx)
{
    if(_trustedCert.empty())
        throw SSLRuntimeError("Trying to use an empty trusted-certificate list!", PT_SOURCEINFO);

    // Clear the previous trusted CA certificates (if any)
    X509_STORE_free(ctx->cert_store);
    ctx->cert_store = X509_STORE_new();
        
    // Try to add the CA X509 certificates (if any)
    for(std::vector<X509*>::const_iterator it = _trustedCert.begin(); it != _trustedCert.end(); ++it) {
        if( ! X509_STORE_add_cert(ctx->cert_store, *it) )
            throw SSLRuntimeError("Could not store the CA certificate as a trusted certificate!", PT_SOURCEINFO);
    }
}

} // namespace Pt
} // namespace Ssl
