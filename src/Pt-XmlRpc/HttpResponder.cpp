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
#include <Pt/XmlRpc/HttpResponder.h>
#include <Pt/XmlRpc/Fault.h>
#include <Pt/XmlRpc/Service.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/Logger.h>
#include <cassert>

log_define("Pt.XmlRpc.Responder")

namespace Pt {

namespace XmlRpc {

HttpResponder::HttpResponder(HttpService& httpService, Service& rpcService)
: Http::Responder(httpService)
, XmlRpc::Responder(rpcService)
, _reply(0)
{
}


HttpResponder::~HttpResponder()
{
}


void HttpResponder::onBeginRequest(Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    XmlRpc::Responder::beginRequest( request.body() );
}


void HttpResponder::onReadRequest(Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    try
    {
        advanceRequest();

        // advanceRquest() should return false if end was reached, either
        // because parsing is complete, or an error occured

        // if an error occured, we save it in Responder and call onEndCall
        // to get a stream where we can format the error response to.
    }
    catch(const Fault& fault)
    {
        log_error( fault.what() );
        replyError(reply, fault.rc(), fault.what());
    }
    catch(const std::exception& error)
    {
        log_error( error.what() );
        throw;
    }
}


void HttpResponder::onBeginReply(Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    try
    {
        _reply = &reply;      
        reply.header().set("Content-Type", "text/xml");

        finishRequest(loop);
    }
    catch (const Fault& fault)
    {
        log_error( fault.what() );
        replyError(reply, fault.rc(), fault.what());
    }
    catch(const std::exception& error)
    {
        log_error( error.what() );
        throw;
    }
}


void HttpResponder::onWriteReply(Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
}


void HttpResponder::onEndCall()
{
    try
    {
        assert(_reply);
        if( ! _reply)
            throw std::logic_error("XML-RPC responder without reply");

        formatReply( _reply->body() );

        _reply->beginSend(true);
    }
    catch (const Fault& fault)
    {
        replyError(*_reply, fault.rc(), fault.what());
    }
    catch(const std::exception& error)
    {
        log_error( error.what() );
        throw;
    }
}


void HttpResponder::replyError(Http::Reply& reply, int rc, const char* msg)
{
    reply.clear();
    reply.header().set("Content-Type", "text/xml");
    reply.header().set("Connection", "close");

    formatError( reply.body(), rc, msg );

    reply.beginSend(true);
}

} // namespace XmlRpc

} // namespace Pt
