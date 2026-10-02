/*
 * Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <Pt/Http/WebSocketService.h>
#include <algorithm>

namespace Pt {
namespace Http {

WebSocketService::WebSocketService()
: _maxSockets(1024)
, _idleTimeout(60000)
, _maxMessageSize(1024 * 1024)
{
}


WebSocketService::~WebSocketService()
{
    while( ! _sockets.empty() )
    {
        WebSocket* socket = _sockets.back();
        _sockets.pop_back();
        delete socket;
    }
}


Signal<WebSocket&>& WebSocketService::accepted()
{
    return _accepted;
}


std::size_t WebSocketService::size() const
{
    return _sockets.size();
}


std::size_t WebSocketService::maxSockets() const
{
    return _maxSockets;
}


void WebSocketService::setMaxSockets(std::size_t n)
{
    _maxSockets = n;
}


std::size_t WebSocketService::idleTimeout() const
{
    return _idleTimeout;
}


void WebSocketService::setIdleTimeout(std::size_t ms)
{
    _idleTimeout = ms;
}


std::size_t WebSocketService::maxMessageSize() const
{
    return _maxMessageSize;
}


void WebSocketService::setMaxMessageSize(std::size_t n)
{
    _maxMessageSize = n;
}

Responder* WebSocketService::onGetResponder(const Request&)
{
    return new WebSocketResponder(*this);
}

void WebSocketService::onReleaseResponder(Responder* r)
{
    delete r;
}


void WebSocketService::onUpgrade(Stream& stream)
{
    WebSocket* socket = new WebSocket(stream);
    socket->setMaxMessageSize(_maxMessageSize);
    socket->setIdleTimeout(_idleTimeout);
    socket->closed() += Pt::slot(*this, &WebSocketService::onClosed);

    _sockets.push_back(socket);
    _accepted.send(*socket);
}


void WebSocketService::onClosed(WebSocket& socket)
{
    std::vector<WebSocket*>::iterator it =
        std::find(_sockets.begin(), _sockets.end(), &socket);

    if(it != _sockets.end())
        _sockets.erase(it);
}

}}
