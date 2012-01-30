/*
 * Copyright (C) 2006-2009 by Marc Boris Duerner, Tommi Maekitalo
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
#include "Pt/Net/TcpSocket.h"
#include "Pt/System/IOError.h"
#include <stdexcept>
#include <memory>
#include <iostream>
#include <cassert>

namespace Pt {

namespace Net {

ConnectFailed::ConnectFailed()
: System::IOError("connect failed")
, _port(0)
{}


ConnectFailed::ConnectFailed(const std::string& host, unsigned short int port)
: System::IOError("connect failed to " + host)
, _host(host)
, _port(port)
{}


TcpSocket::TcpSocket()
: _impl(0)
, _connecting(false)
{
    _impl = new TcpSocketImpl(*this);
}


TcpSocket::TcpSocket(const TcpServer& server, unsigned flags)
: _impl(0)
, _connecting(false)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    this->accept(server, flags);

    impl.release();
}


TcpSocket::TcpSocket(const std::string& ipaddr, unsigned short int port)
: _impl(0)
, _connecting(false)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    this->connect(ipaddr, port);

    impl.release();
}


TcpSocket::TcpSocket(const AddrInfo& addrinfo)
: _impl(0)
, _connecting(false)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    this->connect(addrinfo);

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


std::string TcpSocket::socketAddress() const
{
    return _impl->getSockAddr();
}


std::string TcpSocket::peerAddress() const
{
    return _impl->getPeerAddr();
}


void TcpSocket::setTimeout(std::size_t msecs)
{
    _impl->setTimeout(msecs);
}


std::size_t TcpSocket::timeout() const
{
    return _impl->timeout();
}


void TcpSocket::accept(const TcpServer& server, unsigned flags)
{
    this->close();

    _impl->accept(server, flags);
}


void TcpSocket::connect(const AddrInfo& addrinfo)
{
    this->close();
    _impl->connect(addrinfo);
}


void TcpSocket::connect(const std::string& ipaddr, unsigned short int port)
{ 
    connect(AddrInfo(ipaddr, port)); 
}


bool TcpSocket::beginConnect(const std::string& ipaddr, unsigned short int port)
{ 
    return beginConnect(AddrInfo(ipaddr, port)); 
}


bool TcpSocket::beginConnect(const AddrInfo& addrinfo)
{
    if( ! isActive() )
        throw std::logic_error( PT_ERROR_MSG("socket not active") );

    this->close();

    bool ret = _impl->beginConnect(addrinfo, *parent());
    _connecting = true;
    if(ret)
    {
        this->setReady();
    }

    return ret;
}


void TcpSocket::endConnect()
{
    try
    {
        _connecting = false;
        _impl->endConnect( *parent() );
    }
    catch (...)
    {
        close();
        throw;
    }
}


bool TcpSocket::isConnected() const
{
    return _impl->isConnected();
}


void TcpSocket::onClose()
{
    _impl->close();
}


bool TcpSocket::onRun()
{
    if( _connecting )
    {
        if( this->isConnected() || _impl->runConnect( *parent() ) )
        {
            connected().send(*this);
            return true;
        }

        return false;
    }

    if( this->reading() )
    {
        if( _ravail || _impl->runRead( *parent() ) )
        {
            inputReady().send(*this);
            return true;
        }
    }

    if( this->writing() )
    {
        if( _wavail || _impl->runWrite( *parent() ) )
        {
            outputReady().send(*this);
            return false;
        }
    }

    return false;
}


size_t TcpSocket::onBeginRead(char* buffer, size_t n, bool& eof)
{
    if (!_impl->isConnected())
        throw System::IOPending( PT_ERROR_MSG("connect operation pending") );

    return _impl->beginRead(*parent(), buffer, n, eof);
}


size_t TcpSocket::onEndRead(char* buffer, size_t n, bool& eof)
{
    return _impl->endRead(*parent(), buffer, n, eof);
}


size_t TcpSocket::onRead(char* buffer, size_t count, bool& eof)
{
    return _impl->read(buffer, count, eof);
}


size_t TcpSocket::onBeginWrite(const char* buffer, size_t n)
{
    if (!_impl->isConnected())
        throw System::IOPending( PT_ERROR_MSG("connect operation pending") );

    return _impl->beginWrite(*parent(), buffer, n);
}


size_t TcpSocket::onEndWrite(const char* buffer, size_t n)
{
    return _impl->endWrite(*parent(), buffer, n);
}


size_t TcpSocket::onWrite(const char* buffer, size_t count)
{
    return _impl->write(buffer, count);
}


void TcpSocket::onCancel()
{
    if( isActive() )
    {
        _impl->cancel( *parent() );
    }

    IODevice::onCancel();
}

} // namespace Net

} // namespace Pt
