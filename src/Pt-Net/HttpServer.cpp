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

#include <Pt/Net/HttpServer.h>
#include <Pt/System/Selector.h>
#include <cassert>

namespace Pt {

namespace Net {

void HttpResponder::beginRequest(std::istream& in, HttpRequest& request)
{
}

std::size_t HttpResponder::readBody(std::istream& in)
{
    std::streambuf* sb = in.rdbuf();

    std::size_t ret = 0;
    while (sb->in_avail() > 0)
    {
        sb->sbumpc();
        ++ret;
    }

    return ret;
}

void HttpNotFoundResponder::reply(std::ostream& out, HttpRequest& request, HttpReply& reply)
{
    reply.httpReturn(404, "Not found");
}

HttpResponder* HttpNotFoundService::createResponder(const HttpRequest&)
{
    return &_responder;
}

void HttpNotFoundService::releaseResponder(HttpResponder*)
{ }

void HttpSocket::ParseEvent::onMethod(const std::string& method)
{
    _request.method(method);
}

void HttpSocket::ParseEvent::onUrl(const std::string& url)
{
    _request.url(url);
}

void HttpSocket::ParseEvent::onUrlParam(const std::string& q)
{
    _request.qparams(q);
}

HttpSocket::HttpSocket(System::SelectorBase& selector, HttpServer& server)
    : TcpSocket(server),
      _server(server),
      _parseEvent(_request),
      _parser(_parseEvent, false),
      _readHeader(true)
{
    _stream.attachDevice(*this);
    _stream.buffer().beginRead();
    Pt::connect(_stream.buffer().inputReady, *this, &HttpSocket::onInput);
    Pt::connect(_stream.buffer().outputReady, *this, &HttpSocket::onOutput);

    selector.add(*this);

    _timer.start(_server.readTimeout());
    selector.add(_timer);
}

void HttpSocket::onInput(System::StreamBuffer& sb)
{
    _timer.start(_server.readTimeout());
    if ( _readHeader )
    {
        _parser.advance(sb);

        if (_parser.fail())
            throw std::runtime_error("http parser failed"); // TODO define exception class

        if (_parser.end())
        {
            _responder = _server.getResponder(_request);
            _responder->beginRequest(_stream, _request);

            _contentSize = _request.header().contentSize();
            if (_contentSize == 0)
            {
                _responder->reply(_reply.body(), _request, _reply);
                _responder->release();

                sendReply();

                onOutput(sb);
                return;
            }

            _readHeader = false;
        }
        else
        {
            sb.beginRead();
        }
    }

    if (!_readHeader)
    {
        if (sb.in_avail() > 0)
        {
            std::size_t s = _responder->readBody(_stream);
            assert(s > 0);
            _contentSize -= s;
        }

        if (_contentSize <= 0)
        {
            _responder->reply(_reply.body(), _request, _reply);
            _responder->release();

            sendReply();

            onOutput(sb);
        }
        else
        {
            sb.beginRead();
        }
    }
}

void HttpSocket::onOutput(System::StreamBuffer& sb)
{
    bool keepAlive = false;  // TODO

    sb.beginWrite();

    if ( sb.out_avail() )
    {
        _timer.start(_server.writeTimeout());
    }
    else if (keepAlive)
    {
        _timer.start(_server.keepAliveTimeout());
        _readHeader = true;
        _request.clear();
        _reply.clear();
    }
    else
    {
        close();
        delete this;
    }
}

void HttpSocket::onTimeout()
{
     close();
     delete this;
}

void HttpSocket::sendReply()
{
    const std::string contentSize = "Content-Size";
    const std::string server = "Server";
    const std::string connection = "Connection";
    const std::string date = "Date";

    _stream << "HTTP/"
        << _reply.header().httpVersionMajor() << '.'
        << _reply.header().httpVersionMinor() << ' '
        << _reply.header().httpReturnCode() << ' '
        << _reply.header().httpReturnText() << "\r\n";

    for (HttpReplyHeader::const_iterator it = _reply.header().begin();
        it != _reply.header().end(); ++it)
    {
        _stream << it->first << ": " << it->second << "\r\n";
    }

    if (!_reply.header().hasHeader(contentSize))
    {
        _stream << "Content-Size: " << _reply.bodySize() << "\r\n";
    }

    if (!_reply.header().hasHeader(server))
    {
        _stream << "Server: Pt-Net-HttpServer\r\n";
    }

    if (!_reply.header().hasHeader(connection))
    {
        _stream << "Connection: "
                << (_request.header().keepAlive() ? "keep-alive" : "close")
                << "\r\n";
    }

    if (!_reply.header().hasHeader(date))
    {
        _stream << "Date: " << HttpMessageHeader::htdateCurrent() << "\r\n";
    }

    _stream << "\r\n";

    _reply.sendBody(_stream);

}

HttpServer::HttpServer(System::SelectorBase& selector, const std::string& ip, unsigned short int port)
: TcpServer(ip, port),
  _selector(selector),
  _readTimeout(5000),
  _writeTimeout(5000),
  _keepAliveTimeout(30000)
{
    _selector.add(*this);
    Pt::connect(connectionPending, *this, &HttpServer::onConnect);
}

void HttpServer::addService(const std::string& url, HttpService& service)
{
    _service.insert(ServicesType::value_type(url, &service));
}

void HttpServer::removeService(HttpService& service)
{
    ServicesType::iterator it = _service.begin();
    while (it != _service.end())
    {
        if (it->second == &service)
        {
            _service.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

HttpResponder* HttpServer::getResponder(const HttpRequest& request)
{
    for (ServicesType::const_iterator it = _service.lower_bound(request.url());
        it != _service.end() && it->first == request.url(); ++it)
    {
        HttpResponder* resp = it->second->createResponder(request);
        if (resp)
            return resp;
    }

    return _defaultService.createResponder(request);
}

void HttpServer::onConnect(TcpServer& server)
{
    new HttpSocket(_selector, *this);
}

} // namespace Net

} // namespace Pt
