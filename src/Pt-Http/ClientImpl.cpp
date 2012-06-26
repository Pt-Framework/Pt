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

Client2::Client2(std::iostream& ios)
: _ios(&ios)
, _readHeader(true)
, _parser(_parseEvent, true)
, _parseEvent(_replyHeader)
, _chunkedIStream( ios.rdbuf() )
{
}


Client2::Client2()
: _ios(0)
, _readHeader(true)
, _parser(_parseEvent, true)
, _parseEvent(_replyHeader)
, _chunkedIStream()
{
}


void Client2::writeRequest(const Request& request)
{
    log_debug("send request " << request.url());
    
    if( ! _ios )
        return;

    static const char* contentLength = "Content-Length";
    static const char* connection = "Connection";
    static const char* date = "Date";
    static const char* host = "Host";
    static const char* authorization = "Authorization";
    static const char* userAgent = "User-Agent";

    *_ios << request.method() << ' '
            << request.url() << " HTTP/"
            << request.header().httpVersionMajor() << '.'
            << request.header().httpVersionMinor() << "\r\n";

    for (RequestHeader::const_iterator it = request.header().begin();
        it != request.header().end(); ++it)
    {
        *_ios << it->first << ": " << it->second << "\r\n";
    }

   if (!request.header().hasHeader(contentLength))
    {
        *_ios << "Content-Length: " << request.bodySize() << "\r\n";
    }

    if (!request.header().hasHeader(connection))
    {
        *_ios << "Connection: keep-alive\r\n";
    }

    if (!request.header().hasHeader(date))
    {
        char buffer[50];
        *_ios << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
    }

    /*if (!request.header().hasHeader(host))
    {
        *_ios << "Host: " << addrInfo.host();
        unsigned short port = addrInfo.port();
        if (port != 80)
            *_ios << ':' << port;
        *_ios << "\r\n";
    }*/

    if (!request.header().hasHeader(userAgent))
    {
        *_ios << "User-Agent: Pt-Http-client\r\n";
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
        *_ios << "Authorization: Basic " << d.str() << "\r\n";
    }

    *_ios << "\r\n";

    log_debug("send body; " << request.bodySize() << " bytes");

    request.sendBody(*_ios);

    _readHeader = true;
}


bool Client2::readReply()
{
    if (_readHeader)
    {
        parseHeader();
    }
    else
    {
        parseBody();
    }

    return _ios->rdbuf()->in_avail() == 0;
}


void Client2::parseHeader()
{
    if( ! _ios )
        return;

    _parser.advance(*_ios);

    if( _parser.fail() )
        throw std::runtime_error("http parser failed"); // TODO define exception class

    if( ! _parser.end() )
        return;
    
    bool chunkedEncoding = _replyHeader.chunkedTransferEncoding();

    headerReceived.send(*this);
    _readHeader = false;

    if (chunkedEncoding)
    {
        log_debug("chunked transfer encoding used");

        _chunkedIStream.reset();

        if( _ios->rdbuf()->in_avail() > 0 )
        {
            parseBody();
            return;
        }
    }
    else
    {
        _contentLength = _replyHeader.contentLength();
        log_debug("header received - content-length=" << _contentLength);

        if (_contentLength > 0)
        {
            if( _ios->rdbuf()->in_avail() > 0 )
            {
                parseBody();
                return;
            }
        }

        // TODO: caller must react to close/keepalive in Connection field of reply            
        return;
    }
    
    return;
}

bool Client2::parseBody()
{
    log_trace("processBodyAvailable");

    bool chunkedEncoding = _replyHeader.chunkedTransferEncoding();

    if (chunkedEncoding)
    {
        if (_chunkedIStream.rdbuf()->in_avail() > 0)
        {
            if (!_chunkedIStream.eod())
            {
                log_debug("read chunked encoding body");

                while (_chunkedIStream.good()
                    && _chunkedIStream.rdbuf()->in_avail() > 0
                    && !_chunkedIStream.eod())
                {
                    log_debug("bodyAvailable");
                    bodyAvailable(*this, _chunkedIStream);
                }

                log_debug("in_avail=" << _chunkedIStream.rdbuf()->in_avail() << " eod=" << _chunkedIStream.eod());
                if( _chunkedIStream.eod() )
                {
                    if( _replyHeader.hasHeader("Trailer") )
                        _parser.readHeader();
                    else
                        replyFinished(*this);
                }
            }

            if (_chunkedIStream.eod() && _ios->rdbuf()->in_avail() > 0)
            {
                log_debug("read chunked encoding post headers");

                _parser.advance( *(_ios->rdbuf()) );
                if (_parser.fail())
                    throw std::runtime_error("http parser failed"); // TODO define exception class

                if( _parser.end() )
                {
                    log_debug("reply finished");
                    replyFinished(*this);
                }
            }

            if (_chunkedIStream.fail())
                throw System::IOError( PT_ERROR_MSG("error reading HTTP reply body") );
        }
        else if( _chunkedIStream.eod() )
        {
            if( _replyHeader.hasHeader("Trailer") )
                _parser.readHeader();
            else
                replyFinished(*this);
        }
    }
    else
    {
        log_debug("content-length(pre)=" << _contentLength);

        while (_ios->good() && _contentLength > 0 && _ios->rdbuf()->in_avail() > 0)
        {
            _contentLength -= bodyAvailable(*this, *_ios); // TODO: may throw exception
            log_debug("content-length(post)=" << _contentLength);
        }

        if (_ios->fail())
            throw System::IOError( PT_ERROR_MSG("error reading HTTP reply body") );

        if( _contentLength <= 0 )
        {
            log_debug("reply finished");
            replyFinished(*this);
        }
    }

    return false;
}


