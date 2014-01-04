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
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Logger.h>
#include <cstdio>

#include <Security/Security.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFArray.h>

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


void CertificateStoreImpl::loadPkcs12(const char* pkcs12, std::size_t len, const char* passwd)
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


const Certificate* CertificateStoreImpl::findCertificate(const std::string& subject)
{
    log_trace("find certificate: " << subject);
    
    for(std::vector<Certificate*>::const_iterator it = _allCerts.begin(); it != _allCerts.end(); ++it) 
    {
        if( (*it)->subject().find(subject) != std::string::npos )
            return *it;
    }

    return 0;
}

} // namespace Ssl

} // namespace Pt
