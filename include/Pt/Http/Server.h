/*
 * Copyright (C) 2011-2012 by Marc Boris Duerner
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

#ifndef Pt_Http_Server_h
#define Pt_Http_Server_h

#include <Pt/Http/Api.h>
#include <Pt/Http/IOStream.h>
#include <Pt/Connectable.h>
#include <Pt/NonCopyable.h>
#include <cstddef>

namespace Pt {

namespace System {
class EventLoop;
}

namespace Net {
class Endpoint;
class TcpServerOptions;
}

namespace Ssl {
class Context;
}

namespace Http {

/** @brief An HTTP server.
*/
class PT_HTTP_API Server : public Connectable
                         , private NonCopyable
{
    public:
        /** @brief Default Constructor.
        */
        Server();

        /** @brief Construct with event loop.
        */
        explicit Server(System::EventLoop& loop);

        /** @brief Construct with event loop and listen endpoint.
        */
        Server(System::EventLoop& loop, const Net::Endpoint& ep);

        /** @brief Destructor.
        */
        ~Server();

        /** @brief Returns the used event loop.
        */
        System::EventLoop* loop();

        /** @brief Sets the event loop to use.
        */
        void setActive(System::EventLoop& loop);

        /** @brief Returns the timeout in msecs.
        */
        std::size_t timeout() const;

        /** @brief Sets the timeout in msecs.
        */
        void setTimeout(std::size_t ms);

        /** @brief Enables to use HTTPS.
        */
        void setSecure(Ssl::Context& ctx);
        
        /** @brief Returns the maximum number of server threads.
        */
        std::size_t maxThreads() const;

        /** @brief Sets the maximum number of server threads.
        */
        void setMaxThreads(std::size_t m);

        /** @brief Returns the keepalive timeout in msecs.
        */
        std::size_t keepAliveTimeout() const;

        /** @brief Sets the keepalive timeout in msecs.
        */
        void setKeepAliveTimeout(std::size_t ms);

        /** @brief Returns the maximum size of a request in bytes.
        */
        std::size_t maxRequestSize() const;

        /** @brief Sets the maximum size of a request in bytes.
        */
        void setMaxRequestSize(std::size_t maxSize);

        /** @brief Start listening on endpoint.
        */
        void listen(const Net::Endpoint& ep);

        /** @brief Start listening on endpoint.
        */
        void listen(const Net::Endpoint& ep, const Net::TcpServerOptions& opts);

        /** @brief Cancel all operations.
        */
        void cancel();

        /** @brief Adds a servlet.
        */
        void addServlet(Servlet& servlet);

        /** @brief Removes a servlet.
        */
        void removeServlet(Servlet& servlet);

        /** @internal @brief Set shutdown flag on servlet.
        */
        void shutdownServlet(Servlet& servlet, bool shutdown);

        /** @internal @brief Returns true if servlet is shut down.
        */
        bool isServletIdle(Servlet& servlet);

        //! @internal
        Servlet* getServlet(const Request& request);

        Signal<IOStream*>& upgradeRequested();

    private:
        class ServerImpl* _impl;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Server_h
