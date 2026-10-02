/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/WebSocketServer.h>
#include <Pt/Http/WebSocketResponder.h>
#include <Pt/Http/Stream.h>

namespace Pt {

namespace Http {

WebSocketService::WebSocketService()
: _server(0)
, _maxSockets(1024)
, _idleTimeout(60000)
, _maxMessageSize(1024 * 1024)
{
}


WebSocketService::~WebSocketService()
{
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


std::size_t WebSocketService::sessionCount() const
{
    if( ! _server )
        return 0;

    return _server->size();
}


void WebSocketService::registerServer(WebSocketServer& server)
{
    _server = &server;
}


void WebSocketService::unregisterServer(WebSocketServer& server)
{
    if(_server == &server)
        _server = 0;
}


Responder* WebSocketService::onGetResponder(const Request&)
{
    return new WebSocketResponder(*this);
}


void WebSocketService::onReleaseResponder(Responder* responder)
{
    delete responder;
}


void WebSocketService::onUpgrade(Stream& stream)
{
    if( ! _server )
        return;

    _server->onUpgrade(stream);
}

} // namespace Http

} // namespace Pt
