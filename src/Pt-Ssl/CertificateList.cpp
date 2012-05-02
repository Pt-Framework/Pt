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

log_define("Pt.Ssl.CertificateList")

namespace Pt {

namespace Ssl {

/*std::vector<SSLCertificateInfo> CertificateListImpl::certificateInfo() const
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
}*/

class CertificateImpl
{
    public:
        explicit CertificateImpl(x509_st* x509)
        : _x509(x509)
        , _refs(1)
        {}

        ~CertificateImpl()
        {
            if(_x509)
                X509_free(_x509);
        }

        void ref()
        { ++_refs; }

        unsigned unref()
        { return --_refs; }

        x509_st* getX509() const
        { return _x509; }

    private:
        x509_st* _x509;
        unsigned _refs;
};


Certificate::Certificate(x509_st* x509)
: _impl( new CertificateImpl(x509) )
{
}


Certificate::Certificate(const Certificate& cert)
: _impl(cert._impl)
{
    _impl->ref();
}


Certificate::~Certificate()
{
    if( 0 == _impl->unref() )
    {
        delete _impl;
    }
}


Certificate& Certificate::operator=(const Certificate& cert)
{
    if( 0 == _impl->unref() )
    {
        delete _impl;
    }

    _impl = cert._impl;
    _impl->ref();

    return *this;
}


PublicKey Certificate::publicKey() const
{
    EVP_PKEY* pkey = _impl->getX509() ? 0 : X509_get_pubkey( _impl->getX509()  );
    if( ! pkey)
        throw InvalidCertificate("Could not extract the main certificate's public key!");

    return PublicKey(pkey);
}


x509_st* Certificate::getX509() const
{
    return _impl->getX509();
}



class CertificateListImpl
{
    public:
        CertificateListImpl()
        { }

        CertificateListImpl(const CertificateListImpl& list)
        { _certificates = list._certificates; }

        ~CertificateListImpl()
        { clear(); }

        void push_back(const Certificate& cert)
        { _certificates.push_back(cert); }

        void clear()
        { _certificates.clear(); }

        std::vector<Certificate>& get()
        { return _certificates; }

        const std::vector<Certificate>& get() const
        { return _certificates; }

    private:
        std::vector<Certificate> _certificates;
};


CertificateList::CertificateList()
: _impl( new CertificateListImpl() )
{
}


CertificateList::CertificateList(const CertificateList& list)
: _impl( new CertificateListImpl( *(list._impl) ) )
{
}


CertificateList::~CertificateList()
{
    delete _impl;
}


CertificateList& CertificateList::operator=(const CertificateList& list)
{
    const std::vector<Certificate>& certs = list._impl->get();
    for(std::vector<Certificate>::const_iterator it = certs.begin(); it != certs.end(); ++it) 
    {
        _impl->push_back(*it);
    }

    return *this;
}


void CertificateList::loadFromString(const std::string& certData)
{
    _impl->clear();

    // Create a read-only memory BIO from the given string
    BioAutoPtr in( BIO_new_mem_buf( (void*) certData.c_str(), certData.length() ) );

    // Try to read/parse the CA X509 certificates
    while(true) 
    {
        // Read the certificate
        X509AutoPtr x509 ( PEM_read_bio_X509_AUX(in.get(), 0, 0, 0) );
        if( ! x509) 
          break;

        Certificate cert( x509.get() );
        _impl->push_back(cert);
        
        x509.release();
    }
}


void CertificateList::loadFromFile(const std::string& fileName)
{
    _impl->clear();

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


void CertificateList::clear()
{ 
    _impl->clear(); 
}


bool CertificateList::empty() const
{
    return _impl->get().empty();
}


size_t CertificateList::size() const
{
    return _impl->get().size();
}


CertificateList::Iterator CertificateList::begin() const
{ 
    const std::vector<Certificate>& certs = _impl->get();
    const Certificate* c = certs.empty() ? 0 : &certs[0];
    return Iterator(c); 
}
        

CertificateList::Iterator CertificateList::end() const
{ 
    const std::vector<Certificate>& certs = _impl->get();
    const Certificate* c = certs.empty() ? 0 : &certs[0] + certs.size();
    return Iterator(c); 
}

} // namespace Ssl

} // namespace Pt
