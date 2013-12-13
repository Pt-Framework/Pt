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
#include <Pt/Types.h>
#include <string>

namespace Pt {

namespace XmlRpc {

/** @brief A client for remote procedure calls via HTTP.
*/
class PT_XMLRPC_API HttpClient : public Client
                               , public Connectable
{
    public:
        /** @brief Constructor.
        */
        HttpClient();

        /** @brief Construct with host and service URL.
        */
        HttpClient(const Net::Endpoint& ep, const std::string& url);

        /** @brief Construct with EventLoop used for I/O.
        */
        HttpClient(System::EventLoop& loop);

        /** @brief Construct with host and service URL.
        */
        HttpClient(System::EventLoop& loop, const Net::Endpoint& ep, 
                   const std::string& url);

        /** @brief Destructor.
        */
        virtual ~HttpClient();

        /** @brief Sets the EventLoop to use for I/O.
        */
        void setActive(System::EventLoop& loop);

        /** @brief Gets the used EventLoop.
        */
        System::EventLoop* loop() const;

        /** @brief Sets SSL context.
        */
        void setSecure(Ssl::Context& ctx);

        /** @brief Sets timeout for I/O operations.
        */
        void setTimeout(std::size_t timeout);

        /** @brief Sets target host and service URL.
        */
        void setTarget(const Net::Endpoint& ep, const std::string& url);

        /** @brief Sets target host and service URL.
        */
        void setTarget(const Net::Endpoint& ep, const Net::TcpSocketOptions& opts, 
                       const std::string& url);

        /** @brief Sets host to connect.
        */
        void setHost(const Net::Endpoint& ep);

        /** @brief Sets host to connect.
        */
        void setHost(const Net::Endpoint& ep, const Net::TcpSocketOptions& opts);

        /** @brief Sets the service URL.
        */
        void setServiceUrl(const std::string& url);

        /** @brief Sets the service URL.
        */
        void setServiceUrl(const char* url);

        /** @brief Returns target host.
        */
        const Net::Endpoint& host() const;

    protected:       
        // inheritdoc
        virtual void onInvoke();

        // inheritdoc
        virtual void onCall();

        // inheritdoc
        virtual void onCancel();

        // inheritdoc
        virtual void onError();

    private:
        void init();
        void onRequest(Http::Client& client);
        void onReply(Http::Client& client);

    private:
        Http::Client _client;
        Pt::varint_t _r1;
        Pt::varint_t _r2;
};

} // namespace XmlRpc

} // namespace Pt

#endif // Pt_XmlRpc_HttpClient_h
