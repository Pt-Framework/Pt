/*
 * Copyright (C) 2010-2012 by Marc Boris Duerner
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
#include <Pt/Ssl/SslError.h>
#include <Pt/System/Logger.h>
#include <mbedtls/error.h>
#include <mbedtls/x509_crt.h>
#include <algorithm>
#include <streambuf>

PT_LOG_DEFINE("Pt.Ssl.StreamBuffer")

namespace Pt {

namespace Ssl {

namespace {

std::string mbedErrorString(int ret)
{
    char buf[128];
    mbedtls_strerror(ret, buf, sizeof(buf));
    return std::string(buf);
}

// decodes the verify-result bitmask into the actual failure reasons (expired, revoked, CN mismatch, ...)
std::string mbedVerifyInfoString(uint32_t flags)
{
    char buf[256];
    int n = mbedtls_x509_crt_verify_info(buf, sizeof(buf), "", flags);
    if(n <= 0)
        return "unknown verification error";

    std::string info(buf, n);
    while( ! info.empty() && (info.back() == '\n' || info.back() == '\r') )
        info.erase(info.size() - 1);

    return info;
}

} // namespace


Connection::Connection(Context& ctx, std::ios& ios, OpenMode omode)
: _ctx(&ctx)
, _ios(&ios)
, _connected(false)
, _isWriting(false)
, _isReading(false)
, _maxImport(0)
, _pending(NoneWanted)
, _shutdownSent(false)
, _shutdownReceived(false)
{
    mbedtls_ssl_init(&_ssl);

    mbedtls_ssl_config* config = omode == Accept ? ctx.impl()->serverConfig()
                                                 : ctx.impl()->clientConfig();
    if( mbedtls_ssl_setup(&_ssl, config) != 0 )
        throw SslError("failed to initialize SSL session");

    mbedtls_ssl_set_bio(&_ssl, this, &Connection::bio_send, &Connection::bio_recv, 0);
}


Connection::~Connection()
{
    mbedtls_ssl_free(&_ssl);
}


void Connection::setPeerName(const std::string& peerName)
{
    _peerName = peerName;

    if( ! _peerName.empty() )
        mbedtls_ssl_set_hostname(&_ssl, _peerName.c_str());
}


void Connection::verifyPeerName()
{
    if( _peerName.empty() )
        return;

    uint32_t flags = mbedtls_ssl_get_verify_result(&_ssl);
    if(flags != 0)
    {
        std::string info = mbedVerifyInfoString(flags);
        PT_LOG_WARN("peer verification failed: " << info);
        throw HandshakeFailed("Peer verification failed: " + info);
    }
}


const char* Connection::currentCipher() const
{
    const char* name = mbedtls_ssl_get_ciphersuite(&_ssl);
    return name ? name : "NONE";
}


bool Connection::writeHandshake()
{
    PT_LOG_TRACE("Connection::writeHandshake");

    if( _connected || _pending == WantRead )
        return false;

    _pending = NoneWanted;
    _isWriting = true;
    _isReading = false;
    _maxImport = 0;

    int ret = mbedtls_ssl_handshake(&_ssl);
    PT_LOG_DEBUG("mbedtls_ssl_handshake: " << ret);

    _isWriting = false;

    if(ret == 0)
    {
        verifyPeerName();
        _connected = true;
        return false;
    }

    if(ret == MBEDTLS_ERR_SSL_WANT_READ)
    {
        _pending = WantRead;
        return false;
    }

    if(ret == MBEDTLS_ERR_SSL_WANT_WRITE)
        return true;

    PT_LOG_WARN("handshake failed: " << mbedErrorString(ret));
    throw HandshakeFailed("SSL handshake failed");
}


bool Connection::readHandshake()
{
    PT_LOG_TRACE("Connection::readHandshake");

    if( _connected || _pending == WantWrite )
        return false;

    _pending = NoneWanted;
    _isWriting = false;
    _isReading = true;
    _maxImport = 0;

    int ret = mbedtls_ssl_handshake(&_ssl);
    PT_LOG_DEBUG("mbedtls_ssl_handshake: " << ret);

    _isReading = false;

    if(ret == 0)
    {
        verifyPeerName();
        _connected = true;
        return false;
    }

    if(ret == MBEDTLS_ERR_SSL_WANT_WRITE)
    {
        _pending = WantWrite;
        return false;
    }

    if(ret == MBEDTLS_ERR_SSL_WANT_READ)
        return true;

    PT_LOG_WARN("handshake failed: " << mbedErrorString(ret));
    throw HandshakeFailed("SSL handshake failed");
}


bool Connection::shutdown()
{
    PT_LOG_DEBUG("Connection::shutdown");

    if( ! _connected )
        return true;

    if( ! _shutdownSent )
    {
        _isWriting = true;
        _isReading = false;
        _maxImport = 0;

        int ret = mbedtls_ssl_close_notify(&_ssl);

        _isWriting = false;

        if(ret == 0)
        {
            _shutdownSent = true;
        }
        else if(ret == MBEDTLS_ERR_SSL_WANT_WRITE)
        {
            return false;
        }
        else
        {
            throw SslError("shutdown failed");
        }
    }

    // wait for the peer's close_notify
    _isWriting = false;
    _isReading = true;
    _maxImport = 0;

    unsigned char buf[16];
    int ret = mbedtls_ssl_read(&_ssl, buf, sizeof(buf));

    _isReading = false;

    if(ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
    {
        _shutdownReceived = true;
        _connected = false;
        return true;
    }

    if(ret == MBEDTLS_ERR_SSL_WANT_READ)
        return false;

    if(ret > 0) // unexpected application data while waiting for close_notify
        return false;

    throw SslError("shutdown failed");
}


bool Connection::isShutdown() const
{
    return _shutdownSent || _shutdownReceived;
}


bool Connection::isClosed() const
{
    return ! _connected;
}


std::streamsize Connection::write(const char* buf, std::size_t n)
{
    _isWriting = true;
    _isReading = false;
    _maxImport = 0;

    int written = mbedtls_ssl_write(&_ssl, reinterpret_cast<const unsigned char*>(buf), n);
    PT_LOG_DEBUG("encrypted " << written << " bytes");

    _isWriting = false;

    if(written >= 0)
        return written;

    if(written == MBEDTLS_ERR_SSL_WANT_WRITE)
        return 0;

    throw SslError("encoding failed");
}


std::streamsize Connection::read(char* buf, std::size_t n, std::streamsize maxImport)
{
    _isWriting = false;
    _isReading = true;
    _maxImport = maxImport;

    int readSize = mbedtls_ssl_read(&_ssl, reinterpret_cast<unsigned char*>(buf), n);
    PT_LOG_DEBUG("read " << readSize << " bytes from _ssl");

    _isReading = false;
    _maxImport = 0;

    if(readSize >= 0)
        return readSize;

    // happens when the peer has sent the close_notify alert
    if(readSize == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
        return 0;

    if(readSize == MBEDTLS_ERR_SSL_WANT_READ)
        return 0;

    PT_LOG_DEBUG("ssl error: " << mbedErrorString(readSize));
    throw SslError("decoding failed");
}


int Connection::bio_send(void* ctx, const unsigned char* buf, std::size_t len)
{
    return static_cast<Connection*>(ctx)->bioWrite(buf, len);
}


int Connection::bio_recv(void* ctx, unsigned char* buf, std::size_t len)
{
    return static_cast<Connection*>(ctx)->bioRead(buf, len);
}


int Connection::bioWrite(const unsigned char* buf, std::size_t len)
{
    std::streambuf* sb = _ios->rdbuf();
    if( ! sb )
        return MBEDTLS_ERR_SSL_WANT_WRITE;

    if(_isReading)
        return MBEDTLS_ERR_SSL_WANT_WRITE;

    std::streamsize n = sb->sputn(reinterpret_cast<const char*>(buf), len);
    return static_cast<int>(n);
}


int Connection::bioRead(unsigned char* buf, std::size_t len)
{
    std::streambuf* sb = _ios->rdbuf();
    if( ! sb || _isWriting )
        return MBEDTLS_ERR_SSL_WANT_READ;

    std::streamsize avail = sb->in_avail();
    if(avail == 0 && _maxImport == 0)
        return MBEDTLS_ERR_SSL_WANT_READ;

    std::streamsize n = static_cast<std::streamsize>(len);
    if(_maxImport != 0)
        n = std::min(n, _maxImport);
    else
        n = std::min(n, avail);

    std::streamsize r = sb->sgetn(reinterpret_cast<char*>(buf), n);
    return static_cast<int>(r);
}

} // namespace Ssl

} // namespace Pt
