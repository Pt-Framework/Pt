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

namespace Pt {

namespace Net {

TcpSocket::TcpSocket()
: _impl(0)
{
    _impl = new TcpSocketImpl(*this);
}


TcpSocket::TcpSocket(const TcpServer& server, unsigned flags)
: _impl(0)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    this->accept(server, flags);

    impl.release();
}


TcpSocket::TcpSocket(const std::string& ipaddr, unsigned short int port)
: _impl(0)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    this->connect(ipaddr, port);

    impl.release();
}


TcpSocket::TcpSocket(const AddrInfo& addrinfo)
: _impl(0)
{
    _impl = new TcpSocketImpl(*this);
    std::auto_ptr<TcpSocketImpl> impl(_impl);

    this->connect(addrinfo);

    impl.release();
}


TcpSocket::~TcpSocket()
{
    std::cerr << "TcpSocket::~TcpSocket()" << std::endl;
    try
    {
        std::cerr << "TcpSocket::~TcpSocket() close" << std::endl;
        this->close();
    }
    catch(...)
    {}

    std::cerr << "TcpSocket::~TcpSocket() delete _impl" << std::endl;
    delete _impl;
    std::cerr << "TcpSocket::~TcpSocket() return" << std::endl;
}


std::string TcpSocket::getSockAddr() const
{
    return _impl->getSockAddr();
}


std::string TcpSocket::getPeerAddr() const
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


void TcpSocket::connect(const AddrInfo& addrinfo)
{
    this->close();
    _impl->connect(addrinfo);
    //this->setEnabled(true);
    this->setAsync(true);
    this->setEof(false);
}


bool TcpSocket::beginConnect(const AddrInfo& addrinfo)
{
    this->close();
    bool ret = _impl->beginConnect(addrinfo);
    //this->setEnabled(true);
    this->setAsync(true);
    this->setEof(false);

    if(ret)
        connected(*this);
    return ret;
}


void TcpSocket::endConnect()
{
    try
    {
        _impl->endConnect();
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


void TcpSocket::accept(const TcpServer& server, unsigned flags)
{
    this->close();
    _impl->accept(server, flags);
    //this->setEnabled(true);
    this->setAsync(true);
    this->setEof(false);
}


void TcpSocket::onClose()
{
    std::cerr << "TcpSocket::onClose() 1" << std::endl;
    cancel();
    std::cerr << "TcpSocket::onClose() 2" << std::endl;
    _impl->close();
}


bool TcpSocket::onWait(std::size_t msecs)
{
    return _impl->wait(msecs);
}


void TcpSocket::onAttach(System::EventLoop& sb)
{
    _impl->attach(sb);
}


void TcpSocket::onDetach(System::EventLoop& sb)
{
    _impl->detach(sb);
}


bool TcpSocket::onAvail()
{
    return _impl->avail();
}


size_t TcpSocket::onBeginRead(char* buffer, size_t n, bool& eof)
{
    if (!_impl->isConnected())
        throw System::IOPending( PT_ERROR_MSG("connect operation pending") );

    return _impl->beginRead(buffer, n, eof);
}


size_t TcpSocket::onEndRead(bool& eof)
{
    return _impl->endRead(eof);
}


size_t TcpSocket::onRead(char* buffer, size_t count, bool& eof)
{
    return _impl->read(buffer, count, eof);
}


size_t TcpSocket::onBeginWrite(const char* buffer, size_t n)
{
    if (!_impl->isConnected())
        throw System::IOPending( PT_ERROR_MSG("connect operation pending") );

    return _impl->beginWrite(buffer, n);
}


size_t TcpSocket::onEndWrite()
{
    return _impl->endWrite();
}


size_t TcpSocket::onWrite(const char* buffer, size_t count)
{
    return _impl->write(buffer, count);
}


void TcpSocket::onCancel()
{
    if (_impl->isConnected())
    {
        _impl->cancel();
    }
    else //if (enabled())
    {
        // we are in connecting state
        _impl->close();
        //setEnabled(false);
    }
}


System::IODeviceImpl& TcpSocket::ioimpl()
{
	System::IODeviceImpl* dev = (System::IODeviceImpl*)  _impl;
    return *dev;
}

} // namespace Net

} // namespace Pt
