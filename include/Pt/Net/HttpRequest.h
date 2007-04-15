/***************************************************************************
 *   Copyright (C) 2003-2007 Tommi Maekitalo                               *
 *   Copyright (C) 2007      Ulrich Termathe                               *
 *   Copyright (C) 2007      PTV AG                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef Pt_Net_HttpRequest_h
#define Pt_Net_HttpRequest_h

#include <Pt/Blob.h>
#include <Pt/Types.h>
#include <Pt/Net/Api.h>
#include <Pt/Net/QueryParams.h>
#include <Pt/Net/TcpSocket.h>

#include <map>

namespace Pt {

namespace Net {

    /**
     * This class provides a HTTP 1.0 request, to be send via a TCP/IP
     * connection.
     */
    class PT_NET_API HttpRequest
    {
    public:
        /** String constants for the HTTP header. */
        static const std::string STR_CONTENT_LENGTH;
        static const std::string STR_END_OF_LINE;
        static const std::string STR_GET;
        static const std::string STR_HOST;
        static const std::string STR_HTTP_VERSION;
        static const std::string STR_POST;

        /** Supported HTTP request types. */
        enum request_type {
            GET, POST
        } type;

    private:
        /** Class to parse the HTTP request header data. */
        class Parser;
        friend class Parser;

        typedef std::map<std::string, std::string> header_type;

        /** The TCP/IP connection. */
        Pt::Net::TcpSocket _connection;

        /** Type of the HTTP request. */
        request_type _method;

        /** The IP address of TCP/IP connection to the server. */
        std::string _host;

        /** The request URL. */
        std::string _url;

        /** The port number of TCP/IP connection to the server. */
        unsigned short _port;

        /** Parameters used in the URL in case of a GET request. */
        QueryParams _params;

        /** Version of HTTP protocol used in the request message. */
        std::string _httpVersion;

        /** Key-value pairs of the HTTP header lines. */
        header_type _header;

        /** Body data that are send in case of a POST request. */
        Pt::Blob _body;

        /**
         * @brief Sends a HTTP GET request.
         *
         * @param connection Reference of the TCP/IP connection.
         */
        void sendGetMethod(Pt::Net::TcpSocket& connection);

        /**
         * @brief Sends a HTTP POST request.
         *
         * @param connection Reference of the TCP/IP connection.
         */
        void sendPostMethod(Pt::Net::TcpSocket& connection);

    public:
        /**
         * @brief Constructs a new HTTP GET request object, using port 80. 
         */
        explicit HttpRequest()
            : _method(GET),
            _port(80)
        { }

        /**
         * @brief Constructs a new HTTP request object.
         *
         * @param url The request URL, send to the server.
         * @param method Type of the HTTP request (GET or POST).
         */
        explicit HttpRequest(const std::string& url, request_type method = GET);

        /**
         * @brief Constructs a new HTTP request object.
         *
         * @param host IP address of the TCP/IP connection to the server.
         * @param url The request URL, send to the server.
         * @param method Type of the HTTP request (GET or POST).
         */
        HttpRequest(const std::string& host, const std::string& url, request_type method = GET)
            : _method(method),
            _host(host),
            _url(url)
        { }

        /**
         * @brief Constructs a new HTTP request object.
         *
         * @param host IP address of the TCP/IP connection to the server.
         * @param port Port number of the TCP/IP connection to the server.
         * @param url The request URL, send to the server.
         * @param method Type of the HTTP request (GET or POST).
         */
        HttpRequest(const std::string& host, unsigned short port, const std::string& url,
            request_type method = GET)
            : _method(method),
            _host(host),
            _url(url),
            _port(port)
        { }

        /**
         * @brief Returns a reference of the TCP/IP connection.
         *
         * @param Reference of the TCP/IP connection.
         */
        Pt::Net::TcpSocket& getConnection()
        {
            return _connection;
        }

        /**
         * @brief Returns the HTTP request method.
         *
         * @return The request method ("GET" or "PUT").
         */
        request_type getMethod() const
        {
            return _method;
        }

        /**
         * @brief Sets the HTTP request method.
         *
         * @param method Type of the request method ("GET" or "PUT").
         */
        void setMethod(request_type method)
        {
            _method = method;
        }

        /**
         * @brief Returns the server's IP address.
         *
         * @return The server's IP address.
         */
        const std::string& getHost() const
        {
            return _host;
        }

        /**
         * @brief Sets the IP address of the server.
         *
         * @param host The server's IP address.
         */
        void setHost(const std::string& host)
        {
            _host = host;
        }

        /**
         * @brief Returns the HTTP request URL.
         *
         * @return The request URL.
         */
        const std::string& getUrl() const
        {
            return _url;
        }

        /**
         * @brief Sets the HTTP request URL.
         *
         * @param The request URL.
         */
        void setUrl(const std::string& url)
        {
            _url = url;
        }

        /**
         * @brief Returns the port number of the TCP/IP connection to the server.
         *
         * @return The server's port number.
         */
        unsigned short getPort() const
        {
            return _port;
        }

        /**
         * @brief Sets the port number for the TCP/IP connection to the server.
         *
         * @param port The server's port number.
         */
        void setPort(unsigned short port)
        {
            _port = port;
        }

        /**
         * @brief Returns all parameters request URL, e.g., "?xxx=yyy&z=a".
         *
         * @params Reference to the query parameters of the request URL.
         */
        const QueryParams& getParams() const
        {
            return _params;
        }

        /**
         * @brief Adds a new query parameter to the request URL.
         *
         * @param name Key name of the query parameter.
         * @param value Value of the query parameter.
         */
        void setParam(const std::string& name, const std::string& value)
        {
            _params.add(name, value);
        }

        /**
         * @brief Adds a single value to the query parameter of the request URL.
         *
         * @param value The value to add to the request URL.
         */
        void setParam(const std::string& value)
        {
            _params.add(value);
        }

        /**
         * @brief Returns the body data used for a POST request.
         *
         * @return The body data as binary data.
         */
        const Pt::Blob getBody() const
        {
            return _body;
        }

        /**
         * @brief Sets the body data for a POST request.
         *
         * @param body The body data as binary data.
         */
        void setBody(const Pt::Blob body)
        {
            _body = body;
        }

        /**
         * @brief Sets the body data for a POST request.
         *
         * @param body The body data as string.
         */
        void setBody(const std::string& body)
        {
            _body.assign(body.data(), body.length());
        }

        /**
         * @brief Sends the HTTP request to the server, using the internal
         * connection.
         */ 
        void send();

        /**
         * @brief Sends the HTTP request to the server using a passed
         * connection.
         *
         * @param connection Socket connection used to send the HTTP request.
         */ 
        void send(Pt::Net::TcpSocket& connection);

        /**
         * @brief Receives data via a connected socket.
         *
         * @param connection Connected socket connection to read the HTTP
         * request data.
         */
        void receive(Pt::Net::TcpSocket& connection);
    };

} // !namespace Net

} // !namespace Pt

#endif // Pt_Net_HttpRequest_h
