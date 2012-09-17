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
, _ssl(false)
#ifdef PT_HTTP_WITH_SSL
, _sslbuf( _sockbuf )
#endif
, _httpbuf()
, _timeout(WaitInfinite)
, _keepaliveTimeout(WaitInfinite)
, _readBytes(0)
, _state(NotConnected)
, _chunked(false)
, _keepAlive(false)
, _onTimeout(false)
{
    _socket.connected() += Pt::slot(*this, &Connection::onConnect);

    _sockbuf.attach(_socket);
    _sockbuf.outputReady() += slot(*this, &Connection::onHttpOutput);
    _sockbuf.inputReady() += slot(*this, &Connection::onHttpInput);

#ifdef PT_HTTP_WITH_SSL
    _sslbuf.handshakeFinished() += slot(*this, &Connection::onHttpsHandshake);
#endif

    _httpbuf.attach(_sockbuf);
}


Connection::~Connection()
{
}


void Connection::accept(Net::TcpServer& tcpServer)
{
    log_trace("Connection::accept");    
    cancel();

    _socket.accept(tcpServer);

    if(_ssl)
        _state = SslNotAccepted;
    else
        _state = Accepted;
}


void Connection::setHost(const Net::AddrInfo& addrinfo)
{
    if(_state != NotConnected)
    {
        cancel();
    }

    _addrInfo = addrinfo;
}

#ifdef PT_HTTP_WITH_SSL
void Connection::setSecure(Ssl::Context& ctx)
{
    log_debug("initialize HTTPS connection");

    _sslbuf.init(ctx);

    if( ! _ssl)
    {
        if(_state == Accepted)
        {
            _state = SslNotAccepted;
        }
        else if(_state != NotConnected)
        {
            cancel();
        }

        _sockbuf.outputReady() -= slot(*this, &Connection::onHttpOutput);
        _sockbuf.inputReady() -= slot(*this, &Connection::onHttpInput);

        _sslbuf.outputReady() += slot(*this, &Connection::onHttpsOutput);
        _sslbuf.inputReady() += slot(*this, &Connection::onHttpsInput);
        _ssl = true;
    }

    _httpbuf.attach(_sslbuf);
}

#else

void Connection::setSecure(Ssl::Context&)
{
}

#endif

void Connection::setActive(System::EventLoop& loop)
{
    _socket.setActive(loop);
    _timer.setActive(loop);
}


void Connection::cancel()
{
    log_debug("cancelling connection");

    _timer.stop();
    _readBytes = 0;
    _socket.close();
    _reply = 0;
    _request = 0;
    _state = NotConnected;
    _chunked = false;
    _keepAlive = false;
    _onTimeout = false;
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
        _timer.start( _timeout );
        _socket.beginConnect(_addrInfo);
        return;
    }

#ifdef PT_HTTP_WITH_SSL
    if(_state == SslHandshake)
    {
        log_debug("begining SSL handshake");
        _timer.start( _timeout );
        _sslbuf.beginConnect();
        return;
    }
#endif

    std::ostream os( _httpbuf.buffer() );

    if( request.isFinished() )
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
            writeRequestHeader(os, request);

            log_debug("writing body: " << request.size() << " bytes");
            os.write(request.data(), request.size());
        }

        log_debug("sending HTTP request");
        _request->onOutput();
        return;
    }

    if( ! _chunked )
    {
        log_debug("sending chunked header");
        _chunked = true;
        writeRequestHeader(os, request);
    }

    log_debug("sending HTTP chunk: "  << _request->size() << " bytes");

    if(_request->size() > 0)
    {
        os << std::hex << _request->size() << std::dec << "\r\n";
        os.write( _request->data(), _request->size() );
        os.write("\r\n", 2);
    }

    // TODO: only if over 8K data to send
    beginWrite();
}


