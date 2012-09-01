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
#include <Pt/Http/Reply.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Logger.h>
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <cassert>

log_define("Pt.Http.Connection")

namespace Pt {

namespace Http {

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


void Connection::ReplyParseEvent::onHttpReturn(unsigned ret, const std::string& text)
{
    _replyHeader->httpReturn(ret, text);
}


Connection::Connection()
: _parseEvent()
, _parser(_parseEvent, false)
, _replyParser(_replyParseEvent, true)
, _request(0)
, _reply(0)
, _loop(0)
, _ssl(false)
#ifdef PT_HTTP_WITH_SSL
, _sslbuf( _sockbuf )
#endif
, _httpbuf()
, _readTimeout(30000)
, _writeTimeout(30000)
, _keepaliveTimeout(45000)
, _state(NotConnected)
, _chunked(false)
, _keepAlive(false)
{
    Net::TcpSocket::connected() += Pt::slot(*this, &Connection::onConnect);

    _sockbuf.attach(*this);
    _sockbuf.outputReady() += slot(*this, &Connection::onHttpOutput);
    _sockbuf.inputReady() += slot(*this, &Connection::onHttpInput);

    _httpbuf.attach(_sockbuf);

    _timer.timeout() += Pt::slot(*this, &Connection::onTimeout);

    _sslbuf.handshakeFinished() += slot(*this, &Connection::onHttpsHandshake);
}


Connection::~Connection()
{
}


void Connection::accept(Net::TcpServer& tcpServer)
{
    log_trace("Connection::accept");

    cancel();

    Net::TcpSocket::accept(tcpServer);

    if(_ssl)
        _state = SslAccept;
    else
        _state = Connected;
}


void Connection::setHost(const Net::AddrInfo& addrinfo)
{
    cancel();
    _addrInfo = addrinfo;
}


void Connection::setHttps(bool ssl)
{
#ifdef PT_HTTP_WITH_SSL
      if(ssl)
      {
          log_debug("initialize HTTPS connection");
          if( ! _ssl)
          {
              _sockbuf.outputReady() -= slot(*this, &Connection::onHttpOutput);
              _sockbuf.inputReady() -= slot(*this, &Connection::onHttpInput);

              _sslbuf.outputReady() += slot(*this, &Connection::onHttpsOutput);
              _sslbuf.inputReady() += slot(*this, &Connection::onHttpsInput);
              _ssl = true;
          }

          _httpbuf.attach(_sslbuf);
      }
      else
      {
          log_debug("initialize HTTP connection");
          if(_ssl)
          {
              _sslbuf.outputReady() -= slot(*this, &Connection::onHttpsOutput);
              _sslbuf.inputReady() -= slot(*this, &Connection::onHttpsInput);

              _sockbuf.outputReady() += slot(*this, &Connection::onHttpOutput);
              _sockbuf.inputReady() += slot(*this, &Connection::onHttpInput);
              _ssl = false;
          }

          _httpbuf.attach(_sockbuf);
      }
#endif
}


#ifdef PT_HTTP_WITH_SSL
void Connection::setContext(Ssl::Context& ctx)
{
    _sslbuf.init(ctx);
#else
void Connection::setContext(Ssl::Context& )
{
#endif
}


void Connection::setEventLoop(System::EventLoop& loop)
{
    this->setActive(loop);
    _timer.setActive(loop);
    _loop = &loop;
}


void Connection::cancel()
{
    close();
    _reply = 0;
    _request = 0;
    _state = NotConnected;
    _chunked = false;
	_keepAlive = false;
    _parser.reset(false);
    _replyParser.reset(true);
    _httpbuf.reset();
}


void Connection::beginSendRequest(Request& request)
{
    log_trace("Connection::beginSendRequest");

    _request = &request;

    if( ! isConnected() )
    {
        log_debug("opening new connection to " << _addrInfo.host());
        _timer.start( _writeTimeout );
        beginConnect(_addrInfo);
        return;
    }

#ifdef PT_HTTP_WITH_SSL
    if(_state == SslHandshake)
    {
        log_debug("begining SSL handshake");
        _timer.start( _writeTimeout );
        _sslbuf.beginConnect();
        return;
    }
#endif

    std::ostream os( _httpbuf.buffer() );

    if( request.finished() )
    {
        log_debug("HTTP request finished");

        if(_chunked)
        {
            log_debug("sending last HTTP chunk: "  << _request->size() << " bytes");
            if(_request->size() > 0)
            {
                os << std::hex << _request->size() << std::dec << "\r\n";
                os.write( _request->data(), _request->size() );
                os.write("\r\n", 2);
            }
            
            os.write("0\r\n\r\n", 5);
            _chunked = false;
        }
        else
        {
            sendRequest(os, *_request);
        }

        log_debug("sending HTTP request");
        _request->onOutput();
        return;
    }

    if( ! _chunked )
    {
        log_debug("sending chunked header");
        _chunked = true;
        sendChunkedHeader(os, *_request);
    }

    log_debug("sending HTTP chunk: "  << _request->size() << " bytes");

    if(_request->size() > 0)
    {
        os << std::hex << _request->size() << std::dec << "\r\n";
        os.write( _request->data(), _request->size() );
        os.write("\r\n", 2);
    }

    // TODO: only if over 8K data to send
    _timer.start( _writeTimeout );
    beginWrite();
}


bool Connection::endSendRequest()
{
    log_trace("Connection::endSendRequest");

    if(_state == NotConnected)
    {
        _timer.stop();
        endConnect();
        log_debug("connected to " << _addrInfo.host());

        if(_ssl)
            _state = SslHandshake;
        else
            _state = Connected;

        return false;
    }

#ifdef PT_HTTP_WITH_SSL
    if(_state == SslHandshake)
    {
        _timer.stop();
        _sslbuf.endHandshake();
        _state = Connected;
        log_debug("SSL handshake finished");
        return false;
    }
#endif

    Request* req = _request;
    _request = 0;

    if( ! req->finished() )
    {
        _timer.stop();
        endWrite();
    }
 
    // indicates that the request or chunk was completely written
    log_debug("request data sent");
    return true;
}


void Connection::beginFlush()
{
    _timer.start( _writeTimeout );
    beginWrite();
}


void Connection::endFlush()
{
    _timer.stop();
    endWrite();
}


// NOTE: maybe add a flag to cause a flush in the future
void Connection::beginSendReply(Reply& reply)
{
    log_trace("Connection::beginSendReply");

    _reply = &reply;

    ReplyHeader& header = _reply->header();
    std::ostream os( _httpbuf.buffer() );

	if( ! _reply->header().keepAlive() && outputAvailable() )
    {
        _timer.start( _writeTimeout );
        beginWrite();
        return;
    }

    if( reply.finished() )
    {
        if(_chunked)
        {
            if(_reply->buffer().size() > 0)
            {
                os << std::hex << _reply->buffer().size() << std::dec << "\r\n";
                os.write( _reply->buffer().data(), _reply->buffer().size() );
                os.write("\r\n", 2);
            }

            os.write("0\r\n\r\n", 5);
            _chunked = false;
        }
        else
        {
            sendReply(os, *_reply);
        }

        log_debug("begin writing reply");
        _timer.start( _writeTimeout );

		if( _reply->header().keepAlive() )
			_reply->onOutput(); 
		else
			beginWrite();

        return;
    }

    if( ! _chunked )
    {
		log_debug("sending chunked header");
        _chunked = true;
        sendChunkedHeader(os, *_reply);
    }

    if(_reply->buffer().size() > 0)
    {
        os << std::hex << _reply->buffer().size() << std::dec << "\r\n";
        os.write( _reply->buffer().data(), _reply->buffer().size() );
        os.write("\r\n", 2);
    }

    // TODO: only if over 8K data to send
    _timer.start( _writeTimeout );
    beginWrite();
}


bool Connection::endSendReply()
{
    log_trace("Connection::endSendReply");
    // TODO: handle exceptions correctly...

    // TODO: only keepalive when request allows it
    bool keepAlive = _reply->header().keepAlive();

    try
    {
		//if(! _reply->finished() || ! keepAlive)
		//{
        _timer.stop();
        endWrite();
		//}
		
		// when keepalive, leave data in the output buffer, otherwise
		// make sure we send all data
        if( ! keepAlive && outputAvailable() )
        {
            log_debug("still data to send");
            return false;
        }
  
        if( ! _reply->finished() )
        {
            // indicates that chunk was completely written
            log_debug("reply is not finished");
            return true;
        }

        _reply = 0;

        if(keepAlive)
        {
            log_debug("do keep alive");
            _timer.start(_keepaliveTimeout);

            // indicates that request was completely written
            return true;
        }
    }
    catch (const std::exception& e)
    {
        log_warn("exception occured when processing request: " << e.what());
    }

    log_debug("closing connection");
    close();

    // indicates that request was completely written
    return true;
}


void Connection::beginReceiveRequest(Request& request)
{
    log_trace("Connection::beginReceiveRequest " << _state);

    _request = &request;
    _parseEvent.init( request.header() );

#ifdef PT_HTTP_WITH_SSL
    if(_state == SslAccept)
    {
        log_debug("beginning SSL handshake");
        _sslbuf.beginAccept();
        _timer.start( _readTimeout );
        return;
    }
#endif

	// keep pipeling replies, until no more requests
    if( outputAvailable() && ! inputAvailable() )
    {
        log_debug("sending remaining reply data");
        beginWrite();
        _state = ReplyOutputPending;
        return;
    }

    log_debug("begin reading request");
    _timer.start( _readTimeout );
    beginRead();
}


bool Connection::endReceiveRequest()
{
    log_trace("Connection::endReceiveRequest");

    bool receivedHeader = false;

#ifdef PT_HTTP_WITH_SSL
    if(_state == SslAccept)
    {
        _timer.stop();
        _sslbuf.endHandshake();
        _state = Connected;
        log_debug("SSL handshake finished");
        return false;
    }
#endif

    if(_state == ReplyOutputPending)
    {
        log_debug("sent remaining reply data");
        endWrite();
        _state = Connected;
        return false;
    }

    endRead();

    if (_httpbuf.buffer()->in_avail() == 0 || Net::TcpSocket::eof())
    {
		// connection was closed prematurely
        close();
        return false;
    }

    if ( ! _parser.end() )
    {
        if( _parser.begin() && ! _timer.started() )
        {
            _timer.start( _readTimeout );
        }
        
        _parser.advance( *_httpbuf.buffer() );

        if( _parser.fail() )
        {
            log_warn("http parser failed");

			// TODO define exception class
            throw std::runtime_error("http parser failed"); 
			
			// TODO: handle any previously pipelined reply
            return false;
        }

        if( _parser.end() )
        {
            _httpbuf.reset();
            _httpbuf.beginBody(_request->header());
            
            if( _httpbuf.isEnd() )
            {
                log_debug("request body finished, no body");
				_timer.stop();
				_request = 0;
				_parser.reset(false);
            }

            return true;
        }

        return false;
    }

    if( _parser.end() )
    {
        _httpbuf.import();
        log_debug("available: " << _httpbuf.in_avail());

        if( _httpbuf.isEnd() )
        {
            log_debug("request body finished");
            _timer.stop();
            _request = 0;
            _parser.reset(false);
        }
    }

    return false;
}


void Connection::beginReceiveReply(Reply& r)
{
	log_trace("Connection::beginReceiveReply");

    _reply = &r;
    _replyParseEvent.init( _reply->header() );

    if( outputAvailable() )
    {
        log_debug("sending remaining request data");
        beginWrite();
        _state = RequestOutputPending;
        return;
    }

    _timer.start( _readTimeout );
    beginRead();
}


bool Connection::endReceiveReply()
{
	log_trace("Connection::endReceiveReply");

    bool receivedHeader = false;

    if(_state == RequestOutputPending)
    {
        log_debug("sent remaining request data");
        endWrite();
        _state = Connected;
        return false;
    }

    endRead();

    if ( ! _replyParser.end() )
    {
        if( Net::TcpSocket::eof() )
            throw System::IOError("unexpected EOF");

        if( _replyParser.begin() && ! _timer.started() )
        {
            _timer.start( _readTimeout );
        }
        
        _replyParser.advance( *_httpbuf.buffer() );

        if( _replyParser.fail() )
        {
            log_warn("http parser failed");
            throw std::runtime_error("http parser failed"); // TODO define exception class
            return false;
        }

        if( _replyParser.end() )
        {
            _httpbuf.reset();
            _httpbuf.beginBody( _reply->header() );

            if( _httpbuf.isEnd() )
            {
                log_debug("reply finished, no body");
				_reply = 0;
				_replyParser.reset(true);
				_timer.stop();
            }

            return true;
        }

        return false;
    }

    if( _replyParser.end() )
    {
        _httpbuf.import();
        log_debug("available: " << _httpbuf.in_avail());

        if( _httpbuf.isEnd() )
        {
            log_debug("reply body finished");
            bool keepalive = _reply->header().keepAlive();
            
            _reply = 0;
            _replyParser.reset(true);
            _timer.stop();

            if( ! keepalive )
            {
                log_debug("closing, no keep alive");
                close();
            }
        }
    }

    return false;
}


#ifdef PT_HTTP_WITH_SSL
void Connection::onHttpsHandshake(Pt::Ssl::IOBuffer& ssl)
{
    log_trace("Connection::onHttpsHandshake");
    if(_request && _state == SslHandshake)
    {
        _request->onOutput();
        return;
    }

    if(_request && _state == SslAccept)
    {
        _request->onInput();
        return;
    }
}


void Connection::onHttpsInput(Pt::Ssl::IOBuffer& ssl)
{
    log_trace("Connection::onHttpsInput");

    if(_request)
    {
        _request->onInput();
        return;
    }

    if(_reply)
        _reply->onInput();
}

void Connection::onHttpsOutput(Pt::Ssl::IOBuffer& ssl)
{
    log_trace("Connection::onHttpsOutput");

    if(_state == RequestOutputPending)
    {
        _reply->onInput();
        return;
    }

    if(_state == ReplyOutputPending)
    {
        _request->onInput();
        return;
    }

    if(_reply)
    {
        _reply->onOutput();
        return;
    }

    if(_request)
    {
        _request->onOutput();
        return;
    }

	flushed.send(*this);
}
#endif


void Connection::onConnect(Net::TcpSocket& socket)
{
    log_trace("Connection::onConnect");
    if(_request)
        _request->onOutput();
}


void Connection::onHttpInput(System::StreamBuffer& sb)
{
    log_trace("Connection::onHttpInput");

    if(_request)
    {
        _request->onInput();
        return;
    }

    if(_reply)
        _reply->onInput();
}


void Connection::onHttpOutput(System::StreamBuffer& sb)
{
    log_trace("Connection::onHttpOutput");

    if(_state == RequestOutputPending)
    {
        _reply->onInput();
        return;
    }

    if(_state == ReplyOutputPending)
    {
        _request->onInput();
        return;
    }

    if(_reply)
    {
        _reply->onOutput();
        return;
    }

    if(_request)
    {
        _request->onOutput();
        return;
    }

	flushed.send(*this);
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
    // TODO: do not call endRead if data was available

#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
        _sslbuf.endRead();
    else
#endif
        _sockbuf.endRead();
}


void Connection::beginWrite()
{
    log_debug("Connection::beginWrite");

#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
    {
        // TODO: need to check _sslbuf.out_avail()
        _sslbuf.pubsync();

        if ( _sslbuf.buffer().out_avail() )
        {
            log_debug("begin writing ssl buffer");
            _sslbuf.beginWrite();
            return;
        }
        
        log_debug("no ssl data to write");
        return;
    }
#endif
    if ( _sockbuf.out_avail() )
    {
        log_debug("begin writing socket buffer");
        _sockbuf.beginWrite();
        return;
    }

    log_debug("no data to write");
    return;
}


bool Connection::inputAvailable()
{
#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
    {  
		return _sslbuf.in_avail() > 0;
    }
#endif

    return _sockbuf.in_avail() > 0;
}


bool Connection::outputAvailable()
{
#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
    {
        // TODO: need to check _sslbuf.out_avail()
        _sslbuf.pubsync();

        if ( _sslbuf.buffer().out_avail() )
        {
            return true;
        }
        
        return false;
    }
#endif
    if ( _sockbuf.out_avail() )
    {
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


void Connection::onTimeout()
{
    log_debug("timeout");
    cancel();
}


void Connection::sendChunkedHeader(std::ostream& os, const Reply& reply)
{
    log_debug("send chunked reply header");

    const char* server = "Server";
    const char* connection = "Connection";
    const char* date = "Date";

    const ReplyHeader& header = reply.header();

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


void Connection::sendChunkedHeader(std::ostream& os, const Request& request)
{
    log_debug("send chunked request header " << request.header().url());

    static const char* connection = "Connection";
    static const char* date = "Date";
    static const char* host = "Host";
    static const char* authorization = "Authorization";
    static const char* userAgent = "User-Agent";

    os << request.header().method() << ' '
       << request.header().url() << " HTTP/"
       << request.header().httpVersionMajor() << '.'
       << request.header().httpVersionMinor() << "\r\n";

    for (RequestHeader::const_iterator it = request.header().begin();
        it != request.header().end(); ++it)
    {
        os << it->first << ": " << it->second << "\r\n";
    }

    os << "Transfer-Encoding: chunked" << "\r\n";

    if( ! request.header().hasHeader(connection) )
    {
        os << "Connection: keep-alive\r\n";
    }

    if (!request.header().hasHeader(date))
    {
        char buffer[50];
        os << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
    }

    if (!request.header().hasHeader(host))
    {
        os << "Host: " << _addrInfo.host();
        unsigned short port = _addrInfo.port();
        if (port != 80)
            os << ':' << port;
        os << "\r\n";
    }

    if (!request.header().hasHeader(userAgent))
    {
        os << "User-Agent: Pt-Http-client\r\n";
    }

    /*if (!_username.empty() && !request.header().hasHeader(authorization))
    {
        std::ostringstream d;
        BasicTextOStream<char, char> b(d, new Base64Codec());
        b << _username
          << ':'
          << _password;
        b.terminate();
        log_debug("set Authorization to " << d.str());
        os << "Authorization: Basic " << d.str() << "\r\n";
    }*/

    os << "\r\n";
}


void Connection::sendRequest(std::ostream& os, const Request& request)
{
    log_debug("send request " << request.header().url());

    static const char* contentLength = "Content-Length";
    static const char* connection = "Connection";
    static const char* date = "Date";
    static const char* host = "Host";
    static const char* authorization = "Authorization";
    static const char* userAgent = "User-Agent";

    os << request.header().method() << ' '
       << request.header().url() << " HTTP/"
       << request.header().httpVersionMajor() << '.'
       << request.header().httpVersionMinor() << "\r\n";

    for (RequestHeader::const_iterator it = request.header().begin();
        it != request.header().end(); ++it)
    {
        os << it->first << ": " << it->second << "\r\n";
    }

    if (!request.header().hasHeader(contentLength))
    {
        os << "Content-Length: " << request.size() << "\r\n";
    }

    if (!request.header().hasHeader(connection))
    {
        os << "Connection: keep-alive\r\n";
    }

    if (!request.header().hasHeader(date))
    {
        char buffer[50];
        os << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
    }

    if (!request.header().hasHeader(host))
    {
        os << "Host: " << _addrInfo.host();
        unsigned short port = _addrInfo.port();
        if (port != 80)
            os << ':' << port;
        os << "\r\n";
    }

    if (!request.header().hasHeader(userAgent))
    {
        os << "User-Agent: Pt-Http-client\r\n";
    }

    /*if (!_username.empty() && !request.header().hasHeader(authorization))
    {
        std::ostringstream d;
        BasicTextOStream<char, char> b(d, new Base64Codec());
        b << _username
          << ':'
          << _password;
        b.terminate();
        log_debug("set Authorization to " << d.str());
        os << "Authorization: Basic " << d.str() << "\r\n";
    }*/

    os << "\r\n";

    log_debug("send body; " << request.size() << " bytes");
    os.write(request.data(), request.size());
}


void Connection::sendReply(std::ostream& os, const Reply& reply)
{
    log_debug("sending HTTP reply");

    const char* server = "Server";
    const char* connection = "Connection";
    const char* date = "Date";
    static const char* contentLength = "Content-Length";

    const ReplyHeader& header = reply.header();

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

    os << contentLength << ": " << _reply->buffer().size() << "\r\n";

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
    os.write( _reply->buffer().data(), _reply->buffer().size() );
}

} // namespace Http

} // namespace Pt
