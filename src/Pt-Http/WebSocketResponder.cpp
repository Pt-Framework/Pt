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
#include <cstdint>
#include <string>
#include <vector>

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

static bool isOws(char ch)
{
    return ch == ' ' || ch == '\t';
}


static bool isProtocolToken(const std::string& name)
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


static bool parseProtocols(const char* field, std::vector<std::string>& names)
{
    names.clear();
    if( ! field || ! *field )
        return true;

    const std::string text(field);
    std::size_t begin = 0;
    while(begin <= text.size())
    {
        const std::size_t comma = text.find(',', begin);
        const std::size_t end = comma == std::string::npos ? text.size() : comma;

        std::size_t left = begin;
        while(left < end && isOws(text[left]))
            ++left;

        std::size_t right = end;
        while(right > left && isOws(text[right - 1]))
            --right;

        const std::string name = text.substr(left, right - left);
        if( ! isProtocolToken(name) )
            return false;

        names.push_back(name);

        if(comma == std::string::npos)
            break;

        begin = comma + 1;
    }

    return true;
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
    const char* connection = request.header().get("Connection");
    const char* upgrade = request.header().get("Upgrade");
    const std::string connectionValue = connection ? toLower(connection) : std::string();
    const std::string upgradeValue = upgrade ? toLower(upgrade) : std::string();

    if (connectionValue.find("upgrade") != std::string::npos && upgradeValue == "websocket")
    {
        WebSocketService& service = static_cast<WebSocketService&>( this->service() );
        if( service.maxSockets() != 0 && service.sessionCount() >= service.maxSockets() )
        {
            reply.setStatus(503, "Service Unavailable");
            reply.header().set("Connection", "close");
            setReady(true);
            return;
        }

        std::vector<std::string> offered;
        if( ! parseProtocols(request.header().get("Sec-WebSocket-Protocol"), offered) )
        {
            reply.setStatus(400, "Bad Request");
            reply.header().set("Connection", "close");
            setReady(true);
            return;
        }

        const std::vector<std::string>& accepted = service.protocols();
        std::string selected;
        if( ! accepted.empty() )
        {
            for(std::size_t i = 0; i < offered.size() && selected.empty(); ++i)
            {
                for(std::size_t n = 0; n < accepted.size(); ++n)
                {
                    if(offered[i] == accepted[n])
                    {
                        selected = offered[i];
                        break;
                    }
                }
            }

            if(selected.empty())
            {
                reply.setStatus(400, "Bad Request");
                reply.header().set("Connection", "close");
                setReady(true);
                return;
            }
        }

        std::string key = request.header().get("Sec-WebSocket-Key");
        reply.setStatus(101, "Switching Protocols");
        reply.header().setUpgrade();
        reply.header().set("Upgrade", "websocket");
        reply.header().set("Connection", "Upgrade");
        reply.header().set("Sec-WebSocket-Accept", computeAccept(key).c_str());
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
