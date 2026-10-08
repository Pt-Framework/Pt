/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketServlet.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/WebSocketSession.h>
#include <Pt/Http/Stream.h>
#include <Pt/System/EventLoop.h>

#include <algorithm>
#include <stdexcept>

namespace Pt {

namespace Http {

WebSocketServlet::WebSocketServlet(WebSocketService& service)
: _service(&service)
{
    _service->attach(*this);
}


WebSocketServlet::~WebSocketServlet()
{
    if(_service)
        _service->detach(*this);

    while( ! _sessions.empty() )
    {
        WebSocketSession* session = _sessions.back();
        _sessions.pop_back();
        session->shutdown();
        if(_service)
            _service->releaseSession(session);
    }
}


void WebSocketServlet::accept(Stream& stream,
                              const Request& request,
                              const Reply& reply)
{
    System::EventLoop* loop = stream.loop();
    if( ! loop )
        throw std::logic_error("WebSocket upgrade has no event loop");

    WebSocketSession* session = _service->getSession(*loop, stream, request, reply);
    if(session)
        _sessions.push_back(session);
}


void WebSocketServlet::close(WebSocketSession& session)
{
    std::vector<WebSocketSession*>::iterator it =
        std::find(_sessions.begin(), _sessions.end(), &session);

    if(it == _sessions.end())
        return;

    _sessions.erase(it);
    _service->releaseSession(&session);
}

} // namespace Http

} // namespace Pt
