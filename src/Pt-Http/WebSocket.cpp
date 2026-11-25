/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocket.h>
#include <Pt/Http/HttpError.h>
#include <Pt/Http/Stream.h>
#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/Message.h>
#include <Pt/System/Uri.h>
#include "WebSocketChannel.h"
#include <stdexcept>
#include <vector>

namespace Pt {

namespace Http {

WebSocket::WebSocket(Client& client)
: _client(&client)
, _channel(new WebSocketChannel())
, _path("/")
, _error()
, _connecting(false)
{
    _channel->inputReady() += Pt::slot(*this, &WebSocket::onInputReady);
    _channel->outputReady() += Pt::slot(*this, &WebSocket::onOutputReady);
    _channel->pingReady() += Pt::slot(*this, &WebSocket::onPingReady);
    _channel->shutdownReady() += Pt::slot(*this, &WebSocket::onShutdownReady);
    _channel->peerClosed() += Pt::slot(*this, &WebSocket::onPeerClosed);
    _channel->pong() += Pt::slot(*this, &WebSocket::onPong);
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


MessageHeader& WebSocket::header()
{
    return _client->request().header();
}


const MessageHeader& WebSocket::header() const
{
    return _client->request().header();
}


const Reply& WebSocket::reply() const
{
    return _client->reply();
}


void WebSocket::addProtocol(const std::string& name)
{
    if( ! MessageHeader::isToken(name.c_str()) )
        throw std::invalid_argument("WebSocket protocol name is not a token");

    _protocols.push_back(name);
}


void WebSocket::clearProtocols()
{
    _protocols.clear();
}


void WebSocket::beginConnect(const std::string& url, const std::string& origin)
{
    if( ! _client )
        throw std::logic_error("WebSocket has no client");

    if(_connecting)
        throw std::logic_error("WebSocket handshake is still pending");

    if(_channel->isOpen())
        throw std::logic_error("WebSocket is already open");

    _error = std::exception_ptr();
    _acceptKey.clear();
    _protocol.clear();
    parseUrl(url, origin);

    Request& request = _client->request();
    request.discard();
    request.body().clear();
    request.setMethod("GET");
    request.setUrl(_path);
    request.setQParams(std::string());
    request.header().setVersion(1, 1);
    request.header().remove("Content-Length");
    request.header().remove("Transfer-Encoding");
    request.header().remove("Trailer");
    request.header().remove("Sec-WebSocket-Protocol");
    request.header().set("Connection", "Upgrade");
    request.header().set("Upgrade", "websocket");
    request.header().set("Sec-WebSocket-Version", "13");

    const std::string key = _channel->createKey();
    _acceptKey = WebSocketChannel::getAcceptKey(key);
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

    _connecting = true;
    _client->requestSent() += Pt::slot(*this, &WebSocket::onRequestSent);
    _client->replyReceived() += Pt::slot(*this, &WebSocket::onReply);
    _client->beginSend(true);
}


void WebSocket::endConnect()
{
    if(_connecting)
        throw std::logic_error("WebSocket handshake is still pending");

    if(_error)
        std::rethrow_exception(_error);
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
        return;
    }
    catch(...)
    {
        _error = std::current_exception();
    }

    onHandshakeFinished();
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
        {
            throw HttpError("WebSocket handshake failed: status "
                            + std::to_string( client.reply().statusCode() ));
        }

        const MessageHeader& replyHeader = client.reply().header();
        if( ! replyHeader.matches("Connection", "upgrade") )
            throw HttpError("WebSocket handshake failed: no Connection upgrade");

        if( ! replyHeader.matches("Upgrade", "websocket") )
            throw HttpError("WebSocket handshake failed: no Upgrade websocket");

        // no extension is offered, so any extension in the reply is unsolicited
        if( replyHeader.field("Sec-WebSocket-Extensions") != replyHeader.end() )
            throw HttpError("WebSocket handshake failed: unsolicited extension");

        const MessageHeader::FieldIterator accept =
            client.reply().header().field("Sec-WebSocket-Accept");
        if(accept == client.reply().header().end() || accept->empty())
            throw HttpError("WebSocket handshake failed");

        MessageHeader::ValueIterator acceptIt = accept->begin();
        const MessageHeader::Value acceptValue = *acceptIt;
        ++acceptIt;
        if( acceptIt != accept->end()
            || ! acceptValue.equals(_acceptKey.c_str()) )
        {
            throw HttpError("WebSocket handshake failed");
        }

        const MessageHeader::FieldIterator echoed =
            client.reply().header().field("Sec-WebSocket-Protocol");
        std::string selected;
        if(echoed != client.reply().header().end() && ! echoed->empty())
        {
            MessageHeader::ValueIterator protocolIt = echoed->begin();
            const MessageHeader::Value protocol = *protocolIt;
            ++protocolIt;
            if(protocolIt != echoed->end())
                throw HttpError("WebSocket handshake failed");

            bool offered = false;
            for(std::size_t i = 0; i < _protocols.size(); ++i)
            {
                if( protocol.equals(_protocols[i].c_str()) )
                {
                    offered = true;
                    break;
                }
            }

            if( ! offered )
                throw HttpError("WebSocket handshake failed");

            selected.assign(protocol.data(), protocol.size());
        }

        _protocol = selected;

        Stream& stream = client.upgrade();
        _channel->open(stream, true);
    }
    catch(...)
    {
        _error = std::current_exception();
    }

