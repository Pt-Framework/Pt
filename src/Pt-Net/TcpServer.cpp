/*
 * Copyright (C) 2006-2012 by Marc Boris Duerner
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

#include "TcpServerImpl.h"
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/AddressInUse.h>
#include <Pt/System/EventLoop.h>
#include <memory>

namespace Pt {

namespace Net {

TcpServer::TcpServer()
: _loop(0)
, _impl(0)
{
    _impl = new TcpServerImpl(*this);
}


TcpServer::TcpServer(const std::string& ipaddr, unsigned short int port, const Options& options)
: _loop(0)
, _impl(0)
{
    _impl = new TcpServerImpl(*this);
    std::auto_ptr<TcpServerImpl> impl(_impl);

    this->listen(ipaddr, port, options);

    impl.release();
}


TcpServer::TcpServer(const Endpoint& ipaddr, const Options& options)
: _loop(0)
, _impl(0)
{
    _impl = new TcpServerImpl(*this);
    std::auto_ptr<TcpServerImpl> impl(_impl);

    this->listen(ipaddr, options);

    impl.release();
}


TcpServer::~TcpServer()
{
    try
    {
        this->close();
    }
    catch(...)
    {}

    delete _impl;
}


void TcpServer::listen(const std::string& ipaddr, unsigned short int port, const Options& options)
{
    this->close();
    _impl->listen(ipaddr, port, options);
}


void TcpServer::listen(const Endpoint& ipaddr, const Options& options)
{
    this->close();
    _impl->listen(ipaddr, options);
}


void TcpServer::beginAccept()
{
    System::EventLoop* loop = this->loop();
    if( ! loop )
        throw std::logic_error("TCP server not active");

    _impl->beginAccept( *loop );
}


void TcpServer::close()
{
    _impl->close();
}


Signal<TcpServer&>& TcpServer::connectionPending()
{ 
    return _connectionPending; 
}


TcpServerImpl& TcpServer::impl() const
{
    return *_impl;
}


void TcpServer::onCancel()
{
    System::EventLoop* loop = this->loop();
    if( loop )
        _impl->cancel( *loop );
}


bool TcpServer::onRun()
{
    bool avail = false;

    System::EventLoop* eloop = loop();
    if(eloop)
    {
        avail = _impl->run(*eloop);
        if(avail)
            _connectionPending.send(*this);
    }

    return avail;
}

} // namespace Net

} // namespace Pt
