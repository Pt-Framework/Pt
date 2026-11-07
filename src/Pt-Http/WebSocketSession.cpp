/* Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketSession.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/HttpError.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Stream.h>
#include <Pt/System/IOError.h>
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
        throw HttpError("WebSocket protocol is not one token");

    return name;
}

}


WebSocketSession::WebSocketSession(WebSocketService& service,
                                   Stream& stream,
                                   const Reply& reply)
: _service(&service)
, _channel(0)
, _protocol(selectedProtocol(reply))
, _inClosed(false)
{
    _channel = new WebSocketChannel();
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
    _channel->closed() += Pt::slot(*this, &WebSocketSession::onClosed);
    _channel->pong() += Pt::slot(*this, &WebSocketSession::onPong);
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
    if(_inClosed)
        return;

    _channel->close();
    _service->onSessionClosed(*this);
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


void WebSocketSession::onClosed()
{
    _inClosed = true;
    _closed.send(*this);
    _inClosed = false;

    _service->onSessionClosed(*this);
}


void WebSocketSession::onPong(const std::string& payload)
{
    try
    {
        _pong.send(*this, payload);
    }
    catch(const System::IOError&)
    {
        close();
    }
}

} // namespace Http

} // namespace Pt
