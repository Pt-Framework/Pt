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
#include <Pt/Http/HttpError.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Logger.h>
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <Pt/Convert.h>

#include <iterator>
#include <cassert>

PT_LOG_DEFINE("Pt.Http.Connection")

namespace Pt {

namespace Http {

void Connection::ParseEvent::onMethod(const std::string& method)
{
    _request->setMethod(method);
}


void Connection::ParseEvent::onUrl(const std::string& url)
{
    _request->setUrl(url);
}


void Connection::ParseEvent::onUrlParam(const std::string& q)
{
    _request->setQParams(q);
}


void Connection::ReplyParseEvent::onHttpReturn(unsigned ret, const std::string& text)
{
    _reply->setStatus(ret, text);
}


Connection::Connection()
: _parseEvent()
, _parser(_parseEvent, false)
, _replyParser(_replyParseEvent, true)
, _request(0)
, _reply(0)
, _sockbuf(8192, true)
, _sockios(&_sockbuf)
, _ssl(false)
, _ctx(0)
, _sslbuf()
, _httpbuf()
, _os(&_sockbuf)
, _inputPipelined(false)
, _outputPipelined(false)
, _timeout(WaitInfinite)
, _keepaliveTimeout(WaitInfinite)
, _maxReadSize( NoRequestSizeLimit )
, _readSize(0)
, _readBytes(0)
, _state(NotConnected)
, _chunked(false)
, _keepAlive(false)
, _onTimeout(false)
, _isFailed(false)
{
    _socket.connected() += slot(*this, &Connection::onConnect);
    //_socket.outputPipelined() += slot(*this, &Connection::onOutput);
    //_socket.inputPipelined() += slot(*this, &Connection::onInput);

    _sockbuf.attach(_socket);
    _sockbuf.outputReady() += slot(*this, &Connection::onHttpOutput);
    _sockbuf.inputReady() += slot(*this, &Connection::onHttpInput);

    _httpbuf.attach(_sockbuf);

    _timer.timeout() += slot(*this, &Connection::onTimeout);
}


Connection::~Connection()
{
  cancel();
}


void Connection::accept(Net::TcpServer& tcpServer)
{
    PT_LOG_TRACE("Connection::accept");    
    cancel();

    _socket.accept(tcpServer);

    if(_ssl)
        _state = SslNotAccepted;
    else
        _state = Accepted;
}


void Connection::setHost(const Net::Endpoint& addrinfo)
{
    if(_state != NotConnected)
    {
        cancel();
    }

    _addrInfo = addrinfo;
}


void Connection::setHost(const Net::Endpoint& addrinfo, const Net::TcpSocketOptions& opts)
{
    if(_state != NotConnected)
    {
        cancel();
    }

    _addrInfo = addrinfo;
    _tcpOptions = opts;
}


void Connection::setSecure(Ssl::Context& ctx)
{
    PT_LOG_DEBUG("initialize HTTPS connection");

    _ctx = &ctx;

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

        _ssl = true;
    }

