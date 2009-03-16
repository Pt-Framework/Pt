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

#ifndef Pt_Net_HttpClient_h
#define Pt_Net_HttpClient_h

#include <Pt/Net/Api.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/IOStream.h>
#include <Pt/System/Timer.h>
#include <Pt/Connectable.h>
#include <Pt/Delegate.h>
#include <string>
#include <sstream>
#include <map>
#include <cstddef>

namespace Pt {

namespace Net {

class HttpRequest
{
        friend class HttpReply;

        std::string _server;
        unsigned short _port;
        std::string _url;
        std::ostringstream _body;
        typedef std::map<std::string, std::string> Headers;
        Headers _headers;

    public:
        HttpRequest(const std::string& server, unsigned short port, const std::string& url)
        : _server(server)
        , _port(port)
        , _url(url)
        {}

        const std::string& server() const
        { return _server; }

        unsigned short port() const
        { return _port; }

        const std::string& url() const
        { return _url; }

        void setUrl(const std::string& url)
        {}

        std::string method() const
        { return "GET"; }

        void setHeader(const std::string& key, const std::string& value)
        {}

        std::string getHeader(const std::string& key) const
        { return std::string(); }

        std::ostream& body()
        { return _body; }

};

class HttpReply : public Pt::Connectable
{
        HttpRequest* _request;
        TcpSocket _socket;
        System::IOStream _stream;
        bool _readHeader;
        std::size_t _contentSize;
        bool _requestReady;
        bool _executed;

        typedef std::map<std::string, std::string> Headers;
        Headers _headers;

    protected:
        void onConnect(TcpSocket& socket);
        void onOutput(System::StreamBuffer& sb);
        void onInput(System::StreamBuffer& sb);

    public:
        HttpReply();

        void beginExecute(HttpRequest& request);

        void setSelector(System::SelectorBase& selector);

        std::string getHeader(const std::string& key) const
        { return std::string(); }

        void wait(std::size_t msecs);

        std::istream& in()   // reply body is received here
        {
            return _stream;
        }

        bool isReady() const   { return _requestReady; }

        Signal<HttpReply&> headerReceived;

        Pt::Delegate<std::size_t, HttpReply&> replyReceived;
};

} // namespace Net

} // namespace Pt

#endif
