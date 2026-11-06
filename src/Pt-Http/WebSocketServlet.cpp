/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketServlet.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/WebSocketSession.h>
#include <Pt/Http/Stream.h>

#include <algorithm>

namespace Pt {

namespace Http {

WebSocketServlet::WebSocketServlet(WebSocketService& service)
: _service(&service)
{
    _service->attach(*this);
}


WebSocketServlet::~WebSocketServlet()
{
    while( ! _sessions.empty() )
        onSessionClosed( *_sessions.back() );

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

    _sessions.push_back(session);
}


void WebSocketServlet::onSessionClosed(WebSocketSession& session)
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
