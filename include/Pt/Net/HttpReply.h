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

#ifndef Pt_Net_HttpReply_h
#define Pt_Net_HttpReply_h

#include <Pt/Net/Api.h>
#include <Pt/Net/QueryParams.h>
#include <Pt/Net/TcpSocket.h>

#include <iostream>
#include <map>

namespace Pt {

namespace Net {

    class HttpRequest;

    /**
     * Class that represents a HTTP response. This class holds the data that
     * should be send or that were received by a TCP/IP connection. The
     * received data can be requested from this class.
     */
    class PT_NET_API HttpReply
    {
    public:
        /**
         * This class represents a responding HTTP status code. It contains
         * code number and the corresponding message string.
         */
        class StatusCode
        {
        private:
            /** Status code number of the HTTP message. */
            unsigned int _code;

            /** Corresponging string of the status code. */
            std::string _message;

        public:
            /**
             * @brief Constructs and initializes a new HTTP status code.
             *
             * @param code The status code number to set.
             * @param message The status code message text so set.
             */
            StatusCode(unsigned int code, const std::string& message)
                : _code(code), _message(message)
            {}

            /**
             * @brief Copies the content of a HTTP status code into this new
             * HTTP status code object.
             *
             * @param source The source HTTP status code object, which
             * content is copied.
             */
            StatusCode(const StatusCode& source)
                : _code(source._code), _message(source._message)
            {}

            /**
             * @brief Assignment-Operator.
             *
             * @param source The source HTTP status code object, which
             * content is copied.
             *
             * @return A reference to this HTTP status code object.
             */
            StatusCode& operator=(const StatusCode& source)
            {
                if (this != &source)
                {
                    _code = source._code;
                    _message = source._message;
                }
                return *this;
            }

            /**
             * @brief Returns the HTTP status code number.
             *
             * @return The HTTP status code number.
             */
            unsigned int getCode() const { return _code; }

            /**
             * @brief Returns the HTTP status code message text.
             *
             * @return The HTTP status code message text.
             */
            const std::string& getMessage() const { return _message; }
        };

    private:
        /** Class to parse the HTTP reply header data. */
        class Parser;
        friend class Parser;

        typedef std::map<std::string, std::string> header_type;

        /** String constants for the HTTP header. */
        static const std::string STR_CONTENT_LENGTH;
        static const std::string STR_END_OF_LINE;
        static const std::string STR_DEFAULT_HTTP_VERSION;

        /** Reference to the TCP/IP connection to receive this HTTP response. */
        TcpSocket& _connection;

        /** Version of HTTP protocol used in the response message. */
        std::string _httpVersion;

        /** Key-value pairs of the HTTP header lines. */
        header_type _header;

        /** Returned status code of the HTTP response. */
        StatusCode _returncode;

        /** The data of the HTTP response. */
        Pt::Blob _body;

    public:
        /* HTTP status codes: 1xx Informational. */
        static const StatusCode CODE_CONTINUE;
        static const StatusCode CODE_SWITCHING_PROTOCOLS;
        static const StatusCode CODE_PROCESSING;

        /* HTTP status codes: 2xx Success. */
        static const StatusCode CODE_OK;
        static const StatusCode CODE_CREATED;
        static const StatusCode CODE_ACCEPTED;
        static const StatusCode CODE_NON_AUTHORITATIVE_INFORMATION;
        static const StatusCode CODE_NO_CONTENT;
        static const StatusCode CODE_RESET_CONTENT;
        static const StatusCode CODE_PARTIAL_CONTENT;
        static const StatusCode CODE_MULTI_STATUS;

        /* HTTP status codes: 3xx Redirection. */
        static const StatusCode CODE_MULTIPLE_CHOICES;
        static const StatusCode CODE_MOVED_PERMANENTLY;
        static const StatusCode CODE_FOUND;
        static const StatusCode CODE_SEE_OTHER;
        static const StatusCode CODE_NOT_MODIFIED;
        static const StatusCode CODE_USE_PROXY;
        static const StatusCode CODE_SWITCH_PROXY;
        static const StatusCode CODE_MOVED_TEMPORARILY;

