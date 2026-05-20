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

#include <Pt/JsonRpc/HttpClient.h>
#include <Pt/JsonRpc/Fault.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Http/HttpError.h>
#include <Pt/Remoting/Fault.h>
#include <sstream>
#include <limits>

namespace Pt {

namespace JsonRpc {

HttpClient::HttpClient()
: _error(false)
{
    init();
}


HttpClient::HttpClient(const Net::Endpoint& ep, const std::string& url)
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
    _client.request().header().set("Content-Type", "application/json");
    _client.request().setMethod("POST");

    _client.requestSent() += Pt::slot(*this, &HttpClient::onRequest);
    _client.replyReceived() += Pt::slot(*this, &HttpClient::onReply);
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
    Http::MessageHeader& header = _client.request().header();
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


Http::Request& HttpClient::request()
{
    return _client.request();
}


Http::Reply& HttpClient::reply()
{
    return _client.reply();
}


void HttpClient::close()
{
    cancel();
}


void HttpClient::onBeginInvoke()
{
    _error = false;
    _client.request().discard();

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

    _client.send(true);
    std::istream& is = _client.receive();

    Client::processResult(is);

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

    if( _client.reply().statusCode() > 300 )
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
        Http::MessageProgress progress = client.endSend();
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
            beginResult( client.reply().body() );
        }

        if( progress.body() )
        {
            parseResult();
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
        setError();
        setReady();
        return;
    }

    setReady();
}

} // namespace JsonRpc

} // namespace Pt
