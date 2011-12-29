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


void UdpSocket::bind(const std::string& ipaddr, unsigned short int port, unsigned flags)
{
    _impl->bind(ipaddr, port, flags);
    this->setEof(false);
}


void UdpSocket::connect(const AddrInfo& addrinfo)
{
    _impl->connect(addrinfo);
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


void UdpSocket::setBroadcast()
{
    _impl->setBroadcast();
}


void UdpSocket::joinMulticastGroup(const std::string& ipaddr)
{
    _impl->joinMulticastGroup(ipaddr);
}


void UdpSocket::dropMulticastGroup(const std::string& ipaddr)
{
    _impl->dropMulticastGroup(ipaddr);
}


std::string UdpSocket::getSockAddr() const
{
    return _impl->getSockAddr();
}


std::string UdpSocket::getPeerAddr() const
{
    return _impl->getPeerAddr();
}


void UdpSocket::setTimeout(std::size_t msecs)
{
    _impl->setTimeout(msecs);
}


std::size_t UdpSocket::timeout() const
{
    return _impl->timeout();
}


void UdpSocket::onClose()
{
    cancel();
    _impl->close();
}


bool UdpSocket::onWait(std::size_t msecs)
{
    return false;
}


void UdpSocket::onAttach(System::EventLoop& loop)
{
    _impl->attach(loop);
}


void UdpSocket::onDetach(System::EventLoop& loop)
{
    _impl->detach(loop);
}


bool UdpSocket::onRun()
{
    return _impl->run();
}


size_t UdpSocket::onBeginRead(char* buffer, size_t n, bool& eof)
{
    if( ! _impl->isConnected() && ! _impl->isBound() )
        throw System::IOError("socket not connected");

    return _impl->beginRead(buffer, n, eof);
}


size_t UdpSocket::onEndRead(bool& eof)
{
    return _impl->endRead(eof);
}


size_t UdpSocket::onRead(char* buffer, size_t count, bool& eof)
{
    return _impl->read(buffer, count, eof);
}


size_t UdpSocket::onBeginWrite(const char* buffer, size_t n)
{
    if( ! _impl->isConnected() && ! _impl->isBound() )
        throw System::IOError("socket not connected");

    return _impl->beginWrite(buffer, n);
}


size_t UdpSocket::onEndWrite()
{
    return _impl->endWrite();
}


size_t UdpSocket::onWrite(const char* buffer, size_t count)
{
    return _impl->write(buffer, count);
}


void UdpSocket::onCancel()
{
    if(_impl->isConnected() || _impl->isBound() )
    {
        _impl->cancel();
    }

    IODevice::onCancel();
}


System::IODeviceImpl& UdpSocket::ioimpl()
{
	System::IODeviceImpl* dev = (System::IODeviceImpl*) _impl;
    return *dev;
}

} //namespace Net

} // namespace Pt
