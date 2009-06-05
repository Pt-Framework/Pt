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

#ifndef Pt_Http_Client_h
#define Pt_Http_Client_h

#include <Pt/Http/Api.h>
#include <Pt/Http/Parser.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>
#include <Pt/System/Selectable.h>
#include <Pt/System/IOStream.h>
#include <Pt/System/Timer.h>
#include <Pt/Connectable.h>
#include <Pt/Delegate.h>
#include <string>
#include <sstream>
#include <map>
#include <cstddef>

namespace Pt {

namespace Http {

class PT_HTTP_API Client : public Pt::Connectable
{
        friend class ParseEvent;

        class PT_HTTP_API ParseEvent : public HeaderParser::MessageHeaderEvent
        {
                ReplyHeader& _replyHeader;

            public:
                explicit ParseEvent(ReplyHeader& replyHeader)
                    : HeaderParser::MessageHeaderEvent(replyHeader),
                      _replyHeader(replyHeader)
                    { }

                void onHttpReturn(unsigned ret, const std::string& text);
        };

        ParseEvent _parseEvent;
        HeaderParser _parser;

        const Request* _request;
        ReplyHeader _replyHeader;

        std::string _server;
        unsigned short int _port;
        Net::TcpSocket _socket;
        System::IOStream _stream;
        bool _readHeader;
        long _contentLength;

        void sendRequest(const Request& request);
        void processHeaderAvailable(System::StreamBuffer& sb);
        void processBodyAvailable(System::StreamBuffer& sb);

        void reexecute(const Request& request);
        void doparse();

    protected:
        void onConnect(Net::TcpSocket& socket);
        void onOutput(System::StreamBuffer& sb);
        void onInput(System::StreamBuffer& sb);

    public:
        Client();
        Client(const std::string& server, unsigned short int port);

        Client(System::SelectorBase& selector, const std::string& server, unsigned short int port);

        void connect(const std::string& server, unsigned short int port)
        {
            _server = server;
            _port = port;
        }

        const ReplyHeader& execute(const Request& request,
            std::size_t timeout = System::Selectable::WaitInfinite);

        const ReplyHeader& header()
        { return _replyHeader; }

        void readBody(std::string& s);

        std::string readBody()
        {
            std::string ret;
            readBody(ret);
            return ret;
        }

        std::string get(const std::string& url,
            std::size_t timeout = System::Selectable::WaitInfinite);

        void beginExecute(const Request& request);

        void setSelector(System::SelectorBase& selector);

        void wait(std::size_t msecs);

        std::istream& in()   // reply body is received here
        {
            return _stream;
        }

        const std::string& server() const
        { return _server; }

        unsigned short int port() const
        { return _port; }

        Signal<Client&> requestSent;
        Signal<Client&> headerReceived;
        Pt::Delegate<std::size_t, Client&> bodyAvailable;
        Signal<Client&> replyFinished;
        Signal<Client&, const std::exception&> errorOccured;
};

} // namespace Http

} // namespace Pt

#endif
