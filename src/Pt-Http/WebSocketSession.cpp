/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketSession.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/WebSocketServer.h>
#include <Pt/Http/Stream.h>
#include <Pt/System/EventLoop.h>

#include <stdexcept>

namespace Pt {

namespace Http {

WebSocketSession::WebSocketSession(WebSocketServer& server,
                                   System::EventLoop& loop,
                                   Stream& stream)
: _service(&server.service())
, _server(&server)
, _loop(&loop)
, _socket()
{
    if( stream.loop() != &loop )
        throw std::logic_error("WebSocketSession loop is not the stream loop");

    _socket.accept(stream);

    _socket.setMaxMessageSize(_service->maxMessageSize());
    _socket.setIdleTimeout(_service->idleTimeout());

    _socket.inputReady() += Pt::slot(*this, &WebSocketSession::onSocketInput);
    _socket.outputReady() += Pt::slot(*this, &WebSocketSession::onSocketOutput);
    _socket.closed() += Pt::slot(*this, &WebSocketSession::onSocketClosed);
}


WebSocketSession::~WebSocketSession()
{
    _socket.close();
}


void WebSocketSession::onSocketInput(WebSocket&)
{
    onInput();
}


void WebSocketSession::onSocketOutput(WebSocket&)
{
    onOutput();
}


void WebSocketSession::onSocketClosed(WebSocket&)
{
    onClose();

    if(_server)
        _server->onSessionClosed(*this);
}

} // namespace Http

} // namespace Pt
