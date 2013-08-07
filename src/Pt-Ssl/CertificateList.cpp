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
#include "CertificateListImpl.h"
#include "PrivateKeyImpl.h"
#include <Pt/Ssl/CertificateList.h>
#include <Pt/Ssl/PrivateKey.h>
#include <Pt/System/Logger.h>
#include <iostream>
#include <cassert>

log_define("Pt.Ssl.CertificateList")

namespace Pt {

namespace Ssl {

Certificate::Certificate()
: _impl(0)
{
}


Certificate::Certificate(const char* data, size_t len)
: _impl(0)
{
    _impl = new CertificateImpl(data, len);
}


Certificate::Certificate(CertificateImpl* impl)
: _impl(impl)
{
}


Certificate::Certificate(const Certificate& cert)
: _impl(cert._impl)
{
    if(_impl)
        _impl->ref();
}


Certificate::~Certificate()
{
    if( _impl && 0 == _impl->unref() )
    {
        delete _impl;
    }
}


Certificate& Certificate::operator=(const Certificate& cert)
{
    if( _impl && 0 == _impl->unref() )
    {
        delete _impl;
    }

    _impl = cert._impl;

    if(_impl)
        _impl->ref();

    return *this;
}


int Certificate::serialNumber() const
{
    if( ! _impl)
        return 0;

    return _impl->serialNumber();
}


std::string Certificate::issuer() const
{
    if( ! _impl)
        return std::string();

    return _impl->issuer();
}


std::string Certificate::subject() const
{
    if( ! _impl)
        return std::string();

    return _impl->subject();
}
   
        
std::string Certificate::notBefore() const
{
    if( ! _impl)
        return std::string();

    return _impl->notBefore();
}


std::string Certificate::notAfter() const
{
    if( ! _impl)
        return std::string();

    return _impl->notAfter();
}


PublicKey Certificate::publicKey() const
{
    if( ! _impl)
        return PublicKey();

    return _impl->publicKey();
}


CertificateImpl* Certificate::impl() const
{
    if( ! _impl)
        throw std::logic_error("invalid certificate implementation");

    return _impl;
}


//
// CertificateStore
//

#ifdef __APPLE__

CertificateStore::CertificateStore()
{
//    CFArrayRef items = NULL;
//
//    // NOTE: kSecMatchSearchList -> (id)keychain
//    const void* keys[]   = { kSecClass,         kSecReturnRef,  kSecMatchLimit, 0 };
//    const void* values[] = { kSecClassIdentity, kCFBooleanTrue, kSecMatchLimitAll, 0 };
//
//    CFDictionaryRef dict = CFDictionaryCreate(NULL, keys, values, 3, NULL, NULL);
//    if( ! dict)
//        throw std::runtime_error("invalid keychain values");
//
//    OSStatus status = SecItemCopyMatching(dict, (CFTypeRef*)&items);
//
//    std::clog << "FOUND:" << CFDictionaryGetCount(dict) << std::endl;
//
//    CFRelease(dict);
//
//    if( status != errSecSuccess && status != errSecItemNotFound ) 
//        throw std::runtime_error("invalid keychain");
//
//    // Do something with certificateRef here
//
//    if(items)
//        CFRelease(items);
//        
//    std::clog << "XXXXXXXXXXXXXXXX" << std::endl;
}


CertificateStore::~CertificateStore()
{
}


void CertificateStore::addPem(const char* data, size_t len, const std::string& passwd)
{
}


void CertificateStore::loadPkcs12(const char* pkcs12, size_t len, const char* passwd)
{
    CFDataRef data = CFDataCreate(NULL, reinterpret_cast<const UInt8*>(pkcs12), len);
    if( ! data)
        throw std::runtime_error("CFDataCreate");

    CFStringRef password = CFStringCreateWithCString(NULL, passwd, kCFStringEncodingUTF8);
    
    const void* keys[]   = { kSecImportExportPassphrase };
    const void* values[] = { NULL };

    CFIndex hasPassword = CFStringGetLength(password) > 0 ? 1 : 0;

    CFDictionaryRef options = CFDictionaryCreate(NULL, keys, values, hasPassword, NULL, NULL);
    if( ! options)
        throw std::runtime_error("CFDictionaryCreate");

    CFArrayRef items = NULL;
    
    OSStatus securityError = SecPKCS12Import(data, options, &items);
    assert(securityError == noErr);
    
    CFRelease(password);
    CFRelease(options);
    CFRelease(data);

    CFIndex count = CFArrayGetCount(items);

    for(CFIndex n = 0; n < count; ++n)
    {
        CFDictionaryRef item = (CFDictionaryRef) CFArrayGetValueAtIndex(items, n);

        SecIdentityRef identity = (SecIdentityRef) CFDictionaryGetValue(item, kSecImportItemIdentity);
        if(identity)
        {
            std::clog << "IDENTITY" << std::endl;
            CFRetain(identity);
            Certificate c( new CertificateImpl(identity) );
            _certificates.push_back(c);
        }

        CFArrayRef certs = (CFArrayRef) CFDictionaryGetValue(item, kSecImportItemCertChain);
        if(certs)
        {
            CFIndex certCount = CFArrayGetCount(certs);
            std::clog << "CERTS: " << certCount << std::endl;
            for(CFIndex i = 0; i < certCount; ++i)
            {
                SecCertificateRef cert = (SecCertificateRef) CFArrayGetValueAtIndex(certs, i);
                
                std::clog << "CERTIFICATE" << std::endl;
                CFRetain(cert);
                Certificate c( new CertificateImpl(cert) );
                _certificates.push_back(c);
            }
        }
    }

    CFRelease(items);
}

#else

CertificateStore::CertificateStore()
{
}


CertificateStore::~CertificateStore()
{
}


int passwordCallback(char* buff, int num, int /*rwflag*/, void* userdata)
{
    // Get the password
    const std::string& password = *((std::string*) userdata);

    // If the wanted length is not the same with the given password length, just return 0
    if((size_t) num < password.length() + 1) 
        return 0;

    // Copy the password to the buffer and return the length
    strcpy(buff, &password[0]);
    return password.length();
}


// Add the contents of a PEM file to the Certificate store
void CertificateStore::addPem(const char* data, size_t len, const std::string& passwd)
{
    BioAutoPtr in( BIO_new_mem_buf( (void*) data, len ) );

    while(true) 
    {
        // Is it a certificate
        X509AutoPtr x509 ( PEM_read_bio_X509_AUX(in.get(), 0, 0, 0) );
        if(x509)
        {
            Certificate cert( new CertificateImpl(x509.get()) );
            _certificates.push_back(cert);
            x509.release();
            continue;
        }

        // ..or is it a private key?
        evp_pkey_st* pkey = PEM_read_bio_PrivateKey(in.get(), 0, &passwordCallback, (void*) &passwd);
        if(pkey)
        {
            continue;
        }

        break;
    }

    // TODO: find out if we have private-key/certificate pairs that form an Identitiy
}


void CertificateStore::loadPkcs12(const char* data, size_t len, const char* passwd)
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
        Certificate c( new CertificateImpl(x509.get(), pkey) );
        _certificates.push_back(c);
        x509.release();
    }
    
