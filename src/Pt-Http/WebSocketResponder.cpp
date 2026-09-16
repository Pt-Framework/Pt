/*
 * Copyright (C) 2015 by Laurentiu-Gheorghe Crisan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <Pt/Http/WebSocketResponder.h>
#include <Pt/Http/WebSocketService.h>
#include <Pt/Base64Codec.h>
#include <Pt/Http/Request.h>
#include <Pt/TextStream.h>
#include "Sha1.h"

namespace Pt {
namespace Http {

static std::string toBase64(const uint8_t* input, size_t size)
{
    static const char base64Chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const char padCharacter = '=';

    std::string encodedString;
    encodedString.reserve(((size / 3) + (size % 3 > 0)) * 4);

    std::uint32_t temp{};
    const uint8_t* it = &input[0];

    for(std::size_t i = 0; i < size / 3; ++i)
    {
        temp = (*it++) << 16;
        temp += (*it++) << 8;
        temp += (*it++);
        encodedString.append(1, base64Chars[(temp & 0x00FC0000) >> 18]);
        encodedString.append(1, base64Chars[(temp & 0x0003F000) >> 12]);
        encodedString.append(1, base64Chars[(temp & 0x00000FC0) >> 6]);
        encodedString.append(1, base64Chars[(temp & 0x0000003F)]);
    }

    switch(size % 3)
    {
    case 1:
        temp = (*it++) << 16;
        encodedString.append(1, base64Chars[(temp & 0x00FC0000) >> 18]);
        encodedString.append(1, base64Chars[(temp & 0x0003F000) >> 12]);
        encodedString.append(2, padCharacter);
        break;

    case 2:
        temp = (*it++) << 16;
        temp += (*it++) << 8;
        encodedString.append(1, base64Chars[(temp & 0x00FC0000) >> 18]);
        encodedString.append(1, base64Chars[(temp & 0x0003F000) >> 12]);
        encodedString.append(1, base64Chars[(temp & 0x00000FC0) >> 6]);
        encodedString.append(1, padCharacter);
        break;
    }

    return encodedString;
}

static std::string toLower(const std::string& str)
{
    std::string lowerString = "";
    std::locale loc;

    for(size_t i = 0; i < str.size(); ++i)
        lowerString += std::tolower(str[i], loc);

    return lowerString;
}

std::string WebSocketResponder::computeAccept(const std::string& key)
{
    std::string accept(key);
    accept += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    Sha1 sha1(accept.data(), accept.size());

    const std::vector<uint8_t>& d = sha1.calc();

    return toBase64(&d[0], d.size());
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
    const std::string up  = toLower(request.header().get("Connection"));
    const std::string upw = toLower(request.header().get("Upgrade"));

    if (up == "upgrade" && upw == "websocket")
    {
        std::string key = request.header().get("Sec-WebSocket-Key");
        reply.setStatus(101, "Switching Protocols");
        reply.header().setUpgrade();
        reply.header().set("Upgrade", "websocket");
        reply.header().set("Connection", "Upgrade");
        reply.header().set("Sec-WebSocket-Accept", computeAccept(key).c_str());
    }
    else
    {
        reply.setStatus(404, "Not found");
    }

    //reply.beginSend(true);
    setReady(true); 
}

}}
