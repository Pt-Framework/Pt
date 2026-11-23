/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketSession.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/HttpError.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Stream.h>
#include <Pt/System/IOError.h>
#include <Pt/SmartPtr.h>
#include "WebSocketChannel.h"
#include <Pt/Http/Message.h>

#include <stdexcept>

namespace Pt {

namespace Http {

namespace {

std::string selectedProtocol(const Reply& reply)
{
    const MessageHeader::FieldIterator field =
        reply.header().field("Sec-WebSocket-Protocol");
    if(field == reply.header().end() || field->empty())
        return std::string();

    MessageHeader::ValueIterator it = field->begin();
    const MessageHeader::Value protocol = *it;
    ++it;
    if(it != field->end())
        throw HttpError("WebSocket protocol is not one value");

    return std::string(protocol.data(), protocol.size());
}

}


WebSocketSession::WebSocketSession(WebSocketService& service,
                                   Stream& stream,
                                   const Reply& reply)
: _service(&service)
, _channel(0)
, _protocol(selectedProtocol(reply))
{
    Pt::AutoPtr guard( new WebSocketChannel() );

    _channel = guard.get();
    _channel->open(stream, false);
    _channel->setMaxMessageSize(_service->maxMessageSize());
    _channel->setIdleTimeout(_service->idleTimeout());
    _channel->setPingInterval(_service->pingInterval());
    _channel->setPingTimeout(_service->pingTimeout());
    _channel->setCloseTimeout(_service->closeTimeout());

    _channel->inputReady() += Pt::slot(*this, &WebSocketSession::onInputReady);
    _channel->outputReady() += Pt::slot(*this, &WebSocketSession::onOutputReady);
    _channel->pingReady() += Pt::slot(*this, &WebSocketSession::onPingReady);
    _channel->shutdownReady() += Pt::slot(*this, &WebSocketSession::onShutdownReady);
    _channel->peerClosed() += Pt::slot(*this, &WebSocketSession::onPeerClosed);
    _channel->released() += Pt::slot(*this, &WebSocketSession::onReleased);
    _channel->pong() += Pt::slot(*this, &WebSocketSession::onPong);

    guard.release();
}


WebSocketSession::~WebSocketSession()
{
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


void WebSocketSession::beginSend(bool finished)
{
    _channel->beginSend(finished);
}


WebSocketProgress WebSocketSession::endSend()
{
    return _channel->endSend();
}


void WebSocketSession::beginReceive()
{
    _channel->beginReceive();
}


WebSocketProgress WebSocketSession::endReceive()
{
    return _channel->endReceive();
}


void WebSocketSession::beginPing(const char* payload, std::size_t n)
{
    _channel->beginPing(payload, n);
}


void WebSocketSession::endPing()
{
    _channel->endPing();
}


void WebSocketSession::beginShutdown(unsigned code, const std::string& reason)
{
    _channel->beginShutdown(code, reason);
}


void WebSocketSession::endShutdown()
{
    _channel->endShutdown();
}


void WebSocketSession::close()
{
    _channel->close();
}


unsigned WebSocketSession::closeCode() const
{
    return _channel->closeCode();
}


const std::string& WebSocketSession::closeReason() const
{
    return _channel->closeReason();
}


bool WebSocketSession::isOpen() const
{
    return _channel && _channel->isOpen();
}


void WebSocketSession::onInputReady()
{
    try
    {
        _inputReady.send(*this);
    }
    catch(const System::IOError&)
    {
        close();
    }
}


void WebSocketSession::onOutputReady()
{
    try
    {
        _outputReady.send(*this);
    }
    catch(const System::IOError&)
    {
        close();
    }
}


void WebSocketSession::onPingReady()
{
    try
    {
        _pingReady.send(*this);
    }
    catch(const System::IOError&)
    {
        close();
    }
}


void WebSocketSession::onShutdownReady()
{
    try
    {
        _shutdownReady.send(*this);
    }
    catch(const System::IOError&)
    {}

    close();
}


void WebSocketSession::onPeerClosed()
{
    try
    {
        _peerClosed.send(*this);
    }
    catch(...)
    {
        close();
        throw;
    }

    close();
}


void WebSocketSession::onReleased()
{
    try
    {
        _released.send(*this);
    }
    catch(...)
    {
        _service->onSessionClosed(*this);
        throw;
    }

    _service->onSessionClosed(*this);
}


void WebSocketSession::onPong(const std::string& payload)
{
    _pong.send(*this, payload);
}

} // namespace Http

} // namespace Pt
