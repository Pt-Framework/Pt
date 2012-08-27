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
#include <Pt/Connectable.h>

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

    public:
        Connection(Server& server, Net::TcpServer& tcpServer);

        virtual ~Connection();

        void beginAccept(System::EventLoop& loop, Request& req, Ssl::Context* ctx = 0);

        void beginReceiveRequest(Request& request);

        bool endReceiveRequest();

        bool isEnd()
        { return _responder && _httpbuf.isEnd(); }

        void beginSendReply(Reply& reply, bool finish = true);

        void endSendReply();

        bool outputAvailable();

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


    public:
        bool beginWrite(); // TODO !!!

    private:
        void endWrite();

        void replyError();

        void onTimeout();

        const RequestHeader& request() const 
        { return _request->header(); }

    public:
        Server& _server;
        Responder* _responder;

    private:
        ParseEvent _parseEvent;
        HeaderParser _parser;
        Request* _request;
        Reply* _reply;
        System::EventLoop* _loop;
        System::Timer _timer;
        bool _chunkedTransfer;
        System::IOBuffer _sockbuf;
        bool _ssl;
#ifdef PT_HTTP_WITH_SSL
        Ssl::IOBuffer _sslbuf;
#endif
        HttpBuffer _httpbuf;

        std::iostream _stream;
        bool _chunked;  
};


class RequestHandler : public Pt::Connectable
{
    public:
        RequestHandler(Server& server, Net::TcpServer& tcpServer);

        ~RequestHandler();

        void begin(System::EventLoop& loop, Ssl::Context* ctx = 0)
        { 
            
            _reply.init(_conn);
            _reply.clear();
            
            _conn.beginAccept(loop, _req, ctx); 
        
        }

        Signal<RequestHandler&>& timeout()
        { return _timeout; }

    protected:
        void onTimeout(Connection&)
        { _timeout.send(*this); }

        void onRequestReceived(Request& req);

        void onReplySent(Reply& r);

    private:
        Connection _conn;
        Request _req;
        Reply _reply;
        Signal<RequestHandler&> _timeout;
};

} // namespace Http

} // namespace Pt

#endif
