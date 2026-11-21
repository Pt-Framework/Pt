/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketServlet.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/WebSocketSession.h>
#include <Pt/Http/Stream.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Signal.h>
#include <Pt/Slot.h>

#include <algorithm>

namespace Pt {

namespace Http {

WebSocketServlet::WebSocketServlet(WebSocketService& service)
: _service(&service)
, _loop(0)
{
    _service->attach(*this);
}


WebSocketServlet::~WebSocketServlet()
{
    if(_loop)
    {
        _loop->eventReceived() -= Pt::slot(*this, &WebSocketServlet::onSessionsClosed);
        _loop = 0;
    }

    while( ! _closing.empty() )
    {
        WebSocketSession* session = _closing.back();
        _closing.pop_back();
        _service->releaseSession(session);
    }

    while( ! _sessions.empty() )
    {
        WebSocketSession* session = _sessions.back();
        _sessions.pop_back();
        _service->releaseSession(session);
    }

    if(_service)
        _service->detach(*this);
}


void WebSocketServlet::accept(Stream& stream,
                              const Request& request,
                              const Reply& reply)
{
    WebSocketSession* session = _service->getSession(stream, request, reply);
    if( ! session )
        return;

    if( ! session->isOpen() )
    {
        _service->releaseSession(session);
        return;
    }

    if( ! _loop )
    {
        _loop = stream.loop();
        if(_loop)
            _loop->eventReceived() += Pt::slot(*this, &WebSocketServlet::onSessionsClosed);
    }

    _sessions.push_back(session);
}


void WebSocketServlet::onSessionClosed(WebSocketSession& session)
{
    std::vector<WebSocketSession*>::iterator it =
        std::find(_sessions.begin(), _sessions.end(), &session);

    if(it == _sessions.end())
        return;

    _sessions.erase(it);
    _closing.push_back(&session);

    if(_loop)
        _loop->commitEvent( SessionsClosedEvent() );
}


void WebSocketServlet::onSessionsClosed(const SessionsClosedEvent&)
{
    while( ! _closing.empty() )
    {
        WebSocketSession* session = _closing.back();
        _closing.pop_back();
        _service->releaseSession(session);
    }
}

} // namespace Http

} // namespace Pt
