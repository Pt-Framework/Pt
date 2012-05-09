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
 * License along with this library; if not, weriwrite to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Pt/Ssl/Server.h>
#include <Pt/System/Logger.h>

log_define("Pt.Ssl.Client")

namespace Pt {

namespace Ssl {

Server::Server(Pt::System::IOStream& ios, Context& ctx, const char* sessionID)
: std::iostream(0)
, _ios(&ios)
, _sslbuf(ios, ctx, sessionID, 1024)
, _errorPending(0)
{ 
    std::iostream::init(&_sslbuf); 
}


Server::~Server()
{}


void Server::beginHandshake(bool verifyClientCert, bool requireCertBasedAuth)
{
    log_debug("_sslbuf.beginClientHandshake(verifyServerCert = "
               << verifyClientCert << ", requireCertBasedAuth = " << requireCertBasedAuth << ")");

    _errorPending = 0;
    _sslbuf.beginServerHandshake(verifyClientCert, requireCertBasedAuth);

    log_debug("_ios->buffer().beginRead()");
    _ios->buffer().beginRead();
    _ios->buffer().outputReady() += Pt::slot(*this, &Server::onWriteHandshake);
    _ios->buffer().inputReady()  += Pt::slot(*this, &Server::onReadHandshake);
}


void Server::endHandshake()
{
    if( _errorPending ) 
    {
        _errorPending = 0;
        throw;
    }
}


void Server::onWriteHandshake(Pt::System::StreamBuffer& sb)
{
    try
    {
        log_debug("_ios->buffer().endWrite()");
        _ios->buffer().endWrite();

        log_debug("_sslbuf.writeHandshake()");
        if(_sslbuf.writeHandshake() || _ios->buffer().out_avail() > 0)
        {
            log_debug("_ios->buffer().beginWrite()");
            _ios->buffer().beginWrite();
            return;
        }

        if( _sslbuf.connected() )
        {
            log_debug("Handshake finished");
            _ios->buffer().outputReady() -= Pt::slot(*this, &Server::onWriteHandshake);
            _ios->buffer().inputReady()  -= Pt::slot(*this, &Server::onReadHandshake);
            handshakeFinished.send(*this);
            return;
        }

        log_debug("_ios->buffer().beginRead()");
        _ios->buffer().beginRead();
    } 
    catch(...)
    {
        log_debug("Handshake failed");
        _errorPending = 1;
        _ios->buffer().outputReady() -= Pt::slot(*this, &Server::onWriteHandshake);
        _ios->buffer().inputReady()  -= Pt::slot(*this, &Server::onReadHandshake);
        handshakeFinished.send(*this);

        if(_errorPending)
        {
            throw;
        }
    }
}


void Server::onReadHandshake(Pt::System::StreamBuffer& sb)
{
    try
    {
        log_debug("_ios->buffer().endRead()");
        _ios->buffer().endRead();

        log_debug("_sslbuf.readHandshake()");
        if(_sslbuf.readHandshake())
        {
            log_debug("_ios->buffer().beginRead()");
            _ios->buffer().beginRead();
            return;
        }

        log_debug("_sslbuf.writeHandshake()");
        if(_sslbuf.writeHandshake()) {
            log_debug("_ios->buffer().beginWrite()");
            _ios->buffer().beginWrite();
            return;
        }
    } 
    catch(...)
    {
        log_debug("Handshake failed");
        _errorPending = 1;
        _ios->buffer().outputReady() -= Pt::slot(*this, &Server::onWriteHandshake);
        _ios->buffer().inputReady()  -= Pt::slot(*this, &Server::onReadHandshake);
        handshakeFinished.send(*this);

        if(_errorPending)
        {
            throw;
        }
    }
}


void Server::beginShutdown()
{
    _ios->buffer().outputReady() += Pt::slot(*this, &Server::onWriteShutdown);
    _ios->buffer().inputReady()  += Pt::slot(*this, &Server::onReadShutdown);

    log_debug("_sslbuf.beginShutdown()");
    _sslbuf.shutdown();

    log_debug("_ios->buffer().beginWrite() " << _ios->buffer().out_avail() << " bytes");
    _ios->buffer().beginWrite();
}


void Server::endShutdown()
{
    if( _errorPending ) 
    {
        _errorPending = 0;
        throw;
    }
}


void Server::onReadShutdown(Pt::System::StreamBuffer& sb)
{
}


void Server::onWriteShutdown(Pt::System::StreamBuffer& sb)
{
    try
    {
        sb.endWrite();
        log_debug("Sent shutdown; remaining = " << sb.out_avail());

        if( _ios->buffer().out_avail() > 0 )
        {
            log_debug("_ios->buffer().beginWrite() " << _ios->buffer().out_avail() << " bytes");
            _ios->buffer().beginWrite();
            return;
        }

        _ios->buffer().outputReady() -= Pt::slot(*this, &Server::onWriteShutdown);
        _ios->buffer().inputReady()  -= Pt::slot(*this, &Server::onReadShutdown);
        shutdownFinished.send(*this);
    } 
    catch(...)
    {
        _errorPending = 1;
        _ios->buffer().outputReady() -= Pt::slot(*this, &Server::onWriteShutdown);
        _ios->buffer().inputReady()  -= Pt::slot(*this, &Server::onReadShutdown);
        shutdownFinished.send(*this);
        
        if(_errorPending)
        {
            throw;
        }
    }
}

} // namespace Ssl

} // namespace Pt
