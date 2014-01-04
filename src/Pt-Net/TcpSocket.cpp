/*
 * Copyright (C) 2006-2013 by Marc Boris Duerner
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

#include "TcpSocketImpl.h"
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/EventLoop.h>
#include <stdexcept>
#include <memory>
#include <cassert>

namespace Pt {

namespace Net {

TcpSocketOptions::TcpSocketOptions()
: _flags(0)
{
}


TcpSocketOptions::TcpSocketOptions(const TcpSocketOptions& opts)
{
}


TcpSocketOptions::~TcpSocketOptions()
{
}


TcpSocketOptions& TcpSocketOptions::operator=(const TcpSocketOptions& opts)
{
    return *this;
}




TcpSocket::TcpSocket()
: _impl(0)
, _connecting(false)
, _isConnected(false)
{
    _impl = new TcpSocketImpl(*this);
}


TcpSocket::TcpSocket(System::EventLoop& loop)
: _impl(0)
, _connecting(false)
, _isConnected(false)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    setActive(loop);
    impl.release();
}


TcpSocket::TcpSocket(TcpServer& server)
: _impl(0)
, _connecting(false)
, _isConnected(false)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    this->accept(server);
    impl.release();
}


TcpSocket::TcpSocket(const Endpoint& ep)
: _impl(0)
, _connecting(false)
, _isConnected(false)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    this->connect(ep);
    impl.release();
}


TcpSocket::~TcpSocket()
{
    try
    {
        this->close();
    }
    catch(...)
    {}

    delete _impl;
}


void TcpSocket::accept(TcpServer& server)
{
    TcpSocketOptions opts;
    accept(server, opts);
}


void TcpSocket::accept(TcpServer& server, const TcpSocketOptions& o)
{
    this->close();

    _impl->accept(server, o);
    _isConnected = true;
}


void TcpSocket::connect(const Endpoint& addrinfo)
{
    TcpSocketOptions opts;
    connect(addrinfo, opts);
}


void TcpSocket::connect(const Endpoint& addrinfo, const TcpSocketOptions& opts)
{
    this->close();
    
    _impl->connect(addrinfo, opts);
    _isConnected = true;
}


void TcpSocket::beginConnect(const Endpoint& ep)
{
    TcpSocketOptions opts;
    return beginConnect(ep, opts);
}


void TcpSocket::beginConnect(const Endpoint& ep, const TcpSocketOptions& opts)
{
    System::EventLoop* loop = this->loop();
    if( ! loop )
        throw std::logic_error("socket not active");

    this->close();

    _isConnected = _impl->beginConnect(*loop, ep, opts);
    _connecting = true;
    
    if(_isConnected)
    {
        loop->setReady(*this);
    }
}


void TcpSocket::endConnect()
{
    try
    {
        if( _isConnected )
        {
            _connecting = false;
            return;
        }

        if(_connecting)
        {
            _connecting = false;
            _impl->endConnect( *loop() );
            _isConnected = true;
        }
    }
    catch (...)
    {
        close();
        throw;
    }
}


void TcpSocket::localEndpoint(Endpoint& ep) const
{
    _impl->localEndpoint(ep);
}


void TcpSocket::remoteEndpoint(Endpoint& ep) const
{
    _impl->remoteEndpoint(ep);
}


void TcpSocket::onClose()
{
    _impl->close();
    _connecting = false;
    _isConnected = false;
}


void TcpSocket::onSetTimeout(std::size_t timeout)
{
    _impl->setTimeout(timeout);
}


bool TcpSocket::onRun()
{
    if( _connecting )
    {
        if( this->isConnected() || _impl->runConnect( *loop(), _isConnected ) )
        {
            connected().send(*this);
            return true;
        }

        return false;
    }

    if( this->isReading() )
    {
        if( _ravail || isEof() || _impl->runRead( *loop() ) )
        {
            inputReady().send(*this);
            return true;
        }
    }

    if( this->isWriting() )
    {
        if( _wavail || _impl->runWrite( *loop() ) )
        {
            outputReady().send(*this);
            return true;
        }
    }

    return false;
}


std::size_t TcpSocket::onBeginRead(System::EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return _impl->beginRead(loop, buffer, n, eof);
}


std::size_t TcpSocket::onEndRead(System::EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return _impl->endRead(loop, buffer, n, eof);
}


std::size_t TcpSocket::onRead(char* buffer, std::size_t count, bool& eof)
{
    return _impl->read(buffer, count, eof);
}


std::size_t TcpSocket::onBeginWrite(System::EventLoop& loop, const char* buffer, std::size_t n)
{
    return _impl->beginWrite(loop, buffer, n);
}


std::size_t TcpSocket::onEndWrite(System::EventLoop& loop, const char* buffer, std::size_t n)
{
    return _impl->endWrite(loop, buffer, n);
}


std::size_t TcpSocket::onWrite(const char* buffer, std::size_t count)
{
    return _impl->write(buffer, count);
}


void TcpSocket::onCancel()
{
    System::EventLoop* loop = this->loop();
    if( loop )
    {
        _impl->cancel(*loop);
        _connecting = false;
    }

    IODevice::onCancel();
}

} // namespace Net

} // namespace Pt
