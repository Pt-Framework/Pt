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

void SslInputBuffer::setContentLength(long n)
{
    log_trace("available content: " << n);
    _contentLength = n;
}

SslInputBuffer::int_type SslInputBuffer::underflow()
{ 
    log_trace("SslInputBuffer::underflow()");

    std::streamsize avail = this->in_avail();
    if(avail > 0 || _contentLength == -1)
    {
        return Ssl::IOBuffer::underflow();
    }
    
    if(_contentLength == 0)
    {
        log_trace("received all content -> EOF");

        if( ! _keepAlive)
        {
            log_debug("closing socket, no keep alive");
            this->buffer().device()->close();
        }

        return traits_type::eof();
    }
    
    SslInputBuffer::int_type ret = Ssl::IOBuffer::underflow(); 
    _contentLength -= this->in_avail();
    log_trace("remaining: " << _contentLength);
    return ret;
}




void InputBuffer::setContentLength(long n)
{
    log_trace("available content: " << n);
    _contentLength = n;
}

InputBuffer::int_type InputBuffer::underflow()
{ 
    log_trace("InputBuffer::underflow()");

    std::streamsize avail = this->in_avail();
    if(avail > 0 || _contentLength == -1)
    {
        return System::IOBuffer::underflow();
    }
    
    if(_contentLength == 0)
    {
        log_trace("received all content -> EOF");

        if( ! _keepAlive)
        {
            log_debug("closing socket, no keep alive");
            this->device()->close();
        }

        return traits_type::eof();
    }
    
    InputBuffer::int_type ret = System::IOBuffer::underflow(); 
    _contentLength -= this->in_avail();
    log_trace("remaining: " << _contentLength);
    return ret;
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
, _chunkedBuffer(&_sockbuf)
, _stream(&_sockbuf)
, _contentLength(0)
, _reusedConnection(false)
, _errorPending(false)
, _state( &ClientImpl::onHttpHeader )
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
, _chunkedBuffer(&_sockbuf)
, _stream(&_sockbuf)
, _contentLength(0)
, _reusedConnection(false)
, _errorPending(false)
, _state( &ClientImpl::onHttpHeader )
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
, _chunkedBuffer(&_sockbuf)
, _stream(&_sockbuf)
, _contentLength(0)
, _reusedConnection(false)
, _errorPending(false)
, _state( &ClientImpl::onHttpHeader )
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

              _stream.rdbuf(&_sslbuf);
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

          _stream.rdbuf(&_sockbuf);
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

    _replyHeader.clear();
    _parser.reset(true);

#ifdef PT_HTTP_WITH_SSL
    _sslbuf.setContentLength(-1);
#endif

    _sockbuf.setContentLength(-1);

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

        // extra flush for https: _stream -> _sslbuf -> _sockbuf
        _sockbuf.pubsync();

        log_debug("reading reply");
        char ch = ' ';
        while( ! _parser.end() && _stream.get(ch) )
        {
            _parser.parse(ch);
        }

        if( _parser.fail() )
        {
            log_debug("invalid HTTP reply");
            throw System::IOError( PT_ERROR_MSG("invalid HTTP reply") );
        }

        if( _parser.end() )
        {
            log_debug("reply ready");
            break;
        }

        if( ! _stream && _parser.begin() && reuseConnection)
        {
            // received pending EOF from previous response -> reconnect
            log_debug("reconnect to lost connection");
            reuseConnection = false;
            _socket.close();
            _sockbuf.discard();
#ifdef PT_HTTP_WITH_SSL
            _sslbuf.discard();
#endif
            _stream.clear();
            continue;  
        }

        log_debug("HTTP I/O error");
        throw System::IOError("HTTP I/O error");
    }

    log_debug("content-length: " << _replyHeader.contentLength());
    _contentLength = _replyHeader.contentLength();

    if( _replyHeader.chunkedTransferEncoding() )
    {
        _chunkedBuffer.reset( _stream.rdbuf() );
        _stream.rdbuf(&_chunkedBuffer);

        // TODO: close if not keepalive
    }
    else
    {
#ifdef PT_HTTP_WITH_SSL
        if(_ssl)
        {
            _sslbuf.setContentLength(_replyHeader.keepAlive());
            _sslbuf.setContentLength(_contentLength);
        }
#endif

        // TODO: implement InputBuffer for non-ssl case
    }

    return _replyHeader;
}


void ClientImpl::readBody(std::string& s)
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


void ClientImpl::beginRequest(const Request& request)
{
    log_trace("beginExecute");

    _reusedConnection = false;
    _errorPending = false;
    _request = &request;
    _replyHeader.clear();
    _parser.reset(true);

#ifdef PT_HTTP_WITH_SSL
    _sslbuf.setContentLength(-1);
#endif
    _sockbuf.setContentLength(-1);

    if(_ssl)
        _state = &ClientImpl::onHttpsHeader;
    else
        _state = &ClientImpl::onHttpHeader;
    
    if(  ! _socket.isConnected() )
    {
        log_debug("opening new connection to " << _addrInfo.host());
        _socket.beginConnect(_addrInfo);
        return;
    }

    log_debug("reusing previous connection");
    _reusedConnection = true;
    sendRequest(_stream, *_request);
        
    try
    {
        _sockbuf.beginWrite();
    }
    catch (const System::IOError&)
    {
        log_debug("write failed, reconnecting");

        _stream.clear();
        _sockbuf.discard();
#ifdef PT_HTTP_WITH_SSL
        _sslbuf.discard();
#endif
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
        (this->*_state)(sb);
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
        (this->*_state)(_sockbuf);
    }
    catch (const std::exception& e)
    {
        log_warn("exception occured: " << e.what());
        this->onError();
    }
}
#endif

