/*
 * Copyright (C) 2003-2007 Tommi Maekitalo
 * Copyright (C) 2007      Ulrich Termathe
 * Copyright (C) 2007      PTV AG
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
#include "Pt/Net/HttpReply.h"
#include "Pt/Net/HttpRequest.h"
#include <sstream>
#include <cctype>
#include <cstring> // memcpy/memmove()


namespace Pt {

namespace Net {

class HttpReply::Parser
{
private:
    typedef bool (Parser::*state_type)(char ch);

    /** HTTP reply that uses this parser. */
    HttpReply& _reply;

    /** Parser state. */
    state_type _state;

    /** Returned HTTP status code number. */
    unsigned int _returncode;

    /** Returned HTTP status code message text. */
    std::string _returnmessage;

    /** Saves the name of one HTTP header line while parsing. */
    std::string _name;

    /** Saves the value of one HTTP header line while parsing. */
    std::string _value;

    bool state_httpversion0(char ch);
    bool state_httpversion(char ch);
    bool state_returncode0(char ch);
    bool state_returncode(char ch);
    bool state_returnmessage0(char ch);
    bool state_returnmessage(char ch);
    bool state_request(char ch);
    bool state_name0(char ch);
    bool state_name(char ch);
    bool state_value0(char ch);
    bool state_value(char ch);
    bool state_valuee(char ch);

public:
    Parser(HttpReply& reply)
        : _reply(reply)
        , _state(&Parser::state_httpversion0)
        , _returncode(0)
    { }

    bool parse(char ch)
    {
        return (this->*_state)(ch);
    }
};

bool HttpReply::Parser::state_httpversion0(char ch)
{
    if (!std::isspace(ch))
    {
        _reply._httpVersion = ch;
        _state = &Parser::state_httpversion;
    }
    return false;
}

bool HttpReply::Parser::state_httpversion(char ch)
{
    if (std::isspace(ch))
    {
        _state = &Parser::state_returncode0;
    }
    else
    {
        _reply._httpVersion += ch;
    }
    return false;
}

bool HttpReply::Parser::state_returncode0(char ch)
{
    if (std::isdigit(ch))
    {
        _returncode = (ch - '0');
        _state = &Parser::state_returncode;
    }
    return false;
}

bool HttpReply::Parser::state_returncode(char ch)
{
    if (std::isdigit(ch))
    {
        _returncode = _returncode * 10 + (ch - '0');
    }
    else if (std::isspace(ch))
    {
        _reply.setReturnValue(_returncode, "");
        _state = &Parser::state_returnmessage0;
    }
    else
    {
        _reply.setReturnValue(_returncode, "");
        _state = &Parser::state_request;
    }
    return false;
}

bool HttpReply::Parser::state_returnmessage0(char ch)
{
    if (ch != '\n')
    {
        _returnmessage = ch;
        _state = &Parser::state_returnmessage;
    }
    return false;
}

bool HttpReply::Parser::state_returnmessage(char ch)
{
    if (ch == '\n')
    {
        _reply.setReturnValue(_returncode, _returnmessage);
        _state = &Parser::state_name0;
    }
    else if (ch == '\r')
    {
        _reply.setReturnValue(_returncode, _returnmessage);
        _state = &Parser::state_valuee;
    }
    else
    {
        _returnmessage += ch;
    }
    return false;
}

bool HttpReply::Parser::state_request(char ch)
{
    if (ch == '\n')
    {
        _state = &Parser::state_name0;
    }
    return false;
}

bool HttpReply::Parser::state_name0(char ch)
{
    if (ch == '\n')
    {
        return true;
    }

    if (!std::isspace(ch))
    {
        _name = ch;
        _state = &Parser::state_name;
    }
    return false;
}

bool HttpReply::Parser::state_name(char ch)
{
    if (ch == ':')
    {
        _state = &Parser::state_value0;
    }
    else
    {
        _name += ch;
    }
    return false;
}

bool HttpReply::Parser::state_value0(char ch)
{
    if (ch == '\n')
    {
        _reply._header.insert(header_type::value_type(_name, _value));
        _state = &Parser::state_name0;
    }

    if (!std::isspace(ch))
    {
        _value = ch;
        _state = &Parser::state_value;
    }

    return false;
}

bool HttpReply::Parser::state_value(char ch)
{
    if (ch == '\n')
    {
        _reply._header.insert(header_type::value_type(_name, _value));
        _state = &Parser::state_name0;
    }
    else if (ch == '\r')
    {
        _reply._header.insert(header_type::value_type(_name, _value));
        _state = &Parser::state_valuee;
    }
    else
    {
        _value += ch;
    }
    return false;
}

