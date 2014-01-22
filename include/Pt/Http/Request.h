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

#ifndef Pt_Http_Request_h
#define Pt_Http_Request_h

#include <Pt/Http/Api.h>
#include <Pt/Http/Message.h>
#include <Pt/Signal.h>
#include <string>

namespace Pt {

namespace Http {

/** @brief HTTP request message.
*/
class PT_HTTP_API Request : public Message
{
    friend class Connection;

    public:
        /** @brief Construct with connection.
        */
        explicit Request(Http::Connection& conn)
        : Message(conn)
        , _url()
        , _method("GET")
        { }

        /** @brief Construct with connection and URL.
        */
        Request( Http::Connection& conn, const std::string& url)
        : Message(conn)
        , _url(url)
        , _method("GET")
        { }

        /** @brief Construct with connection and URL.
        */
        Request( Http::Connection& conn, const char* url)
        : Message(conn)
        , _url(url)
        , _method("GET")
        { }

        //! @brief Returns the HTTP request URL.
        const std::string& url() const
        { return _url; }

        /** @brief Sets the request URL.
        */
        void setUrl(const std::string& u)
        { _url = u; }

        /** @brief Sets the request URL.
        */
        void setUrl(const char* u)
        { _url = u; }

        //! @brief Returns the HTTP request method.
        const std::string& method() const
        { return _method; }
        
        /** @brief Sets the request method.
        */
        void setMethod(const std::string& m)
        { _method = m; }

        /** @brief Sets the request method.
        */
        void setMethod(const char* m)
        { _method = m; }

        //! @brief Returns the HTTP request URL query.
        const std::string& qparams() const
        { return _qparams; }

        /** @brief Sets the URL query string.
        */
        void setQParams(const std::string& p)
        { _qparams = p; }

        /** @brief Sets the URL query string.
        */
        void setQParams(const char* p)
        { _qparams = p; }

        //! @internal
        void beginReceive();

        //! @internal
        MessageProgress endReceive();

        //! @internal
        void send(bool finish = true);

        //! @internal
        void beginSend(bool finish = true);

        //! @internal
        MessageProgress endSend();

        //! @internal
        Signal<Request&>& inputReceived()
        { return _inputReceived; }

        //! @internal
        Signal<Request&>& outputSent()
        { return _outputSent; }

        /** @brief Clears all content.
        */
        void clear();

    protected:
        //! @internal
        void onInput()
        { _inputReceived.send(*this); }

        //! @internal
        void onOutput()
        { _outputSent.send(*this); }

    private:
        std::string _url;
        std::string _method;
        std::string _qparams;
        Signal<Request&> _inputReceived;
        Signal<Request&> _outputSent;
};

} // namespace Http

} // namespace Pt

#endif // Pt_Http_Request_h
