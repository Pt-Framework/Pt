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

log_define("Pt.Ssl.Context")

namespace Pt {

namespace Ssl {

ContextImpl::ContextImpl(Context::Protocol protocol)
: _protocol(protocol)
, _verify(Context::VerifyPeer)
, _identity(0)
, _certs(0)
, _caCerts(0)
{
    _certs = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    _caCerts = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
}


ContextImpl::~ContextImpl()
{
    if(_certs)
        CFRelease(_certs);
        
    if(_caCerts)
        CFRelease(_caCerts);
        
    std::vector<Certificate*>::iterator it;
    for(it = _allCerts.begin(); it != _allCerts.end(); ++it)
    {
        delete *it;
    }
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
    log_trace("ContextImpl::assign");
    setProtocol(ctx._protocol);
    setVerifyMode(ctx._verify);
    
    std::vector<Certificate*>::iterator iter;
    for(iter = _allCerts.begin(); iter != _allCerts.end(); ++iter)
        delete *iter;
        
    _allCerts.clear();
    
    std::vector<Certificate*>::const_iterator it;
    for(it = ctx._allCerts.begin(); it != ctx._allCerts.end(); ++it)
    {
        CertificateImpl* certImpl = (*it)->impl();
        
        if( certImpl->identity() )
        {
            SecIdentityRef ident = ctx.copyIdentity( certImpl->identity() );
            _allCerts.push_back( new Certificate( new CertificateImpl(ident) ) );
        }
        else if( certImpl->certificate() )
        {
            SecCertificateRef cert = ctx.copyCertificate( certImpl->certificate() );
            _allCerts.push_back( new Certificate( new CertificateImpl(cert) ) );
        }
    }
    
    // copy certificates to be presented to peer

    CFRelease(_certs);
    _certs = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    _identity = NULL;

    CFIndex count = CFArrayGetCount(ctx._certs);
    
    if(ctx._identity)
    {
        SecIdentityRef copy = copyIdentity(ctx._identity);
        CFArrayAppendValue(_certs, copy);
        _identity = copy;
        CFRelease(copy);
    }
        
    for(CFIndex n = 1; n < count; ++n)
    {
        SecCertificateRef cert = (SecCertificateRef) CFArrayGetValueAtIndex(ctx._certs, n);
        SecCertificateRef copy = copyCertificate(cert);
        CFArrayAppendValue(_certs, copy);
        CFRelease(copy);
    }

    // copy trusted CA certificates

    CFRelease(_caCerts);
    _caCerts = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    
    CFIndex caCount = CFArrayGetCount(ctx._caCerts);
    for(CFIndex n = 0; n < caCount; ++n)
    {
        SecCertificateRef cert = (SecCertificateRef) CFArrayGetValueAtIndex(ctx._caCerts, n);
        SecCertificateRef copy = copyCertificate(cert);
        CFArrayAppendValue(_certs, copy);
        CFRelease(copy);
    }
}


void ContextImpl::addCACertificate(const Certificate& trustedCert)
{
    SecCertificateRef cert = trustedCert.impl()->certificate();
    if( ! cert)
        return;

    SecCertificateRef copy = copyCertificate(cert);
    if( ! copy)
        throw std::logic_error("copyCertificate");

    CFArrayAppendValue(_caCerts, copy);
    CFRelease(copy);
}


// rename setIdentity
void ContextImpl::setCertificate(const Certificate& cert)
{
    SecIdentityRef ident = cert.impl()->identity();
    if( ! ident)
        return;
        
    SecIdentityRef copy = copyIdentity(ident);
    if( ! copy)
        throw std::logic_error("copyIdentity");

    if(_identity)
    {
        CFArraySetValueAtIndex(_certs, 0, copy);
    }
    else if(CFArrayGetCount(_certs) > 0)
    {
        CFArrayInsertValueAtIndex(_certs, 0, copy);
    }
    else
    {
        CFArrayAppendValue(_certs, copy);
    }
    
    _identity = copy;
    
    CFRelease(copy);
}


void ContextImpl::addCertificate(const Certificate& certificate)
{
    SecCertificateRef cert = certificate.impl()->certificate();
    if( ! cert)
        return;

    SecCertificateRef copy = copyCertificate(cert);
    if( ! copy)
        throw std::logic_error("copyCertificate");

    CFArrayAppendValue(_certs, copy);
    CFRelease(copy);
}


SecIdentityRef ContextImpl::copyIdentity(SecIdentityRef ident) const
{
    SecIdentityRef foundIdent = NULL;
    
    const void* keys[]   = { kSecClass,         kSecReturnRef,  kSecMatchLimit, 0 };
    const void* values[] = { kSecClassIdentity, kCFBooleanTrue, kSecMatchLimitAll, 0 };

    CFDictionaryRef dict = CFDictionaryCreate(NULL, keys, values, 3, NULL, NULL);
    if( ! dict)
        throw std::runtime_error("invalid keychain values");

    CFArrayRef items = NULL;
    SecItemCopyMatching(dict, (CFTypeRef*)&items);
    CFRelease(dict);

    if( ! items)
        return foundIdent;
        
    SecCertificateRef cert = NULL;
    SecIdentityCopyCertificate (ident, &cert);
    CFDataRef data = SecCertificateCopyData(cert);
    CFIndex dataLength = CFDataGetLength(data);
    
    CFIndex count = CFArrayGetCount(items);
    for(CFIndex n = 0; n < count; ++n)
    {
        SecIdentityRef identCmp = (SecIdentityRef) CFArrayGetValueAtIndex(items, n);
        
        SecCertificateRef certCmp = NULL;
        SecIdentityCopyCertificate (identCmp, &certCmp);
        
        CFDataRef dataCmp = SecCertificateCopyData(certCmp);
        CFIndex lengthCmp = CFDataGetLength(dataCmp);

        if(dataLength == lengthCmp)
        {
            int cmp = memcmp(CFDataGetBytePtr(data), CFDataGetBytePtr(dataCmp), dataLength);
            if(cmp == 0)
                foundIdent = identCmp;
        }

        CFRelease(certCmp);
        CFRelease(dataCmp);
        
        if(foundIdent)
            break;
    }

    if(foundIdent)
        CFRetain(foundIdent); 
   
    CFRelease(cert);
    CFRelease(data);
    CFRelease(items);
    
    return foundIdent;
}


SecCertificateRef ContextImpl::copyCertificate(SecCertificateRef cert) const
{
    SecCertificateRef foundCert = NULL;
    
    const void* keys[]   = { kSecClass,            kSecReturnRef,  kSecMatchLimit, 0 };
    const void* values[] = { kSecClassCertificate, kCFBooleanTrue, kSecMatchLimitAll, 0 };

    CFDictionaryRef dict = CFDictionaryCreate(NULL, keys, values, 3, NULL, NULL);
    if( ! dict)
        throw std::runtime_error("invalid keychain values");

    CFArrayRef items = NULL;
    SecItemCopyMatching(dict, (CFTypeRef*)&items);
    CFRelease(dict);

    if( ! items)
        return foundCert;

    CFDataRef data = SecCertificateCopyData(cert);
    CFIndex dataLength = CFDataGetLength(data);
    
    CFIndex count = CFArrayGetCount(items);
    for(CFIndex n = 0; n < count; ++n)
    {
        SecCertificateRef certCmp = (SecCertificateRef) CFArrayGetValueAtIndex(items, n);
        
        CFDataRef dataCmp = SecCertificateCopyData(certCmp);
        CFIndex lengthCmp = CFDataGetLength(dataCmp);

        if(dataLength == lengthCmp)
        {
            int cmp = memcmp(CFDataGetBytePtr(data), CFDataGetBytePtr(dataCmp), dataLength);
            if(cmp == 0)
                foundCert = certCmp;
        }

        CFRelease(dataCmp);
        
        if(foundCert)
            break;
    }

    if(foundCert)
        CFRetain(foundCert);

    CFRelease(data);
    CFRelease(items);

    return foundCert;
}


void ContextImpl::loadPkcs12(const char* pkcs12, size_t len, const char* passwd)
{
    log_debug("loadPkcs12: " << passwd);

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
    log_trace("SecPKCS12Import: " <<  securityError);

    CFRelease(password);
    CFRelease(options);
    CFRelease(data);
    
    if(securityError != noErr)
    {
        if(items)
            CFRelease(items);
            
        throw InvalidCertificate("invalid PKCS12 data");
    }
    
    if( ! items)
        return;

    CFIndex count = CFArrayGetCount(items);

    for(CFIndex n = 0; n < count; ++n)
    {
        CFDictionaryRef item = (CFDictionaryRef) CFArrayGetValueAtIndex(items, n);

        SecIdentityRef identity = (SecIdentityRef) CFDictionaryGetValue(item, kSecImportItemIdentity);
        if(identity)
        {
            CFRetain(identity);
            Certificate* c = new Certificate( new CertificateImpl(identity) );
            _allCerts.push_back(c);
            
            log_debug("imported identity: " << c->subject());
        }

        CFArrayRef certs = (CFArrayRef) CFDictionaryGetValue(item, kSecImportItemCertChain);
        if(certs)
        {
            CFIndex certCount = CFArrayGetCount(certs);
            for(CFIndex i = 0; i < certCount; ++i)
            {
                SecCertificateRef cert = (SecCertificateRef) CFArrayGetValueAtIndex(certs, i);

                CFRetain(cert);
                Certificate* c = new Certificate( new CertificateImpl(cert) );
                _allCerts.push_back(c);

                log_debug("imported certificate: " << c->subject());
            }
        }
    }

    CFRelease(items);
}


Certificate* ContextImpl::findCertificate(const std::string& subject)
{
    log_trace("findCertificate: " << subject);
    
    std::vector<Certificate*>::iterator it;
    for(it = _allCerts.begin(); it != _allCerts.end(); ++it)
    {
        log_trace("compare: " << (*it)->subject());
        if((*it)->subject().find(subject) != std::string::npos)
            return *it;
    }

    log_warn("------ NOT FOUND --------");
    return 0;

    //CFStringRef cfsubj = CFStringCreateWithCString(NULL, subject.c_str(), kCFStringEncodingUTF8);

    // TODO: use dedicated keychain

    // NOTE: kSecMatchSearchList -> (id)keychain
    
    Certificate ret;
    /*
    const void* keys[]   = { kSecClass,         kSecReturnRef,  kSecMatchLimit,    kSecMatchSubjectContains, 0 };
    const void* values[] = { kSecClassIdentity, kCFBooleanTrue, kSecMatchLimitAll, cfsubj, 0 };

    CFDictionaryRef dict = CFDictionaryCreate(NULL, keys, values, 4, NULL, NULL);
    if( ! dict)
        throw std::logic_error("CFDictionaryCreate");

    CFArrayRef items = NULL;
    OSStatus error = SecItemCopyMatching(dict, (CFTypeRef*)&items);
    CFRelease(dict);
    CFRelease(cfsubj);

    if( ! error && items && 0 < CFArrayGetCount(items) )
    {
        SecIdentityRef cert = (SecIdentityRef) CFArrayGetValueAtIndex(items, 0);
        log_trace("found certificate: " << cert);
        CFRetain(cert);
        ret = Certificate( new CertificateImpl(cert) );
        std::cerr << ret.subject() << std::endl;
    }

    if(items)
        CFRelease(items);
    */
    
    // TODO: search among certificates with kSecClassCertificate

    //return ret;
}

} // namespace Ssl

} // namespace Pt
