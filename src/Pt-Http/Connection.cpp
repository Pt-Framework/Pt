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

void HttpBuffer::beginBody(const MessageHeader& reply)
{
    log_trace("HttpBuffer::beginBody()");
    _chunkParser.reset();

    setg(0,0,0);

    _keepAlive = reply.keepAlive();
    _contentLength = reply.contentLength();
    _chunked = reply.chunkedTransferEncoding();

    log_debug("keep-alive: " << _keepAlive);
    log_debug("chunked: " << _chunked);
    log_debug("content-length: " << _contentLength);
}


bool HttpBuffer::isEnd() const
{
    log_trace("HttpBuffer::isEnd()");
    if(_chunked)
        return _chunkParser.end();

    return _contentLength == 0;
}


void HttpBuffer::import(std::streamsize n)
{
    log_trace("HttpBuffer::import(" << n << ")");

    if( ! _sbuf)
        return;

    if(n == 0)
    {
        n = _sbuf->in_avail();
        log_debug("available: " << n);
    }

    // Move unread bytes and putback to front
    size_t putback  = MaxPutback;
    size_t leftover = 0;
    
    if( this->gptr() ) 
    {
        putback = std::min<std::size_t>( this->gptr() - this->eback(), MaxPutback);
        char* to = _buffer + MaxPutback - putback;
        char* from = this->gptr() - putback;

        leftover = this->egptr() - this->gptr();
        std::memmove( to, from, putback + leftover );

        this->setg( _buffer + (MaxPutback - putback),  // start of get area
                    _buffer + MaxPutback,              // gptr position
                    _buffer + MaxPutback + leftover ); // end of get area
    }

    if(_chunked)
    {
        log_debug("getting next chunk");
        _contentLength = 0;
        while(n-- && ! _chunkParser.end())
        {
            char ch = _sbuf->sbumpc();
            _chunkParser.parse(ch);
            if( _chunkParser.hasChunk() )
            {
                _contentLength = _chunkParser.chunkSize();
                break;
            }
        }
    }

    size_t unused = sizeof(_buffer) - (MaxPutback + leftover);
    log_debug("unused buffer area: " << unused);
    if(n > unused)
        n = unused;

    log_debug("content-length: " << _contentLength);
    if(n > _contentLength)
        n = _contentLength;

    if( this->isEnd() )
    {
        log_trace("received all content -> EOF");
        return;
    }

    log_debug("http buffer refill: " << n);
    if(n == 0)
        return;

    n = _sbuf->sgetn(_buffer + MaxPutback + leftover, n);

    setg(_buffer + MaxPutback - putback, // eback - start of get area
         _buffer + MaxPutback,           // gptr - current position
         _buffer + MaxPutback + n);      // egptr - end of get area

    _contentLength -= n;
    log_debug("remaining content length: " << _contentLength);
}


