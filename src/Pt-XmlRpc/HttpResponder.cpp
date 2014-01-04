/*
 * Copyright (C) 2009-2013 by Dr. Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "HttpResponder.h"
#include <Pt/XmlRpc/Fault.h>
#include <Pt/XmlRpc/HttpService.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/Logger.h>
#include <cassert>

log_define("Pt.XmlRpc.Responder")

namespace Pt {

namespace XmlRpc {

HttpResponder::HttpResponder(HttpService& httpService, ServiceDefinition& rpcService)
: Http::Responder(httpService)
, XmlRpc::Responder(rpcService)
, _reply(0)
{
}


HttpResponder::~HttpResponder()
{
}


// pass only ReplyHeader and body stream
void HttpResponder::onBeginRequest(Http::Request& request, Pt::Http::Reply& reply, System::EventLoop& loop)
{
    _reply = 0;
    
    beginMessage( request.body() );
}


// pass only ReplyHeader and body stream
void HttpResponder::onReadRequest(Http::Request& request, Pt::Http::Reply& reply, System::EventLoop& loop)
{
    parseMessage();
}


void HttpResponder::onBeginReply(const Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    _reply = &reply;
    finishMessage(loop);
}


void HttpResponder::onWriteReply(const Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    while( ! advanceResult() )
    {
        if(reply.buffer().size() > 8192)
        {
            reply.beginSend(false);
            return;
        }
    }

    finishResult();
    reply.beginSend(true);
}


void HttpResponder::onResult()
{
    assert(_reply);

    if( _reply )
    {
        _reply->header().set("Content-Type", "text/xml");

        beginResult(_reply->body() );

        while( ! advanceResult() )
        {
            if(_reply->buffer().size() > 8192)
            {
                _reply->beginSend(false);
                return;
            }
        }

        finishResult();
        _reply->beginSend(true);
    }
}


void HttpResponder::onCancel()
{
    // not really possible, since only the HTTP server uses this class
}


void HttpResponder::onError()
{
    if(_reply)
    {
        _reply->header().set("Connection", "close");
    }

    onResult();
}

} // namespace XmlRpc

} // namespace Pt
