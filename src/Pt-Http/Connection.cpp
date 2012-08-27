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

RequestHandler::RequestHandler(Server& server, Net::TcpServer& tcpServer)
: _conn(server, tcpServer)
{
    _conn.timeout += Pt::slot(*this, &RequestHandler::onTimeout);

    _req.inputReceived() += Pt::slot(*this, &RequestHandler::onRequestReceived);
}


RequestHandler::~RequestHandler()
{
}


void RequestHandler::onRequestReceived(Request& req)
{
    bool receivedHeader = _conn.endReceiveRequest();
    log_debug("onRequestReceived: " << receivedHeader);

    if(receivedHeader)
    {
        _conn._responder = _conn._server.getResponder(_req.header());
        _conn._responder->beginRequest( _req.body(), _req.header() );
    }
    
    if( _req.body().rdbuf()->in_avail() )
    {
        _conn._responder->readRequest(_req.body(), _conn._reply);
            
        if( _conn._reply.finished() )
        {
            // TODO: skip unread body
            return;
        }
    } 
    
    if( _req.body().fail() )
        throw System::IOError( PT_ERROR_MSG("error reading HTTP reply body") );

    if( ! _conn.isEnd() )
    {
        log_debug("more data available");
        _conn.beginReceiveRequest(_req);
    }
    else
    {
        log_debug("read request body");
        _conn._responder->beginReply(_req.header(), _conn._reply);
    }
}


void Connection::ParseEvent::onMethod(const std::string& method)
{
    _request->method(method);
}


void Connection::ParseEvent::onUrl(const std::string& url)
{
    _request->url(url);
}


void Connection::ParseEvent::onUrlParam(const std::string& q)
{
    _request->qparams(q);
}


Connection::Connection(Server& server, Net::TcpServer& tcpServer)
: _server(server)
, _responder(0)
, _parseEvent()
, _parser(_parseEvent, false)
, _request(0)
, _loop(0)
, _chunkedTransfer(true)
, _ssl(false)
#ifdef PT_HTTP_WITH_SSL
, _sslbuf( _sockbuf )
#endif
, _httpbuf()
, _stream(&_httpbuf)
, _chunked(false)
, _reply()
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


void Connection::beginAccept(System::EventLoop& loop, Request& req, Ssl::Context* ctx)
{
    _parseEvent.init(req.header());

    _request = &req;
    _reply.outputSent() += Pt::slot(*this, &Connection::onReplySent);
    _reply.init(*this);
    _reply.clear();
    
    _timer.timeout() += Pt::slot(*this, &Connection::onTimeout);
    _timer.setActive(loop);

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
        _timer.start( _server.readTimeout() );
        return;
    }
#endif

    log_debug("beginning HTTP connection");
    _sockbuf.inputReady() += Pt::slot(*this, &Connection::onHttpInput);
    _sockbuf.outputReady() += Pt::slot(*this, &Connection::onHttpOutput);

    _httpbuf.attach(_sockbuf);
    _sockbuf.beginRead();
    _timer.start( _server.readTimeout() );
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
    _request->onInput(_httpbuf);
}

void Connection::onHttpsOutput(Pt::Ssl::IOBuffer& ssl)
{
    log_trace("Connection::onHttpsOutput");
    _reply.onOutput();
}
#endif


void Connection::onHttpInput(System::StreamBuffer& sb)
{
    log_trace("Connection::onHttpInput");
    _request->onInput(_httpbuf);
}


void Connection::onHttpOutput(System::StreamBuffer& sb)
{
    log_trace("Connection::onHttpOutput");
    _reply.onOutput();
}


void Connection::beginRead()
{
#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
        if( _sslbuf.in_avail() )
            onHttpsInput(_sslbuf);
        else
        {
            _sslbuf.beginRead();
        }
    else
#endif
        if( _sockbuf.in_avail() )
            onHttpInput(_sockbuf);
        else
        {
            _sockbuf.beginRead();
        }
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
            return true;
        }
        
        return false;
    }
