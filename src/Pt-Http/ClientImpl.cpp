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

void HttpBuffer::beginBody(const ReplyHeader& reply)
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

        if( ! _keepAlive)
        {
            log_debug("closing socket, no keep alive");
            _iodev->close();
        }

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


void ClientImpl::ParseEvent::onHttpReturn(unsigned ret, const std::string& text)
{
    _replyHeader.httpReturn(ret, text);
}


ClientImpl::ClientImpl(Client* client)
: _client(client)
, _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _ssl(false)
#ifdef PT_HTTP_WITH_SSL
, _sslbuf(_sockbuf)
#endif
, _httpbuf(_socket)
, _stream(&_httpbuf)
, _reusedConnection(false)
, _errorPending(false)
, _state( &ClientImpl::onHeader )
{
    init();
}


ClientImpl::ClientImpl(Client* client, const Net::AddrInfo& addrinfo, bool ssl)
: _client(client)
, _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _ssl(false)
#ifdef PT_HTTP_WITH_SSL
, _sslbuf(_sockbuf)
#endif
, _httpbuf(_socket)
, _stream(&_httpbuf)
, _reusedConnection(false)
, _errorPending(false)
, _state( &ClientImpl::onHeader )
{
    init();
    setHost(addrinfo, ssl);
}


ClientImpl::ClientImpl(Client* client, System::EventLoop& loop, const Net::AddrInfo& addrinfo, bool ssl)
: _client(client)
, _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _ssl(false)
#ifdef PT_HTTP_WITH_SSL
, _sslbuf(_sockbuf)
#endif
, _httpbuf(_socket)
, _stream(&_httpbuf)
, _reusedConnection(false)
, _errorPending(false)
, _state( &ClientImpl::onHeader )
{
    setActive(loop);
    init();
    setHost(addrinfo, ssl);
}


void ClientImpl::init()
{
    _socket.connected() += Pt::slot(*this, &ClientImpl::onConnect);

    _sockbuf.outputReady() += slot(*this, &ClientImpl::onOutput);
    _sockbuf.inputReady() += slot(*this, &ClientImpl::onInput);
    _sockbuf.attach(_socket);

#ifdef PT_HTTP_WITH_SSL
    _sslbuf.handshakeFinished() += slot(*this, &ClientImpl::onSslHandshake);
#endif

    _httpbuf.attach(_sockbuf);
}


void ClientImpl::setHost(const Net::AddrInfo& addrinfo, bool ssl)
{
    _addrInfo = addrinfo;
    _socket.close();

#ifdef PT_HTTP_WITH_SSL
      if(ssl)
      {
          log_debug("begining SSL handshake");
          if( ! _ssl)
          {
              _sockbuf.outputReady() -= slot(*this, &ClientImpl::onOutput);
              _sockbuf.inputReady() -= slot(*this, &ClientImpl::onInput);

              _sslbuf.outputReady() += slot(*this, &ClientImpl::onSslOutput);
              _sslbuf.inputReady() += slot(*this, &ClientImpl::onSslInput);

              _httpbuf.attach(_sslbuf);
              _ssl = true;
          }
          
          return;
      }

      log_debug("begining HTTP request");
      if(_ssl)
      {
          _sslbuf.outputReady() -= slot(*this, &ClientImpl::onSslOutput);
          _sslbuf.inputReady() -= slot(*this, &ClientImpl::onSslInput);

          _sockbuf.outputReady() += slot(*this, &ClientImpl::onOutput);
          _sockbuf.inputReady() += slot(*this, &ClientImpl::onInput);

          _httpbuf.attach(_sockbuf);
          _ssl = false;
      }

#endif
}


#ifdef PT_HTTP_WITH_SSL

void ClientImpl::setContext(Ssl::Context& ctx)
{
    _sslbuf.init(ctx);
}

#else

void ClientImpl::setContext(Ssl::Context& )
{
}

#endif