bool ClientImpl::onHeader(std::streambuf& sb, bool ssl)
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
            return false;
        }

        throw System::IOError("unexpected EOF");
    }

    _reusedConnection = false;

    log_debug("advancing parser");
    _parser.advance(sb);

    if( _parser.fail() )
    {
        log_warn("http parser failed");
        throw std::runtime_error("http parser failed"); // TODO define exception class
    }

    if( _parser.end() )
    {
        log_debug("http header complete");
        _client->headerReceived().send(*_client);
    
        bool chunkedEncoding = _replyHeader.chunkedTransferEncoding();
        if(chunkedEncoding)
        {
            log_debug("chunked transfer encoding used");

            if(ssl)
                _state = &ClientImpl::onHttpsChunkedBody;
            else
                _state = &ClientImpl::onHttpChunkedBody;

            _chunkedBuffer.reset(&sb);
            _stream.rdbuf(&_chunkedBuffer);
        }
        else
        {
          if(ssl)
              _state = &ClientImpl::onHttpsBody;
          else
              _state = &ClientImpl::onHttpBody;

          _contentLength = _replyHeader.contentLength();
          log_debug("received header, content-length=" << _contentLength);
      
          if (_contentLength <= 0)
          {
              if( ! _replyHeader.keepAlive() )
              {
                  log_debug("close socket - no keep alive");
                  _socket.close();
              }

              _client->replyFinished().send(*_client);
              return false;
          }
        }
    
        if(sb.in_avail() > 0)
        {
            (this->*_state)( _sockbuf );
            return false;
        }
    }

    assert(sb.in_avail() <= 0);

    return true;
}


void ClientImpl::onHttpsHeader(System::StreamBuffer& sbuf)
{
    log_trace("onHttpsHeader");

#ifdef PT_HTTP_WITH_SSL
    bool cont = onHeader(_sslbuf, true);
    if(cont)
    {
        log_debug("begin reading body (SSL)");
        _sslbuf.beginRead();
    }
#endif
}


void ClientImpl::onHttpHeader(System::StreamBuffer& sbuf)
{
    log_trace("onHttpHeader");

    bool cont = onHeader(sbuf, false);
    if(cont)
    {
        log_debug("begin reading body (SSL)");
        sbuf.beginRead();
    }
}


bool ClientImpl::onBody(std::istream& is)
{
    log_trace("onBody");

    while( is.good() && _contentLength > 0 && is.rdbuf()->in_avail() > 0 )
    {
        std::streamsize avail = is.rdbuf()->in_avail();
        
        // TODO: may throw exception
        _client->bodyAvailable().send(*_client, is); 
        
        std::streamsize consumed = avail - is.rdbuf()->in_avail();
        _contentLength -= consumed;
        
        log_debug("content-length:" << _contentLength << " left:" << is.rdbuf()->in_avail());
    }

    if( is.fail() )
        throw System::IOError( PT_ERROR_MSG("error reading HTTP reply body") );

    if( _contentLength <= 0 )
    {
        log_debug("reply finished");

        if( ! _replyHeader.keepAlive() )
        {
            log_debug("close socket - no keep alive");
            _socket.close();
        }

        _client->replyFinished().send(*_client);
    }
    else if (_socket.isConnected() && is.good())
    {
        return true;
    }
    else
    {
        cancel();
    }

    return false;
}


void ClientImpl::onHttpsBody(System::StreamBuffer& sbuf)
{
    log_trace("onHttpsBody");

#ifdef PT_HTTP_WITH_SSL
    bool cont = this->onBody(_stream);
    if(cont)
    {
        log_debug("body not finished- begin read(SSL)");
        _sslbuf.beginRead();
    }
#endif
}


void ClientImpl::onHttpBody(System::StreamBuffer& sbuf)
{
    log_trace("onHttpBody");

    bool cont = this->onBody(_stream);
    if(cont)
    {
        sbuf.beginRead();
    }
}


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


void ClientImpl::cancel()
{
    _socket.close();
    _sockbuf.discard();
#ifdef PT_HTTP_WITH_SSL
    _sslbuf.discard();
#endif
    _stream.clear();

    _chunkedBuffer.reset();
}

} // namespace Http

} // namespace Pt




