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

#ifndef Pt_Http_Server_h
#define Pt_Http_Server_h

#include <Pt/Http/Api.h>
#include <Pt/Http/Parser.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/IOStream.h>
#include <Pt/System/Timer.h>
#include <Pt/Connectable.h>
#include <string>
#include <cstddef>
#include <map>

namespace Pt {

namespace System {

    class SelectorBase;

}

namespace Http {

class Responder;

class Service
{
    public:
        virtual ~Service() { }
        virtual Responder* createResponder(const Request&) = 0;
        virtual void releaseResponder(Responder*) = 0;
};

class PT_HTTP_API Responder
{
    public:
        explicit Responder(Service& service)
            : _service(service)
        { }

        virtual ~Responder() { }

        virtual void beginRequest(std::istream& in, Request& request);
        virtual std::size_t readBody(std::istream&);
        virtual void reply(std::ostream&, Request& request, Reply& reply) = 0;
        virtual void replyError(std::ostream&, Request& request, Reply& reply, const std::exception& ex);

        void release()     { _service.releaseResponder(this); }

    private:
        Service& _service;
};

class PT_HTTP_API NotFoundResponder : public Responder
{
    public:
        explicit NotFoundResponder(Service& service)
            : Responder(service)
            { }

        void reply(std::ostream&, Request& request, Reply& reply);
};

class PT_HTTP_API NotFoundService : public Service
{
    public:
        NotFoundService()
            : _responder(*this)
            { }

        Responder* createResponder(const Request&);
        void releaseResponder(Responder*);

    private:
        NotFoundResponder _responder;
};

class PT_HTTP_API Server : public Net::TcpServer, public Connectable
{
    public:
        Server(System::SelectorBase& selector, const std::string& ip, unsigned short int port);

        void addService(const std::string& url, Service& service);
        void removeService(Service& service);

        Responder* getResponder(const Request& request);
        Responder* getDefaultResponder(const Request& request)
            { return _defaultService.createResponder(request); }

        void onConnect(TcpServer& server);

        std::size_t readTimeout() const       { return _readTimeout; }
        std::size_t writeTimeout() const      { return _writeTimeout; }
        std::size_t keepAliveTimeout() const  { return _keepAliveTimeout; }

        void readTimeout(std::size_t ms)      { _readTimeout = ms; }
        void writeTimeout(std::size_t ms)     { _writeTimeout = ms; }
        void keepAliveTimeout(std::size_t ms) { _keepAliveTimeout = ms; }

    private:
        typedef std::multimap<std::string, Service*> ServicesType;
        ServicesType _service;
        System::SelectorBase& _selector;
        NotFoundService _defaultService;

        std::size_t _readTimeout;
        std::size_t _writeTimeout;
        std::size_t _keepAliveTimeout;
};


class PT_HTTP_API Socket : public Net::TcpSocket, public Connectable
{
        class ParseEvent : public HeaderParser::MessageHeaderEvent
        {
                Request& _request;

            public:
                explicit ParseEvent(Request& request)
                    : HeaderParser::MessageHeaderEvent(request.header()),
                      _request(request)
                    { }

                virtual void onMethod(const std::string& method);
                virtual void onUrl(const std::string& url);
                virtual void onUrlParam(const std::string& q);
        };

    public:
        Socket(System::SelectorBase& s, Server& server);

        void onInput(System::StreamBuffer& stream);
        void onOutput(System::StreamBuffer& stream);
        void onTimeout();

        void sendReply();

    private:
        Server& _server;

        ParseEvent _parseEvent;
        HeaderParser _parser;
        Request _request;
        Reply _reply;

        System::Timer _timer;
        int _contentLength;
        Responder* _responder;
        System::IOStream _stream;
};

} // namespace Http

} // namespace Pt

#endif
