/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketSession.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/WebSocketServlet.h>
#include <Pt/Http/Stream.h>
#include <Pt/System/EventLoop.h>
#include "WebSocketChannel.h"

#include <stdexcept>

namespace Pt {

namespace Http {

WebSocketSession::WebSocketSession(WebSocketServlet& servlet,
                                   System::EventLoop& loop,
                                   Stream& stream)
: _service(&servlet.service())
, _servlet(&servlet)
, _loop(&loop)
, _channel(new WebSocketChannel())
{
    if( stream.loop() != &loop )
        throw std::logic_error("WebSocketSession loop is not the stream loop");

    _channel->open(stream, false);
    _channel->setMaxMessageSize(_service->maxMessageSize());
    _channel->setIdleTimeout(_service->idleTimeout());

    _channel->inputReady() += Pt::slot(*this, &WebSocketSession::onInputReady);
    _channel->outputReady() += Pt::slot(*this, &WebSocketSession::onOutputReady);
    _channel->closed() += Pt::slot(*this, &WebSocketSession::onClosed);
}


WebSocketSession::~WebSocketSession()
{
    _servlet = 0;
    delete _channel;
}


WebSocketMessage& WebSocketSession::incoming()
{
    return _channel->incoming();
}


WebSocketMessage& WebSocketSession::outgoing()
{
    return _channel->outgoing();
}


void WebSocketSession::beginSend()
{
    _channel->beginSend();
}


MessageProgress WebSocketSession::endSend()
{
    return _channel->endSend();
}


void WebSocketSession::beginReceive()
{
    _channel->beginReceive();
}


MessageProgress WebSocketSession::endReceive()
{
    return _channel->endReceive();
}


void WebSocketSession::ping(const char* payload, std::size_t n)
{
    _channel->ping(payload, n);
}


void WebSocketSession::close(unsigned code, const std::string& reason)
{
    _channel->close(code, reason);
}


unsigned WebSocketSession::closeCode() const
{
    return _channel->closeCode();
}


const std::string& WebSocketSession::closeReason() const
{
    return _channel->closeReason();
}


void WebSocketSession::shutdown()
{
    _channel->detach();
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