/*

void ClientImpl::processHeader(System::StreamBuffer& sbuf)
{
    log_trace("processHeader");

    std::streambuf* sb = &sbuf;

#ifdef PT_HTTP_WITH_SSL
    if(_ctx)
        sb = &_sslbuf;
#endif

    // reconnect when received pending EOF from previous response
    if( sbuf.device()->eof() )
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
    _parser.advance(*sb);

    if( _parser.fail() )
    {
        log_warn("http parser failed");
        throw std::runtime_error("http parser failed"); // TODO define exception class
    }

    if( _parser.end() )
    {
        log_debug("http header complete");
        _client->headerReceived(*_client);
    
        bool chunkedEncoding = _replyHeader.chunkedTransferEncoding();
        if(chunkedEncoding)
        {
            log_debug("chunked transfer encoding used");
            _state = &ClientImpl::processChunkedBody;
            _chunkedIStream.reset(sb);
        }
        else
        {
          _state = &ClientImpl::processBody;
          _contentLength = _replyHeader.contentLength();
          log_debug("received header, content-length=" << _contentLength);
      
          if (_contentLength <= 0)
          {
              if( ! _replyHeader.keepAlive() )
              {
                  log_debug("close socket - no keep alive");
                  _socket.close();
              }

              _client->replyFinished(*_client);
              return;
          }
        }
    
        if(sb->in_avail() > 0)
        {
            (this->*_state)(sbuf);
            return;
        }
    }

#ifdef PT_HTTP_WITH_SSL
    if(_ctx)
    {
        if(_sslbuf.in_avail() <= 0)
        {
          log_debug("begin reading body (SSL)");
          _sslbuf.beginRead();
          return;
        }
    }
    else
#endif
    if( sbuf.in_avail() <= 0 )
    {
        log_debug("begin reading body");
        sbuf.beginRead();
        return;
    }
}

void ClientImpl::processBody(System::StreamBuffer& sbuf)
{
    log_trace("processBody: content-length:" << _contentLength);

    std::streambuf* sb = &sbuf;
    std::istream* is = &_ios;

#ifdef PT_HTTP_WITH_SSL
    std::istream sslios(0);
    if(_ctx)
    {
        sslios.rdbuf(&_sslbuf);
        is = &sslios;
        sb = &_sslbuf;
    }
#endif

    while( is->good() && _contentLength > 0 && sb->in_avail() > 0 )
    {
        std::streamsize avail = sb->in_avail();
        
        // TODO: may throw exception
        _client->bodyAvailable(*_client, *is); 
        
        std::streamsize consumed = avail - sb->in_avail();
        _contentLength -= consumed;
        
        log_debug("content-length:" << _contentLength << " left:" << sb->in_avail());
    }

    if( is->fail() )
        throw System::IOError( PT_ERROR_MSG("error reading HTTP reply body") );

    if( _contentLength <= 0 )
    {
        log_debug("reply finished");

        if( ! _replyHeader.keepAlive() )
        {
            log_debug("close socket - no keep alive");
            _socket.close();
        }

        _client->replyFinished(*_client);
    }
    else if (_socket.isConnected() && is->good())
    {
#ifdef PT_HTTP_WITH_SSL
        if(_ctx)
        {
            log_debug("body not finished- begin read(SSL)");
            _sslbuf.beginRead();
        }
        else
#endif
        sbuf.beginRead();
    }
    else
    {
        cancel();
    }
}

void ClientImpl::processChunkedBody(System::StreamBuffer& sbuf)
{
    log_trace("processChunkedBody");

    std::streambuf* sb = &sbuf;

#ifdef PT_HTTP_WITH_SSL

    if(_ctx)
    {
        sb = &_sslbuf;
    }
#endif

    if( _chunkedIStream.rdbuf()->in_avail() > 0 )
    {
        if( ! _chunkedIStream.eod() )
        {
            log_debug("read chunked encoding body");

            while (_chunkedIStream.good()
                && _chunkedIStream.rdbuf()->in_avail() > 0
                && !_chunkedIStream.eod())
            {
                log_debug("bodyAvailable");
                _client->bodyAvailable(*_client, _chunkedIStream);
            }

            log_debug("in_avail=" << _chunkedIStream.rdbuf()->in_avail() << " eod=" << _chunkedIStream.eod());
            if( _chunkedIStream.eod() )
            {
                if( _replyHeader.hasHeader("Trailer") )
                    _parser.readHeader();
                else
                    _client->replyFinished(*_client);
            }
        }

        if (_chunkedIStream.eod() && sb->in_avail() > 0)
        {
            log_debug("read chunked encoding post headers");

            _parser.advance(*sb);
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

                _client->replyFinished(*_client);
            }
        }

        if( _chunkedIStream.fail() )
            throw System::IOError( PT_ERROR_MSG("error reading HTTP reply body") );
    }
    else if( _chunkedIStream.eod() )
    {
        if( _replyHeader.hasHeader("Trailer") )
            _parser.readHeader();
        else
            _client->replyFinished(*_client);
    }

    if (_socket.isConnected())
    {
        if ((!_chunkedIStream.eod() || !_parser.end()))
        {
            log_debug("call beginRead");

#ifdef PT_HTTP_WITH_SSL
            if(_ctx)
                _sslbuf.beginRead();
            else
#endif
            sbuf.beginRead();
        }
    }
    else
    {
        cancel();
    }
}
*/