const ReplyHeader& ClientImpl::execute(const Request& request)
{
    log_trace("ClientImpl::execute " << request.url());

    _stream.rdbuf( _httpbuf.buffer() );
    _errorPending = false;
    _request = &request;
    _replyHeader.clear();
    _parser.reset(true);
    
    for(;;)
    {
        bool reuseConnection = _socket.isConnected();
        if( ! reuseConnection)
        {
            log_debug("connect");
            _socket.connect(_addrInfo);

#ifdef PT_HTTP_WITH_SSL
            if(_ssl)
            {
                log_debug("ssl handshake");
                _sslbuf.connect();
            }
#endif
        }

        log_debug("sending request");
        sendRequest(_stream, request);
        _stream.flush();
        _sockbuf.pubsync(); // extra flush for https: _stream -> _sslbuf -> _sockbuf

        log_debug("reading reply");
        _stream.peek();
        
        if( _stream || ! reuseConnection)
            break;

        cancel();
    }

    unsigned headerSize = 0;
    char ch = ' ';
    while( ! _parser.end() && _stream.get(ch) )
    {
        _parser.parse(ch);
        ++headerSize;
    }
         
    if( ! _parser.end() )
    {
        log_info("invalid HTTP reply");
        throw System::IOError( PT_ERROR_MSG("invalid HTTP reply") );
    }

    log_debug("content-length: " << _replyHeader.contentLength());
    log_debug("chunked: " << _replyHeader.chunkedTransferEncoding());
    log_debug("header-size: " << headerSize);

    _stream.rdbuf(&_httpbuf);
    _httpbuf.beginBody(_replyHeader);
    return _replyHeader;
}


void ClientImpl::beginRequest(const Request& request)
{
    log_trace("beginRequest");

    _stream.rdbuf( _httpbuf.buffer() );
    _errorPending = false;
    _request = &request;
    _replyHeader.clear();
    _parser.reset(true);
    _state = &ClientImpl::onHeader;
    
    _reusedConnection = _socket.isConnected();

    if( ! _reusedConnection )
    {
        log_debug("opening new connection to " << _addrInfo.host());
        _socket.beginConnect(_addrInfo);
        return;
    }

    log_debug("reusing previous connection");
    sendRequest(_stream, *_request);
        
    try
    {
        _sockbuf.beginWrite();
    }
    catch (const System::IOError&)
    {
        log_debug("write failed, reconnecting");
        cancel();
        _socket.beginConnect(_addrInfo);
    }
}


void ClientImpl::endExecute()
{
    _request = 0;

    if (_errorPending)
    {
        _errorPending = false;
        throw;
    }
}


