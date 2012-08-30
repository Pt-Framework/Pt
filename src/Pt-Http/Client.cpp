/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#include <Pt/Http/Client.h>
#include <Pt/Net/AddrInfo.h>
#include "ClientImpl.h"

namespace Pt {

namespace Http {

ConnectionClosed::ConnectionClosed()
: IOError("HTTP connection closed")
{
}

Client::Client()
: _impl( new ClientImpl(this) )
{
}

Client::Client(const Net::AddrInfo& addrinfo, bool ssl)
: _impl( new ClientImpl(this) )
{
    _impl->setHost(addrinfo);
    _impl->setHttps(ssl);
}

Client::Client(const std::string& host, unsigned short int port, bool ssl)
: _impl( new ClientImpl(this) )
{
    _impl->setHost( Net::AddrInfo(host, port) );
    _impl->setHttps(ssl);
}


Client::Client(System::EventLoop& loop, const Net::AddrInfo& addrinfo, bool ssl)
: _impl( new ClientImpl(this) )
{
    _impl->setActive(loop);
    _impl->setHost(addrinfo);
    _impl->setHttps(ssl);
}

Client::Client(System::EventLoop& loop, const std::string& host, unsigned short int port, bool ssl)
: _impl( new ClientImpl(this) )
{
    _impl->setActive(loop);
    _impl->setHost( Net::AddrInfo(host, port) );
    _impl->setHttps(ssl);
}

Client::~Client()
{
    delete _impl;
}

void Client::setHost(const Net::AddrInfo& addrinfo, bool ssl)
{
    _impl->setHost(addrinfo);
    _impl->setHttps(ssl);
}

void Client::setHost(const std::string& host, unsigned short int port, bool ssl)
{
    _impl->setHost( Net::AddrInfo(host, port) );
    _impl->setHttps(ssl);
}

const Net::AddrInfo& Client::host() const
{
    return _impl->host();
}

System::EventLoop* Client::loop() const
{
    return _impl->loop();
}

void Client::setActive(System::EventLoop& selector)
{
    _impl->setActive(selector);
}

void Client::setTimeout(std::size_t timeout)
{
    _impl->setTimeout(timeout);
}

void Client::setContext(Ssl::Context& ctx)
{
    _impl->setContext(ctx);
}


void Client::send()
{
    return _impl->send();
}


std::istream& Client::receive()
{
    return _impl->receive();
}


void Client::beginSend()
{
    _impl->beginSend();
}


bool Client::endSend()
{
    return _impl->endSend();
}


void Client::beginReceive()
{
    _impl->beginReceive();
}


bool Client::endReceive()
{
    return _impl->endReceive();
}


bool Client::isEnd() const
{
    return _impl->isEnd();
}


Request& Client::request()
{ 
    return _impl->request();
}


const Request& Client::request() const
{ 
    return _impl->request();
}


Reply& Client::reply()
{
    return _impl->reply();
}


const Reply& Client::reply() const
{
    return _impl->reply();
}


// TODO: Sets the username and password for all subsequent requests.
void Client::setAuthorization(const std::string& username, const std::string& password)
{
    _impl->setAuth(username, password);
}

void Client::clearAuthorization()
{
    _impl->clearAuth();
}

void Client::cancel()
{
    _impl->cancel();
}

} // namespace Http

} // namespace Pt
