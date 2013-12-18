/*
 * Copyright (C) 2012 by Marc Boris Duerner
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

#ifndef Pt_Http_Connection_h
#define Pt_Http_Connection_h

#include "Parser.h"
#include "HttpBuffer.h"

#include <Pt/Http/Api.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Ssl/StreamBuffer.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/IOBuffer.h>
#include <Pt/System/Timer.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Signal.h>
#include <Pt/Connectable.h>

#include <iostream>

namespace Pt {

namespace Ssl {
    class Context;
}

namespace Http {

class Reply;
class Request;

class Socket : public Net::TcpSocket
{
    public:
        Socket()
        : _output(false)
        , _input(false)
        {}

        void setInputPipelined()
        { 
            _input = true;

            System::EventLoop* loop = this->loop();
            if( ! loop )
                throw std::logic_error("socket not active");
            
            loop->setReady(*this); 
        }

        void setOutputPipelined()
        { 
            _output = true;
            
            System::EventLoop* loop = this->loop();
            if( ! loop )
                throw std::logic_error("socket not active");
            
            loop->setReady(*this);  
        }

        Signal<>& outputPipelined()
        { return _outputPipelined; }

        Signal<>& inputPipelined()
        { return _inputPipelined; }

    protected:
        virtual void onCancel()
        {
            _output = false;
            _input = false;
            Net::TcpSocket::onCancel();
        }
        
        virtual bool onRun()
        { 
            if(_output)
            {
                _output = false;
                _outputPipelined.send();
                return true;
            }
            
            if(_input)
            {
                _input = false;
                _inputPipelined.send();
                return true;
            }

            return Net::TcpSocket::onRun(); 
        }

    private:
        bool _output;
        bool _input;
        Signal<> _outputPipelined;
        Signal<> _inputPipelined;
};

class Connection : public Connectable
{
    friend class Request;
    friend class Reply;

    class ParseEvent : public HeaderParser::MessageHeaderEvent
    {
            Request* _request;

        public:
            explicit ParseEvent()
            : HeaderParser::MessageHeaderEvent()
            , _request(0)
            { }

            void init(Request& request)
            { 
                _request = &request; 
                HeaderParser::MessageHeaderEvent::init(request.header());
            }

            virtual void onMethod(const std::string& method);
            virtual void onUrl(const std::string& url);
            virtual void onUrlParam(const std::string& q);
    };

    class ReplyParseEvent : public HeaderParser::MessageHeaderEvent
    {
            Reply* _reply;

        public:
            explicit ReplyParseEvent()
                : HeaderParser::MessageHeaderEvent(),
                  _reply(0)
                { }

            void init(Reply& reply)
            { 
                _reply = &reply; 
                HeaderParser::MessageHeaderEvent::init(reply.header());
            }

            void onHttpReturn(unsigned ret, const std::string& text);
    };

    public:
        static const std::size_t WaitInfinite = static_cast<const std::size_t>(-1);

        static const std::size_t NoRequestSizeLimit = static_cast<const std::size_t>(-1);

    public:
        Connection();

        virtual ~Connection();

        void accept(Net::TcpServer& tcpServer);

        void setHost(const Net::Endpoint& addrinfo);

        void setHost(const Net::Endpoint& addrinfo, const Net::TcpSocketOptions& opts);

        const Net::Endpoint& host() const
        { return _addrInfo; }

        void setSecure(Ssl::Context& ctx);

        void setActive(System::EventLoop& loop);

        System::EventLoop* loop() const
        { return _socket.loop(); }

        void setTimeout(std::size_t timeout)
        { 
            _socket.setTimeout(timeout);
            _timeout = timeout;
        }

        void setKeepAliveTimeout(std::size_t timeout)
        { _keepaliveTimeout = timeout; }

        void setMaxReadSize(std::size_t maxSize)
        { _maxReadSize = maxSize; }

        bool isConnected() const
        { return _socket.isConnected(); }

        void cancel();

    protected:
        void sendRequest(Request& r);

        void receiveReply(Reply& r);

        void beginSendRequest(Request& r);

        MessageProgress endSendRequest();

        void beginSendReply(Reply& r);

        MessageProgress endSendReply();

        void beginReceiveRequest(Request& r);

        MessageProgress endReceiveRequest();

        void beginReceiveReply(Reply& r);

        MessageProgress endReceiveReply();

        std::streambuf& buffer()
        { return _httpbuf; }

        void onConnect(Net::TcpSocket& socket);

        void onOutput();

        void onInput();

        void onTimeout();

        void onHttpInput(System::IOBuffer& sb);

        void onHttpOutput(System::IOBuffer& sb);

        void beginRead();

        void endRead();

        void beginWrite();

        void endWrite();

        bool inputAvailable();

        bool outputAvailable();
      
        void writeRequestHeader(std::ostream& os, Request& request);

        void writeReplyHeader(std::ostream& os, Reply& reply);

    private:
        ParseEvent _parseEvent;
        HeaderParser _parser;

        ReplyParseEvent _replyParseEvent;
        HeaderParser _replyParser;

        Request* _request;
        Reply* _reply;

        System::Timer _timer;
        Socket _socket;
        System::IOBuffer _sockbuf;
        std::iostream _sockios;
        Net::Endpoint _addrInfo;
        Net::TcpSocketOptions _tcpOptions;

        bool _ssl;
        Ssl::Context* _ctx;
        Ssl::StreamBuffer _sslbuf;

        HttpBuffer _httpbuf;
        std::ostream _os; // TODO: remove, only needed to write hex values

        std::size_t _timeout;
        std::size_t _keepaliveTimeout;
        std::size_t _maxReadSize;
        std::size_t _readSize;
        std::streamsize _readBytes;

        enum State
        {
            NotConnected = 0,
            Accepted = 1,
            Connected = 2,
            SslHandshake = 3,
            SslNotAccepted = 4,
            RequestOutputPending = 5,
            ReplyOutputPending = 6,

            SslHandshakeWrite,
            SslHandshakeRead,

            SslAcceptRead,
            SslAcceptWrite

        } _state;

        bool _chunked;
        bool _keepAlive;
        bool _onTimeout;
};

} // namespace Http

} // namespace Pt

#endif
