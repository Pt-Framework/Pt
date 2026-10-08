/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketSession.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Stream.h>
#include <Pt/System/EventLoop.h>
#include "WebSocketChannel.h"

#include <stdexcept>

namespace Pt {

namespace Http {

namespace {

bool isOws(char ch)
{
    return ch == ' ' || ch == '\t';
}


bool isProtocolToken(const std::string& name)
{
    if(name.empty())
        return false;

    for(std::size_t i = 0; i < name.size(); ++i)
    {
        const unsigned char ch = static_cast<unsigned char>(name[i]);
        if(ch <= 32 || ch == 127 || ch == ',' || ch == '(' || ch == ')'
           || ch == '<' || ch == '>' || ch == '@' || ch == ';'
           || ch == ':' || ch == '\\' || ch == '"' || ch == '/'
           || ch == '[' || ch == ']' || ch == '?' || ch == '='
           || ch == '{' || ch == '}')
        {
            return false;
        }
    }

    return true;
}


std::string selectedProtocol(const Reply& reply)
{
    const char* field = reply.header().get("Sec-WebSocket-Protocol");
    if( ! field || ! *field )
        return std::string();

    const std::string text(field);
    std::size_t left = 0;
    std::size_t right = text.size();
    while(left < right && isOws(text[left]))
        ++left;
    while(right > left && isOws(text[right - 1]))
        --right;

    const std::string name = text.substr(left, right - left);
    if(name.empty())
        return std::string();

    if(name.find(',') != std::string::npos || ! isProtocolToken(name))
        throw std::runtime_error("WebSocket protocol is not one token");

    return name;
}

}


WebSocketSession::WebSocketSession(WebSocketService& service,
                                   System::EventLoop& loop,
                                   Stream& stream,
                                   const Reply& reply)
: _service(&service)
, _loop(&loop)
, _channel(0)
, _protocol(selectedProtocol(reply))
, _ended(false)
{
    if( stream.loop() != &loop )
        throw std::logic_error("WebSocketSession loop is not the stream loop");

    _channel = new WebSocketChannel();
    _channel->open(stream, false);
    _channel->setMaxMessageSize(_service->maxMessageSize());
    _channel->setIdleTimeout(_service->idleTimeout());

    _channel->inputReady() += Pt::slot(*this, &WebSocketSession::onInputReady);
    _channel->outputReady() += Pt::slot(*this, &WebSocketSession::onOutputReady);
    _channel->closed() += Pt::slot(*this, &WebSocketSession::onClosed);
}


WebSocketSession::~WebSocketSession()
{
    _ended = true;
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
    if(_ended)
        return;

    _ended = true;

    onClose();

    _service->close(*this);
}

} // namespace Http

} // namespace Pt
