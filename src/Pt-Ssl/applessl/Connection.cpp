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
#include <Pt/Ssl/StreamBuffer.h>
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Logger.h>
#include <Pt/System/IOError.h>
#include <cassert>
#include <cstring>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFDictionary.h>

log_define("Pt.Ssl.StreamBuffer")

namespace Pt {

namespace Ssl {

const char* toCipherName(SSLCipherSuite cipher) ;

Connection::Connection(Context& ctx, std::streambuf& ios, int mode)
: _ctx(&ctx)
, _context(0)
, _ios(&ios)
, _iocount(0)
, _connected(false)
, _wantRead(false)
, _isReading(false)
, _isWriting(false)
, _receivedShutdown(false)
, _sentShutdown(false)
{
    Boolean isServer = (mode == StreamBuffer::Accept);

    SSLNewContext(isServer, &_context);
    
    SSLSetConnection(_context, (SSLConnectionRef) this);

    SSLSetIOFuncs(_context, 
                  &Connection::sslReadCallback, 
                  &Connection::sslWriteCallback);
   
    SSLSetProtocolVersionEnabled(_context, kSSLProtocolAll, false);

    switch(_ctx->protocol()) 
    {
        case SSLv2:
            SSLSetProtocolVersionEnabled(_context, kSSLProtocol2, true);
            break;

        case SSLv3or2:
            SSLSetProtocolVersionEnabled(_context, kSSLProtocol2, true);
            SSLSetProtocolVersionEnabled(_context, kSSLProtocol3, true);
            break;

        default:
        case SSLv3:
            SSLSetProtocolVersionEnabled(_context, kSSLProtocol3, true);
            break;
      
        case TLSv1:
            SSLSetProtocolVersionEnabled(_context, kTLSProtocol1, true);
            break;
    }

    if(isServer)
    {
#ifdef PT_IOS
        SSLSetEnableCertVerify(_context, false);
        SSLSetSessionOption(_context, kSSLSessionOptionBreakOnClientAuth, true);
#else
        if(_ctx->verifyMode() == NoVerify)
        {
            SSLSetClientSideAuthenticate(_context, kNeverAuthenticate);
        }
        else if(_ctx->verifyMode() == TryVerify)
        {
            SSLSetClientSideAuthenticate(_context, kTryAuthenticate);
        }
        else if(_ctx->verifyMode() == AlwaysVerify)
        {
            SSLSetClientSideAuthenticate(_context, kAlwaysAuthenticate);
        }
        
        CFArrayRef caArr = _ctx->impl()->caCertificates();
        SSLSetCertificateAuthorities(_context, caArr, true);
        SSLSetTrustedRoots(_context, caArr, true);
#endif
    }
    else
    {
        SSLSetEnableCertVerify(_context, false);
        SSLSetSessionOption(_context, kSSLSessionOptionBreakOnServerAuth, true);
    }

    // certificates to present to peer
    CFArrayRef certs = _ctx->impl()->certificates();
    if(certs)
    {
        log_debug("using " << CFArrayGetCount(certs) << " certificates");
        SSLSetCertificate(_context, certs);
    }
}


Connection::~Connection()
{
    SSLDisposeContext(_context);
}


bool Connection::writeHandshake()
{
    log_trace("Connection::writeHandshake");
    
    assert( _ios );

    _iocount = 0;
    _isWriting = true;
    OSStatus status = SSLHandshake(_context);
    _isWriting = false;

    log_debug("SSLHandshake returns " << status);

    if(status == noErr)
    {       
        log_debug("SSL handshake completed");
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
    log_trace("Connection::readHandshake");
    
    assert( _ios );

    _wantRead = false;
    _isReading = true;
    OSStatus status = SSLHandshake(_context);
    _isReading = false;

    log_debug("SSLHandshake returns " << status);
    
    if( status == noErr )
    {
        log_debug("SSL handshake completed");
        _connected = true;
        return false;
    }
#ifdef PT_IOS
    if(status == errSSLPeerAuthCompleted)
#else
    if(status == errSSLServerAuthCompleted)
#endif
    {
        log_debug("authenticating peer");

        if( _ctx->verifyMode() != NoVerify )
        {
            log_debug("evaluating trust");
            
            SecTrustRef trust = NULL;
            SSLCopyPeerTrust(_context, &trust);

            CFArrayRef caArr = _ctx->impl()->caCertificates();
            SecTrustSetAnchorCertificates(trust, caArr);
            SecTrustSetAnchorCertificatesOnly(trust, true);

            SecTrustResultType result;
            OSStatus evalErr = SecTrustEvaluate(trust, &result);
            if(evalErr)
                throw HandshakeFailed("SSL handshake failed");
        
            CFIndex count = SecTrustGetCertificateCount(trust);
            log_debug("SecTrustEvaluate: " << result << " certs: " << count);
            
            if(trust)
                CFRelease(trust);

            if(_ctx->verifyMode() == AlwaysVerify && count == 0)
                throw HandshakeFailed("SSL handshake failed");

            if( (result != kSecTrustResultProceed) && 
                (result != kSecTrustResultUnspecified) )
                throw HandshakeFailed("SSL handshake failed");

            log_debug("authentication successful");
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

    if( ! _sentShutdown)
    {
        // write shutdown notify
        log_debug("write shutdown notify");

        _isWriting = true;
        OSStatus error = SSLClose(_context);
        _isWriting = false;

        log_debug("SSLClose: " << error);

        if(error == errSSLWouldBlock)
        {
            // need to read shutdown alert
            log_debug("want to read shutdown alert");
            _sentShutdown = true;
            return false;
        }

        if(error != noErr)
            throw SslError("shutdown failed");

        log_debug("shutdown complete");
        _connected = false;
        _sentShutdown = false;
        _receivedShutdown = false;
        return true;
    }
    
    // read shutdown notify
    log_debug("read shutdown notify");

    _wantRead = false;
    _isReading = true;
    OSStatus error = SSLClose(_context);
    _isReading = false;

    log_debug("SSLClose: " << error);

    if(error == errSSLWouldBlock)
    {
        return false;
    }

    if(error != noErr)
        throw SslError("shutdown failed");

    log_debug("shutdown complete");
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


std::streamsize Connection::write(const char* buf, size_t n)
{
    size_t processed = 0;
    
    OSStatus error = SSLWrite(_context, buf, n, &processed);
    
    if(error != noErr && error != errSSLWouldBlock)
        throw SslError("encoding failed");

    return static_cast<std::streamsize>(processed);
}


std::streamsize Connection::read(char* buf, size_t n, std::streamsize isize)
{
    log_trace("Connection::read");

    if(isize == 0) 
        isize = _ios->in_avail();

    size_t processed = 0;

    // TODO: consume isize bytes from input
    
    _isReading = true;
    OSStatus error = SSLRead(_context, buf, n, &processed);
    _isReading = false;

    log_trace("Connection::read: " << error);
    
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


OSStatus Connection::sslRead(void* data, size_t* n)
{    
    log_trace("Connection::sslRead: wants " << *n << " bytes");

    if(_isWriting)
    {
        *n = 0;
        return errSSLWouldBlock;
    }       

    if(_ios->in_avail() <= 0)
    {
        _wantRead = true;
        *n = 0;
        return errSSLWouldBlock;
    }  

    log_debug("avail input: " << _ios->in_avail());
    std::streamsize gsize = std::min( _ios->in_avail(), static_cast<std::streamsize>(*n) );
    std::streamsize r = _ios->sgetn(reinterpret_cast<char*>(data), gsize);
    log_debug("read " << r << " bytes from input");

    OSStatus ret = noErr;
    if( static_cast<size_t>(r) < (*n) ) 
        ret = errSSLWouldBlock;

    *n = static_cast<size_t>(r);

    if(r <= 0)
        return errSSLClosedGraceful;
    
    log_debug("sslRead: return " << ret);
    return ret;
}


OSStatus Connection::sslWrite(const void* data, size_t* n)
{           
    log_trace("Connection::sslWrite: " << *n);
    if(_isReading)
    {
        *n = 0;
        return errSSLWouldBlock;
    }

    _iocount = _ios->sputn(reinterpret_cast<const char*>(data), *n);
    assert(static_cast<size_t>(_iocount) == *n);
    log_trace("wrote: " << _iocount);
    
    return noErr;
}


OSStatus Connection::sslWriteCallback(SSLConnectionRef connection, const void* data, size_t* n)
{
    return ((Connection*)(connection))->sslWrite(data, n);
}


OSStatus Connection::sslReadCallback(SSLConnectionRef connection, void* data, size_t* n)
{
    return ((Connection*)(connection))->sslRead(data, n);
}


const char* toCipherName(SSLCipherSuite cipher) 
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

    return "unknowm cipher";
}

} // namespace Ssl

} // namespace Pt