bool HttpReply::Parser::state_valuee(char ch)
{
    if (ch == '\n')
    {
        _state = &Parser::state_name0;
    }
    return false;
};

const std::string HttpReply::STR_CONTENT_LENGTH("Content-Length: ");
const std::string HttpReply::STR_END_OF_LINE("\r\n");
const std::string HttpReply::STR_DEFAULT_HTTP_VERSION("HTTP/1.1");

// 1xx Informational
const HttpReply::StatusCode HttpReply::CODE_CONTINUE(100, "Continue");
const HttpReply::StatusCode HttpReply::CODE_SWITCHING_PROTOCOLS(101, "Switching Protocols");
const HttpReply::StatusCode HttpReply::CODE_PROCESSING(102, "Processing");

// 2xx Success
const HttpReply::StatusCode HttpReply::CODE_OK(200, "OK");
const HttpReply::StatusCode HttpReply::CODE_CREATED(201, "created");
const HttpReply::StatusCode HttpReply::CODE_ACCEPTED(202, "accepted");
const HttpReply::StatusCode HttpReply::CODE_NON_AUTHORITATIVE_INFORMATION(203, "Non-Authoritative Information");
const HttpReply::StatusCode HttpReply::CODE_NO_CONTENT(204, "No Content");
const HttpReply::StatusCode HttpReply::CODE_RESET_CONTENT(205, "Reset Content");
const HttpReply::StatusCode HttpReply::CODE_PARTIAL_CONTENT(206, "Partial Content");
const HttpReply::StatusCode HttpReply::CODE_MULTI_STATUS(207, "Multi-Status");

// 3xx Redirection
const HttpReply::StatusCode HttpReply::CODE_MULTIPLE_CHOICES(300, "Multiple Choices");
const HttpReply::StatusCode HttpReply::CODE_MOVED_PERMANENTLY(301, "Moved Permanently");
const HttpReply::StatusCode HttpReply::CODE_FOUND(302, "Found");
const HttpReply::StatusCode HttpReply::CODE_SEE_OTHER(303, "See Other");
const HttpReply::StatusCode HttpReply::CODE_NOT_MODIFIED(304, "Not Modified");
const HttpReply::StatusCode HttpReply::CODE_USE_PROXY(305, "Use Proxy");
const HttpReply::StatusCode HttpReply::CODE_SWITCH_PROXY(306, "(Unused)");
const HttpReply::StatusCode HttpReply::CODE_MOVED_TEMPORARILY(307, "Moved Temporarily");

// 4xx Client Error
const HttpReply::StatusCode HttpReply::CODE_BAD_REQUEST(400, "Bad Request");
const HttpReply::StatusCode HttpReply::CODE_UNAUTHORIZED(401, "Unauthorized");
const HttpReply::StatusCode HttpReply::CODE_PAYMENT_REQUIRED(402, "Payment Required");
const HttpReply::StatusCode HttpReply::CODE_FORBIDDEN(403, "Forbidden");
const HttpReply::StatusCode HttpReply::CODE_NOT_FOUND(404, "Not Found");
const HttpReply::StatusCode HttpReply::CODE_METHOD_NOT_ALLOWED(405, "Method Not Allowed");
const HttpReply::StatusCode HttpReply::CODE_NOT_ACCEPTABLE(406, "Not Acceptable");
const HttpReply::StatusCode HttpReply::CODE_PROXY_AUTHENTICATION_REQUIRED(407, "Proxy Authentication Required");
const HttpReply::StatusCode HttpReply::CODE_REQUEST_TIMEOUT(408, "Request Timeout");
const HttpReply::StatusCode HttpReply::CODE_CONFLICT(409, "Conflict");
const HttpReply::StatusCode HttpReply::CODE_GONE(410, "Gone");
const HttpReply::StatusCode HttpReply::CODE_LENGTH_REQUIRED(411, "Length Required");
const HttpReply::StatusCode HttpReply::CODE_PRECONDITION_FAILED(412, "Precondition Failed");
const HttpReply::StatusCode HttpReply::CODE_REQUEST_ENTITY_TOO_LARGE(413, "Request Entity Too Large");
const HttpReply::StatusCode HttpReply::CODE_REQUEST_URI_TOO_LONG(414, "Request-URI Too Long");
const HttpReply::StatusCode HttpReply::CODE_UNSUPPORTED_MEDIA_TYPE(415, "Unsupported Media Type");
const HttpReply::StatusCode HttpReply::CODE_REQUESTED_RANGE_NOT_SATISFIABLE(416, "Requested Range Not Satisfiable");
const HttpReply::StatusCode HttpReply::CODE_EXPECTATION_FAILED(417, "Expectation Failed");
const HttpReply::StatusCode HttpReply::CODE_UNPROCESSABLE_ENTITY(422, "Unprocessable Entity");
const HttpReply::StatusCode HttpReply::CODE_LOCKED(423, "Locked");
const HttpReply::StatusCode HttpReply::CODE_FAILED_DEPENDENCY(424, "Failed Dependency");
const HttpReply::StatusCode HttpReply::CODE_UNORDERED_COLLECTION(425, "Unordered Collection");
const HttpReply::StatusCode HttpReply::CODE_UPGRADE_REQUIRED(426, "Upgrade Required");
const HttpReply::StatusCode HttpReply::CODE_RETRY_WITH(449, "Retry With");

