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

void Client::connect(const Net::AddrInfo& addrinfo)
{
    _impl->connect(addrinfo);
}

void Client::connect(const std::string& host, unsigned short int port)
{
    _impl->connect(Net::AddrInfo(host, port));
}

const ReplyHeader& Client::execute(const Request& request, std::size_t timeout)
{
    try
    {
        return _impl->execute(request, timeout);
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

std::string Client::get(const std::string& url, std::size_t timeout)
{
    return _impl->get(url, timeout);
}

void Client::beginExecute(const Request& request)
{
    _impl->beginExecute(request);
}

void Client::endExecute()
{
    _impl->endExecute();
}

void Client::setActive(System::EventLoop& selector)
{
    _impl->setActive(selector);
}

/*void Client::wait(std::size_t msecs)
{
    _impl->wait(msecs);
}*/

std::istream& Client::in()
{
    return _impl->in();
}

const std::string& Client::host() const
{
    return _impl->host();
}

unsigned short int Client::port() const
{
    return _impl->port();
}

// Sets the username and password for all subsequent requests.
void Client::auth(const std::string& username, const std::string& password)
{
    _impl->auth(username, password);
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
