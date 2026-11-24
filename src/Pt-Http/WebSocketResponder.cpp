/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include "WebSocketResponder.h"
#include "WebSocketChannel.h"

#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Message.h>
#include <Pt/Base64Codec.h>

#include <string>

namespace Pt {

namespace Http {

static bool isWebSocketKey(const MessageHeader::Value& key)
{
    if(key.size() != 24)
        return false;

    const char* p = key.data();
    std::size_t decoded = 0;

    for(std::size_t i = 0; i < 24; i += 4)
    {
        const Pt::uint8_t a = Pt::fromBase64(p[i]);
        const Pt::uint8_t b = Pt::fromBase64(p[i + 1]);
        const Pt::uint8_t c = Pt::fromBase64(p[i + 2]);
        const Pt::uint8_t d = Pt::fromBase64(p[i + 3]);
        if(a > 63 || b > 63 || c > 64 || d > 64)
            return false;

        const bool last = i + 4 == 24;
        if(c == 64)
        {
            if( ! last || d != 64 )
                return false;

            ++decoded;
        }
        else if(d == 64)
        {
            if( ! last )
                return false;

            decoded += 2;
        }
        else
        {
            decoded += 3;
        }
    }

    return decoded == 16;
}


WebSocketResponder::WebSocketResponder(Pt::Http::WebSocketService& s)
: Pt::Http::Responder(s)
, _reserved(false)
{

}


WebSocketResponder::~WebSocketResponder()
{
    if(_reserved)
        static_cast<WebSocketService&>( service() ).releasePending();
}


void WebSocketResponder::onBeginRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
{
    setReady(false);
}

void WebSocketResponder::onReadRequest(Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
{
    setReady(false);
}

void WebSocketResponder::onBeginReply(const Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
{
    onWriteReply(request, reply, loop);
}

void WebSocketResponder::onWriteReply(const Pt::Http::Request& request, Pt::Http::Reply& reply, Pt::System::EventLoop& loop)
{
    const MessageHeader& header = request.header();
    if( ! header.matches("Connection", "upgrade") || ! header.matches("Upgrade", "websocket") )
    {
        reply.setStatus(404, "Not found");
        setReady(true);
        return;
    }

    WebSocketService& service = static_cast<WebSocketService&>( this->service() );
    if( ! service.reservePending() )
    {
        reply.setStatus(503, "Service Unavailable");
        reply.header().set("Connection", "close");
        setReady(true);
        return;
    }

    _reserved = true;

    if( ! header.matches("Sec-WebSocket-Version", "13") )
    {
        reply.setStatus(426, "Upgrade Required");
        reply.header().set("Sec-WebSocket-Version", "13");
        reply.header().set("Connection", "close");
        setReady(true);
        return;
    }

    const MessageHeader::FieldIterator key = header.field("Sec-WebSocket-Key");
    MessageHeader::Value keyValue;
    if(key != header.end())
    {
        MessageHeader::ValueIterator it = key->begin();
        if(it != key->end())
        {
            keyValue = *it;
            ++it;
            if(it != key->end())
                keyValue = MessageHeader::Value();
        }
    }

    if( keyValue.empty() || ! isWebSocketKey(keyValue) )
    {
        reply.setStatus(400, "Bad Request");
        reply.header().set("Connection", "close");
        setReady(true);
        return;
    }

    std::string protocol;
    if( ! service.selectProtocol(*header.field("Sec-WebSocket-Protocol"), protocol) )
    {
        reply.setStatus(400, "Bad Request");
        reply.header().set("Connection", "close");
        setReady(true);
        return;
    }

    if( ! protocol.empty() )
        reply.header().set("Sec-WebSocket-Protocol", protocol.c_str());

    reply.setStatus(101, "Switching Protocols");
    reply.header().setUpgrade();
    reply.header().set("Upgrade", "websocket");

    const std::string keyText(keyValue.data(), keyValue.size());
    const std::string accept = WebSocketChannel::getAcceptKey(keyText);
    reply.header().set("Sec-WebSocket-Accept", accept.c_str());

    setReady(true);
}

} // namespace

} // namespace
