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

IOBuffer::IOBuffer()
: StreamBuffer()
, _sb(0)
, _errorPending(0)
, _reading(false)
, _input(false)
{
}


IOBuffer::~IOBuffer()
{}


void IOBuffer::connect(Context& ctx, Pt::System::IOBuffer& sb)
{
    log_trace("IOBuffer::connect");
    
    //this->setConnecting();
    _sb = &sb;
    this->open(ctx, sb, StreamBuffer::Connect);

    for( ; ; )
    {
        log_debug("writing handshake");
        while( this->writeHandshake() )
            ;

        log_debug("syncing buffer");
        _sb->pubsync();

        log_debug("reading handshake");
        while( this->readHandshake() )
            ;

        if( this->connected() )
            break;

        log_debug("continuing handshake");
    }
}


void IOBuffer::beginConnect(Context& ctx, Pt::System::IOBuffer& sb)
{
    log_trace("IOBuffer::beginConnect");

    _errorPending = 0;
    
    //this->setConnecting();
    _sb = &sb;
    this->open(ctx, sb, StreamBuffer::Connect);
    
    StreamBuffer::writeHandshake();

    log_debug("_sb->beginWrite()");
    _sb->beginWrite();
    _sb->outputReady() += Pt::slot(*this, &IOBuffer::onWriteHandshake);
    _sb->inputReady()  += Pt::slot(*this, &IOBuffer::onReadHandshake);
}


void IOBuffer::beginAccept(Context& ctx, Pt::System::IOBuffer& sb)
{
    log_trace("IOBuffer::beginAccept");

    _errorPending = 0;
    
    //StreamBuffer::setAccepting();
    _sb = &sb;
    this->open(ctx, sb, StreamBuffer::Accept);

    log_debug("_sb->beginRead()");
    _sb->beginRead();
    _sb->outputReady() += Pt::slot(*this, &IOBuffer::onWriteServerHandshake);
    _sb->inputReady()  += Pt::slot(*this, &IOBuffer::onReadServerHandshake);
}


void IOBuffer::endHandshake()
{
    if( _errorPending ) 
    {
        _errorPending = 0;
        throw;
    }
}


void IOBuffer::onWriteHandshake(Pt::System::IOBuffer& sb)
{
    try
    {
        log_debug("_sb->endWrite()");
        _sb->endWrite();

        log_debug("_sslbuf.writeHandshake()");
        if(StreamBuffer::writeHandshake() || _sb->out_avail() > 0)
        {
            log_debug("_sb->beginWrite()");
            _sb->beginWrite();
            return;
        }

        log_debug("_sb->beginRead()");
        _sb->beginRead();
    }
    catch(...)
    {
        log_debug("Handshake failed");
        _errorPending = 1;
        _sb->outputReady() -= Pt::slot(*this, &IOBuffer::onWriteHandshake);
        _sb->inputReady()  -= Pt::slot(*this, &IOBuffer::onReadHandshake);
        _handshakeFinished.send(*this);
        
        if(_errorPending)
        {
            throw;
        }
    }
}


void IOBuffer::onReadHandshake(Pt::System::IOBuffer& sb)
{
    try
    {
        log_debug("_sb->endRead()");
        _sb->endRead();

        log_debug("_sslbuf.readHandshake()");
        if( StreamBuffer::readHandshake() )
        {
            log_debug("_sb->beginRead()");
            _sb->beginRead();
            return;
        }

        if(StreamBuffer::connected())
        {
            log_debug("Handshake finished");
            _sb->outputReady() -= Pt::slot(*this, &IOBuffer::onWriteHandshake);
            _sb->inputReady()  -= Pt::slot(*this, &IOBuffer::onReadHandshake);
            _sb->outputReady() += Pt::slot(*this, &IOBuffer::onOutput);
            _sb->inputReady()  += Pt::slot(*this, &IOBuffer::onInput);
            _handshakeFinished.send(*this);
            return;
        }

        log_debug("_sslbuf.writeHandshake()");
        if(StreamBuffer::writeHandshake()) {
            log_debug("_sb->beginWrite()");
            _sb->beginWrite();
            return;
        }
    }
    catch(...)
    {
        log_debug("Handshake failed");
        _errorPending = 1;
        _sb->outputReady() -= Pt::slot(*this, &IOBuffer::onWriteHandshake);
        _sb->inputReady()  -= Pt::slot(*this, &IOBuffer::onReadHandshake);
        _handshakeFinished.send(*this);

        if(_errorPending)
        {
            throw;
        }
    }
}


void IOBuffer::fakeAfterConnect()
{
    _sb->outputReady() += Pt::slot(*this, &IOBuffer::onOutput);
    _sb->inputReady()  += Pt::slot(*this, &IOBuffer::onInput);
}


void IOBuffer::fakeAfterAccept()
{
    _sb->outputReady() += Pt::slot(*this, &IOBuffer::onOutput);
    _sb->inputReady()  += Pt::slot(*this, &IOBuffer::onInput);
}


