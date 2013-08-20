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

#ifndef PT_SSL_CERTIFICATEIMPL_H
#define PT_SSL_CERTIFICATEIMPL_H

#include <Pt/Ssl/SslError.h>
#include <Pt/Atomicity.h>
#include <cassert>
#include <string>
#include <iostream>

#import <Security/Security.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreFoundation/CFDictionary.h>

namespace Pt {

namespace Ssl {

class CertificateImpl
{
    public:
        explicit CertificateImpl(SecCertificateRef cert)
        : _ident(0)
        , _cert(cert)
        , _refs(1)
        {
            assert(_cert);
        }

        explicit CertificateImpl(SecIdentityRef identity)
        : _ident(identity)
        , _cert(0)
        , _refs(1)
        {
            SecIdentityCopyCertificate(identity, &_cert);
            assert(_cert);
        }

        ~CertificateImpl()
        {
            if(_ident)
                CFRelease(_ident);

            CFRelease(_cert);
        }

        void ref()
        { atomicIncrement(_refs); }

        int unref()
        { return atomicDecrement(_refs); }

        int serialNumber() const
        {
            CFDataRef data = SecCertificateCopySerialNumber(_cert, NULL);
            
            if( ! data )
                return 0;
                
            const size_t sz = sizeof(int); 
            UInt8 buf[sz];
            CFDataGetBytes(data, CFRangeMake(0, sz), buf);
            CFRelease(data);
            
            int n = 0;
            memcpy(&n, buf, sz);
            return n;
        }

        std::string issuer() const
        {
            return "";
        }

        std::string subject() const
        {
            std::string r("Unknown Organization, Unknown Name");
            
            CFStringRef summary = SecCertificateCopySubjectSummary(_cert);
            if(summary)
            {
                char buf[255];
                CFStringGetCString(summary, buf, sizeof(buf), kCFStringEncodingUTF8);
                r = buf;
                CFRelease(summary);
            }
            
            return r;
        }
        
        std::string notBefore() const
        {
            return "";
        }

        std::string notAfter() const
        {
            return "";
        }
        
        SecCertificateRef certificate() const
        { return _cert; }
        
        SecIdentityRef identity() const
        { return _ident; }

    private:
        SecIdentityRef _ident;
        SecCertificateRef _cert;
        Pt::atomic_t _refs;
};

} // namespace Ssl

} // namespace Pt

#endif
