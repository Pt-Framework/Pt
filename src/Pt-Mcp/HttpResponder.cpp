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
, _rejectMethod(false)
{
}


HttpResponder::~HttpResponder()
{
}


void HttpResponder::onBeginRequest(Http::Request& request,
                                   Http::Reply& /*reply*/,
                                   System::EventLoop& /*loop*/)
{
    _request = &request;
    _reply = 0;
    _rejectMethod = (request.method() != "POST");

    // TODO: early finish on rejection
    if( ! _rejectMethod)
    {
        beginMessage( request.body() );
        parseMessage();
    }

    Http::Responder::setReady(false);
}


void HttpResponder::onReadRequest(Http::Request& /*request*/,
                                  Http::Reply& /*reply*/,
                                  System::EventLoop& /*loop*/)
{
    if( ! _rejectMethod)
        parseMessage();

    Http::Responder::setReady(false);
}


void HttpResponder::onBeginReply(const Http::Request& /*request*/,
                                 Http::Reply& reply,
                                 System::EventLoop& /*loop*/)
{
    _reply = &reply;

    if(_rejectMethod)
    {
        reply.setStatus(405, "Method Not Allowed");
        reply.header().set("Allow", "POST");
        reply.header().set("Content-Type", "application/json");
        reply.body() << "{\"jsonrpc\":\"2.0\",\"error\":"
                        "{\"code\":-32600,\"message\":\"Method Not Allowed\"}}";
        setFinished(true);
        return;
    }

    if(isNotification())
    {
        reply.setStatus(202, "Accepted");
        setFinished(true);
        return;
    }

    reply.header().set("Content-Type", "application/json");
    beginResult( reply.body() );

    bool isFinished = advanceReply(reply);
    setFinished(isFinished);
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
    while( ! advanceResult() )
    {
        if(reply.buffer().size() > 8192)
            return false;
    }

    finishResult();
    return true;
}

} // namespace Mcp

} // namespace Pt
