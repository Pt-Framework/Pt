/*
 * Copyright (C) 2011 by Marc Boris Duerner
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


#include "Connection.h"
#include <Pt/Http/Request.h>
#include <Pt/Http/Server.h>
#include <Pt/Http/Responder.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Logger.h>
#include <cassert>

log_define("Pt.Http.Server")

namespace Pt {

namespace Http {

void Connection::ParseEvent::onMethod(const std::string& method)
{
    _request.method(method);
}


void Connection::ParseEvent::onUrl(const std::string& url)
{
    _request.url(url);
}


void Connection::ParseEvent::onUrlParam(const std::string& q)
{
    _request.qparams(q);
}


Connection::Connection(Server& server, Net::TcpServer& tcpServer)
: _server(server)
, _parseEvent(_request)
, _parser(_parseEvent, false)
, _loop(0)
, _chunkedTransfer(true)
, _responder(0)
, _ssl(false)
#ifdef PT_HTTP_WITH_SSL
, _sslbuf( _sockbuf )
#endif
, _httpbuf(*this)
, _stream(&_httpbuf)
, _reply(_stream)
{
    Net::TcpSocket::accept(tcpServer);
}


Connection::~Connection()
{
    if(_responder)
    {
        _responder->release();
    }
}


void Connection::begin(System::EventLoop& loop, Ssl::Context* ctx)
{
    _reply.init(*this);
    _reply.clear();
    
    _timer.timeout() += Pt::slot(*this, &Connection::onTimeout);
    _timer.setActive(loop);
    _timer.start( _server.readTimeout() );

    _loop = &loop;
    this->setActive(loop);
    
    _sockbuf.attach(*this);

#ifdef PT_HTTP_WITH_SSL
    _ssl = ctx != 0;

    if(_ssl)
    {
        log_debug("beginning HTTPS connection");
        _sslbuf.init(*ctx);
        _sslbuf.handshakeFinished() += slot(*this, &Connection::onHttpsHandshake);
        _sslbuf.outputReady() += slot(*this, &Connection::onHttpsOutput);
        _sslbuf.inputReady() += slot(*this, &Connection::onHttpsInput);

        _httpbuf.attach(_sslbuf);
        _sslbuf.beginAccept();
        return;
    }
#endif
        log_debug("beginning HTTP connection");
        _sockbuf.inputReady() += Pt::slot(*this, &Connection::onHttpInput);
        _sockbuf.outputReady() += Pt::slot(*this, &Connection::onHttpOutput);

        _httpbuf.attach(_sockbuf);
        _sockbuf.beginRead();
}

#ifdef PT_HTTP_WITH_SSL
void Connection::onHttpsHandshake(Pt::Ssl::IOBuffer& ssl)
{
    log_trace("Connection::onAcceptHandshake");
    try 
    {
        ssl.endHandshake();
    }
    catch(...) 
    {
        log_error("accept handshake failed");
        return;
    }

    log_debug("peer name = " << ssl.peerName());
    log_debug("current cipher = " << ssl.currentCipher().name());
    ssl.beginRead();
}

void Connection::onHttpsInput(Pt::Ssl::IOBuffer& ssl)
{
    log_trace("Connection::onHttpsInput");
    processInput();
}

void Connection::onHttpsOutput(Pt::Ssl::IOBuffer& ssl)
{
    processOutput();
}
#endif


void Connection::onHttpInput(System::StreamBuffer& sb)
{
    processInput();
}


void Connection::onHttpOutput(System::StreamBuffer& sb)
{
    processOutput();
}


void Connection::beginRead()
{
#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
        if( _sslbuf.in_avail() )
            onHttpsInput(_sslbuf);
        else
            _sslbuf.beginRead();
    else
#endif
        if( _sockbuf.in_avail() )
            onHttpInput(_sockbuf);
        else
            _sockbuf.beginRead();
}


void Connection::endRead()
{
#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
        _sslbuf.endRead();
    else
#endif
        _sockbuf.endRead();
}


bool Connection::beginWrite()
{
#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
    {
        // TODO: need to check _sslbuf.out_avail()
        _sslbuf.pubsync();

        if ( _sslbuf.buffer().out_avail() )
        {
            _sslbuf.beginWrite();
            _timer.start(_server.writeTimeout());
            return true;
        }
        
        return false;
    }
#endif
    if ( _sockbuf.out_avail() )
    {
        _sockbuf.beginWrite();
        _timer.start(_server.writeTimeout());
        return true;
    }

    return false;
}


void Connection::endWrite()
{
#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
        _sslbuf.endWrite();
    else
#endif
        _sockbuf.endWrite();
}


void Connection::processInput()
{
    // TODO: handle exceptions correctly...

    endRead();

    if (_httpbuf.buffer()->in_avail() == 0 || Net::TcpSocket::eof())
    {
        close();
        timeout(*this);
        return;
    }

    _timer.start(_server.readTimeout());

    if ( _responder == 0 )
    {
        _parser.advance( *_httpbuf.buffer() );

        if( _parser.fail() )
        {
            _responder = _server.getDefaultResponder(_request);
            replyError();
            return;
        }

        if( _parser.end() )
        {
            _responder = _server.getResponder(_request);

            // new code using HttpBuffer:
            _httpbuf.beginBody(_request);

            _responder->beginRequest(_stream, _request);

            //TODO: allow immediate reply from this method
        }
        else
        {
            beginRead();
        }
    }

    if (_responder)
    {
        // new code using HttpBuffer:
        _httpbuf.import();

        log_debug("available: " << _httpbuf.in_avail());

        while( _httpbuf.in_avail() )
        {
            // TODO: readBody could write to _reply
            _responder->readRequest(_stream, _reply);
            
            if( _reply.finished() )
            {
                return;
            }

            _httpbuf.import();
            log_debug("available: " << _httpbuf.in_avail());
        } 
    
        if( _stream.fail() )
            throw System::IOError( PT_ERROR_MSG("error reading HTTP reply body") );

        if( _httpbuf.isEnd() )
        {
            log_debug("request body finished");
            _timer.stop();

            _httpbuf.reset();
            _responder->beginReply(_request, _reply);
        }
        else
        {
            log_debug("continue reading body");
            beginRead();
        }
    }
}


void Connection::processOutput()
{
    // TODO: handle exceptions correctly...

    if( _responder && _reply.finished() )
    {
        _responder->release();
        _responder = 0;
    }

    try
    {
        endWrite();

        if( beginWrite() )
            return;

        if( ! _reply.finished() )
        {
            _responder->writeReply(_request, _reply);
            return;
        }
  
        bool keepAlive = _request.keepAlive() && _reply.header().keepAlive();
        if(keepAlive)
        {
            log_debug("do keep alive");
            _timer.start(_server.keepAliveTimeout());
            _request.clear();
            _reply.clear();
            _parser.reset(false);

            beginRead();
            return;
        }
    }
    catch (const std::exception& e)
    {
        log_warn("exception occured when processing request: " << e.what());
    }

    close();
    timeout(*this);
}


void Connection::advanceReply()
{
    if( ! _chunkedTransfer)
    {
        _responder->writeReply(_request, _reply);
        return;
    }

    _reply.header().setHeader("Transfer-Encoding", "chunked");

   _httpbuf.writeReply( _reply.header() );
    
    beginWrite();
    _timer.start( _server.writeTimeout() );
}


void Connection::finishReply()
{
    _httpbuf.finishReply( _reply.header() );
    
    beginWrite();
    _timer.start(_server.writeTimeout());
}


void Connection::replyError()
{
    _reply.header().httpReturn(500, "internal server error");
    _reply.header().setHeader("Content-Type", "text/plain");
    _reply.header().setHeader("Connection", "close");
    _stream << "Error 500: Internal server error.";

    finishReply();
}


void Connection::onTimeout()
{
    //log_debug("timeout");
    timeout(*this);
}

} // namespace Http

} // namespace Pt
