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

Client::Client()
: _impl(new ClientImpl(this))
{
}

Client::Client(const Net::AddrInfo& addrinfo)
: _impl(new ClientImpl(this, addrinfo))
{
}

Client::Client(const std::string& host, unsigned short int port)
: _impl(new ClientImpl(this, Net::AddrInfo(host, port)))
{
}


Client::Client(System::EventLoop& selector, const Net::AddrInfo& addrinfo)
: _impl(new ClientImpl(this, selector, addrinfo))
{
}

Client::Client(System::EventLoop& selector, const std::string& host, unsigned short int port)
: _impl(new ClientImpl(this, selector, Net::AddrInfo(host, port)))
{
}

Client::~Client()
{
    delete _impl;
}

void Client::setHost(const Net::AddrInfo& addrinfo)
{
    _impl->setHost(addrinfo);
}

void Client::setHost(const std::string& host, unsigned short int port)
{
    _impl->setHost(Net::AddrInfo(host, port));
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

void Client::setSecure(Ssl::Context& ctx)
{
    _impl->setSecure(ctx);
}

const ReplyHeader& Client::execute(const Request& request)
{
    try
    {
        return _impl->execute(request);
    }
    catch (...)
    {
        cancel();
        throw;
    }
}

const ReplyHeader& Client::header()
{
    return _impl->header();
}

void Client::readBody(std::string& s)
{
    _impl->readBody(s);
}

std::string Client::get(const std::string& url)
{
    Request request(url);
    execute(request);
    return readBody();
}

void Client::beginExecute(const Request& request)
{
    _impl->beginRequest(request);
}

void Client::endExecute()
{
    _impl->endExecute();
}

std::istream& Client::in()
{
    return _impl->in();
}

// Sets the username and password for all subsequent requests.
void Client::setAuth(const std::string& username, const std::string& password)
{
    _impl->setAuth(username, password);
}

void Client::clearAuth()
{
    _impl->clearAuth();
}

void Client::cancel()
{
    _impl->cancel();
}

} // namespace Http

} // namespace Pt
