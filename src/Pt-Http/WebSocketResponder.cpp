/* Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
   Copyright (C) 2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#include <Pt/Http/WebSocketResponder.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Http/Request.h>
#include "WebSocketChannel.h"
#include <Pt/Http/Message.h>
#include <Pt/Base64Codec.h>
#include <string>
#include <vector>

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
{

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
    if( header.matches("Connection", "upgrade") && header.matches("Upgrade", "websocket") )
    {
        WebSocketService& service = static_cast<WebSocketService&>( this->service() );
        if( ! service.hasServlet()
            || (service.maxSockets() != 0 && service.socketCount() >= service.maxSockets()) )
        {
            reply.setStatus(503, "Service Unavailable");
            reply.header().set("Connection", "close");
            setReady(true);
            return;
        }

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

        const MessageHeader::FieldIterator offered =
            header.field("Sec-WebSocket-Protocol");
        const std::vector<std::string>& accepted = service.protocols();
        std::string selected;
        if(offered != header.end())
        {
            for(MessageHeader::ValueIterator it = offered->begin();
                it != offered->end() && selected.empty();
                ++it)
            {

                for(std::size_t n = 0; n < accepted.size(); ++n)
                {
                    if( it->equals(accepted[n].c_str()) )
                    {
                        selected.assign(it->data(), it->size());
                        break;
                    }
                }
            }

            if( ! accepted.empty() && selected.empty() )
            {
                reply.setStatus(400, "Bad Request");
                reply.header().set("Connection", "close");
                setReady(true);
                return;
            }
        }
        else if( ! accepted.empty() )
        {
            reply.setStatus(400, "Bad Request");
            reply.header().set("Connection", "close");
            setReady(true);
            return;
        }

        reply.setStatus(101, "Switching Protocols");
        reply.header().setUpgrade();
        reply.header().set("Upgrade", "websocket");
        const std::string keyText(keyValue.data(), keyValue.size());
        const std::string accept = WebSocketChannel::getAcceptKey(keyText);
        reply.header().set("Sec-WebSocket-Accept", accept.c_str());
        if( ! selected.empty() )
            reply.header().set("Sec-WebSocket-Protocol", selected.c_str());
    }
    else
    {
        reply.setStatus(404, "Not found");
    }

    //reply.beginSend(true);
    setReady(true);
}

}}
