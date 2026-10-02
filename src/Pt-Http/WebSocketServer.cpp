/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketServer.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/WebSocketSession.h>
#include <Pt/Http/Stream.h>
#include <Pt/System/EventLoop.h>

#include <algorithm>
#include <stdexcept>

namespace Pt {

namespace Http {

WebSocketServer::WebSocketServer(WebSocketService& service)
: _service(&service)
{
    _service->registerServer(*this);
}


WebSocketServer::~WebSocketServer()
{
    if(_service)
        _service->unregisterServer(*this);

    while( ! _sessions.empty() )
    {
        WebSocketSession* session = _sessions.back();
        _sessions.pop_back();
        releaseSession(session);
    }
}


void WebSocketServer::onUpgrade(Stream& stream)
{
    System::EventLoop* loop = stream.loop();
    if( ! loop )
        throw std::logic_error("WebSocket upgrade has no event loop");

    WebSocketSession* session = _service->onGetSession(*this, *loop, stream);
    if( ! session )
        return;

    _sessions.push_back(session);
}


void WebSocketServer::onSessionClosed(WebSocketSession& session)
{
    std::vector<WebSocketSession*>::iterator it =
        std::find(_sessions.begin(), _sessions.end(), &session);

    if(it == _sessions.end())
        return;

    _sessions.erase(it);
    releaseSession(&session);
}


void WebSocketServer::releaseSession(WebSocketSession* session)
{
    if( ! session || ! _service )
        return;

    _service->onReleaseSession(session);
}

} // namespace Http

} // namespace Pt
