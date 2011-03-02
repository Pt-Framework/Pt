/*
 * Copyright (C) 2010-2010 by Marc Boris Duerner
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

#include <Pt/SourceInfo.h>
#include <Pt/Ssl/SSLServer.h>

namespace Pt {
namespace Ssl {

///// Logger for Pt-SSL ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define PT_SSL_LOG(CODE) SSLContext::pt_ssl_logger().info() << SSLContext::pt_ssl_gen_call_info("SSLServer   ", PT_FUNCTION) << CODE << Pt::System::endlog
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SSLServer::SSLServer(Pt::System::IOStream& ios, SSLContext& ctx, const char* sessionID)
: std::iostream(0),
  _ios         (&ios),
  _sslbuf      (ios, ctx, sessionID, 1 * 1024)
{ std::iostream::init(&_sslbuf); }

SSLServer::~SSLServer()
{}

void SSLServer::beginHandshake(bool verifyClientCert, bool requireCertBasedAuth)
{
    _sslbuf.beginServerHandshake(verifyClientCert, requireCertBasedAuth);
    PT_SSL_LOG("out_avail = " << _ios->buffer().out_avail());

    PT_SSL_LOG("Begin read");
    _ios->buffer().beginRead();
    _ios->buffer().outputReady += Pt::slot(*this, &SSLServer::onWriteHandshake);
    _ios->buffer().inputReady  += Pt::slot(*this, &SSLServer::onReadHandshake);
}

void SSLServer::onWriteHandshake(Pt::System::StreamBuffer& sb)
{
    _ios->buffer().endWrite();
    PT_SSL_LOG("out_avail = " << _ios->buffer().out_avail());
    PT_SSL_LOG("Underlying _ios state = good : " << _ios->good() << ", fail : " << _ios->fail() << ", eof : " << _ios->eof());

    // NOTE - ALOYSIUS : Is this a good way to report handshake has failed ???
    try {
        if(_sslbuf.writeHandshake() || _ios->buffer().out_avail() > 0)
        {
            PT_SSL_LOG("Begin write");
            _ios->buffer().beginWrite();
            return;
        }
    }
    catch(...) {
        _ios->buffer().outputReady -= Pt::slot(*this, &SSLServer::onWriteHandshake);
        _ios->buffer().inputReady  -= Pt::slot(*this, &SSLServer::onReadHandshake);
        handshakeFailed.send(*this);
        return;
    }

    if(_sslbuf.connected())
    {
        PT_SSL_LOG("Successfully connected to the client");
        _ios->buffer().outputReady -= Pt::slot(*this, &SSLServer::onWriteHandshake);
        _ios->buffer().inputReady  -= Pt::slot(*this, &SSLServer::onReadHandshake);
        handshakeFinished.send(*this);
        return;
    }

    PT_SSL_LOG("Begin read");
    _ios->buffer().beginRead();
}


void SSLServer::onReadHandshake(Pt::System::StreamBuffer& sb)
{
    _ios->buffer().endRead();
    PT_SSL_LOG("in_avail = " << _ios->buffer().in_avail());
    PT_SSL_LOG("Underlying _ios state = good : " << _ios->good() << ", fail : " << _ios->fail() << ", eof : " << _ios->eof());

    // NOTE - ALOYSIUS : Is this a good way to report handshake has failed ???
    try {
        if(_sslbuf.readHandshake())
        {
            PT_SSL_LOG("Read more handshake bytes");
            _ios->buffer().beginRead();
            return;
        }
        
        PT_SSL_LOG("Write handshake");
        _sslbuf.writeHandshake();
    }
    catch(...) {
        _ios->buffer().outputReady -= Pt::slot(*this, &SSLServer::onWriteHandshake);
        _ios->buffer().inputReady  -= Pt::slot(*this, &SSLServer::onReadHandshake);
        handshakeFailed.send(*this);
        return;
    }


    PT_SSL_LOG("Begin write");
    _ios->buffer().beginWrite();
}

} // namespace Ssl
} // namespace Pt
