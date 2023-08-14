/*
 * Copyright (C) 2014 by Dr. Marc Boris Duerner
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

#include <Pt/Soap/HttpResponder.h>
#include <Pt/Soap/HttpService.h>
#include <Pt/Soap/Fault.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>
#include <cassert>

namespace Pt {

namespace Soap {

HttpResponder::HttpResponder(HttpService& httpService, const ServiceDeclaration& decl, Remoting::ServiceDefinition& def)
: Http::Responder(httpService)
, Soap::Responder(decl, def)
, _request(0)
, _reply(0)
{
}

HttpResponder::~HttpResponder()
{
}


// pass only ReplyHeader and body stream
void HttpResponder::onBeginRequest(Http::Request& request, 
                                   Pt::Http::Reply& reply,
                                   System::EventLoop& loop)
{
    _request = &request;
    _reply = 0;
    
    beginMessage( request.body() );
    parseMessage();

    Http::Responder::setReady(false);
}


// pass only ReplyHeader and body stream
void HttpResponder::onReadRequest(Http::Request& request, 
                                  Pt::Http::Reply& reply,
                                  System::EventLoop& loop)
{
    parseMessage();

    Http::Responder::setReady(false);
}


void HttpResponder::onBeginReply(const Http::Request& request, 
                                 Http::Reply& reply,
                                 System::EventLoop& loop)
{
    _reply = &reply;
    finishMessage(loop);

    // not ready yet...
}


void HttpResponder::onWriteReply(const Http::Request& request, 
                                 Http::Reply& reply,
                                 System::EventLoop& loop)
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
            if(reply.buffer().size() > 8192)
            {
                //reply.beginSend(false);
                return false;
            }
        }

        finishResult();
        //reply.beginSend(true);
        return true;
    }
    catch(const Xml::XmlError& e)
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

    if( _reply )
    {
        //_reply->header().set("Content-Type", "text/xml");
        _reply->header().set("Content-Type", "application/soap+xml");
        beginResult(_reply->body() );
        
        bool isFinished = advanceReply(*_reply);
        setFinished(isFinished);
    }
}


void HttpResponder::onFault(const Fault& fault)
{
    assert(_reply);

    if( _reply )
    {
        //_reply->header().set("Content-Type", "text/xml");
        _reply->header().set("Content-Type", "application/soap+xml");
        _reply->header().set("Connection", "close");
        beginFault(_reply->body(), fault );
        
        bool isFinished = advanceReply(*_reply);
        setFinished(isFinished);
    }
}


void HttpResponder::onCancel()
{
    Soap::Responder::onCancel();

    // not really possible, since only the HTTP server uses this class
}

} // namespace Soap

} // namespace Pt
