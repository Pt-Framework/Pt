/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketSession.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/WebSocketServlet.h>
#include <Pt/Http/Stream.h>
#include <Pt/System/EventLoop.h>
#include "WebSocketConnection.h"

#include <stdexcept>

namespace Pt {

namespace Http {

WebSocketSession::WebSocketSession(WebSocketServlet& servlet,
                                   System::EventLoop& loop,
                                   Stream& stream)
: _service(&servlet.service())
, _servlet(&servlet)
, _loop(&loop)
, _connection(new WebSocketConnection())
{
    if( stream.loop() != &loop )
        throw std::logic_error("WebSocketSession loop is not the stream loop");

    _connection->open(stream, false);
    _connection->setMaxMessageSize(_service->maxMessageSize());
    _connection->setIdleTimeout(_service->idleTimeout());

    _connection->inputReady() += Pt::slot(*this, &WebSocketSession::onInputReady);
    _connection->outputReady() += Pt::slot(*this, &WebSocketSession::onOutputReady);
    _connection->closed() += Pt::slot(*this, &WebSocketSession::onClosed);
}


WebSocketSession::~WebSocketSession()
{
    _servlet = 0;
    delete _connection;
}


WebSocketMessage& WebSocketSession::incoming()
{
    return _connection->incoming();
}


WebSocketMessage& WebSocketSession::outgoing()
{
    return _connection->outgoing();
}


void WebSocketSession::beginSend()
{
    _connection->beginSend();
}


MessageProgress WebSocketSession::endSend()
{
    return _connection->endSend();
}


void WebSocketSession::beginReceive()
{
    _connection->beginReceive();
}


MessageProgress WebSocketSession::endReceive()
{
    return _connection->endReceive();
}


void WebSocketSession::ping(const char* payload, std::size_t n)
{
    _connection->ping(payload, n);
}


void WebSocketSession::close(unsigned code, const std::string& reason)
{
    _connection->close(code, reason);
}


unsigned WebSocketSession::closeCode() const
{
    return _connection->closeCode();
}


const std::string& WebSocketSession::closeReason() const
{
    return _connection->closeReason();
}


void WebSocketSession::shutdown()
{
    _connection->detach();
}


void WebSocketSession::onInputReady()
{
    onInput();
}


void WebSocketSession::onOutputReady()
{
    onOutput();
}


void WebSocketSession::onClosed()
{
    if( ! _servlet )
        return;

    WebSocketServlet* servlet = _servlet;
    _servlet = 0;
    onClose();
    servlet->onSessionClosed(*this);
}

} // namespace Http

} // namespace Pt