MessageProgress Connection::endSendRequest()
{
    log_trace("Connection::endSendRequest");
    MessageProgress progress;

    if(_onTimeout)
        throw Pt::System::IOError("timeout");

    if(_state == NotConnected)
    {
        _timer.stop();
        _socket.endConnect();
        log_debug("connected to " << _addrInfo.host());

        if(_ssl)
            _state = SslHandshake;
        else
            _state = Connected;

        return progress;
    }

#ifdef PT_HTTP_WITH_SSL
    if(_state == SslHandshake)
    {
        _timer.stop();
        _sslbuf.endHandshake();
        _state = Connected;
        log_debug("SSL handshake finished");
        return progress;
    }
#endif

    Request* req = _request;
    _request = 0;

    if( ! req->isFinished() )
    {
        endWrite();

        if( _socket.eof() )
        {
            throw Pt::System::IOError("connection lost while sending request");
        }
    }
 
    // indicates that the request or chunk was completely written
    log_debug("request data sent");
    progress.setFinished();
    return progress;
}


// NOTE: maybe add a flag to cause a flush in the future
void Connection::beginSendReply(Reply& reply)
{
    log_trace("Connection::beginSendReply");

    _reply = &reply;

    ReplyHeader& header = _reply->header();
    std::ostream os( _httpbuf.buffer() );

    _keepAlive = _keepAlive && _reply->header().keepAlive();

    if( ! _keepAlive && outputAvailable() )
    {
        beginWrite();
        return;
    }

    if( reply.isFinished() )
    {
        if(_chunked)
        {
            if(_reply->size() > 0)
            {
                os << std::hex << _reply->size() << std::dec << "\r\n";
                os.write( _reply->data(), _reply->size() );
                os.write("\r\n", 2);
            }

            os.write("0\r\n\r\n", 5);
            _chunked = false;
        }
        else
        {          
            writeReplyHeader(os, reply);

            log_debug("writing body: " << reply.size() << " bytes");
            os.write( reply.data(), reply.size() );
        }

        log_debug("begin writing reply");

        if( _keepAlive )
            _reply->onOutput(); 
        else
            beginWrite();

        return;
    }

    if( ! _chunked )
    {
        log_debug("sending chunked header");
        _chunked = true;
        writeReplyHeader(os, *_reply);
    }

    if(_reply->size() > 0)
    {
        os << std::hex << _reply->size() << std::dec << "\r\n";
        os.write( _reply->data(), _reply->size() );
        os.write("\r\n", 2);
    }

    // TODO: only if over 8K data to send
    // this way the timeout can be for the 8K chunk
    beginWrite();
}


MessageProgress Connection::endSendReply()
{
    log_trace("Connection::endSendReply");

    MessageProgress progress;
    
    if(_onTimeout)
        throw Pt::System::IOError("timeout");
        
    endWrite();

    if( _socket.eof() )
    {
        throw Pt::System::IOError("connection lost while sending reply");
    }

    // keepalive -> leave data in the output buffer
    // close -> make sure we sent all data
    if( ! _keepAlive && outputAvailable() )
    {
        log_debug("still data to send");
        return progress;
    }
  
    progress.setFinished();

    if( ! _reply->isFinished() )
    {
        log_debug("reply is not finished");

        // indicates that chunk was completely written
        return progress;
    }

    _reply = 0;

    if( ! _keepAlive)
    {
        log_debug("no keep alive, closing connection");
        cancel();
    }

    // indicates that request was completely written
    return progress;
}