    if(ca)
    {
        for(int i = 0; i < sk_X509_num(ca); i++)
        {
            X509AutoPtr x509( sk_X509_pop(ca) );
            Certificate cert( new CertificateImpl(x509.get()) );
            _certificates.push_back(cert);
            x509.release();
        }

        sk_X509_pop_free(ca, X509_free);
        ca = NULL;
    }  
}

#endif

void CertificateStore::loadPkcs12(std::istream& is, const char* passwd)
{
    std::vector<char> data;
    char rbuf[4096];
    const std::streamsize rbufSize = sizeof(rbuf);

    while( is )
    {
        is.read( rbuf, rbufSize );
        data.insert( data.end(), rbuf, rbuf + is.gcount() );
    }

    if( data.empty() )
        return;

    loadPkcs12(&data[0], data.size(), passwd);
}

//
// CertificateList
//

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


void CertificateList::fromPem(const char* data, size_t len)
{
    _impl->fromPem(data, len);
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

    _impl->fromPem( data.c_str(), data.size() );
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


void CertificateList::push_back(const Certificate& cert)
{
    _impl->push_back(cert);
}


bool CertificateList::empty() const
{
    return _impl->empty();
}


size_t CertificateList::size() const
{
    return _impl->size();
}


CertificateList::Iterator CertificateList::begin()
{ 
    return Iterator( _impl->begin() );
}
        

CertificateList::Iterator CertificateList::end()
{ 
    return Iterator( _impl->end() ); 
}


CertificateList::ConstIterator CertificateList::begin() const
{ 
    return ConstIterator( _impl->begin() );
}
        

CertificateList::ConstIterator CertificateList::end() const
{ 
    return ConstIterator( _impl->end() ); 
}

} // namespace Ssl

} // namespace Pt


