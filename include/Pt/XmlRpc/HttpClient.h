/*
 * Copyright (C) 2012-2013 by Marc Duerner
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

#ifndef Pt_XmlRpc_HttpClient_h
#define Pt_XmlRpc_HttpClient_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Client.h>
#include <Pt/Http/Client.h>
#include <Pt/Connectable.h>
#include <string>

namespace Pt {

namespace XmlRpc {

class PT_XMLRPC_API HttpClient : public Client
                               , public Connectable
{
    public:
        HttpClient();

        HttpClient(const Net::AddrInfo& addrinfo, 
                   const std::string& serviceUrl);

        HttpClient(const std::string& addr, unsigned short port,
                   const std::string& serviceUrl);

        HttpClient(System::EventLoop& loop);

        HttpClient(System::EventLoop& loop, const Net::AddrInfo& addrinfo, 
                   const std::string& serviceUrl);

        HttpClient(System::EventLoop& loop, 
                   const std::string& addr, unsigned short port, 
                   const std::string& serviceUrl);

        virtual ~HttpClient();

        void setActive(System::EventLoop& loop);

        System::EventLoop* loop() const;

        void setSecure(Ssl::Context& ctx);

        /** @brief Set timeout for I/O operations.
        */
        void setTimeout(std::size_t timeout);

        void setTarget(const Net::AddrInfo& addrinfo, 
                       const std::string& serviceUrl);

        void setTarget(const std::string& addr, unsigned short port,
                       const std::string& serviceUrl);

        void setServiceUrl(const std::string& serviceUrl);

        void setHost(const Net::AddrInfo& addrinfo);
        
        void setHost(const std::string& host, unsigned short int port);

        const Net::AddrInfo& host() const;

        Http::Client& httpClient();

    protected:       
        virtual void onInvoke();

        virtual void onCall();

        virtual void onCancel();

        virtual void onError();

    private:
        void init();
        void onRequest(Http::Client& client);
        void onReply(Http::Client& client);

    private:
        Http::Client _client;
        void* _v1;
        void* _v2;
};

} // namespace XmlRpc

} // namespace Pt

#endif
