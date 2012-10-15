/*
 * Copyright (C) 2011-2012 by Marc Boris Duerner
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

#include "ServerImpl.h"
#include <Pt/Http/Server.h>
#include <Pt/Http/Servlet.h>
#include <Pt/Net/AddrInfo.h>

namespace Pt {

namespace Http {

Server::Server()
: _impl(0)
{
    _impl = new ServerImpl();
}


Server::Server(System::EventLoop& loop)
: _impl(0)
{
    _impl = new ServerImpl();
    setActive(loop);
}


Server::Server(System::EventLoop& loop, const std::string& ip, unsigned short int port, int backlog)
: _impl(0)
{
    _impl = new ServerImpl();
    setActive(loop);
    listen(ip, port, backlog);
}


Server::Server(System::EventLoop& loop, const Pt::Net::AddrInfo& addr, int backlog)
: _impl(0)
{
    _impl = new ServerImpl();
    setActive(loop);
    listen(addr, backlog);
}


Server::~Server()
{
    delete _impl;
}


System::EventLoop* Server::loop()
{ 
    return _impl->loop(); 
}


void Server::setActive(System::EventLoop& loop)
{
     _impl->setActive(loop);
}


void Server::setSecure(Ssl::Context& ctx)
{
    _impl->setSecure(ctx);
}


std::size_t Server::timeout() const
{
    return _impl->timeout();
}


void Server::setTimeout(std::size_t ms)
{
    _impl->setTimeout(ms);
}


std::size_t Server::keepAliveTimeout() const
{
    return _impl->keepAliveTimeout();
}


void Server::setKeepAliveTimeout(std::size_t ms)
{
    _impl->setKeepAliveTimeout(ms);
}


unsigned Server::maxThreads() const
{
    return _impl->maxThreads();
}


void Server::setMaxThreads(unsigned m)
{
    _impl->setMaxThreads(m);
}


void Server::listen(const Pt::Net::AddrInfo& addr, int backlog)
{
    _impl->listen(addr, backlog);
}


void Server::listen(const std::string& ip, unsigned short int port, int backlog)
{
    Net::AddrInfo ai(ip, port, true);
    _impl->listen(ai, backlog);
}


void Server::cancel()
{
    _impl->cancel();
}


void Server::addServlet(Servlet& servlet)
{
    _impl->addServlet(servlet);
    servlet.registerServer(*this);
}


void Server::removeServlet(Servlet& servlet)
{
    _impl->removeServlet(servlet);
    servlet.unregisterServer(*this);
}


Servlet* Server::getServlet(const Request& request)
{
    return _impl->getServlet(request);
}

} // namespace Http

} // namespace Pt
