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
#include <Pt/Http/RequestHeader.h>
#include <Pt/Http/ReplyHeader.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/IOBuffer.h>
#include <Pt/System/Timer.h>
#include <Pt/Signal.h>
#include <Pt/Connectable.h>

#ifdef PT_HTTP_WITH_SSL
#include <Pt/Ssl/IOBuffer.h>
#endif

#include <iostream>

namespace Pt {

namespace Http {

class Reply;
class Request;

class Connection : public Net::TcpSocket
                 , public Connectable
{
    friend class Request;
    friend class Reply;

    class ParseEvent : public HeaderParser::MessageHeaderEvent
    {
            RequestHeader* _request;

        public:
            explicit ParseEvent()
            : HeaderParser::MessageHeaderEvent()
            , _request(0)
            { }

            void init(RequestHeader& request)
            { 
                _request = &request; 
                HeaderParser::MessageHeaderEvent::init(request);
            }

            virtual void onMethod(const std::string& method);
            virtual void onUrl(const std::string& url);
            virtual void onUrlParam(const std::string& q);
    };

    class ReplyParseEvent : public HeaderParser::MessageHeaderEvent
    {
            ReplyHeader* _replyHeader;

        public:
            explicit ReplyParseEvent()
                : HeaderParser::MessageHeaderEvent(),
                  _replyHeader(0)
                { }

            void init(ReplyHeader& replyHeader)
            { 
                _replyHeader = &replyHeader; 
                HeaderParser::MessageHeaderEvent::init(replyHeader);
            }

            void onHttpReturn(unsigned ret, const std::string& text);
    };

    public:
        Connection();

        virtual ~Connection();

        void accept(Net::TcpServer& tcpServer);

        void setHost(const Net::AddrInfo& addrinfo);

        const Net::AddrInfo& host() const
        { return _addrInfo; }

        void setSecure();

        void setContext(Ssl::Context& ctx);

        void setEventLoop(System::EventLoop& loop);

        System::EventLoop* loop() const
        { return _loop; }

        bool isConnected()
        { return TcpSocket::isConnected(); }

        void cancel();

    protected:
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

#ifdef PT_HTTP_WITH_SSL
        void onHttpsHandshake(Pt::Ssl::IOBuffer& ssl);

        void onHttpsInput(Pt::Ssl::IOBuffer& ssl);

        void onHttpsOutput(Pt::Ssl::IOBuffer& ssl);
#endif
        void onConnect(Net::TcpSocket& socket);

        void onHttpInput(System::StreamBuffer& sb);

        void onHttpOutput(System::StreamBuffer& sb);

        void beginRead();

        void endRead();

        void beginWrite();

        void endWrite();

        bool inputAvailable();

        bool outputAvailable();

        void onTimeout();
      
        void writeRequestHeader(std::ostream& os, const Request& request);

        void writeReplyHeader(std::ostream& os, const Reply& reply);

    private:
        ParseEvent _parseEvent;
        HeaderParser _parser;

        ReplyParseEvent _replyParseEvent;
        HeaderParser _replyParser;

        Request* _request;
        Reply* _reply;
        System::EventLoop* _loop;
        System::Timer _timer;
        System::IOBuffer _sockbuf;
        Net::AddrInfo _addrInfo;

        bool _ssl;
#ifdef PT_HTTP_WITH_SSL
        Ssl::IOBuffer _sslbuf;
#endif
        HttpBuffer _httpbuf;
        std::size_t _readTimeout;
        std::size_t _writeTimeout;
        std::size_t _keepaliveTimeout;

        enum State
        {
            NotConnected = 0,
            Accepted = 1,
            Connected = 2,
            SslHandshake = 3,
            SslNotAccepted = 4,
            RequestOutputPending = 5,
            ReplyOutputPending = 6

        } _state;

        bool _chunked;
        bool _keepAlive;
};

} // namespace Http

} // namespace Pt

#endif
