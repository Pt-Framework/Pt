/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/WebSocketServlet.h>
#include <Pt/Http/WebSocketResponder.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Stream.h>

#include <algorithm>
#include <stdexcept>

namespace Pt {

namespace Http {

WebSocketService::WebSocketService()
: _servlet(0)
, _maxSockets(1024)
, _idleTimeout(60000)
, _maxMessageSize(1024 * 1024)
{
}


WebSocketService::~WebSocketService()
{
}


void WebSocketService::activate(WebSocketServlet& servlet)
{
    if(_servlet)
        throw std::logic_error("WebSocket service scope is already active");

    _servlet = &servlet;
}


void WebSocketService::deactivate()
{
    _servlet = 0;

    while( ! _sessions.empty() )
    {
        WebSocketSession* session = _sessions.back();
        _sessions.pop_back();
        session->shutdown();
        onReleaseSession(session);
    }
}


void WebSocketService::onSessionClosed(WebSocketSession& session)
{
    std::vector<WebSocketSession*>::iterator it =
        std::find(_sessions.begin(), _sessions.end(), &session);

    if(it == _sessions.end())
        return;

    _sessions.erase(it);
    onReleaseSession(&session);
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


void WebSocketService::addProtocol(const std::string& name)
{
    if(name.empty())
        throw std::invalid_argument("WebSocket protocol name is empty");

    for(std::size_t i = 0; i < name.size(); ++i)
    {
        const unsigned char ch = static_cast<unsigned char>(name[i]);
        if(ch <= 32 || ch == 127 || ch == ',' || ch == '(' || ch == ')'
           || ch == '<' || ch == '>' || ch == '@' || ch == ';'
           || ch == ':' || ch == '\\' || ch == '"' || ch == '/'
           || ch == '[' || ch == ']' || ch == '?' || ch == '='
           || ch == '{' || ch == '}')
        {
            throw std::invalid_argument("WebSocket protocol name is not a token");
        }
    }

    _protocols.push_back(name);
}


void WebSocketService::clearProtocols()
{
    _protocols.clear();
}


Responder* WebSocketService::onGetResponder(const Request&)
{
    return new WebSocketResponder(*this);
}


void WebSocketService::onReleaseResponder(Responder* responder)
{
    delete responder;
}


void WebSocketService::onUpgrade(Stream& stream,
                                 const Request& request,
                                 const Reply& reply)
{
    // NOTE: only create session if servlet is active
    if( ! _servlet )
        return;

    System::EventLoop* loop = stream.loop();
    if( ! loop )
        throw std::logic_error("WebSocket upgrade has no event loop");

    WebSocketSession* session = onGetSession(*loop, stream, request, reply);
    if(session)
        _sessions.push_back(session);
}

} // namespace Http

} // namespace Pt
