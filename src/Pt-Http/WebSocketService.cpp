/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/WebSocketServlet.h>
#include "WebSocketResponder.h"
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Stream.h>
#include <Pt/Http/Message.h>

#include <stdexcept>
#include <string>

namespace Pt {

namespace Http {

namespace {

bool equalsIgnoreCase(const char* value, const std::string& origin)
{
    const char* p = value;
    for(std::size_t i = 0; i < origin.size(); ++i, ++p)
    {
        if(*p == '\0')
            return false;

        unsigned char left = static_cast<unsigned char>(*p);
        unsigned char right = static_cast<unsigned char>(origin[i]);
        if(left >= 'a' && left <= 'z')
            left = static_cast<unsigned char>(left - 'a' + 'A');
        if(right >= 'a' && right <= 'z')
            right = static_cast<unsigned char>(right - 'a' + 'A');
        if(left != right)
            return false;
    }

    return *p == '\0';
}

}


WebSocketService::WebSocketService()
: _servlet(0)
, _socketCount(0)
, _pending(0)
, _maxSockets(1024)
, _idleTimeout(0)
, _pingInterval(0)
, _pingTimeout(0)
, _closeTimeout(10000)
, _maxMessageSize(1024 * 1024)
{
}


WebSocketService::~WebSocketService()
{
}


void WebSocketService::attach(WebSocketServlet& servlet)
{
    System::MutexLock lock(_mutex);

    if(_servlet)
        throw std::logic_error("WebSocket service already has a servlet");

    _servlet = &servlet;
}


void WebSocketService::detach(WebSocketServlet& servlet)
{
    System::MutexLock lock(_mutex);

    if(_servlet == &servlet)
        _servlet = 0;
}


bool WebSocketService::hasServlet() const
{
    return servlet() != 0;
}


WebSocketServlet* WebSocketService::servlet() const
{
    System::MutexLock lock(_mutex);
    return _servlet;
}


std::size_t WebSocketService::maxSockets() const
{
    System::MutexLock lock(_mutex);
    return _maxSockets;
}


void WebSocketService::setMaxSockets(std::size_t n)
{
    System::MutexLock lock(_mutex);
    _maxSockets = n;
}


std::size_t WebSocketService::socketCount() const
{
    System::MutexLock lock(_mutex);
    return _socketCount;
}


std::size_t WebSocketService::idleTimeout() const
{
    System::MutexLock lock(_mutex);
    return _idleTimeout;
}


void WebSocketService::setIdleTimeout(std::size_t ms)
{
    System::MutexLock lock(_mutex);
    _idleTimeout = ms;
}


std::size_t WebSocketService::pingInterval() const
{
    System::MutexLock lock(_mutex);
    return _pingInterval;
}


void WebSocketService::setPingInterval(std::size_t ms)
{
    System::MutexLock lock(_mutex);
    _pingInterval = ms;
}


std::size_t WebSocketService::pingTimeout() const
{
    System::MutexLock lock(_mutex);
    return _pingTimeout;
}


void WebSocketService::setPingTimeout(std::size_t ms)
{
    System::MutexLock lock(_mutex);
    _pingTimeout = ms;
}


std::size_t WebSocketService::closeTimeout() const
{
    System::MutexLock lock(_mutex);
    return _closeTimeout;
}


void WebSocketService::setCloseTimeout(std::size_t ms)
{
    System::MutexLock lock(_mutex);
    _closeTimeout = ms;
}


std::size_t WebSocketService::maxMessageSize() const
{
    System::MutexLock lock(_mutex);
    return _maxMessageSize;
}


void WebSocketService::setMaxMessageSize(std::size_t n)
{
    System::MutexLock lock(_mutex);
    _maxMessageSize = n;
}


void WebSocketService::addProtocol(const std::string& name)
{
    if( ! MessageHeader::isToken(name.c_str()) )
        throw std::invalid_argument("WebSocket protocol name is not a token");

    System::MutexLock lock(_mutex);
    _protocols.push_back(name);
}


void WebSocketService::clearProtocols()
{
    System::MutexLock lock(_mutex);
    _protocols.clear();
}


std::vector<std::string> WebSocketService::protocols() const
{
    System::MutexLock lock(_mutex);
    return _protocols;
}


void WebSocketService::addOrigin(const std::string& origin)
{
    if( origin.empty() )
        throw std::invalid_argument("WebSocket origin is empty");

    System::MutexLock lock(_mutex);
    _origins.push_back(origin);
}


void WebSocketService::clearOrigins()
{
    System::MutexLock lock(_mutex);
    _origins.clear();
}


std::vector<std::string> WebSocketService::origins() const
{
    System::MutexLock lock(_mutex);
    return _origins;
}


bool WebSocketService::selectProtocol(const MessageHeader& header, std::string& selected) const
{
    System::MutexLock lock(_mutex);

    if( _protocols.empty() )
        return true;

    const MessageHeader::FieldIterator offered = header.field("Sec-WebSocket-Protocol");
    if(offered == header.end())
        return false;

    for(MessageHeader::ValueIterator it = offered->begin(); it != offered->end(); ++it)
    {
        for(std::size_t n = 0; n < _protocols.size(); ++n)
        {
            const std::string& protocol = _protocols[n];
            if(protocol.size() == it->size() && protocol.compare(0, it->size(), it->data(), it->size()) == 0)
            {
                selected = protocol;
                return true;
            }
        }
    }

    return false;
}


bool WebSocketService::acceptsOrigin(const char* origin) const
{
    System::MutexLock lock(_mutex);
    if( _origins.empty() )
        return true;

    if( ! origin )
        return false;

    for(std::size_t n = 0; n < _origins.size(); ++n)
    {
        if( equalsIgnoreCase(origin, _origins[n]) )
            return true;
    }

    return false;
}


bool WebSocketService::reservePending()
{
    System::MutexLock lock(_mutex);

    if( ! _servlet )
        return false;

    if(_maxSockets != 0 && _socketCount + _pending >= _maxSockets)
        return false;

    ++_pending;
    return true;
}


void WebSocketService::releasePending()
{
    System::MutexLock lock(_mutex);

    if(_pending > 0)
        --_pending;
}


WebSocketSession* WebSocketService::getSession(Stream& stream,
                                               const Request& request,
                                               const Reply& reply)
{
    WebSocketSession* session = onGetSession(stream, request, reply);
    if(session)
    {
        System::MutexLock lock(_mutex);
        ++_socketCount;
    }

    return session;
}


void WebSocketService::releaseSession(WebSocketSession* session)
{
    if( ! session )
        return;

    onReleaseSession(session);

    System::MutexLock lock(_mutex);
    if(_socketCount > 0)
        --_socketCount;
}


void WebSocketService::onSessionClosed(WebSocketSession& session)
{
    WebSocketServlet* servlet = this->servlet();
    if( ! servlet )
        return;

    servlet->onSessionClosed(session);
}


Responder* WebSocketService::onGetResponder(const Request&)
{
    return new WebSocketResponder(*this);
}


void WebSocketService::onReleaseResponder(Responder* responder)
{
    delete responder;
}


bool WebSocketService::onUpgrade(Stream& stream,
                                 const Request& request,
                                 const Reply& reply)
{
    WebSocketServlet* servlet = this->servlet();
    if( ! servlet )
        return false;

    return servlet->accept(stream, request, reply);
}

} // namespace Http

} // namespace Pt
