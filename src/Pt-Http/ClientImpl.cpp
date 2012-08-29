/*
 * Copyright (C) 2009 by Marc Boris Duerner, Tommi Maekitalo
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

#include "ClientImpl.h"
#include <Pt/Http/Client.h>
#include "Parser.h"
#include <Pt/System/IOError.h>
#include <Pt/System/Logger.h>
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <sstream>
#include <cassert>

log_define("Pt.Http.Client")

namespace Pt {

namespace Http {

ClientImpl::ClientImpl(Client* client)
: _client(client)
, _hstate(Idle)
{
    init();
}


ClientImpl::ClientImpl(Client* client, const Net::AddrInfo& addrinfo, bool ssl)
: _client(client)
, _hstate(Idle)
{
    init();
    setHost(addrinfo, ssl);
}


ClientImpl::ClientImpl(Client* client, System::EventLoop& loop, const Net::AddrInfo& addrinfo, bool ssl)
: _client(client)
, _hstate(Idle)

{
    setActive(loop);
    init();
    setHost(addrinfo, ssl);
}


void ClientImpl::init()
{
    _req.init(_conn);
    _req.outputSent() += Pt::slot(*this, &ClientImpl::onRequestSent);

    _reply.init(_conn);
    _reply.inputReceived() += Pt::slot(*this, &ClientImpl::onReplyReceived);
}


#ifdef PT_HTTP_WITH_SSL

void ClientImpl::setContext(Ssl::Context& ctx)
{
    _conn.setContext(ctx);
}

#else

void ClientImpl::setContext(Ssl::Context& )
{
}

#endif


void ClientImpl::send()
{
}


std::istream& ClientImpl::receive()
{
    return _reply.body();
}


void ClientImpl::onRequestSent(Request& r)
{
    log_trace("onRequestSent: " << _hstate);

    if(_hstate == OnRequestEnd)
        _client->replyReceived().send(*_client);
    else
        _client->requestSent().send(*_client);
}


void ClientImpl::onReplyReceived(Reply& r)
{
    log_trace("onReplyReceived: " << _hstate);
    _client->replyReceived().send(*_client);
}


void ClientImpl::beginSend()
{
    log_trace("beginSend: " << _hstate);
    if(_hstate == Idle)
    {
        // TODO: connect
        _hstate = OnRequest;
    }

    if(_hstate == OnRequest)
    {
        if( _req.size() < 8192)
        {
            log_debug("caching request data");
            _client->requestSent().send(*_client);
            return;
         }

        log_debug("begin sending http chunk");
        _req.beginSend();
        return;
    }

    log_error("sending HTTP request failed: " << _hstate);
    throw System::IOPending("sending HTTP request failed");
}


bool ClientImpl::endSend()
{
    log_trace("endSend: " << _hstate);

    if(_hstate == OnRequest)
    {
        if(_req.size() < 8192)
        {
            log_debug("cached request data not yet sent");
            return true;
        }
        else
        {
            log_debug("sent http request");
            bool complete = _req.endSend();
            if(complete)
            {
                log_debug("sent http request completed");
                _req.clearBody();
            }
            
            return complete;
        }
    }

    return false;
}


void ClientImpl::beginReceive()
{
    log_debug("beginReceive: " << _hstate);
    
    if(_hstate == Idle)
    {
        // TODO: connect
        _hstate = OnRequest;
    }
 
    if(_hstate == OnRequest)
    {
        log_debug("begin sending cached request");

        _req.finish();
        _hstate = OnRequestEnd;
    }

    if(_hstate == OnRequestEnd)
    {
        log_debug("flushing cached request");
        _req.beginSend();
        return;
    }

    if(_hstate == OnReply)
    {
        _reply.beginReceive();
        return;
    }

    throw System::IOPending("failed receiving HTTP request");
}


bool ClientImpl::endReceive()
{
    log_debug("endReceive: " << _hstate);

    if(_hstate == OnRequestEnd)
    {
        log_debug("flushed cached request");
        bool completed = _req.endSend();
        
        if(completed)
        {
            log_debug("request completed");
            _hstate = OnReply;
            _req.clear();
            _reply.clear();
        }
        
        return false;
    }
    
    if(_hstate == OnReply)
    {   
        log_debug("advancing reply");
        bool receivedHeader = _reply.endReceive();

        if( _reply.isEnd() )
        {
            log_debug("reply completed");
            _hstate = Idle;

            if( ! _conn.isConnected() )
            {
                log_debug("connection closed");
            }
        }

        return receivedHeader;
    }

    return false;
}


bool ClientImpl::isEnd() const
{
    return _hstate == Idle;
}


void ClientImpl::cancel()
{
    _hstate = Idle;
}

} // namespace Http

} // namespace Pt

/*void ClientImpl::beginSend()
{
    log_trace("beginSend: " << _hstate);

    if( _hstate == Idle )
    {
        _stream.rdbuf( _httpbuf.buffer() );

        if( ! _socket.isConnected() )
        {
            log_debug("opening new connection to " << _addrInfo.host());
            _socket.beginConnect(_addrInfo);
            _hstate = OnConnect;
            return;
        }

        log_debug("reusing previous connection");
        _hstate = OnRequest;
    }

    if(_hstate == OnRequest)
    {
        log_debug("preparing request");

        if( _req.size() < 8192)
        {
            _client->requestSent().send(*_client);
            return;
        }

        log_debug("begin sending http chunk");
        _hstate = OnChunkedRequest;
        sendChunked(_stream, _req);
    }

    if(_hstate == OnSslHandshake)
    {
#ifdef PT_HTTP_WITH_SSL
        log_debug("begining SSL handshake");
        _sslbuf.beginConnect();
        return;
#endif
    }
    
    if(_hstate == OnChunkedRequest)
    {
        log_debug("sending http chunk");

        _stream << std::hex << _req.size() << std::dec << "\r\n";
        _stream.write( _req.data(), _req.size() );
        _stream.write("\r\n", 2);
        _req.clearBody();

        beginWrite();
        return;
    }

    log_error("pending http reply: " << _hstate);
    throw System::IOPending("pending HTTP reply");
}

void ClientImpl::endSend()
{
    log_trace("endSend: " << _hstate);

    if(_hstate == OnConnect)
    {
        log_debug("ending socket connect");
        _socket.endConnect();
            
        if( ! _ssl)
            _hstate = Idle;
        else
            _hstate = OnSslHandshake;

        return;
    }

    if(_hstate == OnSslHandshake)
    {
        log_debug("ending ssl connect");

#ifdef PT_HTTP_WITH_SSL
        _sslbuf.endHandshake();
#endif
        _hstate = Idle;
        return;
    }

    if(_hstate == OnRequest)
    {
        return;
    }
    
    if(_hstate == OnChunkedRequest)
    {
        log_debug("sent http request");
        endWrite();
        return;
    }
}

void ClientImpl::beginReceive()
{
    log_debug("beginReceive: " << _hstate);

    if( _hstate == Idle )
    {
        _stream.rdbuf( _httpbuf.buffer() );

        if( ! _socket.isConnected() )
        {
            log_debug("opening new connection to " << _addrInfo.host());
            _socket.beginConnect(_addrInfo);
            _hstate = OnConnectReceive;
            return;
        }

        log_debug("reusing previous connection");
        _hstate = OnRequest;
    }

    if(_hstate == OnSslHandshakeReceive)
    {
#ifdef PT_HTTP_WITH_SSL
        log_debug("begining SSL handshake");
        _sslbuf.beginConnect();
        return;
#endif
    }

    if(_hstate == OnRequest)
    {
        log_debug("begin sending cached request");
        sendRequest(_stream, _req);
        _req.clearBody();
        _hstate = OnRequestEnd;

        log_debug("begin write: " << outputAvailable());
        beginWrite();
        return;
    }

    if(_hstate == OnRequestEnd)
    {
        bool remaining = outputAvailable();
        if(remaining)
        {
            beginWrite();
            return;
        }

        log_debug("flushing cached request");
        _stream.rdbuf( _httpbuf.buffer() );
        _replyHeader.clear();
        _parser.reset(true);
        _hstate = OnReplyHeader;
    }

    if(_hstate == OnChunkedRequest)
    {
        log_debug("sending http chunk");

        if(_req.size() > 0)
        {
            _stream << std::hex << _req.size() << std::dec << "\r\n";
            _stream.write( _req.data(), _req.size() );
            _stream.write("\r\n", 2);
            _req.clearBody();
        }
        else
        {
            _stream.write("0\r\n\r\n", 5);
        }

        _hstate = OnRequestEnd;
        beginWrite();
        return;
    }

    if(_hstate == OnReply || _hstate == OnReplyHeader)
    {
        beginRead();
    }
    else
        throw System::IOPending("pending HTTP request");
}

void ClientImpl::onConnect2(Net::TcpSocket& socket)
{
    log_trace("onConnect2");
    if(_hstate == OnConnectReceive)
        _client->replyReceived().send(*_client);
    else
        _client->requestSent().send(*_client);
}


void ClientImpl::onInput2(System::StreamBuffer& sb)
{
    log_trace("onInput2");
    _client->replyReceived().send(*_client);
}


void ClientImpl::onOutput2(System::StreamBuffer& sb)
{
    log_trace("onOutput2");
    if(_hstate == OnRequestEnd)
        _client->replyReceived().send(*_client);
    else
        _client->requestSent().send(*_client);
}

void ClientImpl::onSslHandshake2(Ssl::IOBuffer& sb)
{
    if(_hstate == OnSslHandshakeReceive)
        _client->replyReceived().send(*_client);
    else
        _client->requestSent().send(*_client);
}


void ClientImpl::onSslInput2(Ssl::IOBuffer& sb)
{
    _client->replyReceived().send(*_client);
}


void ClientImpl::onSslOutput2(Ssl::IOBuffer& sb)
{
    log_trace("onOutput2");
    if(_hstate == OnRequestEnd)
        _client->replyReceived().send(*_client);
    else
        _client->requestSent().send(*_client);
}

bool ClientImpl::endReceive()
{
    log_trace("endReceive");

    if(_hstate == OnConnectReceive)
    {
        log_debug("ending socket connect");
        _socket.endConnect();
            
        if( ! _ssl)
            _hstate = OnRequest;
        else
            _hstate = OnSslHandshakeReceive;

        return false;
    }

    if(_hstate == OnSslHandshakeReceive)
    {
        log_debug("ending ssl connect");

#ifdef PT_HTTP_WITH_SSL
        _sslbuf.endHandshake();
#endif
        _hstate = OnRequest;
        return false;
    }

    if(_hstate == OnRequestEnd)
    {
        endWrite();
        return false;
    }

    endRead();
    
    return processReply();
}


bool ClientImpl::processReply()
{
    log_trace("processReply");

    if(_hstate == OnReplyHeader)
    {
        log_debug("processing reply header");

        // TODO: reconnect when received pending EOF from previous response
        if( _sockbuf.device()->eof() )
        {
            throw System::IOError("unexpected EOF");
        }

        log_debug("advancing parser: " << _stream.rdbuf()->in_avail());
        _parser.advance(_stream);

        if( _parser.fail() )
        {
            log_warn("http parser failed");
            throw std::runtime_error("http parser failed"); // TODO define exception class
        }

        if( ! _parser.end() )
        {
            log_debug("continue reading");
            return false;
        }
    
        log_debug("http header complete");
        _stream.rdbuf(&_httpbuf);
        _httpbuf.beginBody(_replyHeader);
        _hstate = OnReply;
        return true;
    }

    if(_hstate != OnReply)
        throw System::IOPending("not receiving HTTP reply");

    log_debug("processing reply body");

    _httpbuf.import();
    log_debug("available: " << _httpbuf.in_avail());

    if( ! _httpbuf.isEnd() || _httpbuf.in_avail() > 0)
        return false;

    _hstate = Idle;

    if( ! _req.header().keepAlive() )
    {
        log_debug("cancelling, no keep alive");
        this->cancel();
    }

    log_debug("http reply finished");
    return false;
}


bool ClientImpl::isEnd() const
{
    return _hstate == Idle;
}

*/