        /* HTTP status codes: 4xx Client Error. */
        static const StatusCode CODE_BAD_REQUEST;
        static const StatusCode CODE_UNAUTHORIZED;
        static const StatusCode CODE_PAYMENT_REQUIRED;
        static const StatusCode CODE_FORBIDDEN;
        static const StatusCode CODE_NOT_FOUND;
        static const StatusCode CODE_METHOD_NOT_ALLOWED;
        static const StatusCode CODE_NOT_ACCEPTABLE;
        static const StatusCode CODE_PROXY_AUTHENTICATION_REQUIRED;
        static const StatusCode CODE_REQUEST_TIMEOUT;
        static const StatusCode CODE_CONFLICT;
        static const StatusCode CODE_GONE;
        static const StatusCode CODE_LENGTH_REQUIRED;
        static const StatusCode CODE_PRECONDITION_FAILED;
        static const StatusCode CODE_REQUEST_ENTITY_TOO_LARGE;
        static const StatusCode CODE_REQUEST_URI_TOO_LONG;
        static const StatusCode CODE_UNSUPPORTED_MEDIA_TYPE;
        static const StatusCode CODE_REQUESTED_RANGE_NOT_SATISFIABLE;
        static const StatusCode CODE_EXPECTATION_FAILED;
        static const StatusCode CODE_UNPROCESSABLE_ENTITY;
        static const StatusCode CODE_LOCKED;
        static const StatusCode CODE_FAILED_DEPENDENCY;
        static const StatusCode CODE_UNORDERED_COLLECTION;
        static const StatusCode CODE_UPGRADE_REQUIRED;
        static const StatusCode CODE_RETRY_WITH;

        /* HTTP status codes: 5xx Server Error. */
        static const StatusCode CODE_INTERNAL_SERVER_ERROR;
        static const StatusCode CODE_NOT_IMPLEMENTED;
        static const StatusCode CODE_BAD_GATEWAY;
        static const StatusCode CODE_SERVICE_UNAVAILABLE;
        static const StatusCode CODE_GATEWAY_TIMEOUT;
        static const StatusCode CODE_HTTP_VERSION_NOT_SUPPORTED;
        static const StatusCode CODE_INSUFFICIENT_STORAGE;
        static const StatusCode CODE_BANDWIDTH_LIMIT_EXCEEDED;

        /**
         * @brief Creates a HTTP response object that reveices the data of
         * the passed HTTP request.
         *
         * @param request The request from which we get the response.
         */
        HttpReply(HttpRequest& request);

        /**
         * @brief Creates a HTTP response object that uses the passed
         * connection for sending or receiving data.
         *
         * @param connection The TCP/IP connection to read from or write to.
         */
        HttpReply(TcpSocket& connection);

        /**
         * @brief Returns the HTTP version, used by the sender.
         *
         * @return The HTTP version string.
         */
        const std::string& getHttpVersion() const
        {
            return _httpVersion;
        }

        /**
         * @brief Sets the HTTP version to send.
         *
         * @param version The HTTP version that is send.
         */
        void setHttpVersion(const std::string& version)
        {
            _httpVersion = version;
        }

        /**
         * @brief Returns the response HTTP header data.
         *
         * @param name Name of the header (e.g. "Content-Length", "Content-Type")
         * @param default The default value if the header was not found.
         * @return The value of the responsed header name.
         */
        const std::string& getHeader(const std::string& name, const std::string& defaultValue) const
        {
            header_type::const_iterator it = _header.find(name);
            return it == _header.end() ? defaultValue : it->second;
        }

        /**
         * @brief Adds one HTTP header data to the response.
         * 
         * @param name Name of the header (e.g. "Content-Length", "Content-Type")
         * @return The value of the responsed header name.
         */
        void addHeader(const std::string& name, const std::string& value)
        {
            _header.insert(header_type::value_type(name, value));
        }

        /**
         * @brief Returns the HTTP response code (e.g. 200 [=Ok]).
         *
         * @return HTTP response code.
         */
        unsigned int getReturnCode() const
        {
            return _returncode.getCode();
        }

        /**
         * @brief Returns the HTTP response text (e.g. "Ok", "Page Not Found").
         *
         * @return HTTP response text.
         */
        const std::string& getReturnMessage() const
        {
            return _returncode.getMessage();
        };

        /**
         * @brief Sets a new HTTP status code for the response.
         *
         * @param returncode The HTTP status code to set.
         */
        void setReturnValue(const StatusCode& returncode)
        {
            _returncode = returncode;
        }

        /**
         * @brief Sets a new HTTP status code for the response.
         *
         * @param code The HTTP status code number to set.
         * @param message The HTTP status code message text to set.
         */
        void setReturnValue(const unsigned int code, const std::string& message)
        {
            setReturnValue(StatusCode(code, message));
        }

        /**
         * @brief Returns the body of the HTTP response.
         *
         * @param The body of the HTTP resonse as binary data.
         */
        Pt::Blob getBody() const
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
            _body.assign(body.m_data, body.m_length);
        }

        /**
         * @brief Sets the body data for a POST response.
         *
         * @param body The body data as string.
         */
        void setBody(const std::string& body)
        {
            _body.assign(body.data(), body.length());
        }

        /**
         * @brief Writes the data from the member variables into the
         * internal connection.
         */
        void send();

        /**
         * @brief Reads the data from the internal connection into the member
         * variables.
         */
        void receive();
    };

} // !namespace Net

} // !namespace Pt

#endif // Pt_Net_HttpReply_h