void ClientImpl::sendRequest(std::ostream& os, const Request& request)
{
    log_debug("send request " << request.url());

    static const char* contentLength = "Content-Length";
    static const char* connection = "Connection";
    static const char* date = "Date";
    static const char* host = "Host";
    static const char* authorization = "Authorization";
    static const char* userAgent = "User-Agent";

    os << request.method() << ' '
       << request.url() << " HTTP/"
       << request.header().httpVersionMajor() << '.'
       << request.header().httpVersionMinor() << "\r\n";

    for (RequestHeader::const_iterator it = request.header().begin();
        it != request.header().end(); ++it)
    {
        os << it->first << ": " << it->second << "\r\n";
    }

   if (!request.header().hasHeader(contentLength))
    {
        os << "Content-Length: " << request.bodySize() << "\r\n";
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

    if (!_username.empty() && !request.header().hasHeader(authorization))
    {
        std::ostringstream d;
        BasicTextOStream<char, char> b(d, new Base64Codec());
        b << _username
          << ':'
          << _password;
        b.terminate();
        log_debug("set Authorization to " << d.str());
        os << "Authorization: Basic " << d.str() << "\r\n";
    }

    os << "\r\n";

    log_debug("send body; " << request.bodySize() << " bytes");

    request.sendBody(os);
}


void ClientImpl::onConnect(Net::TcpSocket& socket)
{
    log_trace("onConnect");

    try
    {
        socket.endConnect();

#ifdef PT_HTTP_WITH_SSL
        if(_ssl)
        {
            log_debug("begining SSL handshake");
            _sslbuf.beginConnect();
            return;
        }
#endif
        sendRequest(_stream, *_request);
        
        log_debug("request sent - begin write");
        _sockbuf.beginWrite();
    }
    catch(...)
    {
        this->onError();
    }
}

void ClientImpl::onOutput(System::StreamBuffer& sb)
{
    log_trace("onOutput: out_avail=" << sb.out_avail());

    try
    {
        sb.endWrite();

        if( sb.out_avail() > 0 )
        {
            sb.beginWrite();
        }
        else
        {
            sb.beginRead();
            _client->requestSent().send(*_client);
        }
    }
    catch (const System::IOError&)
    {
        if (_reusedConnection)
        {
            log_debug("reconnect on error");
            cancel();
            beginRequest(*_request);
            return;
        }

        this->onError();
    }
    catch (const std::exception& e)
    {
        log_warn("exception occured: " << e.what());
        this->onError();
    }
}


void ClientImpl::onInput(System::StreamBuffer& sb)
{
    log_trace("ClientImpl::onInput");

    try
    {
        sb.endRead();
        (this->*_state)();
    }
    catch (const std::exception& e)
    {
        log_warn("exception occured: " << e.what());
        this->onError();
    }
}


void ClientImpl::onError()
{
    _errorPending = true;
    _client->replyFinished().send(*_client);

    if (_errorPending)
    {
        _errorPending = false;
        throw;
    }
}

#ifdef PT_HTTP_WITH_SSL
void ClientImpl::onSslHandshake(Ssl::IOBuffer& ssl)
{
    try
    {
        log_trace("onSslHandshake");
        ssl.endHandshake();

        sendRequest(_stream, *_request);
        _stream << std::flush;

        log_debug("request sent - begin write");
        ssl.beginWrite();
    }
    catch(...)
    {
        this->onError();
    }
}


void ClientImpl::onSslOutput(Ssl::IOBuffer& sb)
{
    log_trace("onSslOutput");

    try
    {
        sb.endWrite();
        sb.beginRead();
        _client->requestSent().send(*_client);
    }
    catch (const System::IOError&)
    {
        if (_reusedConnection)
        {
            log_debug("reconnect on error");
            cancel();
            beginRequest(*_request);
            return;
        }

        this->onError();
    }
    catch (const std::exception& e)
    {
        log_warn("exception occured: " << e.what());
        this->onError();
    }
}


void ClientImpl::onSslInput(Ssl::IOBuffer& sb)
{
    log_trace("ClientImpl::onSslInput");

    try
    {
        sb.endRead();
        (this->*_state)();
    }
    catch (const std::exception& e)
    {
        log_warn("exception occured: " << e.what());
        this->onError();
    }
}
#endif

void ClientImpl::onHeader()
{
    log_trace("onHeader");

    // reconnect when received pending EOF from previous response
    if( _sockbuf.device()->eof() )
    {
        if(_reusedConnection)
        {
            log_debug("reconnect on EOF");
            cancel();
            beginRequest(*_request);
            return;
        }

        throw System::IOError("unexpected EOF");
    }

    _reusedConnection = false;

    log_debug("advancing parser");
    _parser.advance(_stream);

    if( _parser.fail() )
    {
        log_warn("http parser failed");
        throw std::runtime_error("http parser failed"); // TODO define exception class
    }

    if( ! _parser.end() )
    {
        log_debug("continue reading");
        beginRead();
    }
    
    log_debug("http header complete");
    _stream.rdbuf(&_httpbuf);
    _httpbuf.beginBody(_replyHeader);
    _client->headerReceived().send(*_client);
    _state = &ClientImpl::onBody;

    (this->*_state)();
}


void ClientImpl::onBody()
{
    log_trace("onBody");

    _httpbuf.import();
    log_debug("available: " << _httpbuf.in_avail());

    do
    {
        if( _httpbuf.in_avail() )
            _client->bodyAvailable().send(*_client);

        _httpbuf.import();
        log_debug("available: " << _httpbuf.in_avail());
    } 
    while( _httpbuf.in_avail() );
    
    if( _stream.fail() )
        throw System::IOError( PT_ERROR_MSG("error reading HTTP reply body") );

    if( _httpbuf.isEnd() )
    {
        log_debug("reply finished");
        _client->replyFinished().send(*_client);
    }
    else
    {
        log_debug("continue reading body");
        beginRead();
    }
}


void ClientImpl::beginRead()
{
#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
        _sslbuf.beginRead();
    else
#endif
        _sockbuf.beginRead();
}


void ClientImpl::cancel()
{
    _socket.close();
    _sockbuf.discard();
#ifdef PT_HTTP_WITH_SSL
    _sslbuf.discard();
#endif
    _stream.clear();
}

} // namespace Http

} // namespace Pt