//////////////////////////////////////////////////////////////////////////////
// ClientImpl
//////////////////////////////////////////////////////////////////////////////

void ClientImpl::ParseEvent::onHttpReturn(unsigned ret, const std::string& text)
{
    _replyHeader.httpReturn(ret, text);
}


ClientImpl::ClientImpl(Client* client)
: _client(client)
, _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _ios(8192, true)
, _chunkedIStream(_ios.rdbuf())
#ifdef PT_HTTP_WITH_SSL
, _ctx(0)
, _sslbuf(_ios)
#endif
, _contentLength(0)
, _reusedConnection(false)
, _errorPending(false)
, _state( &ClientImpl::processHeader )
{
    _ios.attach(_socket);
    _socket.connected() += Pt::slot(*this, &ClientImpl::onConnect);
    Pt::connect(_ios.buffer().outputReady(), *this, &ClientImpl::onOutput);
    Pt::connect(_ios.buffer().inputReady(), *this, &ClientImpl::onInput);
}


ClientImpl::ClientImpl(Client* client, const Net::AddrInfo& addrinfo)
: _client(client)
, _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _addrInfo(addrinfo)
, _ios(8192, true)
, _chunkedIStream(_ios.rdbuf())
#ifdef PT_HTTP_WITH_SSL
, _ctx(0)
, _sslbuf(_ios)
#endif
, _contentLength(0)
, _reusedConnection(false)
, _errorPending(false)
, _state( &ClientImpl::processHeader )
{
    _ios.attach(_socket);
    _socket.connected() += Pt::slot(*this, &ClientImpl::onConnect);
    Pt::connect(_ios.buffer().outputReady(), *this, &ClientImpl::onOutput);
    Pt::connect(_ios.buffer().inputReady(), *this, &ClientImpl::onInput);
}


ClientImpl::ClientImpl(Client* client, System::EventLoop& loop, const Net::AddrInfo& addrinfo)
: _client(client)
, _parseEvent(_replyHeader)
, _parser(_parseEvent, true)
, _request(0)
, _addrInfo(addrinfo)
, _ios(8192, true)
, _chunkedIStream(_ios.rdbuf())
#ifdef PT_HTTP_WITH_SSL
, _ctx(0)
, _sslbuf(_ios)
#endif
, _contentLength(0)
, _reusedConnection(false)
, _errorPending(false)
, _state( &ClientImpl::processHeader )
{
    _ios.attach(_socket);
    _socket.connected() += Pt::slot(*this, &ClientImpl::onConnect);
    Pt::connect(_ios.buffer().outputReady(), *this, &ClientImpl::onOutput);
    Pt::connect(_ios.buffer().inputReady(), *this, &ClientImpl::onInput);
    setActive(loop);
}


const ReplyHeader& ClientImpl::execute(const Request& request)
{
    log_trace("execute request " << request.url());

    _replyHeader.clear();
    _parser.reset(true);

    for(;;)
    {
        bool reuseConnection = _socket.isConnected();
        if( ! reuseConnection)
        {
            log_debug("connect");
            _socket.connect(_addrInfo);
        }
        
        log_debug("sending request");
        sendRequest(request);
        _ios.flush();

        log_debug("reading reply");
        char ch = ' ';
        while( ! _parser.end() && _ios.get(ch) )
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

        if( ! _ios && _parser.begin() && reuseConnection)
        {
            // received pending EOF from previous response -> reconnect
            log_debug("reconnect to lost connection");
            reuseConnection = false;
            _socket.close();
            _ios.clear();
            _ios.buffer().discard();
            continue;  
        }

        log_debug("HTTP I/O error");
        throw System::IOError("HTTP I/O error");
    }

    return _replyHeader;
}


