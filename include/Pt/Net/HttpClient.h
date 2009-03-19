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
#include <Pt/Net/HttpParser.h>
#include <Pt/Net/HttpRequest.h>
#include <Pt/Net/HttpReply.h>
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

class HttpClient : public Pt::Connectable
{
        friend class ParseEvent;

        class ParseEvent : public HttpHeaderParser::HttpMessageHeaderEvent
        {
                HttpReplyHeader& _replyHeader;

            public:
                explicit ParseEvent(HttpReplyHeader& replyHeader)
                    : HttpHeaderParser::HttpMessageHeaderEvent(replyHeader),
                      _replyHeader(replyHeader)
                    { }

                void onHttpReturn(unsigned ret, const std::string& text);
        };

        ParseEvent _parseEvent;
        HttpHeaderParser _parser;

        const HttpRequest* _request;
        HttpReplyHeader _replyHeader;

        std::string _server;
        unsigned short int _port;
        TcpSocket _socket;
        System::IOStream _stream;
        bool _readHeader;
        long _contentSize;

        void sendRequest(const HttpRequest& request);

    protected:
        void onConnect(TcpSocket& socket);
        void onOutput(System::StreamBuffer& sb);
        void onInput(System::StreamBuffer& sb);

    public:
        HttpClient(const std::string& server, unsigned short int port);

        HttpClient(const std::string& server, unsigned short int port, System::SelectorBase& selector);

        const HttpReplyHeader& execute(const HttpRequest& request,
            std::size_t timeout = System::Selectable::WaitInfinite);

        const HttpReplyHeader& header()
        { return _replyHeader; }

        void readBody(std::string& s);

        std::string readBody()
        {
            std::string ret;
            readBody(ret);
            return ret;
        }

        std::string get(const std::string& url);

        void beginExecute(const HttpRequest& request);

        void setSelector(System::SelectorBase& selector);

        void wait(std::size_t msecs);

        std::istream& in()   // reply body is received here
        {
            return _stream;
        }

        Signal<HttpClient&> requestSent;
        Signal<HttpClient&> headerReceived;
        Pt::Delegate<std::size_t, HttpClient&> bodyAvailable;
        Signal<HttpClient&> replyFinished;
};

} // namespace Net

} // namespace Pt

#endif
