/*
 * Copyright (C) 2009 by Tommi Maekitalo
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
#ifndef Pt_XmlRpc_HttpClientImpl_h
#define Pt_XmlRpc_HttpClientImpl_h

#include <Pt/Http/Client.h>
#include <Pt/Http/Request.h>
#include "ClientImpl.h"

namespace Pt
{

namespace Net
{
    class AddrInfo;
}

namespace XmlRpc
{

class HttpClientImpl : public ClientImpl
{
    public:
        HttpClientImpl();

        HttpClientImpl(System::EventLoopBase& selector, const std::string& addr,
               unsigned short port, const std::string& url);

        HttpClientImpl(const std::string& addr, unsigned short port, const std::string& url);

        void connect(const Net::AddrInfo& addrinfo, const std::string& url)
        {
            _client.connect(addrinfo);
            _request.url(url);
        }

        void connect(const std::string& addr, unsigned short port,
                     const std::string& url)
        {
            _client.connect(addr, port);
            _request.url(url);
        }

        void url(const std::string& url)
        {
            _request.url(url);
        }

        void auth(const std::string& username, const std::string& password)
        {
            _client.auth(username, password);
        }

        void clearAuth()
        {
            _client.clearAuth();
        }

        std::string url() const;

    protected:
        void onReplyHeader(Http::Client& client);

        std::size_t onReplyBody(Http::Client& client);

        void onReplyFinished(Http::Client& client);

        virtual void beginExecute();

        virtual void endExecute();

        virtual std::string execute();

        virtual std::ostream& prepareRequest();

        virtual void cancel();

    private:
        static void verifyHeader(const Http::ReplyHeader& header);

        Http::Client _client;
        Http::Request _request;
};

}

}

#endif
