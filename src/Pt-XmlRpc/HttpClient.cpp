/*
 * Copyright (C) 2012-2013 by Marc Duerner
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
: _error(false)
{
    init();
}


HttpClient::HttpClient(const Net::Endpoint& ep, 
                       const std::string& url)
: _error(false)
{
    init();
    setTarget(ep, url);
}


HttpClient::HttpClient(System::EventLoop& loop)
: _client(loop)
, _error(false)
{
    init();
}


HttpClient::HttpClient(System::EventLoop& loop, const Net::Endpoint& ep,
                       const std::string& url)
: _client(loop)
, _error(false)
{
    init();
    setTarget(ep, url);
}


HttpClient::~HttpClient()
{
}


void HttpClient::init()
{
    _client.request().header().set("Content-Type", "text/xml");
    _client.request().setMethod("POST");

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


void HttpClient::setPeerName(const std::string& peer)
{
    _client.setPeerName(peer);
}


void HttpClient::setKeepAlive()
{
    Pt::Http::MessageHeader& header = _client.request().header();
    header.set("Connection", "keep-alive");
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


Pt::Http::Request& HttpClient::request()
{
    return _client.request();
}


Pt::Http::Reply& HttpClient::reply()
{
    return _client.reply();
}


void HttpClient::close()
{
    // TODO: this could be part of the Remoting::Client interface

    cancel();
}


void HttpClient::onBeginInvoke()
{
    _error = false;
    _client.request().discard();
    
    // format XML-RPC request
    std::ostream& os = _client.request().body();
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


void HttpClient::onInvoke()
{
    _error = false;   
    _client.request().discard();
    
    // format XML-RPC request
    std::ostream& os = _client.request().body();
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


void HttpClient::onEndInvoke()
{
    if( _error )
    {
        _error = false;
        throw;
    }

    // TODO: an alternative might be to throw AccessFailed for 401 replies
    //       or handle these in a special way. Also consider throwing a 
    //       HttpError in Http::Client if statusCode > 400
    if( _client.reply().statusCode() > 300)
        throw Remoting::Fault("request failed");
}


bool HttpClient::isFailed() const
{
    return _error || Client::isFailed();
}


void HttpClient::onCancel()
{
    Client::onCancel();

    _error = false;
    _client.close();
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
    catch(...)
    {
        // TODO: move _error flag to Soap::Client and join the two methods
        // setError() and setReady(), so that after setReady() it can be checked
        // whether the used called onEndInvoke() or onEndCall()
        //
        // see Pt::Soap::HttpClient

        // defer throw until onEndInvoke() is called
        setError();
        setReady();
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
    catch(...)
    {
        // TODO: move _error flag to Soap::Client and join the two methods
        // setError() and setReady(), so that after setReady() it can be checked
        // whether the used called onEndInvoke() or onEndCall()
        //
        // see Pt::Soap::HttpClient

        // defer throw until onEndInvoke() is called
        setError();
        setReady();
        return;
    }

    // send finished signal
    setReady();
}

} // namespace XmlRpc

} // namespace Pt
