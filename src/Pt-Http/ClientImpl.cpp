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


//void HttpBuffer::writeReply(const ReplyHeader& header, const MessageBuffer& mbuf)
//{
//    log_trace("HttpBuffer::writeReply()");
//
//    if( ! _chunked && ! _contentLength)
//    {
//        const char* server = "Server";
//        const char* connection = "Connection";
//        const char* date = "Date";
//
//        _keepAlive = header.keepAlive();
//        _contentLength = header.contentLength();
//        _chunked = header.chunkedTransferEncoding();
//
//        log_debug("keep-alive: " << _keepAlive);
//        log_debug("chunked: " << _chunked);
//        log_debug("content-length: " << _contentLength);
//
//        std::ostream os(_sbuf);
//
//        os <<"HTTP/"
//           << header.httpVersionMajor() << '.'
//           << header.httpVersionMinor() << ' '
//           << header.httpReturnCode() << ' '
//           << header.httpReturnText() << "\r\n";
//
//        ReplyHeader::const_iterator it;
//        for(it = header.begin(); it != header.end(); ++it)
//        {
//            os << it->first << ": " << it->second << "\r\n";
//        }
//
//        if( ! _chunked)
//        {
//            os << "Content-Length: " << mbuf.size() << "\r\n";
//        }
//
//        if( ! header.hasHeader(server) )
//        {
//            os << "Server: Pt-Net-Server\r\n";
//        }
//
//        if( ! header.hasHeader(connection) )
//        {
//            os << "Connection: "
//               << (_keepAlive ? "keep-alive" : "close")
//               << "\r\n";
//        }
//
//        if( ! header.hasHeader(date) )
//        {
//            char buffer[50];
//            os << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
//        }
//
//        os << "\r\n";
//    }
//
//    if(_chunked)
//    {
//        std::ostream os(_sbuf);
//        os << std::hex << mbuf.size() << "\r\n";
//    }
//
//    _sbuf->sputn( mbuf.data(), mbuf.size() );
//
//    if(_chunked)
//        _sbuf->sputn("\r\n", 2);
//}


//void HttpBuffer::finishReply(const ReplyHeader& header, const MessageBuffer& mbuf)
//{
//    log_trace("HttpBuffer::finishReply()");
//    writeReply(header, mbuf);
//
//    if(_chunked)
//        _sbuf->sputn("0\r\n\r\n", 5);
//}


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
        _bodyFinished.send();
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


int HttpBuffer::sync()
{ 
    typedef HttpBuffer::traits_type traits_type;

    if( this->pptr() )
    {
        while(this->pptr() > this->pbase())
        {
            const HttpBuffer::int_type ch = this->overflow(traits_type::eof());
            if( ch == traits_type::eof() )
                return -1;
        }
    }

    return 0;
}


HttpBuffer::int_type HttpBuffer::overflow(int_type ch)
{
    typedef HttpBuffer::traits_type traits_type;

    if( ! _obuffer)
    {
        _obufferSize = BufferSize;
        _obuffer = new char[_obufferSize];
        this->setp(_obuffer, _obuffer + _obufferSize);
    }
    else if( traits_type::eq_int_type(ch, traits_type::eof()) )
    {
        // normal blocking overflow case
        size_t avail    = this->pptr() - _obuffer;
        size_t written  = _sbuf->sputn(_obuffer, avail);
        size_t leftover = avail - written;

        if(leftover > 0)
            traits_type::move(_obuffer, _obuffer + written, leftover);

        this->setp(_obuffer, _obuffer + _obufferSize);
        this->pbump(leftover);
    }
    else
    {
        // if overflow is not called by sync/flush we copy the output 
        // buffer to a larger one
        size_t bufsize = _obufferSize + BufferSize;
        char* buf = new char[ bufsize ];
        traits_type::copy(buf, _obuffer, _obufferSize);
        std::swap(_obuffer, buf);
        this->setp(_obuffer, _obuffer + bufsize);
        this->pbump(_obufferSize);
        _obufferSize = bufsize;
        delete [] buf;
    }

    // if the overflow char is not EOF put it in buffer
    if(traits_type::eq_int_type(ch, traits_type::eof()) == false)
    {
        *pptr() = traits_type::to_char_type(ch);
        this->pbump(1);
    }

    return traits_type::not_eof(ch);
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
, _httpbuf()
, _stream(&_httpbuf)
, _reusedConnection(false)
, _chunked(false)
, _hstate(Idle)
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
, _httpbuf()
, _stream(&_httpbuf)
, _reusedConnection(false)
, _chunked(false)
, _hstate(Idle)
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
, _httpbuf()
, _stream(&_httpbuf)
, _reusedConnection(false)
, _chunked(false)
, _hstate(Idle)
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
    //_httpbuf.bodyFinished() += Pt::slot(*this, &ClientImpl::onBodyFinished);
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


void ClientImpl::onBodyFinished()
{
    log_trace("onBodyFinished");

    if( ! _request->header().keepAlive() )
    {
        log_debug("cancelling, no keep alive");
        this->cancel();
    }
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
#ifdef PT_HTTP_WITH_SSL
        if(_ssl)
        {
            _sslbuf.beginWrite();
        }
        else
#endif
            _sockbuf.beginWrite();
    }
    catch (const System::IOError&)
    {
        log_debug("write failed, reconnecting");
        cancel();
        _socket.beginConnect(_addrInfo);
    }
}


