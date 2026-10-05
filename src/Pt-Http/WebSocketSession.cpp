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

namespace {

WebSocket::Frame toFrame(WebSocketConnection::Frame frame)
{
    return static_cast<WebSocket::Frame>(frame);
}


WebSocketConnection::Frame toConnectionFrame(WebSocket::Frame frame)
{
    return static_cast<WebSocketConnection::Frame>(frame);
}

} // namespace


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


std::iostream& WebSocketSession::body()
{
    return _connection->body();
}


std::size_t WebSocketSession::available() const
{
    return _connection->available();
}


std::size_t WebSocketSession::pending() const
{
    return _connection->pending();
}


void WebSocketSession::discard()
{
    _connection->discard();
}


WebSocket::Frame WebSocketSession::frame() const
{
    return toFrame( _connection->frame() );
}


void WebSocketSession::beginSend(WebSocket::Frame frame)
{
    _connection->beginSend( toConnectionFrame(frame) );
}


void WebSocketSession::endSend()
{
    _connection->endSend();
}


void WebSocketSession::beginReceive()
{
    _connection->beginReceive();
}


void WebSocketSession::endReceive()
{
    _connection->endReceive();
}


void WebSocketSession::sendPing()
{
    _connection->sendPing();
}


void WebSocketSession::sendPong()
{
    _connection->sendPong();
}


void WebSocketSession::close()
{
    _connection->close();
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