    _httpbuf.attach(_sslbuf);
    _os.rdbuf(&_sslbuf);
}


void Connection::setPeerName(const std::string& peer)
{
    _sslbuf.setPeerName(peer);
}


//void Connection::setActive(System::EventLoop& loop)
//{
//    _socket.setActive(loop);
//    _timer.setActive(loop);
//}


void Connection::onCancel()
{
    PT_LOG_DEBUG("cancelling connection");
    _timer.stop();
    _readSize = 0;
    _readBytes = 0;
    _socket.close();
    _sockbuf.discard();
    _sockios.clear();
    _reply = 0;
    _request = 0;
    _state = NotConnected;
    _outputPipelined = false;
    _inputPipelined = false;
    _chunked = false;
    _keepAlive = false;
    _onTimeout = false;
    _isFailed = false;
    _parser.reset(false);
    _replyParser.reset(true);
    _httpbuf.reset();
    _os.clear();
}

void Connection::reset()
{
    PT_LOG_DEBUG("reset connection");
    _readSize = 0;
    _readBytes = 0;
    _socket.cancel();
    _sockbuf.discard();
    _sockios.clear();
    _reply = 0;
    _request = 0;
    _outputPipelined = false;
    _inputPipelined = false;
    _chunked = false;
    _onTimeout = false;
    _isFailed = false;
    _parser.reset(false);
    _replyParser.reset(true);
    _httpbuf.reset();
    _os.clear();
}


void Connection::sendRequest(Request& request)
{
    PT_LOG_DEBUG("Connection::sendRequest");

    if( ! isConnected() )
    {
        PT_LOG_DEBUG("opening new connection to " << _addrInfo.toString());
        _socket.connect(_addrInfo, _tcpOptions);

        if(_ssl)
        {
            PT_LOG_DEBUG("SSL connect");
            _sslbuf.open(*_ctx, _sockios, Ssl::Connect);

            for( ; ; )
            {
                PT_LOG_DEBUG("writing handshake");
                while( _sslbuf.writeHandshake() )
                    ;

                PT_LOG_DEBUG("syncing buffer");
                _sockios.flush();

                if( _sslbuf.isConnected() )
                    break;

                PT_LOG_DEBUG("reading handshake");
                for( ; ; )
                {
                    if( _sockbuf.sgetc() == std::char_traits<char>::eof() )
                        throw System::IOError("connection lost");

                    if( ! _sslbuf.readHandshake() )
                        break;
                }

                if( _sslbuf.isConnected() )
                    break;

                PT_LOG_DEBUG("continuing handshake");
            }
        }
    }
    
    std::ostream& os = _os; 
    MessageBuffer& mbuf = request.buffer();

    if( request.isFinished() )
    {
        PT_LOG_DEBUG("HTTP request finished");
        
        if(_chunked)
        {
            PT_LOG_DEBUG("sending last HTTP chunk: "  << mbuf.size() << " bytes");
            if(mbuf.size() > 0)
            {
                os << std::hex << mbuf.size() << std::dec << "\r\n";
                os.write( mbuf.data(), mbuf.size() );
                os.write("\r\n", 2);
            }
            
            os.write("0\r\n\r\n", 5);
            _chunked = false;
        }
        else
        {
            writeRequestHeader(os, request);
            PT_LOG_DEBUG("writing body: " << mbuf.size() << " bytes");
            os.write( mbuf.data(), mbuf.size() );
        }
    }
    else
    {
        if( ! _chunked )
        {
            PT_LOG_DEBUG("sending chunked header");
            _chunked = true;
            writeRequestHeader(os, request);
        }

        PT_LOG_DEBUG("sending HTTP chunk: "  << mbuf.size() << " bytes");

        if(mbuf.size() > 0)
        {
            os << std::hex << mbuf.size() << std::dec << "\r\n";
            os.write( mbuf.data(), mbuf.size() );
            os.write("\r\n", 2);
        }
    }

    if(_ssl)
    {
        PT_LOG_DEBUG("flushing ssl buffer");
        _sslbuf.pubsync();
    }

    PT_LOG_DEBUG("flushing socket buffer: " << _sockbuf.out_avail());
    _sockbuf.pubsync();
}


void Connection::receiveReply(Reply& reply)
{
    PT_LOG_DEBUG("Connection::receiveReply");
    
    char ch = ' ';
    std::istream is( _httpbuf.buffer() );

    _replyParseEvent.init( reply );
        
    while( ! _replyParser.end() && is.get(ch) )
    {
        _replyParser.parse(ch);
    }

    if( ! _replyParser.end() || _replyParser.fail() )
    {
        PT_LOG_INFO("invalid HTTP reply");
        throw HttpError("invalid HTTP message");
    }

    _httpbuf.reset();
    _httpbuf.beginBody( reply.header() );
    PT_LOG_DEBUG("reply size: " << reply.header().contentLength() << ", chunked: " << reply.header().isChunked());

    // read whole body into MessageBuffer...
    reply.body() << &_httpbuf;
    PT_LOG_DEBUG("reply body finished");

    if( ! _httpbuf.isEnd() )
    {
        PT_LOG_INFO("invalid HTTP reply");
        throw HttpError("invalid HTTP message");
    }

    _replyParser.reset(true);
            
    bool keepalive = reply.header().isKeepAlive();
    if( ! keepalive )
    {
        PT_LOG_DEBUG("closing, no keep alive");
        cancel();

        // TODO: do SSL shutdown here
    }
}


void Connection::beginSendRequest(Request& request)
{
    PT_LOG_TRACE("Connection::beginSendRequest");

    _request = &request;

    if( ! isConnected() )
    {
        PT_LOG_DEBUG("opening new connection to " << _addrInfo.toString());
        _timer.start( _timeout );
        _socket.beginConnect(_addrInfo, _tcpOptions);
        return;
    }

    if(_state == SslHandshake)
    {
        PT_LOG_DEBUG("begining SSL handshake");
        _timer.start( _timeout );
        _sslbuf.open(*_ctx, _sockios, Ssl::Connect);
        _state = SslHandshakeWrite;
    }

    if(_state == SslHandshakeWrite)
    {
        if(_sslbuf.writeHandshake() || _sockbuf.out_avail() > 0)
        {
            PT_LOG_DEBUG("writing SSL handshake");
            _sockbuf.beginWrite();
              _state = SslHandshakeWrite;
            return;
        }

        if( _sslbuf.readHandshake() && _sockbuf.in_avail() <= 0)
        {
            PT_LOG_DEBUG("reading SSL handshake");
            _sockbuf.beginRead();
            _state = SslHandshakeRead;
            return;
        }
        
        if( ! _sslbuf.isConnected() )
            throw HttpError("HTTP I/O error");

        PT_LOG_DEBUG("Handshake finished");
        _timer.stop();
        _state = Connected;
    }
    
    if( _state == SslHandshakeRead)
    {
        if( _sslbuf.readHandshake() && _sockbuf.in_avail() <= 0)
        {
            PT_LOG_DEBUG("reading SSL handshake");
            _sockbuf.beginRead();
            _state = SslHandshakeRead;
            return;
        }

        if( _sslbuf.writeHandshake() || _sockbuf.out_avail() > 0 )
        {
            PT_LOG_DEBUG("writing SSL handshake");
            _sockbuf.beginWrite();
              _state = SslHandshakeWrite;
            return;
        }
        
        if( ! _sslbuf.isConnected() )
            throw HttpError("HTTP I/O error");
            
        PT_LOG_DEBUG("Handshake finished");
        _timer.stop();
        _state = Connected;
    }

    std::ostream& os = _os; //( _httpbuf.buffer() );
    MessageBuffer& mbuf = _request->buffer();

    if( request.isFinished() )
    {
        PT_LOG_DEBUG("HTTP request finished");
        
        if(_chunked)
        {
            PT_LOG_DEBUG("sending last HTTP chunk: "  << mbuf.size() << " bytes");
            if(mbuf.size() > 0)
            {
                os << std::hex << mbuf.size() << std::dec << "\r\n";
                os.write( mbuf.data(), mbuf.size() );
                os.write("\r\n", 2);
            }
            
            os.write("0\r\n\r\n", 5);
            _chunked = false;
        }
        else
        {
            writeRequestHeader(os, request);
            
            PT_LOG_DEBUG("writing body: " << mbuf.size() << " bytes");
            if(mbuf.size() > 0)
                os.write( mbuf.data(), mbuf.size() );
        }

        PT_LOG_DEBUG("pipelining HTTP request");

        // signal that output was sent, so the request data can be pipelined
        // until we begin receiving the next reply from the server.
        //
        // TODO: beginWrite() if over 8K data to send
        setOutputReady();
        return;
    }

    if( ! _chunked )
    {
        PT_LOG_DEBUG("sending chunked header");
        _chunked = true;
        writeRequestHeader(os, request);
    }

    PT_LOG_DEBUG("sending HTTP chunk: "  << mbuf.size() << " bytes");

    if(mbuf.size() > 0)
    {
        os << std::hex << mbuf.size() << std::dec << "\r\n";
        os.write( mbuf.data(), mbuf.size() );
        os.write("\r\n", 2);
    }

    beginWrite();
}


MessageProgress Connection::endSendRequest()
{
    PT_LOG_TRACE("Connection::endSendRequest");
    MessageProgress progress;

    if(_onTimeout)
        throw System::IOError("timeout");

    if(_isFailed)
        throw System::IOError("I/O Error");

    if(_state == NotConnected)
    {
        _timer.stop();
        _socket.endConnect();
        PT_LOG_DEBUG("connected to " << _addrInfo.toString());

        if(_ssl)
            _state = SslHandshake;
        else
            _state = Connected;

        return progress;
    }

    if(_state == SslHandshakeWrite)
    {
        PT_LOG_DEBUG("wrote SSL handshake");
        _sockbuf.endWrite();
        return progress;
    }

    if(_state == SslHandshakeRead)
    {
        PT_LOG_DEBUG("read SSL handshake");
        _sockbuf.endRead();

        if( _socket.isEof() )
            throw System::IOError("connection lost");

        return progress;
    }

    Request* req = _request;
    _request = 0;

    if( ! req->isFinished() )
    {
        endWrite();

        if( _socket.isEof() )
        {
            throw System::IOError("connection lost");
        }
    }
 
    // indicates that the request or chunk was completely written
    PT_LOG_DEBUG("request data sent");
    progress.setFinished();
    return progress;
}


// NOTE: maybe add a flag to cause a flush in the future
void Connection::beginSendReply(Reply& reply)
{
    PT_LOG_TRACE("Connection::beginSendReply");

    _reply = &reply;

    MessageHeader& header = _reply->header();
    std::ostream& os =  _os; //( _httpbuf.buffer() );
    MessageBuffer& mbuf = _reply->buffer();

    _keepAlive = _keepAlive && header.isKeepAlive();

    if( ! _keepAlive && outputAvailable() )
    {
        beginWrite();
        return;
    }

    if( reply.isFinished() )
    {
        if(_chunked)
        {
            if(mbuf.size() > 0)
            {
                os << std::hex << mbuf.size() << std::dec << "\r\n";
                os.write( mbuf.data(), mbuf.size() );
                os.write("\r\n", 2);
            }

            os.write("0\r\n\r\n", 5);
            _chunked = false;
        }
        else
        {          
            writeReplyHeader(os, reply);

            PT_LOG_DEBUG("writing body: " << mbuf.size() << " bytes");
            if(mbuf.size() > 0)
                os.write( mbuf.data(), mbuf.size() );
        }

        PT_LOG_DEBUG("begin writing reply");

        if( _keepAlive )
        {
            // signal that output was sent, so the reply data can be pipelined
            // until we begin receiving the next request from the client
            setOutputReady(); 
        }
        else
            beginWrite();

        return;
    }

    if( ! _chunked )
    {
        PT_LOG_DEBUG("sending chunked header");
        _chunked = true;
        writeReplyHeader(os, *_reply);
    }

    if(mbuf.size() > 0)
    {
        os << std::hex << mbuf.size() << std::dec << "\r\n";
        os.write( mbuf.data(), mbuf.size() );
        os.write("\r\n", 2);
    }

    // TODO: only if over 8K data to send
    // this way the timeout can be for the 8K chunk
    beginWrite();
}


MessageProgress Connection::endSendReply()
{
    PT_LOG_TRACE("Connection::endSendReply");

    MessageProgress progress;
    
    if(_onTimeout)
        throw System::IOError("timeout");

    if(_isFailed)
        throw System::IOError("I/O Error");
        
    if( ! _reply->isFinished() || ! _keepAlive)
    {
        endWrite();
    }

    if( _socket.isEof() )
    {
        throw System::IOError("connection lost");
    }

    // keepalive -> leave data in the output buffer
    // close -> make sure we sent all data
    if( ! _keepAlive && outputAvailable() )
    {
        PT_LOG_DEBUG("still data to send");
        return progress;
    }
  
    progress.setFinished();

    if( ! _reply->isFinished() )
    {
        PT_LOG_DEBUG("reply is not finished");

        // indicates that chunk was completely written
        return progress;
    }

    bool isUpgrade = _reply->header().isUpgrade();

    _reply = 0;

    if( ! _keepAlive && ! isUpgrade )
    {
        PT_LOG_DEBUG("no keep alive, closing connection");
        cancel();

        //TODO: start SSL shutdown here
    }

    // indicates that request was completely written
    return progress;
}


void Connection::beginReceiveRequest(Request& request)
{
    PT_LOG_TRACE("Connection::beginReceiveRequest " << _state);

    _request = &request;

    if(_state == SslNotAccepted)
    {
        PT_LOG_DEBUG("beginning SSL handshake");
        _timer.start( _timeout );
        _sslbuf.open(*_ctx, _sockios, Ssl::Accept);
        _state = SslAcceptRead;
    }

    if(_state == SslAcceptWrite)
    {
        if(_sslbuf.writeHandshake() || _sockbuf.out_avail() > 0)
        {
            PT_LOG_DEBUG("writing SSL handshake");
            _sockbuf.beginWrite();
              _state = SslAcceptWrite;
            return;
        }

        if( _sslbuf.readHandshake() && _sockbuf.in_avail() <= 0)
        {
            PT_LOG_DEBUG("reading SSL handshake");
            _sockbuf.beginRead();
            _state = SslAcceptRead;
            return;
        }

        if( ! _sslbuf.isConnected() )
            throw HttpError("HTTP I/O error");

        PT_LOG_DEBUG("Handshake finished");
        _timer.stop();
        _state = Accepted;
    }
    
    if( _state == SslAcceptRead)
    {
        if( _sslbuf.readHandshake() && _sockbuf.in_avail() <= 0)
        {
            PT_LOG_DEBUG("reading SSL handshake");
            _sockbuf.beginRead();
            _state = SslAcceptRead;
            return;
        }

        if(_sslbuf.writeHandshake() || _sockbuf.out_avail() > 0)
        {
            PT_LOG_DEBUG("writing SSL handshake");
            _sockbuf.beginWrite();
              _state = SslAcceptWrite;
            return;
        }
        
        if( ! _sslbuf.isConnected() )
            throw HttpError("HTTP I/O error");
        
        PT_LOG_DEBUG("Handshake finished");
        _timer.stop();
        _state = Accepted;
    }

    // send remaining pipelined replies, if no further requests
    // are in the pipeline.
    if( outputAvailable() && ! inputAvailable() )
    {
        PT_LOG_DEBUG("sending remaining reply data");
        beginWrite();
        _state = ReplyOutputPending;
        return;
    }

    PT_LOG_DEBUG("begin reading request");
    _parseEvent.init( request );

    // NOTE: the http header parser is also not at begin if data from the
    // last request has not been read. 

    if( _parser.begin() )
    {
        _readSize = 0;
        _readBytes = 0;

        if(_keepAlive)
        {
            PT_LOG_DEBUG("use keep alive timeout: " << _keepaliveTimeout);
            _timer.start(_keepaliveTimeout);
        }
        else
        {
            PT_LOG_DEBUG("use I/O timeout: " << _timeout);
            _timer.start( _timeout );
        }
    }
    
    beginRead();
}


MessageProgress Connection::endReceiveRequest()
{
    PT_LOG_TRACE("Connection::endReceiveRequest");
    MessageProgress progress;

    if(_onTimeout)
        throw System::IOError("timeout");

    if(_isFailed)
        throw System::IOError("I/O Error");
   
    if(_state == SslAcceptWrite)
    {
        PT_LOG_DEBUG("wrote SSL handshake");
        _sockbuf.endWrite();
        return progress;
    }

    if(_state == SslAcceptRead)
    {
        PT_LOG_DEBUG("read SSL handshake");
        _sockbuf.endRead();

        if( _socket.isEof() )
            throw System::IOError("connection lost");
        
        return progress;
    }

    if(_state == ReplyOutputPending)
    {
        PT_LOG_DEBUG("sent remaining reply data");
        endWrite();

        PT_LOG_DEBUG("remaining: " << _sockbuf.out_avail());
        _state = Accepted;
        return progress;
    }

    endRead();

    // TODO: throw if SSL shutdown was received

    if( _socket.isEof() )
    {
        throw System::IOError("connection lost");
    }

    if( ! _parser.end() )
    {       
        // switch from keepalive timeout to receive timeout
        if( _parser.begin() && _keepAlive )
            _timer.start(_timeout);

        _parser.advance( *_httpbuf.buffer() );

        if( _parser.fail() )
        {
            PT_LOG_WARN("http parser failed");

            // TODO define exception class
            // TODO: handle any previously pipelined reply
            throw HttpError("invalid HTTP message"); 
        }

        if( ! _parser.end() )
        {
            return progress;
        }

        _keepAlive = _request->header().isKeepAlive();
        progress.setHeader();
        _httpbuf.reset();
        _httpbuf.beginBody( _request->header() );
    }

    _request = 0;

    if( _parser.end() )
    {
        std::streamsize avail = _httpbuf.in_avail();

        if(avail < 0)
            throw System::IOError("connection lost");
        
        _httpbuf.import();
        PT_LOG_DEBUG("bytes available: " << _httpbuf.in_avail());
               
        if(_httpbuf.in_avail() > 0)
            progress.setBody();

        if( _httpbuf.isEnd() )
        {
            PT_LOG_DEBUG("request body finished");
            progress.setFinished();

            _timer.stop();
            _readSize = 0;
            _readBytes = 0;
            _parser.reset(false);
        }
    }

    return progress;
}


void Connection::beginReceiveReply(Reply& r)
{
    PT_LOG_TRACE("Connection::beginReceiveReply");

    _reply = &r;

    if( outputAvailable() )
    {
        PT_LOG_DEBUG("sending remaining request data");
        beginWrite();
        _state = RequestOutputPending;
        return;
    }

    _replyParseEvent.init( *_reply );

    if( _replyParser.begin() )
    {
        _readSize = 0;
        _readBytes = 0;
        _timer.start( _timeout );
    }

    beginRead();
}


MessageProgress Connection::endReceiveReply()
{
    PT_LOG_TRACE("Connection::endReceiveReply");
    MessageProgress progress;

    if(_onTimeout)
    {
        // normal I/O timeout or keep-alive timeout
        throw System::IOError("timeout");
    }

    if(_isFailed)
        throw System::IOError("I/O Error");

    if(_state == RequestOutputPending)
    {
        PT_LOG_DEBUG("sent remaining request data");
        endWrite();
        _state = Connected;
        return progress;
    }

    endRead();

    PT_LOG_DEBUG("input available: " << inputAvailable());

    // TODO: throw if SSL shutdown was received

    if( _socket.isEof() )
    {
        throw System::IOError("connection lost");
    }

    if ( ! _replyParser.end() )
    {
        _replyParser.advance( *_httpbuf.buffer() );

        if( _replyParser.fail() )
        {
            PT_LOG_WARN("http parser failed");
            throw HttpError("invalid HTTP message"); // TODO define exception class
        }

        if( ! _replyParser.end() )
        {
            PT_LOG_DEBUG("received part of header");
            return progress;
        }

        PT_LOG_DEBUG("received header");
        progress.setHeader();
        _httpbuf.reset();
        _httpbuf.beginBody( _reply->header() );
    }

    if( _replyParser.end() )
    {
        std::streamsize avail = _httpbuf.in_avail();

        if(avail < 0)
            throw System::IOError("connection lost");
        
        _httpbuf.import();
        PT_LOG_DEBUG("bytes available: " << _httpbuf.in_avail());

        if(_httpbuf.in_avail() > 0)
            progress.setBody();

        if( _httpbuf.isEnd() )
        {
            PT_LOG_DEBUG("reply body finished");
            progress.setFinished();
            
            bool keepalive = _reply->header().isKeepAlive();
            
            _reply = 0;
            _replyParser.reset(true);
            _timer.stop();
            _readSize = 0;
            _readBytes = 0;

            if( ! keepalive )
            {
                PT_LOG_DEBUG("closing, no keep alive");
                cancel();

                //TODO: start SSL shutdown here
            }
        }
    }

    PT_LOG_DEBUG("reply progress: " << progress.mask());
    return progress;
}


void Connection::onConnect(Net::TcpSocket& socket)
{
    PT_LOG_TRACE("Connection::onConnect");
    if(_request)
        _request->onOutput();
}


void Connection::onOutput()
{
    PT_LOG_TRACE("Connection::onOutput");

    if(_request)
    {
        if( _request->isReceiving() )
            _request->onInput();
        else
            _request->onOutput();
        
        return;
    }

    if(_reply)
    {
        if( _reply->isReceiving() )
            _reply->onInput();
        else
            _reply->onOutput();
    }
}


void Connection::onInput()
{
    PT_LOG_TRACE("Connection::onInput");

    if(_request)
    {
        if( _request->isReceiving() )
            _request->onInput();
        else
            _request->onOutput();
        
        return;
    }

    if(_reply)
    {
        if( _reply->isReceiving() )
            _reply->onInput();
        else
            _reply->onOutput();
    }
}


void Connection::onTimeout()
{
    PT_LOG_TRACE("Connection::onTimeout");
    _onTimeout = true;
    _timer.stop();

    onInput();
}


void Connection::onHttpInput(System::IOBuffer&)
{
    PT_LOG_TRACE("Connection::onHttpInput");

    onInput();
}


void Connection::onHttpOutput(System::IOBuffer&)
{
    PT_LOG_TRACE("Connection::onHttpOutput");

    onOutput();
}


void Connection::beginRead()
{
    if(_ssl)
    {
        _sslbuf.import();

        if(_sslbuf.in_avail() > 0)
        {
            setInputReady();
            return;
        }
    }

    if( _sockbuf.in_avail() )
        setInputReady();
    else
        _sockbuf.beginRead();
}


void Connection::endRead()
{
    // TODO: do not call endRead if data was available

    std::size_t readSize = _sockbuf.endRead();

    if(_ssl)
    {
        _sslbuf.import();
    }

    _readBytes += readSize;

    if(_maxReadSize != NoRequestSizeLimit)
    {
        _readSize += readSize; // TODO: handle overflow?

        if(_readSize > _maxReadSize)
        {
            PT_LOG_WARN("request too large");
            throw HttpError("request too large");
        }
    }

    if(_readBytes >= 8192)
    {
        _readBytes = 0;
        _timer.start(_timeout);
    }
}


bool Connection::inputAvailable()
{
    if(_ssl)
    {
        //_sslbuf.import();
        if( _sslbuf.in_avail() > 0 ) 
            return true;
    }

    return _sockbuf.in_avail() > 0;
}


void Connection::beginWrite()
{
    PT_LOG_DEBUG("Connection::beginWrite");

    try
    {
        if(_ssl)
        {
            PT_LOG_DEBUG("flushing ssl buffer");
            _sslbuf.pubsync();
        }

        PT_LOG_DEBUG("begin writing socket buffer: " << _sockbuf.out_avail());
        
        _sockbuf.beginWrite();
        _timer.start(_timeout);
    } 
    catch(System::IOError&)
    {
        PT_LOG_DEBUG("deferred I/O error");
        _isFailed = true;
        setOutputReady();
    }
}


void Connection::endWrite()
{
    _timer.stop();
    _sockbuf.endWrite();
}


bool Connection::outputAvailable()
{
    if(_ssl)
    {
        _sslbuf.pubsync();
    }

    return _sockbuf.out_avail() > 0;
}


void Connection::setInputReady()
{ 
    _inputPipelined = true;

    System::EventLoop* loop = this->loop();
    if( ! loop )
        throw std::logic_error("socket not active");
            
    loop->setReady(*this); 
}


void Connection::setOutputReady()
{ 
    _outputPipelined = true;
            
    System::EventLoop* loop = this->loop();
    if( ! loop )
        throw std::logic_error("socket not active");
            
    loop->setReady(*this);  
}

   
bool Connection::onRun()
{ 
    if(_outputPipelined)
    {
        _outputPipelined = false;
        this->onOutput();
        return true;
    }
            
    if(_inputPipelined)
    {
        _inputPipelined = false;
        this->onInput();
        return true;
    }

    return false; 
}


void Connection::onAttach(System::EventLoop& loop)
{
    _socket.setActive(loop);
    _timer.setActive(loop);
}


void Connection::onDetach(System::EventLoop& loop)
{ 
}


void Connection::writeRequestHeader(std::ostream& os, Request& request)
{
    PT_LOG_DEBUG("writing request header " << request.url());

    const MessageHeader& header = request.header();

    std::ostream_iterator<char> oit (os);
    os << request.method() << ' ';
    os << request.url() << '?' << request.qparams();
    os.write(" HTTP/", 6);
    formatInt(oit, header.versionMajor());
    os << '.';
    formatInt(oit, header.versionMinor());
    os.write("\r\n", 2);

    MessageHeader::ConstIterator it;
    for (it = header.begin(); it != header.end(); ++it)
    {
        os << it->name() << ": " << it->value() << "\r\n";
    }

    if(_chunked)
    {
        os.write("Transfer-Encoding: chunked\r\n", 28);
    }
    else
    {
        os.write("Content-Length: ", 16);
        formatInt( oit, request.buffer().size() );
        os.write("\r\n", 2);
    }

    if( ! header.has("Connection") )
    {
        os.write("Connection: close\r\n", 19);
    }

    if( ! header.has("Date"))
    {
        char buffer[50];
        os.write("Date: ", 6);
        os << MessageHeader::htdateCurrent(buffer);
        os.write("\r\n", 2);
    }

    if( ! header.has("Host"))
    {
        // TODO: use operator << (ostream, Endpoint)
        os << "Host: " << _addrInfo.toString();
        os.write("\r\n", 2);
    }

    if( ! header.has("User-Agent") )
    {
        os.write("User-Agent: Pt-Http-client\r\n", 28);
    }

    os.write("\r\n", 2);
}


void Connection::writeReplyHeader(std::ostream& os, Reply& reply)
{
    PT_LOG_DEBUG("writing reply header " << reply.statusCode());

    const MessageHeader& header = reply.header();

    std::ostream_iterator<char> oit (os);
    os.write("HTTP/", 5);
    formatInt(oit, header.versionMajor());
    os << '.';
    formatInt(oit, header.versionMinor()); 
    os << ' ';
    formatInt(oit, reply.statusCode());
    os << ' ';
    os << reply.statusText();
    os.write("\r\n", 2);

    MessageHeader::ConstIterator it;
    for(it = header.begin(); it != header.end(); ++it)
    {
        os << it->name() << ": " << it->value() << "\r\n";
    }

    if( ! header.has("Connection") )
    {
        os.write("Connection: ", 12);
        os << (_keepAlive ? "keep-alive" : "close");
        os.write("\r\n", 2);
    }

    if(_chunked)
        os.write("Transfer-Encoding: chunked\r\n", 28);
    else
    {
        os.write("Content-Length: ", 16);
        formatInt( oit, _reply->buffer().size() ); 
        os.write("\r\n", 2);
    }

    if( ! header.has("Server") )
    {
        os.write("Server: Platinum 1.0\r\n", 22);
    }

    if( ! header.has("Date") )
    {
        char buffer[50];
        os.write("Date: ", 6); 
        os << MessageHeader::htdateCurrent(buffer);
        os.write("\r\n", 2);
    }

    os.write("\r\n", 2);
}

} // namespace Http

} // namespace Pt