void ClientImpl::readBody(std::string& s)
{
    s.clear();

    bool chunkedEncoding = _replyHeader.chunkedTransferEncoding();
    _chunkedIStream.reset();

    if(chunkedEncoding)
    {
        log_debug("read body with chunked encoding");

        char ch;
        while (_chunkedIStream.get(ch))
            s += ch;

        log_debug("eod=" << _chunkedIStream.eod());

        if (!_chunkedIStream.eod())
            throw System::IOError( PT_ERROR_MSG("error reading HTTP reply body: incomplete chunked data stream") );
    }
    else
    {
        unsigned n = _replyHeader.contentLength();

        log_debug("read body; content-size: " << n);

        s.reserve(n);

        char ch;
        while (n-- && _ios.get(ch))
            s += ch;

        if (_ios.fail())
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
    _state = &ClientImpl::processHeader;
    
    if(  ! _socket.isConnected() )
    {
        log_debug("opening new connection to " << _addrInfo.host());
        _socket.beginConnect(_addrInfo);
        return;
    }

    log_debug("reusing previous connection");
    _reusedConnection = true;
    sendRequest(*_request);
        
    try
    {
        _ios.buffer().beginWrite();
    }
    catch (const System::IOError&)
    {
        log_debug("write failed, reconnecting");

        _ios.clear();
        _ios.buffer().discard();
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


void ClientImpl::sendRequest(const Request& request)
{
    log_debug("send request " << request.url());

    static const char* contentLength = "Content-Length";
    static const char* connection = "Connection";
    static const char* date = "Date";
    static const char* host = "Host";
    static const char* authorization = "Authorization";
    static const char* userAgent = "User-Agent";

    _ios << request.method() << ' '
         << request.url() << " HTTP/"
         << request.header().httpVersionMajor() << '.'
         << request.header().httpVersionMinor() << "\r\n";

    for (RequestHeader::const_iterator it = request.header().begin();
        it != request.header().end(); ++it)
    {
        _ios << it->first << ": " << it->second << "\r\n";
    }

   if (!request.header().hasHeader(contentLength))
    {
        _ios << "Content-Length: " << request.bodySize() << "\r\n";
    }

    if (!request.header().hasHeader(connection))
    {
        _ios << "Connection: keep-alive\r\n";
    }

    if (!request.header().hasHeader(date))
    {
        char buffer[50];
        _ios << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
    }

    if (!request.header().hasHeader(host))
    {
        _ios << "Host: " << _addrInfo.host();
        unsigned short port = _addrInfo.port();
        if (port != 80)
            _ios << ':' << port;
        _ios << "\r\n";
    }

    if (!request.header().hasHeader(userAgent))
    {
        _ios << "User-Agent: Pt-Http-client\r\n";
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
        _ios << "Authorization: Basic " << d.str() << "\r\n";
    }

    _ios << "\r\n";

    log_debug("send body; " << request.bodySize() << " bytes");

    request.sendBody(_ios);
}


void ClientImpl::onConnect(Net::TcpSocket& socket)
{
    log_trace("onConnect");

    try
    {
        socket.endConnect();
        sendRequest(*_request);

        log_debug("request sent - begin write");
        _ios.buffer().beginWrite();
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
            _client->requestSent(*_client);
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
    _client->replyFinished(*_client);

    if (_errorPending)
    {
        _errorPending = false;
        throw;
    }
}


void ClientImpl::processHeader(System::StreamBuffer& sb)
{
    // reconnect when received pending EOF from previous response
    if( sb.device()->eof() )
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
    _parser.advance(sb);

    if( _parser.fail() )
        throw std::runtime_error("http parser failed"); // TODO define exception class

    if( ! _parser.end() )
    {
        sb.beginRead();
        return;
    }

    _client->headerReceived(*_client);
    
    bool chunkedEncoding = _replyHeader.chunkedTransferEncoding();
    if(chunkedEncoding)
    {
        log_debug("chunked transfer encoding used");
        _state = &ClientImpl::processChunkedBody;
        _chunkedIStream.reset();
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

    if( sb.in_avail() <= 0 )
    {
        sb.beginRead();
        return;
    }

    (this->*_state)(sb);
}


void ClientImpl::processBody(System::StreamBuffer& sb)
{
    log_trace("processBody: content-length(pre)=" << _contentLength);

    while( _ios.good() && _contentLength > 0 && sb.in_avail() > 0 )
    {
        std::streamsize avail = sb.in_avail();
        
        // TODO: may throw exception
        _client->bodyAvailable(*_client, _ios); 
        
        std::streamsize consumed = avail - sb.in_avail();
        _contentLength -= consumed;
        
        log_debug("content-length(post)=" << _contentLength);
    }

    if( _ios.fail() )
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
    else if (_socket.isConnected() && _ios.good())
    {
        sb.beginRead();
    }
    else
    {
        cancel();
    }
}


void ClientImpl::processChunkedBody(System::StreamBuffer& sb)
{
    log_trace("processChunkedBody");

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

        if (_chunkedIStream.eod() && sb.in_avail() > 0)
        {
            log_debug("read chunked encoding post headers");

            _parser.advance(sb);
            if (_parser.fail())
                throw std::runtime_error("http parser failed"); // TODO define exception class

            if( _parser.end() )
            {
                log_debug("reply finished");

                if (!_replyHeader.keepAlive())
                {
                    log_debug("close socket - no keep alive");
                    _socket.close();
                }

                _client->replyFinished(*_client);
            }
        }

        if (_chunkedIStream.fail())
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
            sb.beginRead();
        }
    }
    else
    {
        cancel();
    }
}


void ClientImpl::cancel()
{
    _socket.close();
    _ios.clear();
    _ios.buffer().discard();

    _chunkedIStream.reset();
}

} // namespace Http

} // namespace Pt