void IOBuffer::onWriteServerHandshake(Pt::System::IOBuffer& sb)
{
    try
    {
        log_debug("_sb->endWrite()");
        _sb->endWrite();

        log_debug("_sslbuf.writeHandshake()");
        if(StreamBuffer::writeHandshake() || _sb->out_avail() > 0)
        {
            log_debug("_sb->beginWrite()");
            _sb->beginWrite();
            return;
        }

        if( StreamBuffer::connected() )
        {
            log_debug("SERVER Handshake finished");
            _sb->outputReady() -= Pt::slot(*this, &IOBuffer::onWriteServerHandshake);
            _sb->inputReady()  -= Pt::slot(*this, &IOBuffer::onReadServerHandshake);
            _sb->outputReady() += Pt::slot(*this, &IOBuffer::onOutput);
            _sb->inputReady()  += Pt::slot(*this, &IOBuffer::onInput);
            _handshakeFinished.send(*this);
            return;
        }

        log_debug("_sb->beginRead()");
        _sb->beginRead();
    } 
    catch(...)
    {
        log_debug("Handshake failed");
        _errorPending = 1;
        _sb->outputReady() -= Pt::slot(*this, &IOBuffer::onWriteServerHandshake);
        _sb->inputReady()  -= Pt::slot(*this, &IOBuffer::onReadServerHandshake);
        _handshakeFinished.send(*this);

        if(_errorPending)
        {
            throw;
        }
    }
}


void IOBuffer::onReadServerHandshake(Pt::System::IOBuffer& sb)
{
    try
    {
        log_debug("_sb->endRead()");
        _sb->endRead();

        log_debug("_sslbuf.readHandshake()");
        if(StreamBuffer::readHandshake())
        {
            log_debug("_sb->beginRead()");
            _sb->beginRead();
            return;
        }

        log_debug("_sslbuf.writeHandshake()");
        if(StreamBuffer::writeHandshake()) {
            log_debug("_sb->beginWrite()");
            _sb->beginWrite();
            return;
        }
    } 
    catch(...)
    {
        log_debug("Handshake failed");
        _errorPending = 1;
        _sb->outputReady() -= Pt::slot(*this, &IOBuffer::onWriteServerHandshake);
        _sb->inputReady()  -= Pt::slot(*this, &IOBuffer::onReadServerHandshake);
        _handshakeFinished.send(*this);

        if(_errorPending)
        {
            throw;
        }
    }
}


void IOBuffer::beginShutdown()
{
    if( ! _sb)
        return;

    _sb->outputReady() -= Pt::slot(*this, &IOBuffer::onOutput);
    _sb->inputReady()  -= Pt::slot(*this, &IOBuffer::onInput);
    _sb->outputReady() += Pt::slot(*this, &IOBuffer::onWriteShutdown);
    _sb->inputReady()  += Pt::slot(*this, &IOBuffer::onReadShutdown);

    log_debug("_sslbuf.beginShutdown()");
    StreamBuffer::writeShutdown();

    log_debug("_sb->beginWrite() " << _sb->out_avail() << " bytes");
    _sb->beginWrite();
}


void IOBuffer::endShutdown()
{
    if( _errorPending ) 
    {
        _errorPending = 0;
        throw;
    }
}


void IOBuffer::onReadShutdown(Pt::System::IOBuffer& sb)
{
}


void IOBuffer::onWriteShutdown(Pt::System::IOBuffer& sb)
{
    try
    {
        sb.endWrite();
        log_debug("Sent shutdown; remaining = " << sb.out_avail());

        if( _sb->out_avail() > 0 )
        {
            log_debug("_sb->beginWrite() " << _sb->out_avail() << " bytes");
            _sb->beginWrite();
            return;
        }

        _sb->outputReady() -= Pt::slot(*this, &IOBuffer::onWriteShutdown);
        _sb->inputReady()  -= Pt::slot(*this, &IOBuffer::onReadShutdown);
        _shutdownFinished.send(*this);
    }
    catch(...)
    {
        _errorPending = 1;
        _sb->outputReady() -= Pt::slot(*this, &IOBuffer::onWriteShutdown);
        _sb->inputReady()  -= Pt::slot(*this, &IOBuffer::onReadShutdown);
        _shutdownFinished.send(*this);
        
        if(_errorPending)
        {
            throw;
        }
    }
}


void IOBuffer::beginRead()
{
    if( ! _sb)
        return;

    log_debug("begin reading");
    _reading = true;

    StreamBuffer::import();

    if( ! _input || this->in_avail() == 0 )
    {
        log_debug("begin ios reading");
        _sb->beginRead();
    }
}


void IOBuffer::endRead()
{
    log_debug("end reading");
    _reading = false;
    
    StreamBuffer::import();
}


void IOBuffer::onInput(Pt::System::IOBuffer& sb)
{
    try
    {
        _sb->endRead();
        log_debug("client received raw = " << _sb->in_avail());

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
    if( ! _sb)
        return;

    log_debug("sync with underlying strambuf");
    this->pubsync();

    log_debug("begin writing");
    _sb->beginWrite();
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


void IOBuffer::onOutput(Pt::System::IOBuffer& sb)
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
