/*
 * Copyright (C) 2010-2012 by Marc Boris Duerner
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

#include <Pt/Ssl/IOBuffer.h>
#include <Pt/System/Logger.h>

log_define("Pt.Ssl.IOStream")

namespace Pt {

namespace Ssl {

IOBuffer::IOBuffer(Context& ctx, Pt::System::IOStream& ios, const char* sessionID)
: StreamBuffer(ctx, ios, sessionID, 1024)
, _ios(&ios)
, _errorPending(0)
, _reading(false)
, _input(false)
{
}


IOBuffer::~IOBuffer()
{}


void IOBuffer::beginConnect(bool verifyServerCert)
{
    log_debug("_sslbuf.beginClientHandshake(verifyServerCert = " << verifyServerCert << ")");
    
    _errorPending = 0;
    StreamBuffer::setConnecting(verifyServerCert);
    StreamBuffer::writeHandshake();

    log_debug("_ios->buffer().beginWrite()");
    _ios->buffer().beginWrite();
    _ios->buffer().outputReady() += Pt::slot(*this, &IOBuffer::onWriteHandshake);
    _ios->buffer().inputReady()  += Pt::slot(*this, &IOBuffer::onReadHandshake);
}


void IOBuffer::beginAccept(bool verifyClientCert, bool requireCertBasedAuth)
{
    log_debug("_sslbuf.beginClientHandshake(verifyServerCert = "
               << verifyClientCert << ", requireCertBasedAuth = " << requireCertBasedAuth << ")");

    _errorPending = 0;
    StreamBuffer::setAccepting(verifyClientCert, requireCertBasedAuth);

    log_debug("_ios->buffer().beginRead()");
    _ios->buffer().beginRead();
    _ios->buffer().outputReady() += Pt::slot(*this, &IOBuffer::onWriteServerHandshake);
    _ios->buffer().inputReady()  += Pt::slot(*this, &IOBuffer::onReadServerHandshake);
}


void IOBuffer::endHandshake()
{
    if( _errorPending ) 
    {
        _errorPending = 0;
        throw;
    }
}


void IOBuffer::onWriteHandshake(Pt::System::StreamBuffer& sb)
{
    try
    {
        log_debug("_ios->buffer().endWrite()");
        _ios->buffer().endWrite();

        log_debug("_sslbuf.writeHandshake()");
        if(StreamBuffer::writeHandshake() || _ios->buffer().out_avail() > 0)
        {
            log_debug("_ios->buffer().beginWrite()");
            _ios->buffer().beginWrite();
            return;
        }

        log_debug("_ios->buffer().beginRead()");
        _ios->buffer().beginRead();
    }
    catch(...)
    {
        log_debug("Handshake failed");
        _errorPending = 1;
        _ios->buffer().outputReady() -= Pt::slot(*this, &IOBuffer::onWriteHandshake);
        _ios->buffer().inputReady()  -= Pt::slot(*this, &IOBuffer::onReadHandshake);
        _handshakeFinished.send(*this);
        
        if(_errorPending)
        {
            throw;
        }
    }
}


void IOBuffer::onWriteServerHandshake(Pt::System::StreamBuffer& sb)
{
    try
    {
        log_debug("_ios->buffer().endWrite()");
        _ios->buffer().endWrite();

        log_debug("_sslbuf.writeHandshake()");
        if(StreamBuffer::writeHandshake() || _ios->buffer().out_avail() > 0)
        {
            log_debug("_ios->buffer().beginWrite()");
            _ios->buffer().beginWrite();
            return;
        }

        if( StreamBuffer::connected() )
        {
            log_debug("SERVER Handshake finished");
            _ios->buffer().outputReady() -= Pt::slot(*this, &IOBuffer::onWriteServerHandshake);
            _ios->buffer().inputReady()  -= Pt::slot(*this, &IOBuffer::onReadServerHandshake);
            _ios->buffer().outputReady() += Pt::slot(*this, &IOBuffer::onOutput);
            _ios->buffer().inputReady()  += Pt::slot(*this, &IOBuffer::onInput);
            _handshakeFinished.send(*this);
            return;
        }

        log_debug("_ios->buffer().beginRead()");
        _ios->buffer().beginRead();
    } 
    catch(...)
    {
        log_debug("Handshake failed");
        _errorPending = 1;
        _ios->buffer().outputReady() -= Pt::slot(*this, &IOBuffer::onWriteServerHandshake);
        _ios->buffer().inputReady()  -= Pt::slot(*this, &IOBuffer::onReadServerHandshake);
        _handshakeFinished.send(*this);

        if(_errorPending)
        {
            throw;
        }
    }
}


void IOBuffer::onReadHandshake(Pt::System::StreamBuffer& sb)
{
    try
    {
        log_debug("_ios->buffer().endRead()");
        _ios->buffer().endRead();

        log_debug("_sslbuf.readHandshake()");
        if( StreamBuffer::readHandshake() )
        {
            log_debug("_ios->buffer().beginRead()");
            _ios->buffer().beginRead();
            return;
        }

        if(StreamBuffer::connected())
        {
            log_debug("Handshake finished");
            _ios->buffer().outputReady() -= Pt::slot(*this, &IOBuffer::onWriteHandshake);
            _ios->buffer().inputReady()  -= Pt::slot(*this, &IOBuffer::onReadHandshake);
            _ios->buffer().outputReady() += Pt::slot(*this, &IOBuffer::onOutput);
            _ios->buffer().inputReady()  += Pt::slot(*this, &IOBuffer::onInput);
            _handshakeFinished.send(*this);
            return;
        }

        log_debug("_sslbuf.writeHandshake()");
        if(StreamBuffer::writeHandshake()) {
            log_debug("_ios->buffer().beginWrite()");
            _ios->buffer().beginWrite();
            return;
        }
    }
    catch(...)
    {
        log_debug("Handshake failed");
        _errorPending = 1;
        _ios->buffer().outputReady() -= Pt::slot(*this, &IOBuffer::onWriteHandshake);
        _ios->buffer().inputReady()  -= Pt::slot(*this, &IOBuffer::onReadHandshake);
        _handshakeFinished.send(*this);

        if(_errorPending)
        {
            throw;
        }
    }
}


void IOBuffer::onReadServerHandshake(Pt::System::StreamBuffer& sb)
{
    try
    {
        log_debug("_ios->buffer().endRead()");
        _ios->buffer().endRead();

        log_debug("_sslbuf.readHandshake()");
        if(StreamBuffer::readHandshake())
        {
            log_debug("_ios->buffer().beginRead()");
            _ios->buffer().beginRead();
            return;
        }

        log_debug("_sslbuf.writeHandshake()");
        if(StreamBuffer::writeHandshake()) {
            log_debug("_ios->buffer().beginWrite()");
            _ios->buffer().beginWrite();
            return;
        }
    } 
    catch(...)
    {
        log_debug("Handshake failed");
        _errorPending = 1;
        _ios->buffer().outputReady() -= Pt::slot(*this, &IOBuffer::onWriteServerHandshake);
        _ios->buffer().inputReady()  -= Pt::slot(*this, &IOBuffer::onReadServerHandshake);
        _handshakeFinished.send(*this);

        if(_errorPending)
        {
            throw;
        }
    }
}


void IOBuffer::beginShutdown()
{
    _ios->buffer().outputReady() -= Pt::slot(*this, &IOBuffer::onOutput);
    _ios->buffer().inputReady()  -= Pt::slot(*this, &IOBuffer::onInput);
    _ios->buffer().outputReady() += Pt::slot(*this, &IOBuffer::onWriteShutdown);
    _ios->buffer().inputReady()  += Pt::slot(*this, &IOBuffer::onReadShutdown);

    log_debug("_sslbuf.beginShutdown()");
    StreamBuffer::writeShutdown();

    log_debug("_ios->buffer().beginWrite() " << _ios->buffer().out_avail() << " bytes");
    _ios->buffer().beginWrite();
}


void IOBuffer::endShutdown()
{
    if( _errorPending ) 
    {
        _errorPending = 0;
        throw;
    }
}


void IOBuffer::onReadShutdown(Pt::System::StreamBuffer& sb)
{
}


void IOBuffer::onWriteShutdown(Pt::System::StreamBuffer& sb)
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

        _ios->buffer().outputReady() -= Pt::slot(*this, &IOBuffer::onWriteShutdown);
        _ios->buffer().inputReady()  -= Pt::slot(*this, &IOBuffer::onReadShutdown);
        _shutdownFinished.send(*this);
    }
    catch(...)
    {
        _errorPending = 1;
        _ios->buffer().outputReady() -= Pt::slot(*this, &IOBuffer::onWriteShutdown);
        _ios->buffer().inputReady()  -= Pt::slot(*this, &IOBuffer::onReadShutdown);
        _shutdownFinished.send(*this);
        
        if(_errorPending)
        {
            throw;
        }
    }
}


void IOBuffer::beginRead()
{
    log_debug("begin reading");
    _reading = true;

    if( ! _input || StreamBuffer::import() == 0 )
    {
        log_debug("begin ios reading");
        _ios->buffer().beginRead();
    }
}


std::streamsize IOBuffer::endRead()
{
    log_debug("end reading");
    _reading = false;
    
    std::streamsize res = StreamBuffer::import();
    log_debug("client received decoded = " << this->in_avail());

    if(res < 0)
    {                   
        log_debug("client *** The stream has been shutdown by the other peer ***");
    }

    log_debug("imported: " << res);
    return res;
}


void IOBuffer::onInput(Pt::System::StreamBuffer& sb)
{
    try
    {
        _ios->buffer().endRead();
        log_debug("client received raw = " << _ios->buffer().in_avail());

        _input = true;
        do
        {
            _inputReady.send(*this);
            log_debug("keep reading: " << _reading << " avail: " << this->in_avail());
        }
        while( _reading && this->in_avail() > 0);

        _input = false;
    }
    catch(const std::exception& ex)
    {
        log_error("EXCEPTION onInput: " << ex.what());
        _input = false;
        _errorPending = 1;
        _inputReady.send(*this);
        if(_errorPending)
        {
            throw;
        }
    }

}


void IOBuffer::beginWrite()
{
    log_debug("begin writing");
    _ios->buffer().beginWrite();
}


void IOBuffer::endWrite()
{
    log_debug("end writing");
    if( _errorPending ) 
    {
        _errorPending = 0;
        throw;
    }
}


void IOBuffer::onOutput(Pt::System::StreamBuffer& sb)
{
    try
    {
        sb.endWrite();
        log_debug("client sent raw; remaining = " << sb.out_avail());
        
        if(sb.out_avail() > 0)
        {
            sb.beginWrite();
            return;
        }

        _outputReady.send(*this);
    }
    catch(...)
    {
        _errorPending = 1;
        _outputReady.send(*this);
        if(_errorPending)
        {
            throw;
        }
    }
}

} // namespace Ssl

} // namespace Pt
