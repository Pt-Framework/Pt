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

#include <sstream>
#include <cctype>

#include <Pt/Net/HttpRequest.h>

namespace Pt {
namespace Net {

const std::string HttpRequest::STR_CONTENT_LENGTH("Content-Length: ");
const std::string HttpRequest::STR_END_OF_LINE("\r\n");
const std::string HttpRequest::STR_GET("GET");
const std::string HttpRequest::STR_HOST("Host: ");
const std::string HttpRequest::STR_HTTP_VERSION(" HTTP/1.0");
const std::string HttpRequest::STR_POST("POST");

class HttpRequest::Parser
{
private:
    typedef bool (Parser::*state_type)(char ch);

    /** HTTP request that uses this parser. */
    HttpRequest& _request;

    /** Parser state. */
    state_type _state;

    /** Saves the name of one HTTP header line while parsing. */
    std::string _name;

    /** Saves the value of one HTTP header line while parsing. */
    std::string _value;

    bool state_requestmethod0(char ch);
    bool state_requestmethod(char ch);
    bool state_url0(char ch);
    bool state_url(char ch);
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
    Parser(HttpRequest& request)
        : _request(request)
        , _state(&Parser::state_requestmethod0)
    { }

    bool parse(char ch)
    {
        return (this->*_state)(ch);
    }
};

bool HttpRequest::Parser::state_requestmethod0(char ch)
{
    if (!std::isspace(ch))
    {
        _name = ch;
        _state = &Parser::state_requestmethod;
    }
    return false;
}

bool HttpRequest::Parser::state_requestmethod(char ch)
{
    if (std::isspace(ch))
    {
        if (0 == _name.compare(HttpRequest::STR_GET))
        {
            _request.setMethod(HttpRequest::GET);
        }
        else if (0 == _name.compare(HttpRequest::STR_POST))
        {
            _request.setMethod(HttpRequest::POST);
        }
        _name.empty();
        _state = &Parser::state_url0;
    }
    else
    {
        _name += ch;
    }
    return false;
}

bool HttpRequest::Parser::state_url0(char ch)
{
    if (!std::isspace(ch))
    {
        _request._url = ch;
        _state = &Parser::state_url;
    }
    return false;
}

bool HttpRequest::Parser::state_url(char ch)
{
    if (std::isspace(ch))
    {
        _state = &Parser::state_httpversion0;
    }
    else
    {
        _request._url += ch;
    }
    return false;
}

bool HttpRequest::Parser::state_httpversion0(char ch)
{
    if (ch != '\n')
    {
        _request._httpVersion = ch;
        _state = &Parser::state_httpversion;
    }
    return false;
}

bool HttpRequest::Parser::state_httpversion(char ch)
{
    if (ch == '\n')
    {
        _state = &Parser::state_name0;
    }
    else if (ch == '\r')
    {
        _state = &Parser::state_valuee;
    }
    else
    {
        _request._httpVersion += ch;
    }
    return false;
}

bool HttpRequest::Parser::state_request(char ch)
{
    if (ch == '\n')
    {
        _state = &Parser::state_name0;
    }
    return false;
}

bool HttpRequest::Parser::state_name0(char ch)
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

bool HttpRequest::Parser::state_name(char ch)
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

bool HttpRequest::Parser::state_value0(char ch)
{
    if (ch == '\n')
    {
        _request._header.insert(header_type::value_type(_name, _value));
        _state = &Parser::state_name0;
    }

    if (!std::isspace(ch))
    {
        _value = ch;
        _state = &Parser::state_value;
    }

    return false;
}

bool HttpRequest::Parser::state_value(char ch)
{
    if (ch == '\n')
    {
        _request._header.insert(header_type::value_type(_name, _value));
        _state = &Parser::state_name0;
    }
    else if (ch == '\r')
    {
        _request._header.insert(header_type::value_type(_name, _value));
        _state = &Parser::state_valuee;
    }
    else
    {
        _value += ch;
    }
    return false;
}

bool HttpRequest::Parser::state_valuee(char ch)
{
    if (ch == '\n')
    {
        _state = &Parser::state_name0;
    }
    return false;
}

HttpRequest::HttpRequest(const std::string& url, request_type method)
    : _method(method), _port(80)
{
    std::string::size_type pos = 0;
    if (url.compare(0, 7, "http://") == 0)
    {
        pos = 7;
    }

    std::string::size_type e = url.find(':', pos);
    if (e != std::string::npos)
    {
        _host = url.substr(pos, e - pos);
        _port = 0;
        for (++e; e < url.size() && url.at(e) != '/'; ++e)
        {
            if (!std::isdigit(url.at(e)))
            {
                throw std::runtime_error("invalid url \"" + url + '"');
            }
            _port = _port * 10 + (url.at(e) - '0');
        }
        if (e >= url.size())
        {
            throw std::runtime_error("invalid url \"" + url + '"');
        }
    }
    else
    {
        e = url.find('/', pos);
        if (e == std::string::npos)
        {
            throw std::runtime_error("invalid url \"" + url + '"');
        }
        _host = url.substr(pos, e - pos);
    }

    _url = url.substr(e);
}

void HttpRequest::send()
{
	send(_connection);
}

void HttpRequest::send(Pt::Net::TcpSocket& connection)
{
    connection.connect(_host, _port);
    connection.setTimeout(0);

    switch (_method)
    {
    case GET:
        sendGetMethod(connection);
        break;

    case POST:
        sendPostMethod(connection);
        break;
    }
}

void HttpRequest::receive(Pt::Net::TcpSocket& connection)
{
    Parser parser(*this);
    size_t availableBytes = 0;
    char byte = 0;

    // read and parse HTTP header lines.
    if (1 == connection.read(&byte, sizeof(byte)))
    {
        availableBytes = connection.availableBytes();
        while ((!parser.parse(byte)) && (0 < availableBytes))
        {
            connection.read(&byte, sizeof(byte));
            availableBytes--;
        }
    }

    // read binary data.
    if (0 < availableBytes)
    {
        // read from the network as long as pending data are available.
        std::vector<Pt::Blob> singleBuffers;
        size_t totalBufferSize = 0;
        while (0 < (availableBytes = connection.availableBytes()))
        {
            char* buffer = new char[availableBytes];
            connection.read(buffer, availableBytes);
            singleBuffers.push_back(Pt::Blob(buffer, availableBytes));
            totalBufferSize += availableBytes;
        }

        // copy the single input buffers into one total buffer.
        Pt::Blob totalBuffer(new char[totalBufferSize], totalBufferSize);
        const char* currentBuffer = totalBuffer.m_data;
        std::vector<Pt::Blob>::iterator singleBuffer = singleBuffers.begin();
        while (singleBuffer != singleBuffers.end())
        {
            memcpy((void*) currentBuffer, (*singleBuffer).m_data, (*singleBuffer).m_length);
            currentBuffer += (*singleBuffer).m_length;
            singleBuffer++;
        }
        singleBuffers.clear();

        // the total buffer is the read body data.
        _body = totalBuffer;
    }
}

void HttpRequest::sendGetMethod(Pt::Net::TcpSocket& connection)
{
    std::stringstream httpHeader;

    // assemble the HTTP header.
    httpHeader << STR_GET << " ";
    if (_url.size() == 0 || _url.at(0) != '/')
    {
        httpHeader << "/";
    }
    httpHeader << _url;

    if (!_params.empty())
    {
        httpHeader << "?" << _params.getUrl();
    }

    httpHeader << STR_HTTP_VERSION << STR_END_OF_LINE;
    httpHeader << STR_HOST << _host << STR_END_OF_LINE;
    httpHeader << STR_END_OF_LINE << std::flush;

    // send the HTTP request.
    connection.write(httpHeader.str().data(), httpHeader.str().length());
}

void HttpRequest::sendPostMethod(Pt::Net::TcpSocket& connection)
{
    std::stringstream httpHeader;

    // assemble the HTTP header.
    httpHeader << STR_POST << " ";
    if (_url.size() == 0 || _url.at(0) != '/')
    {
        httpHeader << "/";
    }
    httpHeader << _url;
    httpHeader << STR_HTTP_VERSION << STR_END_OF_LINE;
    httpHeader << STR_HOST << _host << STR_END_OF_LINE;
    httpHeader << STR_CONTENT_LENGTH << _body.m_length << STR_END_OF_LINE;
    httpHeader << STR_END_OF_LINE << std::flush;

    // allocate buffer.
    const size_t bufferSize = httpHeader.str().length() + _body.m_length;
    char* buffer = new char[bufferSize];

    // copy the HTTP header.
    char* header = buffer;
    memcpy(header, httpHeader.str().data(), httpHeader.str().length());

    // copy the body data.
    char* body = header + httpHeader.str().length();
    memcpy(body, _body.m_data, _body.m_length);

    // send the HTTP request.
    connection.write(buffer, bufferSize);

    delete[] buffer;
}

} // namespace Net

} // namespace Pt
