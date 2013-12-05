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

#include "UdpSocketImpl.h"
#include "Pt/Net/UdpSocket.h"
#include <stdexcept>
#include <memory>
#include <Pt/System/IOError.h>

namespace Pt {

namespace Net {

UdpSocket::UdpSocket()
: _impl(0)
, _connecting(false)
, _binding(false)
{
    _impl = new UdpSocketImpl(*this);
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


bool UdpSocket::beginBind(const std::string& ipaddr, unsigned short int port, const Options& o)
{ 
    Endpoint ai(ipaddr, port);
    return beginBind( ai, o); 
}


bool UdpSocket::beginBind(const Endpoint& addrinfo, const Options& o)
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


void UdpSocket::bind(const Endpoint& addrinfo, const Options& opts)
{
    _impl->bind(addrinfo, opts);
    this->setEof(false);
}


void UdpSocket::bind(const std::string& ipaddr, unsigned short int port, const Options& opts)
{
    Endpoint ai(ipaddr, port);
    bind(ai, opts);
}


bool UdpSocket::beginConnect(const std::string& ipaddr, unsigned short int port, const Options& opts)
{ 
    return beginConnect(Endpoint(ipaddr, port), opts); 
}


bool UdpSocket::beginConnect(const Endpoint& addrinfo, const Options& opts)
{
    System::EventLoop* loop = this->loop();
    if( ! loop )
        throw std::logic_error( PT_ERROR_MSG("socket not active") );

    bool ret = _impl->beginConnect(*loop, addrinfo, opts);
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


void UdpSocket::connect(const std::string& ipaddr, unsigned short int port, const Options& opts)
{
    connect( Endpoint(ipaddr, port), opts );
}


void UdpSocket::connect(const Endpoint& addrinfo, const Options& opts)
{
    _impl->connect(addrinfo, opts);
    this->setEof(false);
}


void UdpSocket::setTarget(const std::string& ipaddr, unsigned short int port, const Options& opts)
{
    _impl->setTarget(Endpoint(ipaddr, port), opts);
    this->setEof(false);
}


void UdpSocket::setTarget(const Endpoint& addrinfo, const Options& opts)
{
    _impl->setTarget(addrinfo, opts);
    this->setEof(false);
}

bool UdpSocket::isConnected() const
{
    return _impl->isConnected();
}


bool UdpSocket::isBound() const
{
    return _impl->isBound();
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
}


void UdpSocket::onSetTimeout(size_t timeout)
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


size_t UdpSocket::onBeginRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    return _impl->beginRead(loop, buffer, n, eof);
}


size_t UdpSocket::onEndRead(System::EventLoop& loop, char* buffer, size_t n, bool& eof)
{
    return _impl->endRead(loop, buffer, n, eof);
}


size_t UdpSocket::onRead(char* buffer, size_t count, bool& eof)
{
    return _impl->read(buffer, count, eof);
}


size_t UdpSocket::onBeginWrite(System::EventLoop& loop, const char* buffer, size_t n)
{
    return _impl->beginWrite(loop, buffer, n);
}


size_t UdpSocket::onEndWrite(System::EventLoop& loop, const char* buffer, size_t n)
{
    return _impl->endWrite(loop, buffer, n);
}


size_t UdpSocket::onWrite(const char* buffer, size_t count)
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

} //namespace Net

} // namespace Pt