/*void ClientImpl::readBody(std::string& s)
{
    s.clear();

    bool chunkedEncoding = _replyHeader.chunkedTransferEncoding();

    if(chunkedEncoding)
    {
        log_debug("read body with chunked encoding");

        // _stream -> _chunkedBuffer (-> _sslbuf) -> _sockbuf
        _chunkedBuffer.reset( _stream.rdbuf() );
        _stream.rdbuf(&_chunkedBuffer);

        char ch;
        while( _stream.get(ch) )
            s += ch;

        log_debug("eod=" << _chunkedBuffer.eod());

        if( ! _chunkedBuffer.eod() )
            throw System::IOError( PT_ERROR_MSG("error reading HTTP reply body: incomplete chunked data stream") );
    }
    else
    {
        unsigned n = _replyHeader.contentLength();

        log_debug("read body; content-size: " << n);

        s.reserve(n);

        char ch;
        while (n-- && _stream.get(ch))
            s += ch;

        if( _stream.fail() )
            throw System::IOError( PT_ERROR_MSG("error reading HTTP reply body") );

        //log_debug("body read: \"" << s << '"');
    }

    if (!_replyHeader.keepAlive())
    {
        log_debug("close socket - no keep alive");
        _socket.close();
    }
    else
    {
        log_debug("do not close socket - keep alive");
    }
}
*/

/*
bool ClientImpl::onChunkedBody()
{
    log_trace("onChunkedBody");
    assert(&_chunkedBuffer == _stream.rdbuf());

    if( _chunkedBuffer.in_avail() > 0 )
    {
        if( ! _chunkedBuffer.eod() )
        {
            log_debug("read chunked encoding body");

            while( _stream.good() && 
                   _chunkedBuffer.in_avail() > 0 && 
                   ! _chunkedBuffer.eod() )
            {
                log_debug("bodyAvailable");
                _client->bodyAvailable().send(*_client, _stream);
            }

            log_debug("in_avail=" << _chunkedBuffer.in_avail() << " eod=" << _chunkedBuffer.eod());
            if( _chunkedBuffer.eod() )
            {
                if( _replyHeader.hasHeader("Trailer") )
                    _parser.readHeader();
                else
                    _client->replyFinished().send(*_client);
            }
        }

        if (_chunkedBuffer.eod() && _chunkedBuffer.in_avail() > 0)
        {
            log_debug("read chunked encoding post headers");

            _parser.advance(_chunkedBuffer);
            if( _parser.fail() )
                throw std::runtime_error("http parser failed"); // TODO define exception class

            if( _parser.end() )
            {
                log_debug("reply finished");

                if( ! _replyHeader.keepAlive())
                {
                    log_debug("close socket - no keep alive");
                    _socket.close();
                }

                _client->replyFinished().send(*_client);
            }
        }

        if( _stream.fail() )
            throw System::IOError( PT_ERROR_MSG("error reading HTTP reply body") );
    }
    else if( _chunkedBuffer.eod() )
    {
        if( _replyHeader.hasHeader("Trailer") )
            _parser.readHeader();
        else
            _client->replyFinished().send(*_client);
    }

    if (_socket.isConnected())
    {
        if( ! _chunkedBuffer.eod() || ! _parser.end() )
        {
            log_debug("call beginRead");
            return true;
        }
    }
    else
    {
        cancel();
    }

    return false;
}


void ClientImpl::onHttpsChunkedBody(System::StreamBuffer& sbuf)
{
    log_trace("onHttpsChunkedBody");

#ifdef PT_HTTP_WITH_SSL
    bool cont = this->onChunkedBody();
    if(cont)
    {
        log_debug("body not finished- begin read(SSL)");
        _sslbuf.beginRead();
    }
#endif
}


void ClientImpl::onHttpChunkedBody(System::StreamBuffer& sbuf)
{
    log_trace("onHttpChunkedBody");

    bool cont = this->onChunkedBody();
    if(cont)
    {
        sbuf.beginRead();
    }
}
*/
