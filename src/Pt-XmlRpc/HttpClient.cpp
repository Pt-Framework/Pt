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
#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <sstream>

namespace Pt {

namespace XmlRpc {

class HttpClientImpl
{
    public:
        HttpClientImpl()
        : _client()
        , _error(false)
        { }

        HttpClientImpl(System::EventLoop& loop)
        : _client(loop)
        , _error(false)
        { }

        Http::Client& client()
        { return _client; }

        void setError(bool err = true)
        { _error = err; }

        bool isError() const
        { return _error; }

        static void verifyHeader(const Http::Reply& reply)
        {
            if (reply.statusCode() != 200)
            {
                std::ostringstream msg;
                msg << "invalid http return code "
                    << reply.statusCode()
                    << ": "
                    << reply.statusText();
                throw std::runtime_error(msg.str());
            }

            if (! reply.header().isSet("Content-Type", "text/xml"))
            {
                std::ostringstream msg;
                const char* ct = reply.header().get("Content-Type");
                msg << "invalid content type " << (ct ? ct : "");
                throw std::runtime_error(msg.str());
            }

        }

    private:
        Http::Client _client;
        bool _error;
};


HttpClient::HttpClient()
: _impl(new HttpClientImpl())
{
    _impl->client().request().setMethod("POST");
    _impl->client().replyReceived() += Pt::slot( *this, &HttpClient::onReply);
}


HttpClient::HttpClient(const Net::AddrInfo& addrinfo, 
                       const std::string& url)
: _impl(new HttpClientImpl())
{
    _impl->client().request().setMethod("POST");
    _impl->client().replyReceived() += Pt::slot( *this, &HttpClient::onReply);
    connect(addrinfo, url);
}


HttpClient::HttpClient(const std::string& addr, unsigned short port, const std::string& url)
: _impl( new HttpClientImpl() )
{
    _impl->client().request().setMethod("POST");
    _impl->client().replyReceived() += Pt::slot( *this, &HttpClient::onReply);
    connect(addr, port, url);
}


HttpClient::HttpClient(System::EventLoop& loop)
: _impl( new HttpClientImpl(loop) )
{
    _impl->client().request().setMethod("POST");
    _impl->client().replyReceived() += Pt::slot( *this, &HttpClient::onReply);
}


HttpClient::HttpClient(System::EventLoop& loop, const Net::AddrInfo& addrinfo,
                       const std::string& url)
: _impl( new HttpClientImpl(loop) )
{
    _impl->client().request().setMethod("POST");
    _impl->client().replyReceived() += Pt::slot( *this, &HttpClient::onReply);
    connect(addrinfo, url);
}


HttpClient::HttpClient(System::EventLoop& loop, 
                       const std::string& addr, unsigned short port, 
                       const std::string& url)
: _impl( new HttpClientImpl(loop) )
{
    _impl->client().request().setMethod("POST");
    _impl->client().replyReceived() += Pt::slot( *this, &HttpClient::onReply);
    connect(addr, port, url);
}


HttpClient::~HttpClient()
{
    delete _impl;
}


void HttpClient::setActive(System::EventLoop& loop)
{
    _impl->client().setActive(loop);
}


void HttpClient::connect(const Net::AddrInfo& addrinfo, const std::string& url)
{
    _impl->client().setHost(addrinfo);
    _impl->client().request().setUrl(url);
}


void HttpClient::connect(const std::string& addr, unsigned short port, const std::string& url)
{
    _impl->client().setHost(addr, port);
    _impl->client().request().setUrl(url);
}


//std::string HttpClient::url() const
//{
//    std::ostringstream s;
//    s << "http://"
//      << _client.host().host()
//      << ':'
//      << _client.host().port()
//      << _client.request().url();
//
//    return s.str();
//}
//
//
//const std::string& HttpClient::url() const
//{
//    return _impl->client().request().url();
//}
//
//
//void HttpClient::setUrl(const std::string& url)
//{
//    _impl->client().request().setUrl(url);
//}


Http::Client& HttpClient::client()
{
    return _impl->client();
}


void HttpClient::onBeginCall()
{
    // prepare HTTP request
    _impl->client().request().clear();
    _impl->client().request().header().set("Content-Type", "text/xml");
    _impl->client().request().setMethod("POST");
    std::ostream& os = _impl->client().request().body();

    // format XML-RPC request
    Client::formatRequest(os);

    _impl->client().beginReceive();
}


void HttpClient::onEndCall()
{
    if( _impl->isError() ) 
    {
        _impl->setError(false);
        throw;
    }
}


void HttpClient::onCall()
{
    // prepare HTTP request
    _impl->client().request().clear();
    _impl->client().request().header().set("Content-Type", "text/xml");
    _impl->client().request().setMethod("POST");
    std::ostream& os = _impl->client().request().body();

    // format XML-RPC request
    Client::formatRequest(os);

    // send HTTP request and start receiving HTTP reply
    _impl->client().send();
    std::istream& is = _impl->client().receive();

    // parse XML-RPC reply
    Client::readReply(is);
}


void HttpClient::onCancel()
{
    _impl->setError(false);
    _impl->client().cancel();
}


void HttpClient::onReply(Http::Client& client)
{
    try
    {
        Http::MessageProgress progress = client.endReceive();

        if( progress.header() )
        {
            //_impl->verifyHeader( client.reply() );
            
            Client::beginReply( client.reply().body() );
        }

        if( progress.body() )
        {
            bool ok = Client::advanceReply();
            if( ! ok )
            {
                // send finished signal
                Client::finishReply();
                return;
            }
        }

        if( progress.finished() )
        { 
            // send finished signal
            Client::finishReply();
        }
        else
        {
            client.beginReceive();
        }
    }
    catch(...)
    {
        _impl->setError();
        
        // send finished signal
        Client::finishReply();

        // throw if error not handled in slot
        if( _impl->isError() )
        {
            _impl->setError(false);
            throw;
        }
    }   
}

} // namespace XmlRpc

} // namespace Pt
