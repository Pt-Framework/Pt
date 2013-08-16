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

#include "ContextImpl.h"
#include "CertificateImpl.h"
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Mutex.h>
#include <Pt/System/Logger.h>
#include <cstdio>

#include <Security/Security.h>
#include <CoreFoundation/CFArray.h>

log_define("Pt.Ssl.Context")

namespace Pt {

namespace Ssl {

ContextImpl::ContextImpl(Context::Protocol protocol)
: _protocol(protocol)
, _verify(Context::VerifyPeer)
{
}


ContextImpl::~ContextImpl()
{
}


Context::Protocol ContextImpl::protocol() const
{ 
    return _protocol; 
}


void ContextImpl::setProtocol(Context::Protocol protocol)
{
    _protocol = protocol;
}


void ContextImpl::setVerifyDepth(int n)
{
}


void ContextImpl::setVerifyMode(Context::VerifyMode m)
{
    _verify = m;
}


void ContextImpl::assign(const ContextImpl& ctx)
{
    setProtocol(ctx._protocol);
    setVerifyMode(ctx._verify);

    _caCerts =    ctx._caCerts;
    _cert =       ctx._cert;
    //_extraCerts = ctx._extraCerts;
    
    _certificates = ctx._certificates;
    // VerifyMode
    // VerifyDepth

    CFArrayRef items = NULL;

    // NOTE: kSecMatchSearchList -> (id)keychain
    //const void* keys[]   = { kSecClass,         kSecReturnRef,  kSecMatchLimit, 0 };
    //const void* values[] = { kSecClassIdentity, kCFBooleanTrue, kSecMatchLimitAll, 0 };

    //CFDictionaryRef dict = CFDictionaryCreate(NULL, keys, values, 3, NULL, NULL);
    //if( ! dict)
    //    throw std::runtime_error("invalid keychain values");

    //OSStatus status = SecItemCopyMatching(dict, (CFTypeRef*)&items);

    //if(items)
    //    std::clog << "FOUND: " << CFArrayGetCount(items) << std::endl;

    //CFRelease(dict);

    //if( status != errSecSuccess && status != errSecItemNotFound ) 
    //    throw std::runtime_error("invalid keychain");

    //// Do something with certificateRef here

    //if(items)
    //    CFRelease(items);
}


//void ContextImpl::setCACertificates(const std::vector<Certificate>& caCerts)
//{
//    _caCerts.clear();
//
//    for(CertificateStore::ConstIterator it = caCerts.begin(); it != caCerts.end(); ++it) 
//    {
//        _caCerts.push_back(*it);
//    }
//}


void ContextImpl::addCACertificate(const Certificate& trustedCert)
{
    _caCerts.push_back(trustedCert);
}


void ContextImpl::setCertificate(const Certificate& cert)
{
    _cert = cert;
}


//void Context::setCertificateChain(const CertificateList& certs)
//{
//    CertificateList::ConstIterator it = certs.begin();
//    if( it == certs.end() )
//        throw InvalidCertificate("invalid certificate chain");
//
//    this->setCertificate(*it);
//    ++it;
//
//    for(; it != certs.end(); ++it)
//    {
//        _extraCerts.push_back(*it);
//    }
//}


void ContextImpl::loadPkcs12(const char* pkcs12, size_t len, const char* passwd)
{
    std::clog << "loadPkcs12: " << passwd << std::endl;

    CFDataRef data = CFDataCreate(NULL, reinterpret_cast<const UInt8*>(pkcs12), len);
    if( ! data)
        throw std::runtime_error("CFDataCreate");

    CFStringRef password = CFStringCreateWithCString(NULL, passwd, kCFStringEncodingUTF8);
    
    const void* keys[]   = { kSecImportExportPassphrase };
    const void* values[] = { password };

    CFIndex hasPassword = CFStringGetLength(password) > 0 ? 1 : 0;

    CFDictionaryRef options = CFDictionaryCreate(NULL, keys, values, hasPassword, NULL, NULL);
    if( ! options)
        throw std::runtime_error("CFDictionaryCreate");

    CFArrayRef items = NULL;
    
    OSStatus securityError = SecPKCS12Import(data, options, &items);
    std::clog << "SecPKCS12Import: " <<  securityError << std::endl;
    assert(securityError == noErr);
    
    CFRelease(password);
    CFRelease(options);
    CFRelease(data);

    // don't do this, remove _certificates variable.
    // use dedicated keychain instead
    CFIndex count = CFArrayGetCount(items);

    for(CFIndex n = 0; n < count; ++n)
    {
        CFDictionaryRef item = (CFDictionaryRef) CFArrayGetValueAtIndex(items, n);

        SecIdentityRef identity = (SecIdentityRef) CFDictionaryGetValue(item, kSecImportItemIdentity);
        if(identity)
        {
            std::clog << "IDENTITY_" << n << std::endl;
            CFRetain(identity);
            Certificate c( new CertificateImpl(identity) );
            _certificates.push_back(c);
        }

        CFArrayRef certs = (CFArrayRef) CFDictionaryGetValue(item, kSecImportItemCertChain);
        if(certs)
        {
            CFIndex certCount = CFArrayGetCount(certs);
            std::clog << "CERTS_" << n << ": " << certCount << std::endl;
            for(CFIndex i = 0; i < certCount; ++i)
            {
                SecCertificateRef cert = (SecCertificateRef) CFArrayGetValueAtIndex(certs, i);
                
                CFStringRef summary = SecCertificateCopySubjectSummary(cert);
                if(summary)
                {
                    char buf[200];
                    CFStringGetCString(summary, buf, 200, kCFStringEncodingUTF8);
                    std::cerr << buf << std::endl;
                    CFRelease(summary);
                }

                CFRetain(cert);
                Certificate c( new CertificateImpl(cert) );
                _certificates.push_back(c);
            }
        }
    }

    CFRelease(items);
}


Certificate ContextImpl::findCertificate(const std::string& subject)
{
    log_trace("findCertificate: " << subject);
    
    CFStringRef cfsubj = CFStringCreateWithCString(NULL, subject.c_str(), kCFStringEncodingUTF8);

    // TODO: use dedicated keychain

    // NOTE: kSecMatchSearchList -> (id)keychain
    const void* keys[]   = { kSecClass,         kSecReturnRef,  kSecMatchLimit,    kSecMatchSubjectContains, 0 };
    const void* values[] = { kSecClassIdentity, kCFBooleanTrue, kSecMatchLimitAll, cfsubj, 0 };

    CFDictionaryRef dict = CFDictionaryCreate(NULL, keys, values, 4, NULL, NULL);
    if( ! dict)
        throw std::logic_error("CFDictionaryCreate");

    CFArrayRef items = NULL;
    OSStatus error = SecItemCopyMatching(dict, (CFTypeRef*)&items);
    CFRelease(dict);
    CFRelease(cfsubj);

    Certificate ret;

    if( ! error && items && 0 < CFArrayGetCount(items) )
    {
        SecIdentityRef cert = (SecIdentityRef) CFArrayGetValueAtIndex(items, 0);
        log_trace("found ertificate: " << cert);
        CFRetain(cert);
        ret = Certificate( new CertificateImpl(cert) );
    }

    if(items)
        CFRelease(items);
    
    // TODO: search among certificates with kSecClassCertificate

    return ret;
}

} // namespace Ssl

} // namespace Pt
