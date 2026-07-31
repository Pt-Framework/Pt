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

#include "HttpResponder.h"
#include <Pt/Mcp/HttpService.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Uri.h>
#include <Pt/SerializationError.h>
#include <Pt/ConversionError.h>
#include <cassert>

namespace Pt {

namespace Mcp {

HttpResponder::HttpResponder(HttpService& httpService,
                             Remoting::ServiceDefinition& serviceDef,
                             const ToolDeclaration& decl)
: Http::Responder(httpService)
, Mcp::Responder(serviceDef, decl)
, _request(0)
, _reply(0)
, _httpStatus(200)
{
}


HttpResponder::~HttpResponder()
{
}


void HttpResponder::onBeginRequest(Http::Request& request,
                                   Http::Reply& reply,
                                   System::EventLoop& /*loop*/)
{
    try
    {
        _request = &request;
        _reply = &reply;
        _httpStatus = 200;

        if(request.method() != "POST")
        {
            _httpStatus = 405;
            throw JsonRpc::Fault("Method Not Allowed", JsonRpc::Fault::InvalidRequest);
        }

        // Validate Origin header (MCP spec MUST)
        const char* origin = request.header().get("Origin");
        if(origin)
        {
            const char* host = request.header().get("Host");
            const System::Uri originUri(origin);
            std::string originHost = originUri.host();

            if(originUri.port() != 0)
            {
                originHost += ':';
                originHost += std::to_string(originUri.port());
            }

            if( ! host || originHost != std::string(host) )
            {
                _httpStatus = 403;
                throw JsonRpc::Fault("Forbidden: invalid Origin", JsonRpc::Fault::InvalidRequest);
            }
        }

        beginMessage( request.body() );
        parseMessage();
    }
    catch(const JsonRpc::Fault& fault)
    {
        setFault(fault.code(), fault.what());
    }
    catch(const System::InvalidUri&)
    {
        _httpStatus = 403;
        setFault(JsonRpc::Fault::InvalidRequest, "Forbidden: invalid Origin");
    }

    Http::Responder::setReady(false);
}


void HttpResponder::onReadRequest(Http::Request& /*request*/,
                                  Http::Reply& reply,
                                  System::EventLoop& /*loop*/)
{
    parseMessage();
    Http::Responder::setReady(false);
}


void HttpResponder::onBeginReply(const Http::Request& request,
                                 Http::Reply& reply,
                                 System::EventLoop& loop)
{
    _reply = &reply;

    if( isFailed() )
    {
        finishMessage(loop);
        return;
    }

    if( isNotification() )
    {
        reply.setStatus(202, "Accepted");
        setFinished(true);
        return;
    }

    // Check MCP-Protocol-Version header for non-initialize calls (MCP spec MUST)
    if( method() != "initialize")
    {
        const char* mcpVersion = request.header().get("MCP-Protocol-Version");
        if( mcpVersion && ! ToolDeclaration::isSupportedVersion(mcpVersion) )
        {
            _httpStatus = 400;
            setFault(JsonRpc::Fault::InvalidRequest, "Unsupported MCP-Protocol-Version");
        }
    }

    finishMessage(loop);
}


void HttpResponder::onWriteReply(const Http::Request& /*request*/,
                                 Http::Reply& reply,
                                 System::EventLoop& /*loop*/)
{
    bool isFinished = advanceReply(reply);
    Http::Responder::setReady(isFinished);
}


bool HttpResponder::advanceReply(Http::Reply& reply)
{
    try
    {
        while( ! advanceResult() )
        {
            if( reply.buffer().size() > 8192 )
            {
                return false;
            }
        }

        finishResult();
        return true;
    }
    catch(const Json::JsonError& e)
    {
        throw System::IOError( e.what() );
    }
    catch(const SerializationError& e)
    {
        throw System::IOError( e.what() );
    }
    catch(const ConversionError& e)
    {
        throw System::IOError( e.what() );
    }

    return true;
}


void HttpResponder::onResult()
{
    assert(_reply);

    _reply->setStatus(_httpStatus, "");
    _reply->header().set("Content-Type", "application/json");
    beginResult(_reply->body());

    bool isFinished = advanceReply(*_reply);
    setFinished(isFinished);
}


void HttpResponder::onFault(const JsonRpc::Fault& fault)
{
    assert(_reply);

    if(_httpStatus == 405)
        _reply->header().set("Allow", "POST");

    _reply->setStatus(_httpStatus, "");
    _reply->header().set("Content-Type", "application/json");
    beginFault(_reply->body(), fault);

    bool isFinished = advanceReply(*_reply);
    setFinished(isFinished);
}

} // namespace Mcp

} // namespace Pt
