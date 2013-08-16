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

#import <Security/Security.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreFoundation/CFDictionary.h>

log_define("Pt.Ssl.StreamBuffer")

namespace Pt {

namespace Ssl {

Connection::Connection(Context& ctx, std::streambuf& ios, int mode)
: _ctx(&ctx)
, _context(0)
, _ios(&ios)
, _iocount(0)
, _connected(false)
, _wantRead(false)
, _isReading(false)
, _isWritingHandshake(false)
, _isShutdown(false)
{
    Boolean isServer = (mode == StreamBuffer::Accept);

    SSLNewContext(isServer, &_context);
    
    SSLSetConnection(_context, (SSLConnectionRef) this);

    SSLSetIOFuncs(_context, 
                  &Connection::sslReadCallback, 
                  &Connection::sslWriteCallback);

    _server = false;
    
    if(isServer == true)
    {
        _server = true;

#ifdef PT_IOS
        SSLSetEnableCertVerify(_context, false);
        SSLSetSessionOption(_context, kSSLSessionOptionBreakOnClientAuth, true);
#else
        if(_ctx->verification() == Context::VerifyNone)
        {
            SSLSetClientSideAuthenticate(_context, kNeverAuthenticate);
        }
        else if(_ctx->verification() == Context::VerifyPeer)
        {
            SSLSetClientSideAuthenticate(_context, kTryAuthenticate);
        }
        else if(_ctx->verification() == Context::VerifyPeerRequired)
        {
            SSLSetClientSideAuthenticate(_context, kAlwaysAuthenticate);
        }
        
        const Certificate& ca = _ctx->impl()->caCertificates().at(0);

        SecCertificateRef certs [] = { ca.impl()->certRef() };
        CFArrayRef caArr = CFArrayCreate(NULL, (const void**)certs, 1, &kCFTypeArrayCallBacks);
        SSLSetCertificateAuthorities(_context, caArr, true);
        SSLSetTrustedRoots(_context, caArr, true);
#endif
    }
    else
    {
        SSLSetEnableCertVerify(_context, false);
        SSLSetSessionOption(_context, kSSLSessionOptionBreakOnServerAuth, true);
    }

    if( _ctx->impl()->certificate().isValid() )
    {
        SecIdentityRef ident = _ctx->impl()->certificate().impl()->identity();
        if(ident)
        {
            std::clog << "USING CERTIFICATE " << _server << std::endl;
            CFArrayRef certRefs = CFArrayCreate(NULL, (const void**)(&ident), 1, NULL);
            SSLSetCertificate(_context, certRefs);
        }
    }
}


Connection::~Connection()
{
    SSLDisposeContext(_context);
}


bool Connection::writeHandshake()
{
    log_trace("Connection::writeHandshake " << _server);
    
    if( ! _ios )
        throw System::IOError("SSL Buffer not initialized");

    _iocount = 0;
    _isWritingHandshake = true;
    OSStatus status = SSLHandshake(_context);
    _isWritingHandshake = false;

    log_debug("SSLHandshake returns " << status);

    if(status != noErr && status != errSSLWouldBlock && status != errSSLUnknownRootCert)
    {
        throw HandshakeFailed("SSL handshake failed");
    }

    if(status == noErr)
    {       
        _connected = true;
    }

    return _iocount > 0;
}


bool Connection::readHandshake()
{
    log_trace("Connection::readHandshake " << _server);
    
    if( ! _ios )
        throw System::IOError("SSL Buffer not initialized");

    again:

    _wantRead = false;
    _isReading = true;
    OSStatus status = SSLHandshake(_context);
    _isReading = false;

    log_debug("SSLHandshake returns " << status);
    
    
#ifdef PT_IOS
    if(status == errSSLPeerAuthCompleted)
#else
    if(status == errSSLServerAuthCompleted)
#endif
    {
        log_debug("errSSLPeerAuthCompleted " << _server);

        bool verifyNone = _ctx->verification() == Context::VerifyNone;
        if(verifyNone)
            goto again;

        SecTrustRef trust = NULL;
        SSLCopyPeerTrust(_context, &trust);
        //SecTrustSetPolicies(trust, SecPolicyCreateBasicX509());
        
        const Certificate& ca = _ctx->impl()->caCertificates().at(0);
        log_debug("CA: " << ca.subject() );

        SecCertificateRef certs [] = { ca.impl()->certRef() };

        CFArrayRef caArr = CFArrayCreate(NULL, (const void**)certs, 1, &kCFTypeArrayCallBacks);
        //CFArrayRef caArr = NULL;
        SecTrustSetAnchorCertificates(trust, caArr);
        SecTrustSetAnchorCertificatesOnly(trust, true);

        log_debug("SecTrustEvaluate evaluating " << _server);
        SecTrustResultType result;
        OSStatus evalErr = SecTrustEvaluate(trust, &result);
        
        if(evalErr)
            throw HandshakeFailed("SSL handshake failed");
            
        log_debug("SecTrustEvaluate: " << result);
        
        if(trust)
            CFRelease(trust);

        if( (result != kSecTrustResultProceed) &&
            (result != kSecTrustResultUnspecified) )
            throw HandshakeFailed("SSL handshake failed");

        log_debug("SecTrustEvaluate TRUSTED");
        goto again;
    }
    
    if(status == errSSLUnknownRootCert)
        throw HandshakeFailed("untrusted CA certificate");

    if(status != noErr && status != errSSLWouldBlock )
    {
        throw HandshakeFailed("SSL handshake failed");
    }

    if( status == noErr )
    {
        _connected = true;
    }

    return _wantRead;
}


bool Connection::shutdown()
{
    if( ! _connected )
        return true;

    if(_isShutdown)
    {
        OSStatus error = SSLClose(_context);
        log_trace("SSLClose: " << error);

        if(error == noErr)
        {
            _isShutdown = false;
            _connected = false;
            return true;
        }
        
        if(error == errSSLWouldBlock)
            return false;
            
        throw SslError("shutdown failed");
        
    }
    
    _isReading = true;
    OSStatus error = SSLClose(_context);
    _isReading = false;

    if(error != noErr)
        throw SslError("shutdown failed");

    _isShutdown = true;
    return false;
}


bool Connection::isShutdown() const
{
    return _isShutdown;
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
        _isShutdown = true;
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

    if(_isWritingHandshake)
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

} // namespace Ssl

} // namespace Pt
