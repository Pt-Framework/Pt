/*
 * Copyright (C) 2012 by Marc Boris Duerner
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

#include "ClientImpl.h"
#include <Pt/Http/Client.h>
#include <Pt/Net/Endpoint.h>
#include <Pt/System/Logger.h>
#include <memory>

log_define("Pt.Http.Client")

namespace Pt {

namespace Http {

Client::Client()
: _impl( new ClientImpl() )
{
    std::auto_ptr<ClientImpl> impl(_impl);
    
    init();
    
    impl.release();
}


Client::Client(const Net::Endpoint& ep)
: _impl( new ClientImpl() )
{
    std::auto_ptr<ClientImpl> impl(_impl);

    init();
    _impl->setHost(ep);

    impl.release();
}


Client::Client(System::EventLoop& loop)
: _impl( new ClientImpl() )
{
    std::auto_ptr<ClientImpl> impl(_impl);

    init();
    _impl->setActive(loop);

    impl.release();
}


Client::Client(System::EventLoop& loop, const Net::Endpoint& ep)
: _impl( new ClientImpl() )
{
    std::auto_ptr<ClientImpl> impl(_impl);

    init();
    _impl->setActive(loop);
    _impl->setHost(ep);

    impl.release();
}


void Client::init()
{
    _impl->request().outputSent() += Pt::slot(*this, &Client::onRequestSent);
    _impl->reply().inputReceived() += Pt::slot(*this, &Client::onReplyReceived);
}


Client::~Client()
{
    delete _impl;
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


void Client::setHost(const Net::Endpoint& ep)
{
    _impl->setHost(ep);
}


void Client::setHost(const Net::Endpoint& ep, const Net::TcpSocketOptions& opts)
{
    _impl->setHost(ep, opts);
}


const Net::Endpoint& Client::host() const
{
    return _impl->host();
}


void Client::send(bool finished)
{
    return _impl->send(finished);
}


std::istream& Client::receive()
{
    return _impl->receive();
}


void Client::beginSend(bool finished)
{
    _impl->beginSend(finished);
}


MessageProgress Client::endSend()
{
    return _impl->endSend();
}


void Client::beginReceive()
{
    _impl->beginReceive();
}


MessageProgress Client::endReceive()
{
    return _impl->endReceive();
}


Signal<Client&>& Client::requestSent()
{ 
    return _impl->requestSent(); 
}


Signal<Client&>& Client::replyReceived()
{ 
    return _impl->replyReceived(); 
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


void Client::cancel()
{
    _impl->cancel();
}


void Client::onRequestSent(Request& r)
{
    log_trace("onRequestSent: " << _impl->state());

    if(_impl->state() == ClientImpl::OnRequestEnd)
        _impl->replyReceived().send(*this);
    else
        _impl->requestSent().send(*this);
}


void Client::onReplyReceived(Reply& r)
{
    log_trace("onReplyReceived: " << _impl->state());

    _impl->replyReceived().send(*this);
}

} // namespace Http

} // namespace Pt
