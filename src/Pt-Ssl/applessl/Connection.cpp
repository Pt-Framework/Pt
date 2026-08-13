/*
 * Copyright (C) 2010-2012 by Marc Boris Duerner
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

#include "Connection.h"
#include "ContextImpl.h"
#include "CertificateImpl.h"
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Logger.h>
#include <streambuf>
#include <cassert>
#include <cstring>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFDictionary.h>

PT_LOG_DEFINE("Pt.Ssl.StreamBuffer")

namespace Pt {

namespace Ssl {

Connection::Connection(Context& ctx, std::ios& ios, OpenMode omode)
: _ctx(&ctx)
, _context(0)
, _ios(&ios)
, _mode(omode)
, _iocount(0)
, _connected(false)
, _wantRead(false)
, _isReading(false)
, _isWriting(false)
, _receivedShutdown(false)
, _sentShutdown(false)
{
    Boolean isServer = (omode == Accept);

    _context = SSLCreateContext(kCFAllocatorDefault, 
                                isServer ? kSSLServerSide : kSSLClientSide,
                                kSSLStreamType);

    SSLSetConnection(_context, (SSLConnectionRef) this);

    SSLSetIOFuncs(_context, 
                  &Connection::sslReadCallback, 
                  &Connection::sslWriteCallback);
   
    //SSLSetProtocolVersionEnabled(_context, kSSLProtocolAll, false);

    SSLProtocol protocolMin = kSSLProtocol2;
    SSLProtocol protocolMax = kTLSProtocolMaxSupported;
    
    switch( _ctx->protocol() ) 
    {
        default:
        case SSLv3or2:
            //SSLSetProtocolVersionEnabled(_context, kSSLProtocol2, true);
            //SSLSetProtocolVersionEnabled(_context, kSSLProtocol3, true);
            protocolMin = kSSLProtocol2;
            protocolMax = kTLSProtocolMaxSupported;
            break;

        case SSLv2:
            protocolMin = kSSLProtocol2;
            protocolMax = kSSLProtocol2;
            //SSLSetProtocolVersionEnabled(_context, kSSLProtocol2, true);
            break;

        case SSLv3:
            //SSLSetProtocolVersionEnabled(_context, kSSLProtocol3, true);
            protocolMin = kSSLProtocol3;
            protocolMax = kSSLProtocol3;
            break;
      
        case TLSv1:
            //SSLSetProtocolVersionEnabled(_context, kTLSProtocol1, true);
            protocolMin = kTLSProtocol1;
            protocolMax = kTLSProtocol1;
            break;

        case TLSv1_1:
            //SSLSetProtocolVersionEnabled(_context, kTLSProtocol1, true);
            protocolMin = kTLSProtocol11;
            protocolMax = kTLSProtocol11;
            break;

        case TLSv1_2:
            //SSLSetProtocolVersionEnabled(_context, kTLSProtocol1, true);
            protocolMin = kTLSProtocol12;
            protocolMax = kTLSProtocol12;
            break;
    }

    SSLSetProtocolVersionMin(_context, protocolMin);
    SSLSetProtocolVersionMax(_context, protocolMax);

    if(isServer)
    {
//#ifdef PT_IOS
        //SSLSetEnableCertVerify(_context, false);
        SSLSetSessionOption(_context, kSSLSessionOptionBreakOnClientAuth, TRUE);
// #else
//         if(_ctx->verifyMode() == NoVerify)
//         {
//             SSLSetClientSideAuthenticate(_context, kNeverAuthenticate);
//         }
//         else if(_ctx->verifyMode() == TryVerify)
//         {
//             SSLSetClientSideAuthenticate(_context, kTryAuthenticate);
//         }
//         else if(_ctx->verifyMode() == AlwaysVerify)
//         {
//             SSLSetClientSideAuthenticate(_context, kAlwaysAuthenticate);
//         }
        
//         CFArrayRef caArr = _ctx->impl()->caCertificates();
//         SSLSetCertificateAuthorities(_context, caArr, true);
//         SSLSetTrustedRoots(_context, caArr, true);
// #endif
    }
    else
    {
        //SSLSetEnableCertVerify(_context, false);
        SSLSetSessionOption(_context, kSSLSessionOptionBreakOnServerAuth, TRUE);
    }

    // certificates to present to peer
    CFArrayRef certs = _ctx->impl()->certificates();
    if(certs)
    {
        PT_LOG_DEBUG("using " << CFArrayGetCount(certs) << " certificates");
        SSLSetCertificate(_context, certs);
    }
}


Connection::~Connection()
{
    CFRelease(_context);
}


void Connection::setPeerName(const std::string& peerName)
{
    SSLSetPeerDomainName(_context, peerName.c_str(), peerName.size());
}


const char* Connection::currentCipher() const
{
    SSLCipherSuite cipherSuite;
    SSLGetNegotiatedCipher(_context, &cipherSuite);

    const char* name = toCipherName(cipherSuite);
    return name;
}


bool Connection::writeHandshake()
{
    PT_LOG_TRACE("Connection::writeHandshake");

    _iocount = 0;
    _isWriting = true;
    OSStatus status = SSLHandshake(_context);
    _isWriting = false;

    PT_LOG_DEBUG("SSLHandshake returns " << status);

    if(status == noErr)
    {       
        PT_LOG_DEBUG("SSL handshake completed");
        _connected = true;
    }
    else if(status != errSSLWouldBlock)
    {
        throw HandshakeFailed("SSL handshake failed");
    }

    return _iocount > 0;
}


bool Connection::readHandshake()
{
    PT_LOG_TRACE("Connection::readHandshake");

    std::streambuf* sb = _ios->rdbuf();
    if( ! sb)
        return true;

    _maxImport = sb->in_avail();
    _wantRead = false;
    _isReading = true;
    OSStatus status = SSLHandshake(_context);
    _isReading = false;

    PT_LOG_DEBUG("SSLHandshake returns " << status);
    
    if( status == noErr )
    {
        PT_LOG_DEBUG("SSL handshake completed " << this);
        _connected = true;
        return false;
    }

#ifdef PT_IOS
    if(status == errSSLPeerAuthCompleted)
#else
    if(status == errSSLServerAuthCompleted || status == errSSLClientAuthCompleted)
#endif
    {
        PT_LOG_DEBUG("AUTHENTICATING peer");

        if( _ctx->verifyMode() != NoVerify )
        {
            PT_LOG_DEBUG("evaluating trust " << this);
        
            char peerName[256];
            size_t peerNameSize = 256;
            SSLGetPeerDomainName(_context, peerName, &peerNameSize);
            (std::clog << "PN: ") .write(peerName, peerNameSize) << std::endl;

            CFStringRef pn = CFStringCreateWithBytes(kCFAllocatorDefault, 
                                                     (const UInt8*)peerName, peerNameSize, 
                                                     kCFStringEncodingUTF8, FALSE);

            Boolean isServer = (_mode != Accept); // true on client side
            SecPolicyRef policy = SecPolicyCreateSSL(isServer, pn);
            //SecPolicyRef policy = SecPolicyCreateBasicX509();
            PT_LOG_DEBUG("is server policy " << (isServer == TRUE));

            //SecTrustRef origTrust = NULL;
            SecTrustRef trust = NULL;
            SSLCopyPeerTrust(_context, &trust);

            CFMutableArrayRef policies = NULL;
            policies = CFArrayCreateMutable(kCFAllocatorDefault, 1, &kCFTypeArrayCallBacks);
            CFArrayAppendValue(policies, policy);

            //OSStatus policyErr = SecTrustSetPolicies(trust, policies); 
            //PT_LOG_DEBUG("SecTrustSetPolicies " << policyErr);

            // CFIndex numberOfCerts = SecTrustGetCertificateCount(trust);
            // std::clog << "numberOfCerts: " << numberOfCerts << std::endl;

            // CFMutableArrayRef certs = NULL;
            // certs = CFArrayCreateMutable(NULL,
            //                             numberOfCerts,
            //                             &kCFTypeArrayCallBacks);
            // for (CFIndex index = 0; index < numberOfCerts; ++index) 
            // {
            //     SecCertificateRef cert;
            //     cert = SecTrustGetCertificateAtIndex(trust, index);
            //     CFArrayAppendValue(certs, cert);
            // }

            
            // SecTrustCreateWithCertificates(certs, policy, &trust);

            CFIndex certCount = SecTrustGetCertificateCount(trust);
            std::clog << "certs to check: " << certCount << std::endl;

            CFArrayRef caArr = _ctx->impl()->caCertificates();
            std::clog << "CA certs: " << CFArrayGetCount(caArr) << std::endl;
            SecTrustSetAnchorCertificates(trust, caArr);
            SecTrustSetAnchorCertificatesOnly(trust, true);

            OSStatus policyErr = SecTrustSetPolicies(trust, policies); 
            PT_LOG_DEBUG("SecTrustSetPolicies " << policyErr);

            SecTrustResultType result;
            OSStatus evalErr = SecTrustEvaluate(trust, &result);
            if(evalErr)
                throw HandshakeFailed("SSL handshake failed");
        
            if(result == kSecTrustResultRecoverableTrustFailure)
            {
                CFArrayRef items = SecTrustCopyProperties(trust);

                CFIndex count = CFArrayGetCount(items);
                for(CFIndex n = 0; n < count; ++n)
                {
                    std::clog << "error: ";
                    CFDictionaryRef item = (CFDictionaryRef) CFArrayGetValueAtIndex(items, n);
                    CFStringRef str = (CFStringRef) CFDictionaryGetValue(item, 
                                                                kSecPropertyTypeWarning);
                    if(! str) str = (CFStringRef) CFDictionaryGetValue(item, 
                                                                kSecPropertyTypeError);
                    if(! str) str = (CFStringRef) CFDictionaryGetValue(item, 
                                                                kSecPropertyTypeSuccess);
                    if(! str) str = (CFStringRef) CFDictionaryGetValue(item, 
                                                                kSecPropertyTypeSection);
                    if(! str) str = (CFStringRef) CFDictionaryGetValue(item, 
                                                                kSecPropertyTypeDate);
                    if(! str) str = (CFStringRef) CFDictionaryGetValue(item, 
                                                                kSecPropertyTypeData);
                    if(! str) str = (CFStringRef) CFDictionaryGetValue(item, 
                                                                kSecPropertyTypeString);
                    if(! str) str = (CFStringRef) CFDictionaryGetValue(item, 
                                                                kSecPropertyTypeURL);
                    if(! str) str = (CFStringRef) CFDictionaryGetValue(item, 
                                                                kSecPropertyTypeTitle);

                    if(str)
                        std::clog << CFStringGetCStringPtr(str, kCFStringEncodingASCII);
                        
                    std::clog << std::endl;
                }
            }

            CFIndex count = SecTrustGetCertificateCount(trust);
            PT_LOG_DEBUG("SecTrustEvaluate: " << result << " certs: " << count);

            for(CFIndex n = 0; n < count; ++n)
            {
                SecCertificateRef cert = SecTrustGetCertificateAtIndex(trust, n);

                CFStringRef cn;
                SecCertificateCopyCommonName(cert, &cn);

                char cnbuf[256];
                CFStringGetCString(cn, cnbuf, 256, kCFStringEncodingUTF8);
                std::clog << "CN: " << cnbuf << std::endl; 
            }
            //if(origTrust)
            //    CFRelease(origTrust);

            if(trust)
                CFRelease(trust);
            
            //if(certs)
            //    CFRelease(certs);

            if(policy)
                CFRelease(policy);

            // if peer presented no certificate, SecTrustGetCertificateCount
            // should return 0. If we require one because AlwaysVerify is
            // set, the handshake is considered to be failed
            if(_ctx->verifyMode() == AlwaysVerify && count == 0)
                throw HandshakeFailed("SSL handshake failed");

            if( (result != kSecTrustResultProceed) && 
                (result != kSecTrustResultUnspecified) )
                throw HandshakeFailed("SSL handshake failed");

            PT_LOG_DEBUG("authentication successful");
        }

        return readHandshake();
    }
    
    if( status != errSSLWouldBlock )
    {
        throw HandshakeFailed("SSL handshake failed");
    }

    return _wantRead;
}


bool Connection::shutdown()
{
    if( ! _connected )
        return true;

    std::streambuf* sb = _ios->rdbuf();
    if( ! sb)
        return false;

    if( ! _sentShutdown)
    {
        // write shutdown notify
        PT_LOG_DEBUG("write shutdown notify");

        _isWriting = true;
        OSStatus error = SSLClose(_context);
        _isWriting = false;

        PT_LOG_DEBUG("SSLClose: " << error);

        if(error == errSSLWouldBlock)
        {
            // need to read shutdown alert
            PT_LOG_DEBUG("want to read shutdown alert");
            _sentShutdown = true;
            return false;
        }

        if(error != noErr)
            throw SslError("shutdown failed");

        PT_LOG_DEBUG("shutdown complete");
        _connected = false;
        _sentShutdown = false;
        _receivedShutdown = false;
        return true;
    }
    
    // read shutdown notify
    PT_LOG_DEBUG("read shutdown notify");

    _maxImport = sb->in_avail();
    _wantRead = false;
    _isReading = true;
    OSStatus error = SSLClose(_context);
    _isReading = false;

    PT_LOG_DEBUG("SSLClose: " << error);

    if(error == errSSLWouldBlock)
    {
        return false;
    }

    if(error != noErr)
        throw SslError("shutdown failed");

    PT_LOG_DEBUG("shutdown complete");
    _connected = false;
    _sentShutdown = false;
    _receivedShutdown = false;
    return true;
}


bool Connection::isShutdown() const
{
    return _receivedShutdown || _sentShutdown;
}


bool Connection::isClosed() const
{   
    return ! _connected;
}


std::streamsize Connection::write(const char* buf, std::size_t n)
{
    std::streambuf* sb = _ios->rdbuf();
    if( ! sb)
        return 0;

    std::size_t processed = 0;
    OSStatus error = SSLWrite(_context, buf, n, &processed);
    
    if(error != noErr && error != errSSLWouldBlock)
        throw SslError("encoding failed");

    return static_cast<std::streamsize>(processed);
}


std::streamsize Connection::read(char* buf, std::size_t n, std::streamsize maxImport)
{
    PT_LOG_TRACE("Connection::read");

    std::streambuf* sb = _ios->rdbuf();
    if( ! sb)
        return 0;

    if(maxImport == 0) 
        maxImport = sb->in_avail();

    std::size_t processed = 0;
    
    _isReading = true;
    _maxImport = maxImport;
    OSStatus error = SSLRead(_context, buf, n, &processed);
    _isReading = false;

    PT_LOG_TRACE("Connection::read: " << error);
    
    if(error == errSSLClosedGraceful)
    {
        _receivedShutdown = true;
    }
    else if(error != noErr && error != errSSLWouldBlock)
    {
        throw SslError("decoding failed");
    }

    return static_cast<std::streamsize>(processed);
}


OSStatus Connection::sslRead(void* data, std::size_t* n)
{    
    PT_LOG_TRACE("Connection::sslRead: wants " << *n << " bytes");
    
    _wantRead = false;
    std::streambuf* sb = _ios->rdbuf();

    PT_LOG_DEBUG("max input: " << _maxImport);
    if(_isWriting || ! sb || _maxImport <= 0)
    {
        _wantRead = true;
        *n = 0;
        return errSSLWouldBlock;
    }       

    std::streamsize avail = sb->in_avail();
    if(avail == 0 && _maxImport == 0)
    {
        _wantRead = true;
        *n = 0;
        return errSSLWouldBlock;
    }
    
    std::streamsize refill = static_cast<std::streamsize>(*n);
    if(_maxImport != 0)
        refill = std::min(refill, _maxImport);
    else
        refill = std::min(refill, avail);

    std::streamsize gcount = sb->sgetn(reinterpret_cast<char*>(data), refill);
    PT_LOG_DEBUG("read " << gcount << " bytes from input");

    OSStatus ret = noErr;
    
    if( static_cast<std::size_t>(gcount) < (*n) )
    {
        _wantRead = true;
        ret = errSSLWouldBlock;
    }

    if(gcount <= 0)
    {
        ret = errSSLClosedNoNotify;
    }
    
    PT_LOG_DEBUG("sslRead: " << ret);
    
    *n = static_cast<std::size_t>(gcount);
    return ret;
}


OSStatus Connection::sslWrite(const void* data, std::size_t* n)
{           
    PT_LOG_TRACE("Connection::sslWrite: " << *n);
    
    _iocount = 0;
    std::streambuf* sb = _ios->rdbuf();

    if(_isReading || ! sb)
    {
        *n = 0;
        return errSSLWouldBlock;
    }

    _iocount = sb->sputn(reinterpret_cast<const char*>(data), *n);
    PT_LOG_TRACE("wrote " << _iocount << " bytes to output");

    OSStatus ret = noErr;
    if( static_cast<std::size_t>(_iocount) < *n)
    {
        ret = errSSLClosedAbort;
    }

    PT_LOG_DEBUG("sslWrite: " << ret);
    return ret;
}


OSStatus Connection::sslWriteCallback(SSLConnectionRef connection, const void* data, std::size_t* n)
{
    return ((Connection*)(connection))->sslWrite(data, n);
}


OSStatus Connection::sslReadCallback(SSLConnectionRef connection, void* data, std::size_t* n)
{
    return ((Connection*)(connection))->sslRead(data, n);
}


const char* Connection::toCipherName(SSLCipherSuite cipher) const
{
    switch (cipher) 
    {
        case SSL_RSA_WITH_NULL_MD5: return "SSL_RSA_WITH_NULL_MD5";
        case SSL_RSA_WITH_NULL_SHA: return "SSL_RSA_WITH_NULL_SHA";
        case SSL_RSA_EXPORT_WITH_RC4_40_MD5: return "SSL_RSA_EXPORT_WITH_RC4_40_MD5";
        case SSL_RSA_WITH_RC4_128_MD5: return "SSL_RSA_WITH_RC4_128_MD5";
        case SSL_RSA_WITH_RC4_128_SHA: return "SSL_RSA_WITH_RC4_128_SHA";
        case SSL_RSA_EXPORT_WITH_RC2_CBC_40_MD5: return "SSL_RSA_EXPORT_WITH_RC2_CBC_40_MD5";
        case SSL_RSA_WITH_IDEA_CBC_SHA: return "SSL_RSA_WITH_IDEA_CBC_SHA";
        case SSL_RSA_EXPORT_WITH_DES40_CBC_SHA: return "SSL_RSA_EXPORT_WITH_DES40_CBC_SHA";
        case SSL_RSA_WITH_DES_CBC_SHA: return "SSL_RSA_WITH_DES_CBC_SHA";
        case SSL_RSA_WITH_3DES_EDE_CBC_SHA: return "SSL_RSA_WITH_3DES_EDE_CBC_SHA";
        case SSL_DH_DSS_EXPORT_WITH_DES40_CBC_SHA: return "SSL_DH_DSS_EXPORT_WITH_DES40_CBC_SHA";
        case SSL_DH_DSS_WITH_DES_CBC_SHA: return "SSL_DH_DSS_WITH_DES_CBC_SHA";
        case SSL_DH_DSS_WITH_3DES_EDE_CBC_SHA: return "SSL_DH_DSS_WITH_3DES_EDE_CBC_SHA";
        case SSL_DH_RSA_EXPORT_WITH_DES40_CBC_SHA: return "SSL_DH_RSA_EXPORT_WITH_DES40_CBC_SHA";
        case SSL_DH_RSA_WITH_DES_CBC_SHA: return "SSL_DH_RSA_WITH_DES_CBC_SHA";
        case SSL_DH_RSA_WITH_3DES_EDE_CBC_SHA: return "SSL_DH_RSA_WITH_3DES_EDE_CBC_SHA";
        case SSL_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA: return "SSL_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA";
        case SSL_DHE_DSS_WITH_DES_CBC_SHA: return "SSL_DHE_DSS_WITH_DES_CBC_SHA";
        case SSL_DHE_DSS_WITH_3DES_EDE_CBC_SHA: return "SSL_DHE_DSS_WITH_3DES_EDE_CBC_SHA";
        case SSL_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA: return "SSL_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA";
        case SSL_DHE_RSA_WITH_DES_CBC_SHA: return "SSL_DHE_RSA_WITH_DES_CBC_SHA";
        case SSL_DHE_RSA_WITH_3DES_EDE_CBC_SHA: return "SSL_DHE_RSA_WITH_3DES_EDE_CBC_SHA";
        case SSL_DH_anon_EXPORT_WITH_RC4_40_MD5: return "SSL_DH_anon_EXPORT_WITH_RC4_40_MD5";
        case SSL_DH_anon_WITH_RC4_128_MD5: return "SSL_DH_anon_WITH_RC4_128_MD5";  
        case SSL_DH_anon_EXPORT_WITH_DES40_CBC_SHA: return "SSL_DH_anon_EXPORT_WITH_DES40_CBC_SHA";  
        case SSL_DH_anon_WITH_DES_CBC_SHA: return "SSL_DH_anon_WITH_DES_CBC_SHA";  
        case SSL_DH_anon_WITH_3DES_EDE_CBC_SHA: return "SSL_DH_anon_WITH_3DES_EDE_CBC_SHA";  
        case SSL_FORTEZZA_DMS_WITH_NULL_SHA: return "SSL_FORTEZZA_DMS_WITH_NULL_SHA";  
        case SSL_FORTEZZA_DMS_WITH_FORTEZZA_CBC_SHA: return "SSL_FORTEZZA_DMS_WITH_FORTEZZA_CBC_SHA";  
        case TLS_RSA_WITH_AES_128_CBC_SHA: return "TLS_RSA_WITH_AES_128_CBC_SHA";  
        case TLS_DH_DSS_WITH_AES_128_CBC_SHA: return "TLS_DH_DSS_WITH_AES_128_CBC_SHA";  
        case TLS_DH_RSA_WITH_AES_128_CBC_SHA: return "TLS_DH_RSA_WITH_AES_128_CBC_SHA";  
        case TLS_DHE_DSS_WITH_AES_128_CBC_SHA: return "TLS_DHE_DSS_WITH_AES_128_CBC_SHA";  
        case TLS_DHE_RSA_WITH_AES_128_CBC_SHA: return "TLS_DHE_RSA_WITH_AES_128_CBC_SHA";  
        case TLS_DH_anon_WITH_AES_128_CBC_SHA: return "TLS_DH_anon_WITH_AES_128_CBC_SHA";  
        case TLS_RSA_WITH_AES_256_CBC_SHA: return "TLS_RSA_WITH_AES_256_CBC_SHA";  
        case TLS_DH_DSS_WITH_AES_256_CBC_SHA: return "TLS_DH_DSS_WITH_AES_256_CBC_SHA";  
        case TLS_DH_RSA_WITH_AES_256_CBC_SHA: return "TLS_DH_RSA_WITH_AES_256_CBC_SHA";  
        case TLS_DHE_DSS_WITH_AES_256_CBC_SHA: return "TLS_DHE_DSS_WITH_AES_256_CBC_SHA";  
        case TLS_DHE_RSA_WITH_AES_256_CBC_SHA: return "TLS_DHE_RSA_WITH_AES_256_CBC_SHA";  
        case TLS_DH_anon_WITH_AES_256_CBC_SHA: return "TLS_DH_anon_WITH_AES_256_CBC_SHA";  
        case TLS_ECDH_ECDSA_WITH_NULL_SHA: return "TLS_ECDH_ECDSA_WITH_NULL_SHA";  
        case TLS_ECDH_ECDSA_WITH_RC4_128_SHA: return "TLS_ECDH_ECDSA_WITH_RC4_128_SHA";  
        case TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA: return "TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA";  
        case TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA: return "TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA";  
        case TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA: return "TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA";  
        case TLS_ECDHE_ECDSA_WITH_NULL_SHA: return "TLS_ECDHE_ECDSA_WITH_NULL_SHA";  
        case TLS_ECDHE_ECDSA_WITH_RC4_128_SHA: return "TLS_ECDHE_ECDSA_WITH_RC4_128_SHA";  
        case TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA: return "TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA";  
        case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA: return "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA";  
        case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA: return "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA";  
        case TLS_ECDH_RSA_WITH_NULL_SHA: return "TLS_ECDH_RSA_WITH_NULL_SHA";  
        case TLS_ECDH_RSA_WITH_RC4_128_SHA: return "TLS_ECDH_RSA_WITH_RC4_128_SHA";  
        case TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA: return "TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA";  
        case TLS_ECDH_RSA_WITH_AES_128_CBC_SHA: return "TLS_ECDH_RSA_WITH_AES_128_CBC_SHA";  
        case TLS_ECDH_RSA_WITH_AES_256_CBC_SHA: return "TLS_ECDH_RSA_WITH_AES_256_CBC_SHA";  
        case TLS_ECDHE_RSA_WITH_NULL_SHA: return "TLS_ECDHE_RSA_WITH_NULL_SHA";  
        case TLS_ECDHE_RSA_WITH_RC4_128_SHA: return "TLS_ECDHE_RSA_WITH_RC4_128_SHA";  
        case TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA: return "TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA";  
        case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA: return "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA";  
        case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA: return "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA";  
        case TLS_ECDH_anon_WITH_NULL_SHA: return "TLS_ECDH_anon_WITH_NULL_SHA";  
        case TLS_ECDH_anon_WITH_RC4_128_SHA: return "TLS_ECDH_anon_WITH_RC4_128_SHA";  
        case TLS_ECDH_anon_WITH_3DES_EDE_CBC_SHA: return "TLS_ECDH_anon_WITH_3DES_EDE_CBC_SHA";  
        case TLS_ECDH_anon_WITH_AES_128_CBC_SHA: return "TLS_ECDH_anon_WITH_AES_128_CBC_SHA";  
        case TLS_ECDH_anon_WITH_AES_256_CBC_SHA: return "TLS_ECDH_anon_WITH_AES_256_CBC_SHA";  
        case SSL_RSA_WITH_RC2_CBC_MD5: return "SSL_RSA_WITH_RC2_CBC_MD5";  
        case SSL_RSA_WITH_IDEA_CBC_MD5: return "SSL_RSA_WITH_IDEA_CBC_MD5";  
        case SSL_RSA_WITH_DES_CBC_MD5: return "SSL_RSA_WITH_DES_CBC_MD5";  
        case SSL_RSA_WITH_3DES_EDE_CBC_MD5: return "SSL_RSA_WITH_3DES_EDE_CBC_MD5";
    }

    return "NONE";
}

} // namespace Ssl

} // namespace Pt