void Connection::beginReceiveRequest(Request& request)
{
    log_trace("Connection::beginReceiveRequest " << _state);

    _request = &request;

#ifdef PT_HTTP_WITH_SSL
    if(_state == SslNotAccepted)
    {
        log_debug("beginning SSL handshake");
        _timer.start( _timeout );
        _sslbuf.beginAccept();
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
    _parseEvent.init( request.header() );

    if( _parser.begin() )
    {
        _readBytes = 0;
        if(_keepAlive)
        {
            log_debug("use keep alive timeout");
            _timer.start(_keepaliveTimeout);
        }
        else
        {
            _timer.start( _timeout );
        }
    }
    
    beginRead();
}


MessageProgress Connection::endReceiveRequest()
{
    log_trace("Connection::endReceiveRequest");
    MessageProgress progress;

    if(_onTimeout)
        throw Pt::System::IOError("timeout");

#ifdef PT_HTTP_WITH_SSL
    if(_state == SslNotAccepted)
    {
        _timer.stop();
        _sslbuf.endHandshake();
        _state = Accepted;
        log_debug("SSL handshake finished");
        return progress;
    }
#endif

    if(_state == ReplyOutputPending)
    {
        log_debug("sent remaining reply data");
        endWrite();

        log_debug("remaining: " << _sockbuf.out_avail());
        _state = Accepted;
        return progress;
    }

    endRead();

    // TODO: if we haven't read anything yet we maybe should not report is as
    //       an error, the client might have run into a keepalive timeout. 
    //       Need another MessageProgress state to indicate CLOSED event
    if( _socket.eof() )
    {
        throw Pt::System::IOError("connection lost while receiving request");
    }

    if( ! _parser.end() )
    {       
        // switch from keepalive timeout to receive timeout
        if( _parser.begin() && _keepAlive )
            _timer.start(_timeout);

        _parser.advance( *_httpbuf.buffer() );

        if( _parser.fail() )
        {
            log_warn("http parser failed");

            // TODO define exception class
            // TODO: handle any previously pipelined reply
            throw std::runtime_error("http parser failed"); 
        }

        if( ! _parser.end() )
        {
            return progress;
        }

        _keepAlive = _request->header().keepAlive();
        progress.setOnHeader();
        _httpbuf.reset();
        _httpbuf.beginBody( _request->header() );
    }

    if( _parser.end() )
    {
        std::streamsize avail = _httpbuf.in_avail();
        
        _httpbuf.import();
        log_debug("bytes available: " << _httpbuf.in_avail());
        
        _readBytes += _httpbuf.in_avail() - avail;
        if(_readBytes >= 8192)
            _timer.start(_timeout);
        
        if(_httpbuf.in_avail() > 0)
            progress.setOnBody();

        if( _httpbuf.isEnd() )
        {
            log_debug("request body finished");
            progress.setFinished();

            _timer.stop();
            _readBytes = 0;
            _request = 0;
            _parser.reset(false);
        }
    }

    return progress;
}


void Connection::beginReceiveReply(Reply& r)
{
    log_trace("Connection::beginReceiveReply");

    _reply = &r;

    if( outputAvailable() )
    {
        log_debug("sending remaining request data");
        beginWrite();
        _state = RequestOutputPending;
        return;
    }

    _replyParseEvent.init( _reply->header() );

    if( _parser.begin() )
    {
        _readBytes = 0;
        _timer.start( _timeout );
    }

    beginRead();
}


MessageProgress Connection::endReceiveReply()
{
    log_trace("Connection::endReceiveReply");
    MessageProgress progress;

    if(_onTimeout)
        throw Pt::System::IOError("timeout");

    if(_state == RequestOutputPending)
    {
        log_debug("sent remaining request data");
        endWrite();
        _state = Connected;
        return progress;
    }

    endRead();

    if ( ! _replyParser.end() )
    {
        if( _socket.eof() )
        {
            throw System::IOError("unexpected EOF");
        }
        
        _replyParser.advance( *_httpbuf.buffer() );

        if( _replyParser.fail() )
        {
            log_warn("http parser failed");
            throw std::runtime_error("http parser failed"); // TODO define exception class
        }

        if( ! _replyParser.end() )
        {
            return progress;
        }

        log_debug("received header");
        progress.setOnHeader();
        _httpbuf.reset();
        _httpbuf.beginBody( _reply->header() );
    }

    if( _replyParser.end() )
    {
        std::streamsize avail = _httpbuf.in_avail();
        
        _httpbuf.import();
        log_debug("bytes available: " << _httpbuf.in_avail());
        
        _readBytes += _httpbuf.in_avail() - avail;
        if(_readBytes >= 8192)
            _timer.start(_timeout);

        if(_httpbuf.in_avail() > 0)
            progress.setOnBody();

        if( _httpbuf.isEnd() )
        {
            log_debug("reply body finished");
            progress.setFinished();
            
            bool keepalive = _reply->header().keepAlive();
            
            _reply = 0;
            _replyParser.reset(true);
            _timer.stop();
            _readBytes = 0;

            if( ! keepalive )
            {
                log_debug("closing, no keep alive");
                cancel();
            }
        }
        else if( _socket.eof() )
        {
            throw System::IOError("unexpected EOF");
        }
    }

    return progress;
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

    if(_request && _state == SslNotAccepted)
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

    if( _reply && _reply->isReceiving() )
    {
        _reply->onInput();
        return;
    }

    if( _request && _request->isReceiving() )
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
}
#endif


void Connection::onConnect(Net::TcpSocket& socket)
{
    log_trace("Connection::onConnect");
    if(_request)
        _request->onOutput();
}


void Connection::onTimeout()
{
    log_debug("cancelling connection");
    _onTimeout = true;

    if(_request)
    {
        if( _request->isSending() )
            _request->onOutput();
        else
            _request->onInput();
    }

    if(_reply)
    {
        if( _reply->isSending() )
            _reply->onOutput();
        else
            _reply->onInput();
    }
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
        if(_reply->isReceiving())
            _reply->onInput();
        else
            _reply->onOutput();
        
        return;
    }

    if(_request)
    {
        if( _request->isReceiving())
            _request->onInput();
        else
            _request->onOutput();

        return;
    }
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
            _timer.start(_timeout);
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
        _timer.start(_timeout);
        _sockbuf.beginWrite();
        return;
    }

    log_debug("no data to write");
    return;
}


