/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketServlet.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/WebSocketSession.h>
#include <Pt/Http/Stream.h>
#include <Pt/Signal.h>
#include <Pt/Slot.h>

#include <algorithm>
#include <stdexcept>

namespace Pt {

namespace Http {

WebSocketServlet::WebSocketServlet(WebSocketService& service)
: _service(&service)
{
    _release.ready() += Pt::slot(*this, &WebSocketServlet::onSessionsClosed);
    _service->attach(*this);
}


WebSocketServlet::~WebSocketServlet()
{
    detach();

    if(_service)
        _service->detach(*this);
}


System::EventLoop* WebSocketServlet::loop() const
{
    return _release.loop();
}


void WebSocketServlet::detach()
{
    _release.detach();

    while( ! _sessions.empty() )
    {
        WebSocketSession* session = _sessions.back();
        _sessions.pop_back();
        _service->releaseSession(session);
    }

    while( ! _closing.empty() )
    {
        WebSocketSession* session = _closing.back();
        _closing.pop_back();
        _service->releaseSession(session);
    }
}


bool WebSocketServlet::accept(Stream& stream,
                              const Request& request,
                              const Reply& reply)
{
    System::EventLoop* loop = stream.loop();
    if( ! loop )
        throw std::logic_error("No event loop");

    if( ! _release.loop() )
    {
        _release.setActive(*loop);
    }
    else if( _release.loop() != loop )
    {
        throw std::logic_error("Invalid event loop");
    }

    WebSocketSession* session = _service->getSession(stream, request, reply);
    if( ! session )
        return false;

    if( ! session->isOpen() )
    {
        _service->releaseSession(session);
        return false;
    }

    _sessions.push_back(session);
    return true;
}


void WebSocketServlet::onSessionClosed(WebSocketSession& session)
{
    std::vector<WebSocketSession*>::iterator it =
        std::find(_sessions.begin(), _sessions.end(), &session);

    if(it == _sessions.end())
        return;

    _closing.reserve(_closing.size() + 1);
    _closing.push_back(&session);

    _sessions.erase(it);

    // the loop is gone when it was destroyed before this servlet
    if( _release.loop() )
        _release.run();
}


void WebSocketServlet::onSessionsClosed()
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