// 5xx Server Error
const HttpReply::StatusCode HttpReply::CODE_INTERNAL_SERVER_ERROR(500, "Internal Server Error");
const HttpReply::StatusCode HttpReply::CODE_NOT_IMPLEMENTED(501, "Not Implemented");
const HttpReply::StatusCode HttpReply::CODE_BAD_GATEWAY(502, "Bad Gateway");
const HttpReply::StatusCode HttpReply::CODE_SERVICE_UNAVAILABLE(503, "Service Unavailable");
const HttpReply::StatusCode HttpReply::CODE_GATEWAY_TIMEOUT(504, "Gateway Timeout");
const HttpReply::StatusCode HttpReply::CODE_HTTP_VERSION_NOT_SUPPORTED(505, "HTTP Version Not Supported");
const HttpReply::StatusCode HttpReply::CODE_INSUFFICIENT_STORAGE(507, "Insufficient Storage");
const HttpReply::StatusCode HttpReply::CODE_BANDWIDTH_LIMIT_EXCEEDED(509, "Bandwidth Limit Exceeded");

HttpReply::HttpReply(HttpRequest& request)
    : _connection(request.getConnection())
    , _returncode(CODE_BAD_REQUEST)
//    , CODE_OK(200, "OK")
{
}

HttpReply::HttpReply(TcpSocket& connection)
    : _connection(connection)
    , _returncode(CODE_BAD_REQUEST)
//    , CODE_OK(200, "OK")
{
}

void HttpReply::receive()
{
    Parser parser(*this);
    size_t availableBytes = 0;
    char byte = 0;

    // read and parse HTTP header lines.
    if (1 == _connection.read(&byte, sizeof(byte)))
    {
        availableBytes = _connection.availableBytes();
        while ((!parser.parse(byte)) && (0 < availableBytes))
        {
            _connection.read(&byte, sizeof(byte));
            availableBytes--;
        }
    }

    std::string value = getHeader("Content-Length", "0");
    const size_t contentLength = std::atol( value.c_str() );

    _body.clear();
    _body.reserve( contentLength );

    if(contentLength <= 0 || availableBytes == 0)
        return;

    // read from the network as long as pending data are available.
    char buf[4096];
    size_t totalSize = 0;
    while ( 0 < _connection.availableBytes() )
    {
        size_t readBytes = _connection.read(buf, 4096);
        _body.append(buf, readBytes);
        totalSize += readBytes;
    }
}

void HttpReply::send()
{
    std::stringstream httpHeader;

    if (0 >= _httpVersion.length())
    {
        _httpVersion = STR_DEFAULT_HTTP_VERSION;
    }

    // assemble the HTTP header.
    httpHeader << _httpVersion << " " << _returncode.getCode();
    httpHeader << " " << _returncode.getMessage() << STR_END_OF_LINE;

    header_type::const_iterator header = _header.begin();
    while (header != _header.end())
    {
        if (0 != STR_CONTENT_LENGTH.compare(header->first))
        {
            httpHeader << header->first << "=" << header->second << STR_END_OF_LINE;;
        }
    }

    if (0 < _body.size())
    {
        httpHeader << STR_CONTENT_LENGTH << _body.size() << STR_END_OF_LINE;
    }

    httpHeader << STR_END_OF_LINE << std::flush;

    // allocate buffer.
    const size_t bufferSize = httpHeader.str().length() + _body.size();
    char* buffer = new char[bufferSize];

    // copy the HTTP header.
    char* headerData = buffer;
    std::memcpy(headerData, httpHeader.str().data(), httpHeader.str().length());

    // copy the body data.
    char* bodyData = headerData + httpHeader.str().length();
    std::memcpy(bodyData, _body.data(), _body.size());

    // send the HTTP reply.
    _connection.write(buffer, bufferSize);

    delete[] buffer;
}

} // namespace Net

} // namespace Pt
