/*
 * Copyright (C) 2010-2011 by Marc Boris Duerner
 * Copyright (C) 2010-2011 by Aloysius Indrayanto
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

#include <Pt/Ssl/SSLClient.h>

namespace Pt {

namespace Ssl {

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////
log_define(PT_SSL_LOGGER_CATEGORY);
#define PT_SSL_LOG(CODE) PT_SSL_LOG_INFO("SSLClient   ", CODE)
////////////////////////////////////////////////////////////////////////////////////////////////////

SSLClient::SSLClient(Pt::System::IOStream& ios, SSLContext& ctx, const char* sessionID)
: std::iostream(0),
  _ios         (&ios),
  _sslbuf      (ios, ctx, sessionID, 1 * 1024)
{
    std::iostream::init(&_sslbuf);
}

SSLClient::~SSLClient()
{}

void SSLClient::beginHandshake(bool verifyServerCert)
{
    PT_SSL_LOG("_sslbuf.beginClientHandshake(verifyServerCert = " << verifyServerCert << ")");
    _sslbuf.beginClientHandshake(verifyServerCert);

    PT_SSL_LOG("_ios->buffer().beginWrite()");
    _ios->buffer().beginWrite();
    _ios->buffer().outputReady += Pt::slot(*this, &SSLClient::onWriteHandshake);
    _ios->buffer().inputReady  += Pt::slot(*this, &SSLClient::onReadHandshake);
}

void SSLClient::endHandshake()
{
    if(_sslbuf.handshakeError())
    {
        throw SSLHandshakeFailedError(
            "The client has failed to complete the handshaking process!",
            PT_SOURCEINFO );
    }
}

void SSLClient::onWriteHandshake(Pt::System::StreamBuffer& sb)
{
    PT_SSL_LOG("_ios->buffer().endWrite()");
    _ios->buffer().endWrite();

    PT_SSL_LOG("_sslbuf.writeHandshake()");
    if(_sslbuf.writeHandshake() || _ios->buffer().out_avail() > 0)
    {
        PT_SSL_LOG("_ios->buffer().beginWrite()");
        _ios->buffer().beginWrite();
        return;
    }

    if(_sslbuf.handshakeError())
    {
        PT_SSL_LOG("Handshake failed");
        _ios->buffer().outputReady -= Pt::slot(*this, &SSLClient::onWriteHandshake);
        _ios->buffer().inputReady  -= Pt::slot(*this, &SSLClient::onReadHandshake);
        handshakeFinished.send(*this);
        return;
    }

    PT_SSL_LOG("_ios->buffer().beginRead()");
    _ios->buffer().beginRead();
}

void SSLClient::onReadHandshake(Pt::System::StreamBuffer& sb)
{
    PT_SSL_LOG("_ios->buffer().endRead()");
    _ios->buffer().endRead();

    PT_SSL_LOG("_sslbuf.readHandshake()");
    if(_sslbuf.readHandshake())
    {
        PT_SSL_LOG("_ios->buffer().beginRead()");
        _ios->buffer().beginRead();
        return;
    }

    if(_sslbuf.handshakeError())
    {
        PT_SSL_LOG("Handshake failed");
        _ios->buffer().outputReady -= Pt::slot(*this, &SSLClient::onWriteHandshake);
        _ios->buffer().inputReady  -= Pt::slot(*this, &SSLClient::onReadHandshake);
        handshakeFinished.send(*this);
        return;
    }

    if(_sslbuf.connected())
    {
        PT_SSL_LOG("Handshake finished");
        _ios->buffer().outputReady -= Pt::slot(*this, &SSLClient::onWriteHandshake);
        _ios->buffer().inputReady  -= Pt::slot(*this, &SSLClient::onReadHandshake);
        handshakeFinished.send(*this);
        return;
    }

    PT_SSL_LOG("_sslbuf.writeHandshake()");
    if(_sslbuf.writeHandshake()) {
        PT_SSL_LOG("_ios->buffer().beginWrite()");
        _ios->buffer().beginWrite();
        return;
    }

    if(_sslbuf.handshakeError())
    {
        PT_SSL_LOG("Handshake failed");
        _ios->buffer().outputReady -= Pt::slot(*this, &SSLClient::onWriteHandshake);
        _ios->buffer().inputReady  -= Pt::slot(*this, &SSLClient::onReadHandshake);
        handshakeFinished.send(*this);
        return;
    }
}

void SSLClient::beginShutdown()
{
    _ios->buffer().outputReady += Pt::slot(*this, &SSLClient::onWriteShutdown);
    _ios->buffer().inputReady  += Pt::slot(*this, &SSLClient::onReadShutdown);

    PT_SSL_LOG("_sslbuf.beginShutdown()");
    _sslbuf.shutdown();

    PT_SSL_LOG("_ios->buffer().beginWrite() " << _ios->buffer().out_avail() << " bytes");
    _ios->buffer().beginWrite();
}

void SSLClient::endShutdown()
{
}

void SSLClient::onReadShutdown(Pt::System::StreamBuffer& sb)
{
}

void SSLClient::onWriteShutdown(Pt::System::StreamBuffer& sb)
{
    sb.endWrite();
    PT_SSL_LOG("Sent shutdown; remaining = " << sb.out_avail());

    shutdownFinished.send(*this);
}

} // namespace Ssl

} // namespace Pt
