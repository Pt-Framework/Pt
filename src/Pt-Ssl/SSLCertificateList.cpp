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

#include <Pt/Ssl/SSLCertificateList.h>
#include <fstream>

#include "Utils.h"

namespace Pt {
namespace Ssl {

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////
log_define(PT_SSL_LOGGER_CATEGORY);
#define PT_SSL_LOG(CODE) PT_SSL_LOG_INFO("SSLCertList ", CODE)
////////////////////////////////////////////////////////////////////////////////////////////////////

SSLCertificateList::SSLCertificateList()
: _impl(new Impl())
{}

SSLCertificateList::SSLCertificateList(const std::string& certData)
: _impl(new Impl())
{ _impl->loadFromString(certData); }

SSLCertificateList::~SSLCertificateList()
{}

void SSLCertificateList::loadFromString(const std::string& certData)
{
    _impl = new Impl();
    _impl->loadFromString(certData);
}

void SSLCertificateList::loadFromFile(const std::string& fileName)
{
    _impl = new Impl();
    _impl->loadFromFile(fileName);
}

void SSLCertificateList::clear()
{ _impl = new Impl(); }

const SSLPublicKey SSLCertificateList::getPublicKey() const
{ return _impl->getPublicKey(); }

const std::vector<x509_st*>& SSLCertificateList::impl() const
{ return _impl->_cert; }

////////////////////////////////////////////////////////////////////////////////////////////////////

SSLCertificateList::Impl::Impl()
: _cert(0)
{}

SSLCertificateList::Impl::~Impl()
{ clear(); }

void SSLCertificateList::Impl::loadFromString(const std::string& certData)
{
    // Clear previous certificates
    clear();

    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) certData.c_str(), certData.length() ) );

    // For calculating the fingerprint hash of the certificate
    const EVP_MD* fdig = EVP_sha1();
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int  n;

    // Try to read/parse the CA X509 certificates
    while(true) {
        // Read the certificate
        X509AutoPtr cert ( PEM_read_bio_X509_AUX(in.get(), 0, 0, 0) );
        if(!cert) break;

        // Calculate the fingerprint hash of the certificate
        X509* ptrCert = cert.get();
        if(!X509_digest(ptrCert, fdig, md, &n))
            throw SSLRuntimeError("Could not calculate the certificate's fingerprint hash!", PT_SOURCEINFO);

        // Store the certificate
        _cert.push_back(ptrCert);
        cert.release();

        // Store some information about the certificate
        _certInfo.push_back(SSLCertificateInfo(
                              X509_get_version      (ptrCert),
            ASN1_INTEGER_get( X509_get_serialNumber (ptrCert) ),
            x509nam2string  ( X509_get_issuer_name  (ptrCert) ),
            sslhash2string  ( X509_issuer_name_hash (ptrCert) ),
            x509nam2string  ( X509_get_subject_name (ptrCert) ),
            sslhash2string  ( X509_subject_name_hash(ptrCert) ),
            asn1tim2string  ( X509_get_notBefore    (ptrCert) ),
            asn1tim2string  ( X509_get_notAfter     (ptrCert) ),
            OBJ_nid2sn(EVP_MD_type(fdig)),
            sslhash2string(md, n)
        ));
    }

    PT_SSL_LOG("Certificate data loaded");
    PT_SSL_LOG("> Version           = " << _certInfo[0].version);
    PT_SSL_LOG("> Serial number     = " << _certInfo[0].serialNumber);
    PT_SSL_LOG("> Issuer name       = " << _certInfo[0].issuerName);
    PT_SSL_LOG("> Issuer name hash  = " << _certInfo[0].issuerNameHash);
    PT_SSL_LOG("> Subject name      = " << _certInfo[0].subjectName);
    PT_SSL_LOG("> Subject name hash = " << _certInfo[0].subjectNameHash);
    PT_SSL_LOG("> Not before        = " << _certInfo[0].notBefore);
    PT_SSL_LOG("> Not after         = " << _certInfo[0].notAfter);
    PT_SSL_LOG("> Fingerprint       = " << _certInfo[0].fingerprintType << " " << _certInfo[0].fingerprintHash);
}

void SSLCertificateList::Impl::loadFromFile(const std::string& fileName)
{
    PT_SSL_LOG("Loading certificate file = " << fileName);
    
    std::string   data;
    std::ifstream ifs;
    char          rbuf[4096];

    ifs.open(fileName.c_str(), std::ios::binary);
    while(ifs) {
        ifs.read( rbuf, sizeof(rbuf) );
        data += std::string( rbuf, ifs.gcount() );
    }

    loadFromString(data);
}

void SSLCertificateList::Impl::clear()
{
    for(std::vector<X509*>::const_iterator it = _cert.begin(); it != _cert.end(); ++it) {
        X509_free(*it);
    }
    _cert.clear();

    _certInfo.clear();
}

const SSLPublicKey SSLCertificateList::Impl::getPublicKey() const
{
    EVP_PKEY* pkey = X509_get_pubkey(*_cert.begin());
    if(!pkey) {
        throw SSLRuntimeError("Could not extract the main certificate's public key!", PT_SOURCEINFO);
    }

    return SSLPublicKey(pkey);
}

} // namespace Ssl
} // namespace Pt