void Connection::endWrite()
{
    _timer.stop();

#ifdef PT_HTTP_WITH_SSL
    if(_ssl)
        _sslbuf.endWrite();
    else
#endif
        _sockbuf.endWrite();
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


void Connection::writeRequestHeader(std::ostream& os, const Request& request)
{
    log_debug("writing request header " << request.header().url());

    os << request.header().method() << ' '
       << request.header().url() << " HTTP/"
       << request.header().httpVersionMajor() << '.'
       << request.header().httpVersionMinor() << "\r\n";

    for (RequestHeader::const_iterator it = request.header().begin();
        it != request.header().end(); ++it)
    {
        os << it->first << ": " << it->second << "\r\n";
    }

    if(_chunked)
        os << "Transfer-Encoding: chunked\r\n";
    else
        os << "Content-Length: " << request.size() << "\r\n";

    if( ! request.header().hasHeader("Connection") )
    {
        os << "Connection: keep-alive\r\n";
    }

    if (!request.header().hasHeader("Date"))
    {
        char buffer[50];
        os << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
    }

    if (!request.header().hasHeader("Host"))
    {
        os << "Host: " << _addrInfo.host();
        unsigned short port = _addrInfo.port();
        if (port != 80)
            os << ':' << port;
        os << "\r\n";
    }

    if (!request.header().hasHeader("User-Agent"))
    {
        os << "User-Agent: Pt-Http-client\r\n";
    }

    /*if (!_username.empty() && !request.header().hasHeader("Authorization"))
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


void Connection::writeReplyHeader(std::ostream& os, const Reply& reply)
{
    log_debug("writing reply header " << reply.header().httpReturnCode());

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

    if( ! header.hasHeader("Connection") )
    {
        os << "Connection: "
            << (_keepAlive ? "keep-alive" : "close")
            << "\r\n";
    }

    if(_chunked)
        os << "Transfer-Encoding: chunked\r\n";
    else
        os << "Content-Length: " << _reply->size() << "\r\n";

    if( ! header.hasHeader("Server") )
    {
        os << "Server: Platinum 1.0\r\n";
    }

    if( ! header.hasHeader("Date") )
    {
        char buffer[50];
        os << "Date: " << MessageHeader::htdateCurrent(buffer) << "\r\n";
    }

    os << "\r\n";
}

} // namespace Http

} // namespace Pt
