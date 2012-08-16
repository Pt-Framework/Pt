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
#include "ClientImpl.h"
#include <Pt/Http/Api.h>
#include <Pt/Http/Reply.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/IOBuffer.h>
#include <Pt/System/Timer.h>
#include <Pt/Signal.h>

#ifdef PT_HTTP_WITH_SSL
#include <Pt/Ssl/IOBuffer.h>
#endif

#include <iostream>

namespace Pt {

namespace Http {

class Server;
class Responder;
class RequestHeader;

class Connection : public Net::TcpSocket
                 , public Connectable
{
    class ParseEvent : public HeaderParser::MessageHeaderEvent
    {
            RequestHeader& _request;

        public:
            explicit ParseEvent(RequestHeader& request)
            : HeaderParser::MessageHeaderEvent(request)
            , _request(request)
            { }

            virtual void onMethod(const std::string& method);
            virtual void onUrl(const std::string& url);
            virtual void onUrlParam(const std::string& q);
    };

    public:
        Connection(Server& server, Net::TcpServer& tcpServer);

        ~Connection();

        void begin(System::EventLoop& loop, Ssl::Context* ctx = 0);

        void advanceReply();

        void finishReply();

        Signal<Connection&> timeout;

    protected:
#ifdef PT_HTTP_WITH_SSL
        void onHttpsHandshake(Pt::Ssl::IOBuffer& ssl);

        void onHttpsInput(Pt::Ssl::IOBuffer& ssl);

        void onHttpsOutput(Pt::Ssl::IOBuffer& ssl);
#endif
        void onHttpInput(System::StreamBuffer& sb);

        void onHttpOutput(System::StreamBuffer& sb);

        void beginRead();

        void endRead();

        bool beginWrite();

        void endWrite();

        void processInput();

        void processOutput();

        void replyError();

        void onTimeout();

        const RequestHeader& request() const 
        { return _request; }

        const Reply& reply() const     
        { return _reply; }

    private:
        Server& _server;
        ParseEvent _parseEvent;
        HeaderParser _parser;
        RequestHeader _request;
        System::EventLoop* _loop;
        System::Timer _timer;
        bool _chunkedTransfer;
        Responder* _responder;
        System::IOBuffer _sockbuf;
        bool _ssl;
#ifdef PT_HTTP_WITH_SSL
        Ssl::IOBuffer _sslbuf;
#endif
        HttpBuffer _httpbuf;
        std::iostream _stream;
        bool _chunked;
        Reply _reply;
};

} // namespace Http

} // namespace Pt

#endif
