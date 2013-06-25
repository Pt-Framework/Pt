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


void HttpResponder::onCancel()
{
    _reply = 0;
}


void HttpResponder::onBeginRequest(Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    XmlRpc::Responder::beginRequest( request.body() );
}


void HttpResponder::onReadRequest(Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    try
    {
        bool done = advanceRequest();
        if( done )
        {
            _reply = &reply;
            execute(loop);
            return;
        }
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
        execute(loop);
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


void HttpResponder::onError()
{
    assert(_reply);
    if(_reply)
    {
        _reply->header().set("Content-Type", "text/xml");
        _reply->header().set("Connection", "close");
    }
}


void HttpResponder::onBeginReturn()
{
    try
    {
        assert(_reply);
        if( ! _reply)
            throw std::logic_error("XML-RPC responder without reply");

        _reply->header().set("Content-Type", "text/xml");

        formatReply( _reply->body() );

        _reply->beginSend(true);
    }
    catch(const std::exception& error)
    {
        log_error( error.what() );
        throw;
    }
}

} // namespace XmlRpc

} // namespace Pt
