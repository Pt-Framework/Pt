/*
 * Copyright (C) 2020-2026 by Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <Pt/Mcp/StdioService.h>
#include <Pt/JsonRpc/Fault.h>
#include "Responder.h"
#include <Pt/Json/JsonReader.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <sstream>
#include <cstdlib>

namespace Pt {

namespace Mcp {

StdioService::StdioService(Remoting::ServiceDefinition& serviceDef,
                           const ToolDeclaration& decl)
: _serviceDef(&serviceDef)
, _decl(&decl)
{
}


StdioService::~StdioService()
{
}


std::string StdioService::readMessage(std::istream& is)
{
    std::size_t contentLength = 0;
    std::string line;

    while(std::getline(is, line))
    {
        if( ! line.empty() && line.back() == '\r')
            line.pop_back();

        if(line.empty())
            break;

        if(line.compare(0, 16, "Content-Length: ") == 0)
        {
            contentLength = static_cast<std::size_t>( std::atol(line.c_str() + 16) );
        }
    }

    if(is.eof() || contentLength == 0)
        return std::string();

    std::string body(contentLength, '\0');
    is.read(&body[0], static_cast<std::streamsize>(contentLength));

    if(is.gcount() < static_cast<std::streamsize>(contentLength))
        return std::string();

    return body;
}


void StdioService::writeMessage(std::ostream& os, const std::string& json)
{
    os << "Content-Length: " << json.size() << "\r\n\r\n";
    os << json;
    os.flush();
}


std::string StdioService::dispatch(const std::string& json)
{
    std::string method = extractMethod(json);

    if(method == "initialize")
        return dispatchInitialize(json);
    else if(method == "tools/list")
        return dispatchToolsList(json);
    else if(method == "tools/call")
        return dispatchToolsCall(json);
    else if(method == "notifications/initialized")
        return std::string();

    // Unknown method — return error
    Pt::int64_t id = extractId(json);
    std::ostringstream os;
    os << "{\"jsonrpc\":\"2.0\",\"id\":" << id
       << ",\"error\":{\"code\":-32601,\"message\":\"Method not found\"}}";
    return os.str();
}


std::string StdioService::dispatchInitialize(const std::string& json)
{
    Pt::int64_t id = extractId(json);

    std::ostringstream os;
    os << "{\"jsonrpc\":\"2.0\",\"id\":" << id << ",\"result\":";
    _decl->toInitializeResult(os);
    os << '}';
    return os.str();
}


std::string StdioService::dispatchToolsList(const std::string& json)
{
    Pt::int64_t id = extractId(json);

    std::ostringstream os;
    os << "{\"jsonrpc\":\"2.0\",\"id\":" << id << ",\"result\":";
    _decl->toToolsList(os);
    os << '}';
    return os.str();
}


std::string StdioService::dispatchToolsCall(const std::string& json)
{
    Responder responder(*_serviceDef, *_decl);

    try
    {
        // Parse the JSON and feed nodes to the responder
        std::string input = json + "\n";
        Pt::Utf8Codec utf8(1);
        Pt::TextIStream tis(&utf8);
        std::istringstream iss(input);
        tis.reset(iss);
        tis.textBuffer().import();

        Json::JsonReader reader(tis);

        for(;;)
        {
            const Json::Node* node = reader.advance();
            if( ! node)
                break;

            bool done = responder.advance(*node);
            if(done)
                break;
        }

        // Call the procedure and format the result
        std::ostringstream os;
        responder.formatResult(os);
        return os.str();
    }
    catch(const JsonRpc::Fault& f)
    {
        std::ostringstream os;
        os << "{\"jsonrpc\":\"2.0\",\"id\":" << responder.requestId()
           << ",\"error\":{\"code\":" << f.code()
           << ",\"message\":\"" << f.what() << "\"}}";
        return os.str();
    }
    catch(const std::exception& e)
    {
        std::ostringstream os;
        os << "{\"jsonrpc\":\"2.0\",\"id\":" << responder.requestId()
           << ",\"error\":{\"code\":-32603,\"message\":\""
           << e.what() << "\"}}";
        return os.str();
    }
}


std::string StdioService::extractMethod(const std::string& json)
{
    // Simple scan for "method":"..." in the JSON
    std::size_t pos = json.find("\"method\"");
    if(pos == std::string::npos)
        return std::string();

    pos = json.find(':', pos + 8);
    if(pos == std::string::npos)
        return std::string();

    pos = json.find('"', pos + 1);
    if(pos == std::string::npos)
        return std::string();

    std::size_t end = json.find('"', pos + 1);
    if(end == std::string::npos)
        return std::string();

    return json.substr(pos + 1, end - pos - 1);
}


Pt::int64_t StdioService::extractId(const std::string& json)
{
    std::size_t pos = json.find("\"id\"");
    if(pos == std::string::npos)
        return 0;

    pos = json.find(':', pos + 4);
    if(pos == std::string::npos)
        return 0;

    ++pos;
    while(pos < json.size() && (json[pos] == ' ' || json[pos] == '\t'))
        ++pos;

    return static_cast<Pt::int64_t>( std::atol(json.c_str() + pos) );
}

} // namespace Mcp

} // namespace Pt
