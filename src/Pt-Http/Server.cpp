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
#include <Pt/Net/Endpoint.h>
#include <memory>

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
    std::auto_ptr<ServerImpl> impl(_impl);

    setActive(loop);
    impl.release();
}


Server::Server(System::EventLoop& loop, const Pt::Net::Endpoint& ep)
: _impl(0)
{
    _impl = new ServerImpl();
    std::auto_ptr<ServerImpl> impl(_impl);
    
    setActive(loop);
    listen(ep);
    impl.release();
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


std::size_t Server::timeout() const
{
    return _impl->timeout();
}


void Server::setTimeout(std::size_t ms)
{
    _impl->setTimeout(ms);
}


void Server::setSecure(Ssl::Context& ctx)
{
    _impl->setSecure(ctx); 
}


std::size_t Server::maxThreads() const
{ 
    return _impl->maxThreads(); 
}


void Server::setMaxThreads(std::size_t m)
{ 
    _impl->setMaxThreads(m); 
}


std::size_t Server::keepAliveTimeout() const
{
    return _impl->keepAliveTimeout();
}


void Server::setKeepAliveTimeout(std::size_t ms)
{
    _impl->setKeepAliveTimeout(ms);
}


std::size_t Server::maxRequestSize() const
{
    return _impl->maxRequestSize();
}


void Server::setMaxRequestSize(std::size_t maxSize)
{
    _impl->setMaxRequestSize(maxSize);
}


void Server::listen(const Pt::Net::Endpoint& ep)
{
    Net::TcpServerOptions opts;
    listen(ep, opts);
}


void Server::listen(const Pt::Net::Endpoint& ep, const Net::TcpServerOptions& opts)
{
    _impl->listen(ep, opts);
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


void Server::shutdownServlet(Servlet& servlet, bool shutdown)
{
    _impl->shutdownServlet(servlet, shutdown);
}


bool Server::isServletIdle(Servlet& servlet)
{
    return _impl->isServletIdle(servlet);
}


Servlet* Server::getServlet(const Request& request)
{
    return _impl->getServlet(request);
}

} // namespace Http

} // namespace Pt