HttpBuffer::int_type HttpBuffer::underflow()
{ 
    log_trace("HttpBuffer::underflow()");

    if(this->gptr() < this->egptr())
        return traits_type::to_int_type(*(this->gptr()));

    import( sizeof(_buffer) );

    if( this->gptr() < this->egptr() )
        return traits_type::to_int_type( *this->gptr() );

    return traits_type::eof();
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


void Connection::ReplyParseEvent::onHttpReturn(unsigned ret, const std::string& text)
{
    _replyHeader->httpReturn(ret, text);
}


Connection::Connection(Net::TcpServer& tcpServer)
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
, _chunked(false)
{
    Net::TcpSocket::accept(tcpServer);

    _sockbuf.attach(*this);

    _timer.timeout() += Pt::slot(*this, &Connection::onTimeout);

    this->connected() += Pt::slot(*this, &Connection::onConnect);
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
, _chunked(false)
{
    _sockbuf.attach(*this);

    _timer.timeout() += Pt::slot(*this, &Connection::onTimeout);

    this->connected() += Pt::slot(*this, &Connection::onConnect);
}


Connection::~Connection()
{
}


#ifdef PT_HTTP_WITH_SSL
void Connection::setContext(Ssl::Context& ctx)
{
    _sslbuf.init(ctx);
}
#else
void Connection::setContext(Ssl::Context& )
{
}
#endif


void Connection::setEventLoop(System::EventLoop& loop)
{
    this->setActive(loop);
    _timer.setActive(loop);
    
    _loop = &loop;
}


void Connection::setHost(const Net::AddrInfo& addrinfo, bool ssl)
{
    _addrInfo = addrinfo;
    
    close();

#ifdef PT_HTTP_WITH_SSL
      _sslbuf.handshakeFinished() -= slot(*this, &Connection::onHttpsHandshake);
      _sslbuf.handshakeFinished() -= slot(*this, &Connection::onHttpsClientHandshake);
      _sslbuf.handshakeFinished() += slot(*this, &Connection::onHttpsClientHandshake);

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
          
          return;
      }

      log_debug("initialize HTTP connection");
      if(_ssl)
      {
          _sslbuf.outputReady() -= slot(*this, &Connection::onHttpsOutput);
          _sslbuf.inputReady() -= slot(*this, &Connection::onHttpsInput);

          _sockbuf.outputReady() += slot(*this, &Connection::onHttpOutput);
          _sockbuf.inputReady() += slot(*this, &Connection::onHttpInput);
          _ssl = false;
      }

      _sockbuf.outputReady() += slot(*this, &Connection::onHttpOutput);
      _sockbuf.inputReady() += slot(*this, &Connection::onHttpInput);
      _httpbuf.attach(_sockbuf);
#endif
}


void Connection::init(System::EventLoop& loop, Ssl::Context* ctx)
{
    log_trace("Connection::init");

    _timer.setActive(loop);

    _loop = &loop;
    this->setActive(loop);
    
#ifdef PT_HTTP_WITH_SSL
    _ssl = ctx != 0;

    _sslbuf.handshakeFinished() -= slot(*this, &Connection::onHttpsHandshake);
    _sslbuf.handshakeFinished() -= slot(*this, &Connection::onHttpsClientHandshake);
    _sslbuf.handshakeFinished() += slot(*this, &Connection::onHttpsHandshake);

    if(_ssl)
    {
        log_debug("initialize HTTPS connection");
        _sslbuf.init(*ctx);
        _sslbuf.outputReady() += slot(*this, &Connection::onHttpsOutput);
        _sslbuf.inputReady() += slot(*this, &Connection::onHttpsInput);

        _httpbuf.attach(_sslbuf);
        return;
    }
#endif

    log_debug("initialize HTTP connection");
    _sockbuf.inputReady() += Pt::slot(*this, &Connection::onHttpInput);
    _sockbuf.outputReady() += Pt::slot(*this, &Connection::onHttpOutput);

    _httpbuf.attach(_sockbuf);
}


void Connection::beginSendRequest(Request& request)
{
    _request = &request;

    if( ! isConnected() )
    {
        log_debug("opening new connection to " << _addrInfo.host());
        _timer.start( _writeTimeout );
        beginConnect(_addrInfo);
        return;
    }

    RequestHeader& header = _request->header();
    std::ostream os( _httpbuf.buffer() );

    // TODO: only if finished or over 8K data to send
    if( outputAvailable() )
    { 
        log_debug("output available");
        _timer.start( _writeTimeout );
        beginWrite();
        return;
    }

    if( request.finished() )
    {
        log_debug("HTTP request finished");

        if(_chunked)
        {
            log_debug("sending HTTP chunked request");
            os << std::hex << _request->size() << std::dec << "\r\n";
            os.write( _request->data(), _request->size() );
            
            os.write("\r\n", 2);
            os.write("0\r\n\r\n", 5);
            _chunked = false;
        }
        else
        {
            log_debug("sending HTTP request");
            sendRequest(os, *_request);
         }

        log_debug("begin writing request");
        _timer.start( _writeTimeout );
        beginWrite();
        return;
    }

    if( ! _chunked )
    {
        _chunked = true;
        log_debug("sending chunked HTTP request");
        sendChunked(os, *_request);
    }

    log_debug("sending HTTP chunk");
    os << std::hex << _reply->buffer().size() << std::dec << "\r\n";
    os.write( _reply->buffer().data(), _reply->buffer().size() );
    os.write("\r\n", 2);
    
    _timer.start( _writeTimeout );
    beginWrite();
}


bool Connection::endSendRequest()
{
    log_trace("Connection::endSendRequest");
    // TODO: handle exceptions correctly...

    endWrite();

    if( outputAvailable() > 0)
    {
        log_debug("still data to send");
        return false;
    }

    if( _request->finished() )
    {
        _request = 0;
        _chunked = false;
        _replyParser.reset(true);
        _httpbuf.reset();
    }

    // indicates that the request or chunk was completely written
    log_debug("request data sent");
    return true;
}


void Connection::beginSendReply(Reply& reply)
{
    _reply = &reply;

    const char* server = "Server";
    const char* connection = "Connection";
    const char* date = "Date";

    ReplyHeader& header = _reply->header();
    std::ostream os( _httpbuf.buffer() );

    // TODO: only if finished or over 8K data to send
    if( outputAvailable() )
    {
        _timer.start( _writeTimeout );
        beginWrite();
        return;
    }

    if( reply.finished() )
    {
        if(_chunked)
        {
            os << std::hex << _reply->buffer().size() << std::dec << "\r\n";
            os.write( _reply->buffer().data(), _reply->buffer().size() );
            _reply->clearBody();
            os.write("\r\n", 2);
            os.write("0\r\n\r\n", 5);
            _chunked = false;
        }
        else
        {
            log_debug("sending HTTP reply");

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

            os << "Content-Length: " << _reply->buffer().size() << "\r\n";

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

        log_debug("begin writing reply");
        _timer.start( _writeTimeout );
        beginWrite();
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

    os << std::hex << _reply->buffer().size() << std::dec << "\r\n";
    os.write( _reply->buffer().data(), _reply->buffer().size() );
    _reply->clearBody();
    os.write("\r\n", 2);

    _timer.start( _writeTimeout );
    beginWrite();
}


bool Connection::endSendReply()
{
    log_trace("Connection::endSendReply");
    // TODO: handle exceptions correctly...

    try
    {
        endWrite();

        if( outputAvailable() > 0)
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

        // TODO: only keepalive when request allows it
        bool keepAlive = _reply->header().keepAlive();
        
        _reply = 0;
        _chunked = false;
        _parser.reset(false);
        _httpbuf.reset();

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
    _request = &request;

    _parseEvent.init( request.header() );

    if(_ssl && ! _sslbuf.connected() )
    {
        log_debug("beginning SSL handshake");
        _sslbuf.beginAccept();
        _timer.start( _readTimeout );
        return;
    }

    _timer.start( _readTimeout );
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
            throw std::runtime_error("http parser failed"); // TODO define exception class
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
        _httpbuf.import();
        log_debug("available: " << _httpbuf.in_avail());

        if( _httpbuf.isEnd() )
        {
            log_debug("request body finished");
            _timer.stop();
            _request = 0;
            
            _httpbuf.reset();
            _chunked = false;
        }
    }

    return false;
}


void Connection::beginReceiveReply(Reply& r)
{
    _reply = &r;
    _replyParseEvent.init( _reply->header() );

    _timer.start( _readTimeout );
    beginRead();
}


bool Connection::endReceiveReply()
{
    // TODO: handle exceptions correctly...
    bool receivedHeader = false;

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
            _httpbuf.beginBody( _reply->header() );

            if( _httpbuf.isEnd() )
            {
                log_debug("reply body finished");
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
            _httpbuf.reset();
            _chunked = false;

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
    ssl.endHandshake();

    _timer.start( _readTimeout );
    beginRead();
}

void Connection::onHttpsClientHandshake(Pt::Ssl::IOBuffer& ssl)
{
    log_trace("Connection::onHttpsClientHandshake");
    ssl.endHandshake();

    beginSendRequest(*_request);
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

    if(_reply)
    {
        _reply->onOutput();
        return;
    }

    if(_request)
        _request->onOutput();
}
#endif


void Connection::onConnect(Net::TcpSocket& socket)
{
    log_trace("Connection::onConnect");

    endConnect();

#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
    {
        log_debug("begining SSL handshake");
        _timer.start( _writeTimeout );
        _sslbuf.beginConnect();
        return;
    }
#endif

    beginSendRequest(*_request);
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

    if(_reply)
    {
        _reply->onOutput();
        return;
    }

    if(_request)
        _request->onOutput();
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
    close();
}


void Connection::sendChunked(std::ostream& os, const Request& request)
{
    log_debug("send chunked request " << request.header().url());

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

} // namespace Http

} // namespace Pt
