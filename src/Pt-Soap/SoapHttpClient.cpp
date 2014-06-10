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

#include <Pt/Soap/SoapHttpClient.h>
#include <Pt/Soap/Fault.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/HttpError.h>

namespace Pt {

namespace Soap {

SoapHttpClient::SoapHttpClient(SoapServiceDeclaration& service)
: SoapClient(service)
, _error(false)
{
    init();
}


SoapHttpClient::SoapHttpClient(SoapServiceDeclaration& service, System::EventLoop& loop)
: SoapClient(service)
, _client(loop)
, _error(false)
{
    init();
}


SoapHttpClient::~SoapHttpClient()
{
}


void SoapHttpClient::init()
{
    _client.requestSent() += Pt::slot(*this, &SoapHttpClient::onRequest);
    _client.replyReceived() += Pt::slot( *this, &SoapHttpClient::onReply);
}


void SoapHttpClient::setActive(System::EventLoop& loop)
{
    _client.setActive(loop);
}


System::EventLoop* SoapHttpClient::loop() const
{
    return _client.loop();
}


void SoapHttpClient::setSecure(Ssl::Context& ctx)
{
    _client.setSecure(ctx);
}


void SoapHttpClient::setTimeout(std::size_t timeout)
{
    _client.setTimeout(timeout);
}


void SoapHttpClient::setTarget(const Net::Endpoint& ep, const std::string& url)
{
    _client.setHost(ep);
    _client.request().setUrl(url);
}


void SoapHttpClient::setTarget(const Net::Endpoint& ep, const Net::TcpSocketOptions& opts,
                           const std::string& url)
{
    _client.setHost(ep, opts);
    _client.request().setUrl(url);
}


void SoapHttpClient::setHost(const Net::Endpoint& ep)
{
    _client.setHost(ep);
}


void SoapHttpClient::setHost(const Net::Endpoint& ep, const Net::TcpSocketOptions& opts)
{
    _client.setHost(ep, opts);
}


void SoapHttpClient::setServiceUrl(const std::string& url)
{
    _client.request().setUrl(url);
}


void SoapHttpClient::setServiceUrl(const char* url)
{
    _client.request().setUrl(url);
}


const Net::Endpoint& SoapHttpClient::host() const
{
    return _client.host();
}


void SoapHttpClient::onBeginInvoke()
{
    _error = false;

    //--->
    // prepare HTTP request
    _client.request().clear();
    _client.request().header().set("Content-Type", "text/xml");
    _client.request().setMethod("POST");
    _client.request().header().set("SOAPAction", ""); // TODO: use targetNamespace/MethodName
    std::ostream& os = _client.request().body();
    //---<
    
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


void SoapHttpClient::onInvoke()
{
    _error = false;

    //--->
    // prepare HTTP request
    _client.request().clear();
    _client.request().header().set("Content-Type", "text/xml");
    _client.request().setMethod("POST");
    _client.request().header().set("SOAPAction", ""); // TODO: use targetNamespace/MethodName
    std::ostream& os = _client.request().body();
    //---<

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
    SoapClient::processResult(is);

    // discard remaining data
    std::streamsize all = std::numeric_limits<std::streamsize>::max();
    is.ignore(all);
}


void SoapHttpClient::onEndInvoke()
{
    if( _error )
    {
        _error = false;
        throw;
    }
}


bool SoapHttpClient::isFailed() const
{
    return _error || SoapClient::isFailed();
}


void SoapHttpClient::onCancel()
{
    SoapClient::onCancel();

    _error = false;
    _client.cancel();
}


void SoapHttpClient::onRequest(Http::Client& client)
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
        // defer throw until onEndInvoke() is called
        setError();
        setReady();
    }
}


void SoapHttpClient::onReply(Http::Client& client)
{
    try
    {
        Http::MessageProgress progress = client.endReceive();

        if( progress.header() )
        {
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
        // defer throw until onEndInvoke() is called
        setError();
        setReady();
        return;
    }

    // send finished signal
    setReady();
}

} // namespace Soap

} // namespace Pt