    onHandshakeFinished();
}


void WebSocket::onHandshakeFinished()
{
    if(_client)
    {
        _client->requestSent() -= Pt::slot(*this, &WebSocket::onRequestSent);
        _client->replyReceived() -= Pt::slot(*this, &WebSocket::onReply);
    }

    _connecting = false;
    _connected.send(*this);
}


WebSocketMessage& WebSocket::incoming()
{
    return _channel->incoming();
}


WebSocketMessage& WebSocket::outgoing()
{
    return _channel->outgoing();
}


void WebSocket::beginSend(bool finished)
{
    _channel->beginSend(finished);
}


WebSocketProgress WebSocket::endSend()
{
    return _channel->endSend();
}


void WebSocket::beginReceive()
{
    _channel->beginReceive();
}


WebSocketProgress WebSocket::endReceive()
{
    return _channel->endReceive();
}


void WebSocket::beginPing(const char* payload, std::size_t n)
{
    _channel->beginPing(payload, n);
}


void WebSocket::endPing()
{
    _channel->endPing();
}


void WebSocket::beginShutdown(unsigned code, const std::string& reason)
{
    _channel->beginShutdown(code, reason);
}


void WebSocket::endShutdown()
{
    _channel->endShutdown();
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


void WebSocket::setMaxMessageSize(std::size_t maxSize)
{
    _channel->setMaxMessageSize(maxSize);
}


void WebSocket::setIdleTimeout(std::size_t ms)
{
    _channel->setIdleTimeout(ms);
}


void WebSocket::setPingInterval(std::size_t ms)
{
    _channel->setPingInterval(ms);
}


void WebSocket::setPingTimeout(std::size_t ms)
{
    _channel->setPingTimeout(ms);
}


void WebSocket::setCloseTimeout(std::size_t ms)
{
    _channel->setCloseTimeout(ms);
}


void WebSocket::onInputReady()
{
    _inputReady.send(*this);
}


void WebSocket::onOutputReady()
{
    _outputReady.send(*this);
}


void WebSocket::onPingReady()
{
    _pingReady.send(*this);
}


void WebSocket::onShutdownReady()
{
    _shutdownReady.send(*this);
}


void WebSocket::onPeerClosed()
{
    _closed.send(*this);
}


void WebSocket::onPong(const std::string& payload)
{
    _pong.send(*this, payload);
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


} // namespace Http

} // namespace Pt
