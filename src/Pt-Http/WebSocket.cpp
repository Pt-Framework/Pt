/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocket.h>
#include <Pt/Http/Stream.h>
#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Message.h>
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <Pt/System/Uri.h>
#include "WebSocketChannel.h"
#include <sstream>
#include <ctime>
#include <stdexcept>
#include <vector>

namespace Pt {

namespace Http {

namespace {

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


bool isOws(char ch)
{
    return ch == ' ' || ch == '\t';
}

} // namespace

WebSocket::WebSocket(Client& client)
: _client(&client)
, _channel(new WebSocketChannel())
, _path("/")
, _error(false)
, _connecting(false)
{
}


WebSocket::~WebSocket()
{
    if(_client && _connecting)
    {
        _client->requestSent() -= Pt::slot(*this, &WebSocket::onRequestSent);
        _client->replyReceived() -= Pt::slot(*this, &WebSocket::onReply);
    }

    close();
    delete _channel;
}


void WebSocket::addProtocol(const std::string& name)
{
    if( ! isProtocolToken(name) )
        throw std::invalid_argument("WebSocket protocol name is not a token");

    _protocols.push_back(name);
}


void WebSocket::beginConnect(const std::string& url, const std::string& origin)
{
    if( ! _client )
        throw std::logic_error("WebSocket has no client");

    _error = false;
    _connecting = true;
    parseUrl(url, origin);

    _client->requestSent() += Pt::slot(*this, &WebSocket::onRequestSent);
    _client->replyReceived() += Pt::slot(*this, &WebSocket::onReply);

    Request& request = _client->request();
    request.clear();
    request.setMethod("GET");
    request.setUrl(_path);
    request.header().set("Connection", "Upgrade");
    request.header().set("Upgrade", "websocket");
    request.header().set("Sec-WebSocket-Version", "13");

    const std::string key = createKey();
    request.header().set("Sec-WebSocket-Key", key.c_str());

    if( ! origin.empty() )
        request.header().set("Origin", origin.c_str());

    if( ! _protocols.empty() )
    {
        std::string offered = _protocols[0];
        for(std::size_t i = 1; i < _protocols.size(); ++i)
            offered += ", " + _protocols[i];

        request.header().set("Sec-WebSocket-Protocol", offered.c_str());
    }

    _protocol.clear();
    _client->beginSend(true);
}


void WebSocket::endConnect()
{
    if(_error || _connecting)
        throw std::runtime_error("WebSocket handshake failed");
}


void WebSocket::finishHandshake(bool failed)
{
    if(_client)
    {
        _client->requestSent() -= Pt::slot(*this, &WebSocket::onRequestSent);
        _client->replyReceived() -= Pt::slot(*this, &WebSocket::onReply);
    }

    _connecting = false;
    _error = failed;
    _connected.send(*this);
}


void WebSocket::onRequestSent(Client& client)
{
    try
    {
        MessageProgress progress = client.endSend();
        if( ! progress.finished() )
        {
            client.beginSend(true);
            return;
        }

        client.beginReceive();
    }
    catch(const std::exception&)
    {
        finishHandshake(true);
    }
}


void WebSocket::onReply(Client& client)
{
    try
    {
        MessageProgress progress = client.endReceive();
        if( ! progress.finished() )
        {
            client.beginReceive();
            return;
        }

        if( client.reply().statusCode() != 101 )
            throw std::runtime_error("WebSocket handshake failed");

        const char* echoed = client.reply().header().get("Sec-WebSocket-Protocol");
        std::string selected;
        if(echoed && *echoed)
        {
            const std::string field(echoed);
            std::size_t left = 0;
            std::size_t right = field.size();
            while(left < right && isOws(field[left]))
                ++left;
            while(right > left && isOws(field[right - 1]))
                --right;

            selected = field.substr(left, right - left);
            if(selected.find(',') != std::string::npos || ! isProtocolToken(selected))
                throw std::runtime_error("WebSocket handshake failed");

            bool offered = false;
            for(std::size_t i = 0; i < _protocols.size(); ++i)
            {
                if(_protocols[i] == selected)
                {
                    offered = true;
                    break;
                }
            }

            if( ! offered )
                throw std::runtime_error("WebSocket handshake failed");
        }

        _protocol = selected;

        Stream& stream = client.upgrade();
        _channel->open(stream, true);
        _channel->inputReady() += Pt::slot(*this, &WebSocket::onInputReady);
        _channel->outputReady() += Pt::slot(*this, &WebSocket::onOutputReady);
        _channel->closed() += Pt::slot(*this, &WebSocket::onClosed);
        finishHandshake(false);
    }
    catch(const std::exception&)
    {
        if( ! _connecting )
            throw;

        finishHandshake(true);
    }
}


WebSocketMessage& WebSocket::incoming()
{
    return _channel->incoming();
}


WebSocketMessage& WebSocket::outgoing()
{
    return _channel->outgoing();
}


void WebSocket::beginSend()
{
    _channel->beginSend();
}


MessageProgress WebSocket::endSend()
{
    return _channel->endSend();
}


void WebSocket::beginReceive()
{
    _channel->beginReceive();
}


MessageProgress WebSocket::endReceive()
{
    return _channel->endReceive();
}


void WebSocket::ping(const char* payload, std::size_t n)
{
    _channel->ping(payload, n);
}


void WebSocket::shutdown(unsigned code, const std::string& reason)
{
    _channel->shutdown(code, reason);
}


void WebSocket::close()
{
    _channel->close();
}


unsigned WebSocket::closeCode() const
{
    return _channel->closeCode();
}


const std::string& WebSocket::closeReason() const
{
    return _channel->closeReason();
}


void WebSocket::setTimeout(std::size_t timeout)
{
    _channel->setTimeout(timeout);
}


void WebSocket::setMaxMessageSize(std::size_t maxSize)
{
    _channel->setMaxMessageSize(maxSize);
}


void WebSocket::setIdleTimeout(std::size_t ms)
{
    _channel->setIdleTimeout(ms);
}


void WebSocket::onInputReady()
{
    _inputReady.send(*this);
}


void WebSocket::onOutputReady()
{
    _outputReady.send(*this);
}


void WebSocket::onClosed()
{
    _closed.send(*this);
}


void WebSocket::parseUrl(const std::string& url, const std::string& /*origin*/)
{
    if(url.empty())
    {
        _path = "/";
        return;
    }

    if(url[0] == '/')
    {
        _path = url;
        return;
    }

    Pt::System::Uri uri(url);
    _path = uri.path().empty() ? "/" : uri.path();
    if( ! uri.query().empty() )
        _path += "?" + uri.query();
}


std::string WebSocket::createKey()
{
    std::srand( static_cast<unsigned int>(std::time(0)) );

    std::stringstream ss;
    Pt::BasicTextOStream<char, char> stream(ss, new Pt::Base64Codec());

    for(int i = 0; i < 4; ++i)
    {
        Pt::uint32_t val = static_cast<Pt::uint32_t>(std::rand());
        stream.write( reinterpret_cast<char*>(&val), 4 );
    }

    stream.flush();
    return ss.str();
}

} // namespace Http

} // namespace Pt
