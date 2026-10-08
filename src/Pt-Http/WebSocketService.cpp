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

#include <stdexcept>

namespace Pt {

namespace Http {

WebSocketService::WebSocketService()
: _servlet(0)
, _socketCount(0)
, _maxSockets(1024)
, _idleTimeout(60000)
, _maxMessageSize(1024 * 1024)
{
}


WebSocketService::~WebSocketService()
{
}


void WebSocketService::attach(WebSocketServlet& servlet)
{
    if(_servlet)
        throw std::logic_error("WebSocket service already has a servlet");

    _servlet = &servlet;
}


void WebSocketService::detach(WebSocketServlet& servlet)
{
    if(_servlet == &servlet)
        _servlet = 0;
}


WebSocketSession* WebSocketService::getSession(Stream& stream,
                                               const Request& request,
                                               const Reply& reply)
{
    WebSocketSession* session = onGetSession(stream, request, reply);
    if(session)
        ++_socketCount;

    return session;
}


void WebSocketService::releaseSession(WebSocketSession* session)
{
    if( ! session )
        return;

    onReleaseSession(session);

    if(_socketCount > 0)
        --_socketCount;
}


void WebSocketService::close(WebSocketSession& session)
{
    if( ! _servlet )
        return;

    _servlet->close(session);
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
    if( ! _servlet )
        return;

    _servlet->accept(stream, request, reply);
}

} // namespace Http

} // namespace Pt
