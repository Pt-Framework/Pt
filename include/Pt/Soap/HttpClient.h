/*
 * Copyright (C) 2012-2014 by Marc Duerner
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

#ifndef Pt_Soap_HttpClient_h
#define Pt_Soap_HttpClient_h

#include <Pt/Soap/Api.h>
#include <Pt/Soap/Client.h>
#include <Pt/Http/Client.h>
#include <Pt/Connectable.h>
#include <Pt/Types.h>
#include <string>

namespace Pt {

namespace Soap {

/** @brief A client for SOAP via HTTP.
*/
class PT_SOAP_API HttpClient : public Client
                             , public Connectable
{
    public:
        /** @brief Constructor.
        */
        HttpClient(ServiceDeclaration& service);

        /** @brief Construct with EventLoop used for I/O.
        */
        HttpClient(ServiceDeclaration& service, System::EventLoop& loop);

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

        /** @brief Set expected SSL peer name.
        */
        void setPeerName(const std::string& peer);

        /** @brief Enables HTTP keep-alive.
        */
        void setKeepAlive();

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

        Pt::Http::Request& request();

        Pt::Http::Reply& reply();

        /** @brief Closes the connection.
        */
        void close();

    protected: 
        virtual bool isFailed() const;

        // inheritdoc
        virtual void onBeginInvoke();

        // inheritdoc
        virtual void onInvoke();

        // inheritdoc
        virtual void onEndInvoke();

        // inheritdoc
        virtual void onCancel();

    private:
        /** @brief Fails the current procedure.

            This method is used by derived Clients before calling finishResult()
            so that the RemoteProcedure throws a Fault when the result is 
            processed.
        */
        void setError(bool f = true)
        { _error = f; }

        //! @internal
        void init();

        //! @internal
        void onRequest(Http::Client& client);

        //! @internal
        void onReply(Http::Client& client);

    private:
        Http::Client _client;
        bool _error;
        Pt::varint_t _r1;
        Pt::varint_t _r2;
};

} // namespace Soap

} // namespace Pt

#endif // Pt_Soap_HttpClient_h
