/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketServlet.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/WebSocketSession.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Stream.h>
#include <Pt/System/EventLoop.h>

#include <algorithm>
#include <stdexcept>

namespace Pt {

namespace Http {

WebSocketServlet::WebSocketServlet(WebSocketService& service)
: _service(&service)
{
    _service->registerServlet(*this);
}


WebSocketServlet::~WebSocketServlet()
{
    if(_service)
        _service->unregisterServlet(*this);

    while( ! _sessions.empty() )
    {
        WebSocketSession* session = _sessions.back();
        _sessions.pop_back();
        session->shutdown();
        releaseSession(session);
    }
}


void WebSocketServlet::onUpgrade(Stream& stream,
                                 const Request& request,
                                 const Reply& reply)
{
    System::EventLoop* loop = stream.loop();
    if( ! loop )
        throw std::logic_error("WebSocket upgrade has no event loop");

    const char* selected = reply.header().get("Sec-WebSocket-Protocol");
    stream.setSelectedProtocol(selected ? selected : "");

    WebSocketSession* session = _service->onGetSession(*this, *loop, stream, request);
    if( ! session )
        return;

    _sessions.push_back(session);
}


void WebSocketServlet::onSessionClosed(WebSocketSession& session)
{
    std::vector<WebSocketSession*>::iterator it =
        std::find(_sessions.begin(), _sessions.end(), &session);

    if(it == _sessions.end())
        return;

    _sessions.erase(it);
    releaseSession(&session);
}


void WebSocketServlet::releaseSession(WebSocketSession* session)
{
    if( ! session || ! _service )
        return;

    _service->onReleaseSession(session);
}

} // namespace Http

} // namespace Pt
