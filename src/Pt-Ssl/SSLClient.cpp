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
#include <Pt/Ssl/SSLClient.h>

namespace Pt {
namespace Ssl {

///// JUST FOR TESTING /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SSL_CALL_INFO SSLStreamBuf::_call_info("SSLClient   ", PT_FUNCTION)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SSLClient::SSLClient(Pt::System::IOStream& ios, SSLContext& ctx, const char* sessionID)
: std::iostream(0),
  _ios         (&ios),
  _sslbuf      (ios, ctx, sessionID/*, 16*/) // If the buffer size is small, we will get seg fault
{ std::iostream::init(&_sslbuf); }

SSLClient::~SSLClient()
{}

void SSLClient::beginHandshake()
{
    _sslbuf.beginClientHandshake();
    std::cerr << SSL_CALL_INFO << "out_avail = " << _ios->buffer().out_avail() << std::endl;

    std::cerr << SSL_CALL_INFO << "Begin write" << std::endl;
    _ios->buffer().beginWrite();
    _ios->buffer().outputReady += Pt::slot(*this, &SSLClient::onWriteHandshake);
    _ios->buffer().inputReady  += Pt::slot(*this, &SSLClient::onReadHandshake);
}

void SSLClient::onWriteHandshake(Pt::System::StreamBuffer& sb)
{
    _ios->buffer().endWrite();
    std::cerr << SSL_CALL_INFO << "out_avail = " << _ios->buffer().out_avail() << std::endl;

    if(_sslbuf.writeHandshake() || _ios->buffer().out_avail() > 0)
    {
        std::cerr << SSL_CALL_INFO << "Begin write" << std::endl;
        _ios->buffer().beginWrite();
        return;
    }

    std::cerr << SSL_CALL_INFO << "Begin read" << std::endl;
    _ios->buffer().beginRead();
}

void SSLClient::onReadHandshake(Pt::System::StreamBuffer& sb)
{
    _ios->buffer().endRead();
    std::cerr << SSL_CALL_INFO << "in_avail = " << _ios->buffer().in_avail() << std::endl;

    if(_sslbuf.readHandshake())
    {
        std::cerr << SSL_CALL_INFO << "Read more handshake bytes" << std::endl;
        _ios->buffer().beginRead();
        return;
    }

    if(_sslbuf.connected())
    {
        std::cerr << SSL_CALL_INFO << "Successfully connected to the server" << std::endl;
        _ios->buffer().outputReady -= Pt::slot(*this, &SSLClient::onWriteHandshake);
        _ios->buffer().inputReady  -= Pt::slot(*this, &SSLClient::onReadHandshake);
        handshakeFinished.send(*this);
        return;
    }

    std::cerr << SSL_CALL_INFO << "Write handshake" << std::endl;
    _sslbuf.writeHandshake();

    std::cerr << SSL_CALL_INFO << "Begin write" << std::endl;
    _ios->buffer().beginWrite();
}

} // namespace Ssl
} // namespace Pt
