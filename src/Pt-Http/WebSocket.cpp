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
#include "WebSocketConnection.h"
#include <sstream>
#include <ctime>
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

WebSocket::WebSocket(Client& client)
: _client(&client)
, _connection(new WebSocketConnection())
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

    delete _connection;
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

        Stream& stream = client.upgrade();
        _connection->open(stream, true);
        _connection->inputReady() += Pt::slot(*this, &WebSocket::onInputReady);
        _connection->outputReady() += Pt::slot(*this, &WebSocket::onOutputReady);
        _connection->closed() += Pt::slot(*this, &WebSocket::onClosed);
        finishHandshake(false);
    }
    catch(const std::exception&)
    {
        finishHandshake(true);
    }
}


std::iostream& WebSocket::body()
{
    return _connection->body();
}


std::size_t WebSocket::available() const
{
    return _connection->available();
}


std::size_t WebSocket::pending() const
{
    return _connection->pending();
}


void WebSocket::discard()
{
    _connection->discard();
}


WebSocket::Frame WebSocket::frame() const
{
    return toFrame( _connection->frame() );
}


void WebSocket::beginSend(Frame frame)
{
    _connection->beginSend( toConnectionFrame(frame) );
}


void WebSocket::endSend()
{
    _connection->endSend();
}


void WebSocket::beginReceive()
{
    _connection->beginReceive();
}


void WebSocket::endReceive()
{
    _connection->endReceive();
}


void WebSocket::sendPing()
{
    _connection->sendPing();
}


void WebSocket::sendPong()
{
    _connection->sendPong();
}


void WebSocket::close()
{
    _connection->close();
}


void WebSocket::setTimeout(std::size_t timeout)
{
    _connection->setTimeout(timeout);
}


void WebSocket::setMaxMessageSize(std::size_t maxSize)
{
    _connection->setMaxMessageSize(maxSize);
}


void WebSocket::setIdleTimeout(std::size_t ms)
{
    _connection->setIdleTimeout(ms);
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
