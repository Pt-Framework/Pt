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

#ifndef Pt_Net_HttpServer_h
#define Pt_Net_HttpServer_h

#include <Pt/Net/Api.h>
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

namespace Net {

class HttpResponder;

class HttpService
{
    public:
        virtual ~HttpService() { }
        virtual HttpResponder* createResponder() = 0;
        virtual void releaseResponder(HttpResponder*) = 0;
};

class HttpResponder
{
    public:
        explicit HttpResponder(HttpService& service)
            : _service(service)
        { }
        virtual ~HttpResponder() { }

        virtual std::size_t advance(std::istream&) = 0;
        virtual void finish(std::ostream&) = 0;
        void release()     { _service.releaseResponder(this); }

    private:
        HttpService& _service;
};

class PT_NET_API HttpNotFoundResponder : public HttpResponder
{
    public:
        explicit HttpNotFoundResponder(HttpService& service)
            : HttpResponder(service)
            { }

        std::size_t advance(std::istream&);
        void finish(std::ostream&);
};

class PT_NET_API HttpNotFoundService : public HttpService
{
    public:
        HttpNotFoundService()
            : _responder(*this)
            { }

        HttpResponder* createResponder();
        void releaseResponder(HttpResponder*);

    private:
        HttpNotFoundResponder _responder;
};

/*
class PT_NET_API HttpXmlRpcResponder : public HttpResponder
{
    public:
        HttpXmlRpcResponder(XmlRpc::Service& xmlRpcService, std::iostream& _stream)
            : HttpResponder(_stream),
              _xmlRpcHandler(xmlRpcService, _stream)
        { }

        virtual std::size_t advance(std::istream&);
        virtual void finish(std::ostream&);

    private:
        XmlRpc::RequestHandler _xmlRpcHandler;
};
*/

class PT_NET_API HttpServer : public TcpServer, public Connectable
{
    public:
        HttpServer(System::SelectorBase& selector, const std::string& ip, unsigned short int port);

        void addService(const std::string& url, HttpService& resp);

        HttpService* getService(const std::string& url);

        void onConnect(TcpServer& server);

        std::size_t readTimeout() const       { return _readTimeout; }
        std::size_t writeTimeout() const      { return _writeTimeout; }
        std::size_t keepAliveTimeout() const  { return _keepAliveTimeout; }

        void readTimeout(std::size_t ms)      { _readTimeout = ms; }
        void writeTimeout(std::size_t ms)     { _writeTimeout = ms; }
        void keepAliveTimeout(std::size_t ms) { _keepAliveTimeout = ms; }

    private:
        typedef std::map<std::string, HttpService*> ServicesType;
        ServicesType _service;
        System::SelectorBase& _selector;
        HttpNotFoundService _defaultService;

        std::size_t _readTimeout;
        std::size_t _writeTimeout;
        std::size_t _keepAliveTimeout;
};


class PT_NET_API HttpSocket : public TcpSocket, public Connectable
{
    public:
        HttpSocket(System::SelectorBase& s, HttpServer& server);

        void onInput(System::StreamBuffer& stream);
        void onOutput(System::StreamBuffer& stream);
        void onTimeout();

    private:
        HttpServer& _server;
        System::Timer _timer;
        int _contentSize;
        HttpResponder* _responder;
        System::IOStream _stream;
        bool _readHeader;
};

} // namespace Net

} // namespace Pt

#endif
