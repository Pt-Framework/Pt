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
#include <cassert>
#include <openssl/ssl.h>
#include <openssl/pem.h>
#include <openssl/err.h>

log_define("Pt.Ssl.CertificateList")

namespace Pt {

namespace Ssl {

std::string toString(const X509_NAME* val)
{
    int len = 0;
    char buf[1024];
    
    BioAutoPtr out( BIO_new(BIO_s_mem()) );
    if( X509_NAME_print( out.get(), (X509_NAME*) val, 0) ) 
    {
        len = BIO_read( out.get(), buf, sizeof(buf) );
    }
    
    return std::string(buf, len);
}

std::string toString(ASN1_TIME* val)
{
    int len = 0;
    char buf[1024];

    BioAutoPtr out( BIO_new(BIO_s_mem()) );
    if( ASN1_TIME_print( out.get(), val) )
    {
        len = BIO_read( out.get(), buf, sizeof(buf) );
    }

    return std::string(buf, len);
}

class CertificateImpl
{
    public:
        explicit CertificateImpl(x509_st* x509)
        : _x509(x509)
        , _refs(1)
        {
            assert(_x509);
        }

        ~CertificateImpl()
        {
            X509_free(_x509);
        }

        void ref()
        { ++_refs; }

        unsigned unref()
        { return --_refs; }

        int serialNumber() const
        {
            return ASN1_INTEGER_get( X509_get_serialNumber(_x509) );
        }

        std::string issuer() const
        {
            return toString( X509_get_issuer_name(_x509) );
        }

        std::string subject() const
        {
            return toString( X509_get_subject_name(_x509) );
        }
        
        std::string notBefore() const
        {
            return toString( X509_get_notBefore(_x509) );
        }

        std::string notAfter() const
        {
            return toString( X509_get_notBefore(_x509) );
        }

        x509_st* getX509() const
        { return _x509; }

    private:
        x509_st* _x509;
        unsigned _refs;
};


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

        size_t size() const
        { return _certificates.size(); }

        bool empty() const
        { return _certificates.empty(); }

        const Certificate* begin() const
        { 
            return _certificates.empty() ? 0 : &_certificates[0]; 
        }

        const Certificate* end() const
        { 
            return _certificates.empty() ? 0 : &_certificates[0] + _certificates.size(); 
        }

    private:
        std::vector<Certificate> _certificates;
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
    *_impl = *(list._impl);
    return *this;
}


//For PEM we use:
//   PEM_read_PUBKEY
//   PEM_read_bio_PrivateKey
//
//For reading  ASN1 (DER) we use:
//  d2i_PUBKEY_bio
//  d2i_PrivateKey_bio
//
//For writing, I believe the functions are:
//   PEM_write_bio_PUBKEY
//   PEM_write_bio_PrivateKey
//   i2d_PUBKEY_bio
//   i2d_PrivateKey_bio

void CertificateList::fromPem(const char* data, size_t len)
{
    _impl->clear();

    BioAutoPtr in( BIO_new_mem_buf( (void*) data, len ) );

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


void CertificateList::fromPem(std::istream& is)
{
    char rbuf[4096];
    const std::streamsize rbufSize = sizeof(rbuf);
    std::string data;
    while( is ) 
    {
        is.read(rbuf, rbufSize);
        size_t count = size_t( is.gcount() );
        data.append(rbuf, count);
    }

    fromPem( data.c_str(), data.size() );
}


void CertificateList::fromPemFile(const char* path)
{
    std::ifstream ifs(path, std::ios::binary);
    fromPem(ifs);
}


void CertificateList::clear()
{ 
    _impl->clear(); 
}


bool CertificateList::empty() const
{
    return _impl->empty();
}


size_t CertificateList::size() const
{
    return _impl->size();
}


CertificateList::Iterator CertificateList::begin() const
{ 
    return Iterator( _impl->begin() );
}
        

CertificateList::Iterator CertificateList::end() const
{ 
    return Iterator( _impl->end() ); 
}

} // namespace Ssl

} // namespace Pt

/*   
std::string asn1str2string(ASN1_STRING* asn1Val)
{
    BioAutoPtr out( BIO_new(BIO_s_mem()) );
    if(!ASN1_STRING_print(out.get(), asn1Val)) return "";

    char      buf[1024];
    const int len = BIO_read(out.get(), buf, sizeof(buf));

    return len ? std::string(buf, len) : "";
}


std::string ssldata2string(const unsigned char* md, unsigned int n)
{
    std::string hash;

    char buf[1024];
    for(unsigned int i = 0; i < n; ++i) {
        sprintf(buf, "%02X", md[i]);
        hash += buf;
    }

    return hash;
}

unsigned int string2ssldata(const char* str, int slen, unsigned char* md, unsigned int nmax)
{
    const char*          ptrcur = str;
    const char*          ptrmax = ptrcur + slen;
          unsigned char* mdcur  = md;
    const unsigned char* mdmax  = mdcur + nmax;
    char                 cnv[3] = { 0, 0, 0 };

    for(;;) {
        if(ptrcur >= ptrmax || md >= mdmax) break;

        cnv[0] = *ptrcur++;
        cnv[1] = (ptrcur < ptrmax) ? (*ptrcur++) : 0;

        *mdcur++ = static_cast<unsigned char>( strtoul(cnv, 0, 16) );
    }

    return mdcur - md;
}

std::string sslhash2string(long md)
{
    char buf[1024];
    sprintf(buf, "%08lx", md);
    return buf;
}

std::string sslhash2string(const unsigned char* md, unsigned int n)
{
    std::string hash;

    char buf[1024];
    for(unsigned int i = 0; i < n; ++i) {
        sprintf(buf, "%02X", md[i]);
        if(!hash.empty()) hash += ':';
        hash += buf;
    }

    return hash;
}

void dumpCertInfo(X509* ptrCert)

    // For calculating the fingerprint hash of the certificate
    const EVP_MD* fdig = EVP_sha1();
    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int  n;

    if( ! X509_digest(ptrCert, fdig, md, &n) )
        throw InvalidCertificate("Could not calculate the certificate's fingerprint hash!");

    X509_get_version(ptrCert),
    sslhash2string  ( X509_issuer_name_hash (ptrCert) ),
    sslhash2string  ( X509_subject_name_hash(ptrCert) ),
    OBJ_nid2sn( EVP_MD_type(fdig) ),
    sslhash2string(md, n) );
}
*/