void ClientImpl::beginSend(bool endOfRequest)
{
    if( _hstate == Idle )
    {
        _hstate = endOfRequest ? Idle : OnRequest;

        _stream.rdbuf( _httpbuf.buffer() );
        _errorPending = false;
    
        if( ! _socket.isConnected() )
        {
            log_debug("opening new connection to " << _addrInfo.host());
            _socket.beginConnect(_addrInfo);
            return;
        }

        log_debug("reusing previous connection");

        if(endOfRequest)
            sendRequest(_stream, _req);
        else
            sendChunked(_stream, _req);

        beginWrite();
        return;
    }
    
    if(_hstate != OnRequest)
        throw System::IOPending("pending HTTP reply");

    _stream << std::hex << _req.size() << std::dec << "\r\n";
    _stream.write( _req.data(), _req.size() );
    _stream.write("\r\n", 2);
    _req.clearBody();

    if(endOfRequest)
    {
        _stream.write("\r\n", 2);
        _stream.write("0\r\n\r\n", 5);

        _hstate = Idle;
    }

    //TODO: only write in 8K blocks
    beginWrite();
}


bool ClientImpl::endSend()
{
    endWrite();

    bool remaining = outputAvailable();
    return (! remaining) && (_hstate == Idle);
}


void ClientImpl::onOutput2(System::StreamBuffer& sb)
{
    _client->requestSent().send(*_client);
}


#ifdef PT_HTTP_WITH_SSL
void ClientImpl::onSslOutput2(Ssl::IOBuffer& sb)
{
    _client->requestSent().send(*_client);
}
#endif


void ClientImpl::beginReceive()
{
    if(_hstate == Idle)
    {
        _replyHeader.clear();
        _parser.reset(true);
        _hstate = OnReplyHeader;
    }

    if(_hstate == OnReply|| _hstate == OnReplyHeader)
    {
#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
        if( _sslbuf.in_avail() )
        {
            _client->replyReceived().send(*_client);
            return;
        }
#endif
        if( _sockbuf.in_avail() )
        {
            _client->replyReceived().send(*_client);
            return;
        }

        beginRead();
    }
    else
        throw System::IOPending("pending HTTP request");
}


bool ClientImpl::endReceive()
{
    endRead();
    processReply();
    return _parser.end() || _httpbuf.in_avail();
}


bool ClientImpl::isEnd() const
{
    return _hstate == Idle;
}


void ClientImpl::onInput2(System::StreamBuffer& sb)
{
    _client->replyReceived().send(*_client);
}


#ifdef PT_HTTP_WITH_SSL
void ClientImpl::onSslInput2(Ssl::IOBuffer& sb)
{
    _client->replyReceived().send(*_client);
}
#endif



void ClientImpl::processReply()
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
            return;
        }
    
        log_debug("http header complete");
        _stream.rdbuf(&_httpbuf);
        _httpbuf.beginBody(_replyHeader);
        //_client->headerReceived().send(*_client);
        _hstate = OnReply;
    }

    if(_hstate == OnReply)
    {
        log_debug("processing reply body");

        _httpbuf.import();
        log_debug("available: " << _httpbuf.in_avail());

        if( _httpbuf.isEnd() )
        {
            _hstate = Idle;
        }
        
        if( ! _req.header().keepAlive() )
        {
            log_debug("cancelling, no keep alive");
            this->cancel();
        }
    }
}


void ClientImpl::onConnect2(Net::TcpSocket& socket)
{
    log_trace("onConnect");

    socket.endConnect();

#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
    {
        log_debug("begining SSL handshake");
        _sslbuf.beginConnect();
        return;
    }
#endif
    sendRequest(_stream, _req);
        
    log_debug("request sent - begin write");
    beginWrite();

}


bool ClientImpl::outputAvailable()
{
#ifdef PT_HTTP_WITH_SSL
    // TODO: need to check _sslbuf.out_avail()
    if(_ssl)
        _sslbuf.pubsync();
#endif

    return _sockbuf.out_avail() > 0;
}


bool ClientImpl::beginWrite()
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


void ClientImpl::endWrite()
{
#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
        _sslbuf.endWrite();
    else
#endif
        _sockbuf.endWrite();
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


void ClientImpl::endRead()
{
#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
        _sslbuf.endRead();
    else
#endif
        _sockbuf.endRead();
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


void ClientImpl::sendChunked(std::ostream& os, const Request& request)
{
    log_debug("send chunked request " << request.url());

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

    _stream << std::hex << _req.size() << std::dec << "\r\n";
    _stream.write( _req.data(), _req.size() );
    _stream.write("\r\n", 2);

    _req.clearBody();
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

    log_debug("send body; " << request.size() << " bytes");

    os.write(request.data(), request.size());
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

    /////////////////////////////////////////////////////////////////
    // TODO: only send signal and call endWrite in endRequest !!!!
    //       error handling gets much easier this way !!!!!!!!!!
    /////////////////////////////////////////////////////////////////
    try
    {
        sb.endWrite();

        if( sb.out_avail() > 0 )
        {
            sb.beginWrite();
        }
        else
        {
            _client->requestSent().send(*_client);

            if( sb.out_avail() )
                sb.beginWrite();
            else
                sb.beginRead();

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

    while( _httpbuf.in_avail() )
    {
        _client->bodyReceived().send(*_client);

        _httpbuf.import();
        log_debug("available: " << _httpbuf.in_avail());
    } 
    
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
