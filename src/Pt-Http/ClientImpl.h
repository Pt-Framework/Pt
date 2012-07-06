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

#ifndef Pt_Http_ClientImpl_h
#define Pt_Http_ClientImpl_h

#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/Net/TcpServer.h>
#include <Pt/Net/TcpSocket.h>

#ifdef PT_HTTP_WITH_SSL
#include <Pt/Ssl/IOBuffer.h>
#endif

#include <Pt/System/Selectable.h>
#include <Pt/System/IOStream.h>
#include <Pt/System/Timer.h>
#include <Pt/Connectable.h>
#include <Pt/Delegate.h>
#include <string>
#include <sstream>
#include <cstddef>
#include "ChunkedReader.h"
#include "Parser.h"

namespace Pt {

namespace Ssl {
  class Context;
}

namespace Http {

class Client;

class Client2 : public Connectable
{
        class PT_HTTP_API ParseEvent : public HeaderParser::MessageHeaderEvent
        {
                ReplyHeader& _replyHeader;

            public:
                explicit ParseEvent(ReplyHeader& replyHeader)
                    : HeaderParser::MessageHeaderEvent(replyHeader),
                      _replyHeader(replyHeader)
                    { }

                void onHttpReturn(unsigned ret, const std::string& text) {}
        };

    public:
        Client2(std::iostream& ios);

        void writeRequest(const Request& request);

        bool readReply();

        // Signals that the header is received.
        Signal<Client2&> headerReceived;

        // This delegate is called, when data is arrived while reading the
        // body. The connected functor must return the number of bytes read.
        Pt::Delegate<std::size_t, Client2&, std::istream&> bodyAvailable;

        // Signals that the reply is completely processed.
        Signal<Client2&> replyFinished;

    protected:
        Client2();

        void init(std::iostream& ios)
        { _ios = &ios; }

    private:
        void parseHeader();

        bool parseBody();

    private:
        std::iostream* _ios;
        std::string _username;
        std::string _password;
        HeaderParser _parser;
        ParseEvent _parseEvent;
        ReplyHeader _replyHeader;
        long _contentLength;
        ChunkedIStream _chunkedIStream;
        bool _readHeader;
};


class TcpClient : public Client2
{
    public:
        TcpClient()
        : _reusedConnection(false)
        {
            _socket.connected() += Pt::slot(*this, &TcpClient::onConnect);
            
            _stream.attach(_socket);
            _stream.buffer().outputReady() += Pt::slot(*this, &TcpClient::onOutput);
            _stream.buffer().inputReady() += Pt::slot(*this, &TcpClient::onInput);
            
            Client2::init(_stream);
        }

        ~TcpClient()
        {
        }

        void setActive(System::EventLoop& loop)
        {
            _socket.setActive(loop);
        }

        void setHost(const Net::AddrInfo& addrinfo)
        {
            _addrInfo = addrinfo;
            _socket.close();
        }

        void beginRequest(const Request& request)
        {
            _socket.beginConnect(_addrInfo);
        }

    private:
        void onConnect(Net::TcpSocket& socket)
        {
            _socket.endConnect();
            _stream.buffer().beginRead();
        }

        void onOutput(System::StreamBuffer& sb)
        {
        }

        void onInput(System::StreamBuffer& sb)
        {
        }

    private:
        Net::AddrInfo _addrInfo;
        Net::TcpSocket _socket;
        System::IOStream _stream;
        bool _reusedConnection;
};


class ClientImpl : public Connectable
{
    friend class ParseEvent;

    public:
        ClientImpl(Client* client);
        
        ClientImpl(Client* client, const Net::AddrInfo& addrinfo);
        
        ClientImpl(Client* client, System::EventLoop& selector, const Net::AddrInfo& addrinfo);

        System::EventLoop* loop() const
        {
            return _socket.parent();
        }

        void setActive(System::EventLoop& loop)
        {
            _socket.setActive(loop);
        }

        void setTimeout(std::size_t timeout)
        {
            _socket.setTimeout(timeout);
        }

        void setHost(const Net::AddrInfo& addrinfo)
        {
            _addrInfo = addrinfo;
            _socket.close();
        }

        const Net::AddrInfo& host() const
        {
            return _addrInfo;
        }

        void setAuth(const std::string& username, const std::string& password)
        { 
            _username = username; 
            _password = password; 
        }

        void clearAuth()
        { 
            _username.clear(); 
            _password.clear(); 
        }

        void setSecure(Ssl::Context& ctx);

        const ReplyHeader& execute(const Request& request);

        const ReplyHeader& header()
        { return _replyHeader; }

        void readBody(std::string& s);

        std::string get(const std::string& url);

        void beginRequest(const Request& request);

        void endExecute();

        std::istream& in()
        {
            return _stream;
            //return _replyHeader.chunkedTransferEncoding() ? static_cast<std::istream&>(_chunkedIStream)
            //                                              : static_cast<std::istream&>(_ios);
        }

        void cancel();

    protected:
        void onConnect(Net::TcpSocket& socket);
        void onOutput(System::StreamBuffer& sb);
        void onInput(System::StreamBuffer& sb);
        void onError();

#ifdef PT_HTTP_WITH_SSL
        void onSslHandshake(Ssl::IOBuffer& sb);
        void onSslOutput(Ssl::IOBuffer& sb);
        void onSslInput(Ssl::IOBuffer& sb);
#endif

    private:
        void init();

        void sendRequest(std::ostream& os, const Request& request);

        bool onHeader(std::streambuf& sb, bool ssl);
        void onHttpsHeader(System::StreamBuffer& sbuf);
        void onHttpHeader(System::StreamBuffer& sbuf);

        bool onBody(std::istream& is);
        void onHttpsBody(System::StreamBuffer& sbuf);
        void onHttpBody(System::StreamBuffer& sbuf);

        bool onChunkedBody();
        void onHttpsChunkedBody(System::StreamBuffer& sbuf);
        void onHttpChunkedBody(System::StreamBuffer& sbuf);

    private:
        class ParseEvent : public HeaderParser::MessageHeaderEvent
        {
                ReplyHeader& _replyHeader;

            public:
                explicit ParseEvent(ReplyHeader& replyHeader)
                    : HeaderParser::MessageHeaderEvent(replyHeader),
                      _replyHeader(replyHeader)
                    { }

                void onHttpReturn(unsigned ret, const std::string& text);
        };
        
        Client* _client;
        ParseEvent _parseEvent;
        HeaderParser _parser;

        const Request* _request;
        ReplyHeader _replyHeader;

        Net::AddrInfo _addrInfo;
        Net::TcpSocket _socket;
        System::IOStream _ios;
        ChunkedReader _chunkedBuffer;
        std::iostream _stream;
        
#ifdef PT_HTTP_WITH_SSL
        Ssl::Context* _ctx;
        Ssl::IOBuffer _sslbuf;
#endif

        std::string _username;
        std::string _password;
        long _contentLength;
        bool _reusedConnection;
        bool _errorPending;

        void (ClientImpl::*_state)(System::StreamBuffer&);
};

} // namespace Http

} // namespace Pt

#endif
