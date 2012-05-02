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
#include "Utils.h"
#include <Pt/Ssl/CertificateList.h>
#include <Pt/System/Logger.h>
#include <fstream>

log_define("Pt.Ssl.SSLCertificateList")

namespace Pt {

namespace Ssl {

SSLCertificateList::SSLCertificateList()
: _impl(new Impl())
{
}


SSLCertificateList::SSLCertificateList(const std::string& certData)
: _impl(new Impl())
{ 
    _impl->loadFromString(certData); 
}


SSLCertificateList::~SSLCertificateList()
{
}


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
{ 
    _impl = new Impl(); 
}


PublicKey SSLCertificateList::publicKey() const
{ 
    return _impl->publicKey(); 
}


std::vector<SSLCertificateInfo> SSLCertificateList::certificateInfo() const
{ 
    return _impl->certificateInfo(); 
}

const std::vector<x509_st*>& SSLCertificateList::impl() const
{ 
    return _impl->_cert; 
}


SSLCertificateList::Impl::Impl()
: _cert(0)
{
}


SSLCertificateList::Impl::~Impl()
{ 
    clear(); 
}


void SSLCertificateList::Impl::loadFromString(const std::string& certData)
{
    // Clear previous certificates
    clear();

    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) certData.c_str(), certData.length() ) );

    // Try to read/parse the CA X509 certificates
    while(true) 
    {
        // Read the certificate
        X509AutoPtr cert ( PEM_read_bio_X509_AUX(in.get(), 0, 0, 0) );
        if( ! cert) 
          break;

        _cert.push_back(cert.get());
        cert.release();
    }
}


void SSLCertificateList::Impl::loadFromFile(const std::string& fileName)
{
    log_info("Loading certificate from file: " << fileName);

    char rbuf[4096];
    const std::streamsize rbufSize = sizeof(rbuf);
    
    std::string data;
    std::ifstream ifs( fileName.c_str(), std::ios::binary );
    while( ifs.read(rbuf, rbufSize) ) 
    {
        size_t count = size_t( ifs.gcount() );
        data += std::string(rbuf, count);
    }

    loadFromString(data);
}


std::vector<SSLCertificateInfo> SSLCertificateList::Impl::certificateInfo() const
{
    std::vector<SSLCertificateInfo> cinfos;
    
    // For calculating the fingerprint hash of the certificate
    const EVP_MD* fdig = EVP_sha1();
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int  n;

    std::vector<x509_st*>::const_iterator it;
    for(it = _cert.begin(); it != _cert.end(); ++it)
    {
        X509* ptrCert = *it;
        if( ! X509_digest(ptrCert, fdig, md, &n) )
            throw InvalidCertificate("Could not calculate the certificate's fingerprint hash!");

        SSLCertificateInfo ci( X509_get_version(ptrCert),
                               ASN1_INTEGER_get( X509_get_serialNumber (ptrCert) ),
                               x509nam2string  ( X509_get_issuer_name  (ptrCert) ),
                               sslhash2string  ( X509_issuer_name_hash (ptrCert) ),
                               x509nam2string  ( X509_get_subject_name (ptrCert) ),
                               sslhash2string  ( X509_subject_name_hash(ptrCert) ),
                               asn1tim2string  ( X509_get_notBefore    (ptrCert) ),
                               asn1tim2string  ( X509_get_notAfter     (ptrCert) ),
                               OBJ_nid2sn( EVP_MD_type(fdig) ),
                               sslhash2string(md, n) );

        // Store some information about the certificate
        cinfos.push_back(ci);
    }

    return cinfos;
}


void SSLCertificateList::Impl::clear()
{
    for(std::vector<X509*>::const_iterator it = _cert.begin(); it != _cert.end(); ++it) 
    {
        X509_free(*it);
    }
    
    _cert.clear();
}


PublicKey SSLCertificateList::Impl::publicKey() const
{
    EVP_PKEY* pkey = _cert.empty() ? 0 : X509_get_pubkey( _cert[0] );
    if( ! pkey)
        throw InvalidCertificate("Could not extract the main certificate's public key!");

    return PublicKey(pkey);
}

} // namespace Ssl

} // namespace Pt
