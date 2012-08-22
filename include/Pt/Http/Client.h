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
#include <Pt/Http/Request.h>
#include <Pt/System/Selectable.h>
#include <Pt/System/IOError.h>
#include <Pt/Signal.h>
#include <Pt/Delegate.h>
#include <Pt/NonCopyable.h>
#include <string>

namespace Pt {

namespace Net {
    class AddrInfo;
}

namespace Ssl {
  class Context;
}

namespace Http {

class ReplyHeader;
class RequestHeader;
class Request;

class PT_HTTP_API ConnectionClosed : public System::IOError
{
    public:
        explicit ConnectionClosed();

        ~ConnectionClosed() throw()
        {}
};

class PT_HTTP_API Client : private NonCopyable
{
    public:
        Client();
        
        Client(const std::string& host, unsigned short int port, bool ssl = false);
        
        Client(const Net::AddrInfo& addr, bool ssl = false);

        Client(System::EventLoop& loop, const std::string& host, unsigned short int port, bool ssl = false);
        
        Client(System::EventLoop& loop, const Net::AddrInfo& addrinfo, bool ssl = false);

        ~Client();

        System::EventLoop* loop() const;

        void setActive(System::EventLoop& loop);

        void setTimeout(std::size_t timeout);

        // Sets the host and port. No actual network connect is done.
        void setHost(const Net::AddrInfo& addrinfo, bool ssl = false);
        
        void setHost(const std::string& host, unsigned short int port, bool ssl = false);

        const Net::AddrInfo& host() const;

        void setContext(Ssl::Context& ctx);

        // Sets the username and password for all subsequent requests.
        void setAuthorization(const std::string& username, const std::string& password);

        void clearAuthorization();

        // Sends the passed request to the server and parses the headers.
        // The body must be read with readBody.
        // This method blocks or times out until the body is parsed.
        const ReplyHeader& execute(const Request& request);

        const ReplyHeader& header();

        // Returns the underlying stream, where the reply may be read from.
        std::istream& body();

        // Starts a new request.
        // This method does not block. To actually process the request, the
        // event loop must be executed. The state of the request is signaled
        // with the corresponding signals and delegates.
        // The delegate "bodyAvailable" must be connected, if a body is
        // received.
        void beginExecute(const Request& request);

        void endExecute();

        // NEW API: //////////////////////
        void beginSend(bool endOfRequest = true);

        Progress endSend();

        void beginReceive();

        Progress endReceive();

        bool isEnd() const;

        Request& request();

        //////////////////////////////////

        void cancel();

        // Signals that the request is sent to the server.
        //
        // beginRequest / endRequest reports error
        //
        Signal<Client&>& requestSent()
        { return _requestSent; }

        Signal<Client&>& replyReceived()
        { return _replyReceived; }

        // Signals that the header is received.
        //
        // beginReply -> no errors can occur
        //
        Signal<Client&>& headerReceived()
        { return _headerReceived; }

        // Signals that body data has arrived.
        //
        // beginReply -> no errors can occur
        //
        Signal<Client&>& bodyReceived()
        { return _bodyAvailable; }

        // Signals that the reply is completely processed.
        //
        // beginReply / endReply reports error
        //
        Signal<Client&>& replyFinished()
        { return _replyFinished; }

    private:
        // Signals that the request is sent to the server.
        Signal<Client&> _requestSent;

        Signal<Client&> _replyReceived;

        // Signals that the header is received.
        Signal<Client&> _headerReceived;

        // Signals that body data has arrived.
        Signal<Client&> _bodyAvailable;

        // Signals that the reply is completely processed.
        Signal<Client&> _replyFinished;

        class ClientImpl* _impl;
};

} // namespace Http

} // namespace Pt

#endif
