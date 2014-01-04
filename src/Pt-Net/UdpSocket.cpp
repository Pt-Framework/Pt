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

#include "UdpSocketImpl.h"
#include <Pt/Net/UdpSocket.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/IOError.h>
#include <stdexcept>
#include <memory>

namespace Pt {

namespace Net {

UdpSocketOptions::UdpSocketOptions()
: _flags(0)
, _hoplimit(1)
{
}


UdpSocketOptions::UdpSocketOptions(const UdpSocketOptions& opts)
: _flags(opts._flags)
, _hoplimit(opts._hoplimit)
{
}


UdpSocketOptions::~UdpSocketOptions()
{
}


UdpSocketOptions& UdpSocketOptions::operator=(const UdpSocketOptions& opts)
{
    _flags = opts._flags;
    _hoplimit = opts._hoplimit;
    return *this;
}




UdpSocket::UdpSocket()
: _impl(0)
, _connecting(false)
, _binding(false)
{
    _impl = new UdpSocketImpl(*this);
}


UdpSocket::UdpSocket(System::EventLoop& loop)
: _impl(0)
, _connecting(false)
, _binding(false)
{
    _impl = new UdpSocketImpl(*this);
    std::auto_ptr<UdpSocketImpl> impl(_impl);

    setActive(loop);
    impl.release();
}


UdpSocket::~UdpSocket()
{
    try
    {
        this->close();
    }
    catch(...)
    {}

    delete _impl;
}


void UdpSocket::bind(const Endpoint& ep)
{
    UdpSocketOptions opts;
    bind(ep, opts);
}


void UdpSocket::bind(const Endpoint& ep, const UdpSocketOptions& opts)
{
    _impl->bind(ep, opts);
    this->setEof(false);
}


bool UdpSocket::beginBind(const Endpoint& ep)
{
    UdpSocketOptions opts;
    return beginBind(ep, opts);
}


bool UdpSocket::beginBind(const Endpoint& addrinfo, const UdpSocketOptions& o)
{
    System::EventLoop* loop = this->loop();
    if( ! loop )
        throw std::logic_error("socket not active");

    bool ret = _impl->beginBind(*loop, addrinfo, o);
    _binding = true;
    
    if(ret)
    {
        loop->setReady(*this);
    }

    return ret;
}


void UdpSocket::endBind()
{
    try
    {
        if(_binding)
        {
            _binding = false;
            _impl->endBind( *loop() );
            this->setEof(false);
        }
    }
    catch (...)
    {
        close();
        throw;
    }
}


bool UdpSocket::isBound() const
{
    return _impl->isBound();
}


void UdpSocket::connect(const Endpoint& ep)
{
    UdpSocketOptions opts;
    connect(ep, opts);
}


void UdpSocket::connect(const Endpoint& ep, const UdpSocketOptions& opts)
{
    _impl->connect(ep, opts);
    this->setEof(false);
}


void UdpSocket::setTarget(const Endpoint& ep)
{
    UdpSocketOptions opts;
    setTarget(ep, opts);
}


void UdpSocket::setTarget(const Endpoint& ep, const UdpSocketOptions& opts)
{
    _impl->setTarget(ep, opts);
    this->setEof(false);
}


bool UdpSocket::beginConnect(const Endpoint& ep)
{
    UdpSocketOptions opts;
    return beginConnect(ep, opts);
}


bool UdpSocket::beginConnect(const Endpoint& ep, const UdpSocketOptions& opts)
{
    System::EventLoop* loop = this->loop();
    if( ! loop )
        throw std::logic_error( PT_ERROR_MSG("socket not active") );

    bool ret = _impl->beginConnect(*loop, ep, opts);
    _connecting = true;
    
    if(ret)
    {
        loop->setReady(*this);
    }

    return ret;
}


void UdpSocket::endConnect()
{
    try
    {
        if(_connecting)
        {
            _connecting = false;
            _impl->endConnect( *loop() );
            this->setEof(false);
        }
    }
    catch (...)
    {
        close();
        throw;
    }
}


bool UdpSocket::isConnected() const
{
    return _impl->isConnected();
}


void UdpSocket::joinMulticastGroup(const std::string& ipaddr)
{
    _impl->joinMulticastGroup(ipaddr);
}


//void UdpSocket::dropMulticastGroup(const std::string& ipaddr)
//{
//    _impl->dropMulticastGroup(ipaddr);
//}


void UdpSocket::localEndpoint(Endpoint& ep) const
{
    _impl->localEndpoint(ep);
}


const Endpoint& UdpSocket::remoteEndpoint() const
{
    return _impl->remoteEndpoint();
}


void UdpSocket::onClose()
{
    _impl->close();

    _connecting = false;
    _binding = false;
}


void UdpSocket::onSetTimeout(std::size_t timeout)
{
    _impl->setTimeout(timeout);
}


bool UdpSocket::onRun()
{
    if( _connecting )
    {
        if( this->isConnected() || _impl->runConnect( *loop() ) )
        {
            connected().send(*this);
            return true;
        }

        return false;
    }

    if( _binding )
    {
        if( this->isBound() || _impl->runBind( *loop() ) )
        {
            bound().send(*this);
            return true;
        }

        return false;
    }

    if( this->isReading() )
    {
        if( _ravail || _impl->runRead( *loop() ) )
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


std::size_t UdpSocket::onBeginRead(System::EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return _impl->beginRead(loop, buffer, n, eof);
}


std::size_t UdpSocket::onEndRead(System::EventLoop& loop, char* buffer, std::size_t n, bool& eof)
{
    return _impl->endRead(loop, buffer, n, eof);
}


std::size_t UdpSocket::onRead(char* buffer, std::size_t count, bool& eof)
{
    return _impl->read(buffer, count, eof);
}


std::size_t UdpSocket::onBeginWrite(System::EventLoop& loop, const char* buffer, std::size_t n)
{
    return _impl->beginWrite(loop, buffer, n);
}


std::size_t UdpSocket::onEndWrite(System::EventLoop& loop, const char* buffer, std::size_t n)
{
    return _impl->endWrite(loop, buffer, n);
}


std::size_t UdpSocket::onWrite(const char* buffer, std::size_t count)
{
    return _impl->write(buffer, count);
}


void UdpSocket::onCancel()
{
    System::EventLoop* loop = this->loop();
    if( loop )
    {
        _impl->cancel(*loop);
        _connecting = false;
        _binding = false;
    }

    IODevice::onCancel();
}

} // namespace Net

} // namespace Pt
