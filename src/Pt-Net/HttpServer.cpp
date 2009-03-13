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

namespace Pt {

namespace Net {

/*
std::size_t HttpXmlRpcResponder::advance(std::istream& in)
{
    return _xmlRpcHandler.advance(in);
}

void HttpXmlRpcResponder::finish(std::ostream& out)
{
    std::ostringstream result;
    _xmlRpcHandler.finish(result);
    out << "HTTP/1.1 200 OK\r\n"
           "Connection: close\r\n"
           "Content-Size: " << result.str().size() <<
           "Content-Type: text/xml\r\n"
           "Server: Pt-Net Http server\r\n\r\n"
        << result.str();
}
*/

std::size_t HttpNotFoundResponder::advance(std::istream& in)
{
    std::size_t ret = in.rdbuf()->in_avail();
    in.ignore(ret);
    return ret;
}

void HttpNotFoundResponder::finish(std::ostream& out)
{
    out << "HTTP/1.1 404 Not found\r\n"
           "Connection: close\r\n"
           "Content-Size: 0\r\n"
           "Server: Pt-Net Http server\r\n\r\n";
}

HttpResponder* HttpNotFoundService::createResponder()
{
    return &_responder;
}

void HttpNotFoundService::releaseResponder(HttpResponder*)
{ }

HttpSocket::HttpSocket(System::SelectorBase& selector, HttpServer& server)
    : TcpSocket(server),
      _server(server),
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
        // TODO http parser
        while ( sb.in_avail() > 0 )
        {
            sb.sbumpc();
        }

        std::string url = "/"; // TODO
        bool ready = true; // TODO
        if (ready)
        {
            _contentSize = 0; // TODO
            HttpService* service = _server.getService(url);
            _responder = service->createResponder();

            if (_contentSize == 0)
            {
                _responder->finish(_stream);
                _responder->release();
                onOutput(sb);
                return;
            }

            _readHeader = false;
        }
    }

    if (!_readHeader)
    {
        if (sb.in_avail() > 0)
            _contentSize -= _responder->advance(_stream);

        if (_contentSize <= 0)
        {
            _responder->finish(_stream);
            _responder->release();
            onOutput(sb);
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

void HttpServer::addService(const std::string& url, HttpService& resp)
{
    _service[url] = &resp;
}

HttpService* HttpServer::getService(const std::string& url)
{
    ServicesType::iterator it = _service.find(url);
    return it == _service.end() ? &_defaultService : it->second;
}

void HttpServer::onConnect(TcpServer& server)
{
    new HttpSocket(_selector, *this);
}

} // namespace Net

} // namespace Pt
