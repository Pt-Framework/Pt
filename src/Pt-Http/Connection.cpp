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
, _responder(0)
, _ssl(false)
#ifdef PT_HTTP_WITH_SSL
, _sslbuf( _sockbuf )
#endif
, _httpbuf(*this)
, _stream(0)
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
        _stream.rdbuf(&_sslbuf);
        _sslbuf.beginAccept();
        return;
    }
#endif
        log_debug("beginning HTTP connection");
        _sockbuf.inputReady() += Pt::slot(*this, &Connection::onHttpInput);
        _sockbuf.outputReady() += Pt::slot(*this, &Connection::onHttpOutput);

        _httpbuf.attach(_sockbuf);
        _stream.rdbuf(&_sockbuf);
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

    if (_stream.rdbuf()->in_avail() == 0 || Net::TcpSocket::eof())
    {
        close();
        timeout(*this);
        return;
    }

    _timer.start(_server.readTimeout());

    if ( _responder == 0 )
    {
        _parser.advance(_stream);

        if( _parser.fail() )
        {
            _responder = _server.getDefaultResponder(_request);
            replyError();
            return;
        }

        if( _parser.end() )
        {
            _responder = _server.getResponder(_request);
            _responder->beginRequest(_stream, _request);
            _contentLength = _request.contentLength();

            //log_debug("content length of request is " << _contentLength);
            if (_contentLength == 0)
            {
                _timer.stop();
                _responder->beginReply(_reply.body(), _request, _reply);
                return;
            }

            // new code using HttpBuffer:
            _stream.rdbuf(&_httpbuf);
            _httpbuf.beginBody(_request);
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
            _responder->readBody(_stream, _reply);
            // TODO: readBody could write to _reply

            if( _reply.finished() )
            {
                _stream.rdbuf( _httpbuf.buffer() );
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

            _stream.rdbuf( _httpbuf.buffer() );
            _responder->beginReply(_reply.body(), _request, _reply);
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

    if(_responder)
    {
        _responder->release();
        _responder = 0;
    }

    try
    {
        endWrite();

        if( ! beginWrite() )
        {
            bool keepAlive = _request.keepAlive() && _reply.header().keepAlive();

            if(keepAlive)
            {
                //log_debug("do keep alive");
                _timer.start(_server.keepAliveTimeout());
                _request.clear();
                _reply.clear();
                _parser.reset(false);

                beginRead();
            }
            else
            {
                //log_debug("don't do keep alive");
                close();
                timeout(*this); // TODO: notify server that socket is done
            }
        }
    }
    catch (const std::exception& e)
    {
        //log_warn("exception occured when processing request: " << e.what());
        close();
        timeout(*this);
    }
}


void Connection::endReply()
{
    const char* contentLength = "Content-Length";
    const char* server = "Server";
    const char* connection = "Connection";
    const char* date = "Date";

    _stream << "HTTP/"
        << _reply.header().httpVersionMajor() << '.'
        << _reply.header().httpVersionMinor() << ' '
        << _reply.header().httpReturnCode() << ' '
        << _reply.header().httpReturnText() << "\r\n";

    for (ReplyHeader::const_iterator it = _reply.header().begin();
        it != _reply.header().end(); ++it)
    {
        _stream << it->first << ": " << it->second << "\r\n";
    }

    if (!_reply.header().hasHeader(contentLength))
    {
        _stream << "Content-Length: " << _reply.bodySize() << "\r\n";
    }

    if (!_reply.header().hasHeader(server))
    {
        _stream << "Server: Pt-Net-Server\r\n";
    }

    if (!_reply.header().hasHeader(connection))
    {
        _stream << "Connection: "
                << (_request.keepAlive() ? "keep-alive" : "close")
                << "\r\n";
    }

    if (!_reply.header().hasHeader(date))
    {
        char buffer[50];
        _stream << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
    }

    _stream << "\r\n";

    _reply.sendBody(_stream);

    beginWrite();

    _timer.start(_server.writeTimeout());
}


void Connection::replyError()
{
    _reply.httpReturn(500, "internal server error");
    _reply.setHeader("Content-Type", "text/plain");
    _reply.setHeader("Connection", "close");
    _stream << "Error 500";

    endReply();
}


void Connection::onTimeout()
{
    //log_debug("timeout");
    timeout(*this);
}

} // namespace Http

} // namespace Pt
