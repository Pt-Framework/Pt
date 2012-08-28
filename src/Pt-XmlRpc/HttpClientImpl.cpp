/*
 * Copyright (C) 2009 by Tommi Meakitalo
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

#include "HttpClientImpl.h"
#include "Pt/Http/ReplyHeader.h"
#include "Pt/Net/AddrInfo.h"
#include <Pt/System/Logger.h>

log_define("Pt.XmlRpc.HttpClient")

namespace Pt {

namespace XmlRpc {

HttpClientImpl::HttpClientImpl()
: _error(false)
{
    _client.request().header().method("POST");
    _client.replyReceived() += Pt::slot( *this, &HttpClientImpl::onReply);
}


HttpClientImpl::HttpClientImpl(System::EventLoop& selector, const std::string& addr,
       unsigned short port, const std::string& url)
: _client(selector, addr, port)
, _error(false)
{
    _client.request().header().method("POST");
    _client.request().header().url(url);
    _client.replyReceived() += Pt::slot( *this, &HttpClientImpl::onReply);
}


HttpClientImpl::HttpClientImpl(const std::string& addr, unsigned short port, const std::string& url)
: _client(addr, port)
, _error(false)
{
    _client.request().header().method("POST");
    _client.request().header().url(url);
    _client.replyReceived() += Pt::slot( *this, &HttpClientImpl::onReply);
}


std::string HttpClientImpl::url() const
{
    std::ostringstream s;
    s << "http://"
      << _client.host().host()
      << ':'
      << _client.host().port()
      << _client.request().header().url();

    return s.str();
}


void HttpClientImpl::onReply(Http::Client& client)
{
    try
    {
        bool received = client.endReceive();
        if(received)
        {
            verifyHeader(client.replyHeader());
            ClientImpl::onReadReplyBegin(client.reply());
        }

        if( client.reply().rdbuf()->in_avail() )
        {
            ClientImpl::onReadReply();
        }

        if( client.isEnd() )
        {
            ClientImpl::onReplyFinished();
            return;
        }

        client.beginReceive();
    }
    catch(const std::exception& ex)
    {
        _error = true;
        ClientImpl::onReplyFinished();
    }
}


void HttpClientImpl::beginExecute()
{
    _client.beginReceive();
}


void HttpClientImpl::endExecute()
{
    if( _errorPending || _error) 
    {
        _error = false;
        throw;
    }
}


std::string HttpClientImpl::execute()
{
    _client.setTimeout( timeout() );
    _client.send();
    std::istream& is = _client.receive();

    std::string body;

    try
    {
        verifyHeader( _client.replyHeader() );

        char ch = ' ';
        while( is.get(ch) )
            body += ch;
    }
    catch (...)
    {
        _client.cancel();
        throw;
    }

    return body;
}


std::ostream& HttpClientImpl::prepareRequest()
{
    _client.request().clear();
    _client.request().header().setHeader("Content-Type", "text/xml");
    _client.request().header().method("POST");
    return _client.request().body();
}


void HttpClientImpl::cancel()
{
    _error = false;
    _client.cancel();
    ClientImpl::cancel();
}


void HttpClientImpl::verifyHeader(const Http::ReplyHeader& header)
{
    if (header.httpReturnCode() != 200)
    {
        std::ostringstream msg;
        msg << "invalid http return code "
            << header.httpReturnCode()
            << ": "
            << header.httpReturnText();
        throw std::runtime_error(msg.str());
    }

    if (!header.isHeaderValue("Content-Type", "text/xml"))
    {
        std::ostringstream msg;
        msg << "invalid content type " << header.getHeader("Content-Type");
        throw std::runtime_error(msg.str());
    }

}

}

}