#endif
    if ( _sockbuf.out_avail() )
    {
        _sockbuf.beginWrite();
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


void Connection::beginReceiveRequest(Request& request)
{
    beginRead();
}


bool Connection::endReceiveRequest()
{
    // TODO: handle exceptions correctly...
    bool receivedHeader = false;

    endRead();

    if (_httpbuf.buffer()->in_avail() == 0 || Net::TcpSocket::eof())
    {
        close();
        timeout(*this);
        return false;
    }

    if ( ! _parser.end() )
    {
        if( _parser.begin() && ! _timer.started() )
        {
            _timer.start( _server.readTimeout() );
        }
        
        _parser.advance( *_httpbuf.buffer() );

        if( _parser.fail() )
        {
            log_warn("http parser failed");
            throw std::runtime_error("http parser failed"); // TODO define exception class
            //_responder = _server.getDefaultResponder(_request->header());
            //replyError();
            return false;
        }

        if( _parser.end() )
        {
            _httpbuf.beginBody(_request->header());
            if( _httpbuf.isEnd() )
            {
                log_debug("request body finished");
                _timer.stop();
            }

            return true;
        }

        return false;
    }

    if( _parser.end() )
    {
        // new code using HttpBuffer:
        _httpbuf.import();
        log_debug("available: " << _httpbuf.in_avail());

        if( _httpbuf.isEnd() )
        {
            log_debug("request body finished");
            _timer.stop();
            
            _httpbuf.reset();
            _chunked = false;
        }
    }

    return false;
}


void Connection::beginSendReply(bool finish)
{
    const char* server = "Server";
    const char* connection = "Connection";
    const char* date = "Date";

    ReplyHeader& header = _reply.header();
    std::ostream os( _httpbuf.buffer() );

    if(finish)
    {
        if(_chunked)
        {
            os << std::hex << _reply.buffer().size() << std::dec << "\r\n";
            os.write( _reply.buffer().data(), _reply.buffer().size() );
            _reply.clearBody();
            os.write("\r\n", 2);
            os.write("0\r\n\r\n", 5);
        }
        else
        {
            os <<"HTTP/"
                << header.httpVersionMajor() << '.'
                << header.httpVersionMinor() << ' '
                << header.httpReturnCode() << ' '
                << header.httpReturnText() << "\r\n";

            ReplyHeader::const_iterator it;
            for(it = header.begin(); it != header.end(); ++it)
            {
                os << it->first << ": " << it->second << "\r\n";
            }

            os << "Content-Length: " << _reply.buffer().size() << "\r\n";

            if( ! header.hasHeader(server) )
            {
                os << "Server: Pt-Net-Server\r\n";
            }

            if( ! header.hasHeader(connection) )
            {
                os << "Connection: "
                    << (header.keepAlive() ? "keep-alive" : "close")
                    << "\r\n";
            }

            if( ! header.hasHeader(date) )
            {
                char buffer[50];
                os << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
            }

            os << "\r\n";
            os.write( _reply.buffer().data(), _reply.buffer().size() );
        }

        _timer.start( _server.writeTimeout() );
        beginWrite();
        return;
    }

    if(_reply.buffer().size() < 8192)
    {
        _responder->writeReply(_request->header(), _reply);
        return;
    }

    if( ! _chunked )
    {
        _chunked = true;

        os <<"HTTP/"
           << header.httpVersionMajor() << '.'
           << header.httpVersionMinor() << ' '
           << header.httpReturnCode() << ' '
           << header.httpReturnText() << "\r\n";

        ReplyHeader::const_iterator it;
        for(it = header.begin(); it != header.end(); ++it)
        {
            os << it->first << ": " << it->second << "\r\n";
        }

        os << "Transfer-Encoding: chunked\r\n";

        if( ! header.hasHeader(server) )
        {
            os << "Server: Pt-Net-Server\r\n";
        }

        if( ! header.hasHeader(connection) )
        {
            os << "Connection: "
               << (header.keepAlive() ? "keep-alive" : "close")
               << "\r\n";
        }

        if( ! header.hasHeader(date) )
        {
            char buffer[50];
            os << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
        }

        os << "\r\n";
    }

    os << std::hex << _reply.buffer().size() << std::dec << "\r\n";
    os.write( _reply.buffer().data(), _reply.buffer().size() );
    _reply.clearBody();
    os.write("\r\n", 2);

    _timer.start( _server.writeTimeout() );
    beginWrite();
}


void Connection::onReplySent(Reply&)
{
    endSendReply();
}


void Connection::endSendReply()
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
            _responder->writeReply(_request->header(), _reply);
            return;
        }
  
        bool keepAlive = _request->header().keepAlive() && _reply.header().keepAlive();
        if(keepAlive)
        {
            log_debug("do keep alive");
            _timer.start(_server.keepAliveTimeout());
            _chunked = false;
            _request->clear();
            _reply.clear();
            _parser.reset(false);
            _httpbuf.reset();

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


void Connection::replyError()
{
    _reply.header().httpReturn(500, "internal server error");
    _reply.header().setHeader("Content-Type", "text/plain");
    _reply.header().setHeader("Connection", "close");
    _stream << "Error 500: Internal server error.";

    beginSendReply(true);
}


void Connection::onTimeout()
{
    //log_debug("timeout");
    timeout(*this);
}

} // namespace Http

} // namespace Pt
