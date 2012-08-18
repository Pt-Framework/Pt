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
#include <Pt/System/IOBuffer.h>
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

class HttpBuffer : public std::streambuf
{
    static const unsigned int MaxPutback = 4;
    static const unsigned int BufferSize = 512;

    public:
        HttpBuffer()
        : _sbuf(0)
        , _obuffer(0)
        , _obufferSize(0)
        , _contentLength(0)
        , _chunked(false)
        , _keepAlive(false)
        {
            setg(0,0,0);
            setp(0,0);
        }

        ~HttpBuffer()
        {
            delete [] _obuffer;
        }
        
        void attach(std::streambuf& sbuf)
        { _sbuf = &sbuf; }

        std::streambuf* buffer()
        { return _sbuf; }

        void reset()
        { 
            _contentLength = 0;
            _chunked = false;
            _keepAlive = false;
        }

        void beginBody(const MessageHeader& reply);

        //void writeReply(const ReplyHeader& reply, const MessageBuffer& mbuf);

        //void finishReply(const ReplyHeader& reply, const MessageBuffer& mbuf);

        void import(std::streamsize n = 0);

        bool isEnd() const;

        std::size_t out_avail()
        { return pptr() - pbase(); }

        Signal<>& bodyFinished()
        { return _bodyFinished; }

    protected:
        virtual int_type underflow();

        virtual int sync();

        virtual int_type overflow(int_type ch);

    private:
        ChunkParser _chunkParser;
        std::streambuf* _sbuf;
        Signal<> _bodyFinished;
        char _buffer[4096];
        char* _obuffer;
        std::size_t  _obufferSize;
        long _contentLength;
        bool _chunked;
        bool _keepAlive;
};

class ClientImpl : public Connectable
{
    friend class ParseEvent;

    public:
        ClientImpl(Client* client);
        
        ClientImpl(Client* client, const Net::AddrInfo& addrinfo, bool ssl);
        
        ClientImpl(Client* client, System::EventLoop& selector, const Net::AddrInfo& addrinfo, bool ssl);

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

        void setHost(const Net::AddrInfo& addrinfo, bool ssl);

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

        void setContext(Ssl::Context& ctx);

        const ReplyHeader& execute(const Request& request);

        const ReplyHeader& header()
        { return _replyHeader; }

        std::string get(const std::string& url);

        void beginRequest(const Request& request);

        void beginRequest2(const Request& request);

        void endExecute();

        std::istream& body()
        {
            return _stream;
        }

        void cancel();

    protected:
        void onBodyFinished();

        void onConnect(Net::TcpSocket& socket);
        void onOutput(System::StreamBuffer& sb);
        void onInput(System::StreamBuffer& sb);
        void onError();
        void beginRead();

#ifdef PT_HTTP_WITH_SSL
        void onSslHandshake(Ssl::IOBuffer& sb);
        void onSslOutput(Ssl::IOBuffer& sb);
        void onSslInput(Ssl::IOBuffer& sb);
#endif

    private:
        void init();
        void sendChunked(std::ostream& os, const Request& request);
        void sendRequest(std::ostream& os, const Request& request);
        void onHeader();
        void onBody();

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
        bool _ssl;
        Net::TcpSocket _socket;
        System::IOBuffer _sockbuf;
#ifdef PT_HTTP_WITH_SSL
        Ssl::IOBuffer _sslbuf;
#endif
        HttpBuffer _httpbuf;
        std::iostream _stream;
        std::string _username;
        std::string _password;
        bool _reusedConnection;
        bool _errorPending;

        void (ClientImpl::*_state)();
};

} // namespace Http

} // namespace Pt

#endif
