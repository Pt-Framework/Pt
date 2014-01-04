/*
 * Copyright (C) 2012-2013 by Marc Dürner
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

#include <Pt/XmlRpc/HttpClient.h>
#include <Pt/XmlRpc/Fault.h>

#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/HttpError.h>
#include <sstream>

namespace {

void verifyHeader(const Pt::Http::Reply& reply)
{
    if( reply.statusCode() != 200 )
    {
        std::ostringstream msg;
        msg << "invalid return code "
            << reply.statusCode()
            << ": "
            << reply.statusText();
        throw Pt::Http::HttpError( msg.str() );
    }

    if ( ! reply.header().isSet("Content-Type", "text/xml") )
    {
        std::string msg =  "invalid content type";
        
        const char* ct = reply.header().get("Content-Type");
        msg += ct ? ct : "";
        
        throw Pt::Http::HttpError(msg);
    }
}

}

namespace Pt {

namespace XmlRpc {

HttpClient::HttpClient()
{
    init();
}


HttpClient::HttpClient(const Net::Endpoint& ep, 
                       const std::string& url)
{
    init();
    setTarget(ep, url);
}


HttpClient::HttpClient(System::EventLoop& loop)
: _client(loop)
{
    init();
}


HttpClient::HttpClient(System::EventLoop& loop, const Net::Endpoint& ep,
                       const std::string& url)
: _client(loop)
{
    init();
    setTarget(ep, url);
}


HttpClient::~HttpClient()
{
}


void HttpClient::init()
{
    _client.requestSent() += Pt::slot(*this, &HttpClient::onRequest);
    _client.replyReceived() += Pt::slot( *this, &HttpClient::onReply);
}


void HttpClient::setActive(System::EventLoop& loop)
{
    _client.setActive(loop);
}


System::EventLoop* HttpClient::loop() const
{
    return _client.loop();
}


void HttpClient::setSecure(Ssl::Context& ctx)
{
    _client.setSecure(ctx);
}


void HttpClient::setTimeout(std::size_t timeout)
{
    _client.setTimeout(timeout);
}


void HttpClient::setTarget(const Net::Endpoint& ep, const std::string& url)
{
    _client.setHost(ep);
    _client.request().setUrl(url);
}


void HttpClient::setTarget(const Net::Endpoint& ep, const Net::TcpSocketOptions& opts,
                           const std::string& url)
{
    _client.setHost(ep, opts);
    _client.request().setUrl(url);
}


void HttpClient::setHost(const Net::Endpoint& ep)
{
    _client.setHost(ep);
}


void HttpClient::setHost(const Net::Endpoint& ep, const Net::TcpSocketOptions& opts)
{
    _client.setHost(ep, opts);
}


void HttpClient::setServiceUrl(const std::string& url)
{
    _client.request().setUrl(url);
}


void HttpClient::setServiceUrl(const char* url)
{
    _client.request().setUrl(url);
}


const Net::Endpoint& HttpClient::host() const
{
    return _client.host();
}


void HttpClient::onInvoke()
{
    // prepare HTTP request
    _client.request().clear();
    _client.request().header().set("Content-Type", "text/xml");
    _client.request().setMethod("POST");
    std::ostream& os = _client.request().body();

    // format XML-RPC request
    beginMessage(os);

    while( ! advanceMessage() )
    {
        if(_client.request().buffer().size() > 8192)
        {
            _client.beginSend(false);
            return;
        }
    }
        
    finishMessage();

    _client.beginReceive();
}


void HttpClient::onCall()
{
    // prepare HTTP request
    _client.request().clear();
    _client.request().header().set("Content-Type", "text/xml");
    _client.request().setMethod("POST");
    std::ostream& os = _client.request().body();

    // format XML-RPC request
    beginMessage(os);
    
    while( ! advanceMessage() )
    {
        if(_client.request().buffer().size() > 8192)
        {
            _client.send(false);
        }
    }

    finishMessage();

    // send HTTP request and start receiving HTTP reply
    _client.send(true);
    std::istream& is = _client.receive();

    // parse XML-RPC reply
    Client::processResult(is);

    // discard remaining data
    std::streamsize all = std::numeric_limits<std::streamsize>::max();
    is.ignore(all);
}


void HttpClient::onCancel()
{
    _client.cancel();
}


void HttpClient::onError()
{
    throw;
}


void HttpClient::onRequest(Http::Client& client)
{
    try
    {
        Pt::Http::MessageProgress progress = client.endSend();
        if( ! progress.finished() )
        {
            client.beginSend(false);
            return;
        }

        while( ! advanceMessage() )
        {
            if(client.request().buffer().size() > 8192)
            {
                client.beginSend(false);
                return;
            }
        }
        
        finishMessage();

        client.beginReceive();
    }
    catch(const System::IOError&) // HttpError is also an IOError
    {
        // setError() makes finishResult() call onError() where we throw
        setError();
        finishResult();
    }
}


void HttpClient::onReply(Http::Client& client)
{
    try
    {
        Http::MessageProgress progress = client.endReceive();

        if( progress.header() )
        {
            //_impl->verifyHeader( client.reply() );
            
            beginResult( client.reply().body() );
        }

        if( progress.body() )
        {
            // reads until error or XML was consumed
            parseResult();

            // discard remaining data
            client.reply().discard();
        }
        
        if( ! progress.finished() )
        { 
            client.beginReceive();
            return;
        }
    }
    catch(const System::IOError&) // HttpError is also an IOError
    {
        // finished signal will call onError()
        setError();
        finishResult();
        return;
    }

    // send finished signal
    finishResult();
}

} // namespace XmlRpc

} // namespace Pt
