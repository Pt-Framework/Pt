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

#ifndef Pt_Http_Socket_h
#define Pt_Http_Socket_h

#include <Pt/Net/TcpSocket.h>
#include <Pt/Http2/Request.h>
#include <Pt/Http2/Reply.h>
#include <Pt/System/IOStream.h>
#include <Pt/System/Timer.h>
#include <Pt/Connectable.h>
#include <Pt/Signal.h>
#include <Pt/Method.h>
#include "Parser.h"

namespace Pt {

namespace Http {

class ServerImpl;
class Responder;

class Socket : public Net::TcpSocket, public Connectable
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
        Socket(ServerImpl& server, Net::TcpServer& tcpServer);
        explicit Socket(Socket& socket);
        ~Socket();

        void accept();
        bool hasAccepted() const  { return _accepted; }

        void setParent(System::EventLoop* parent);

        void onIODeviceInput(System::IODevice& iodevice);
        void onInput(System::StreamBuffer& sb);
        bool onOutput(System::StreamBuffer& sb);
        void onTimeout();

        bool doReply();
        void sendReply();
        bool isReady() const
        { return _parser.end() && _contentLength == 0; }

        const Request& request() const { return _request; }
        const Reply& reply() const     { return _reply; }

        Signal<Socket&> inputReady;
        Signal<Socket&> timeout;

        System::StreamBuffer& buffer()         { return _stream.buffer(); }

        MethodSlot<void, Socket, System::StreamBuffer&> inputSlot;

        Pt::Connection inputConnection;
        Pt::Connection timeoutConnection;

    private:
        Net::TcpServer& _tcpServer;
        ServerImpl& _server;

        ParseEvent _parseEvent;
        HeaderParser _parser;
        Request _request;
        Reply _reply;

        System::Timer _timer;
        int _contentLength;
        Responder* _responder;
        System::IOStream _stream;

        bool _accepted;
};

} // namespace Http

} // namespace Pt

#endif
