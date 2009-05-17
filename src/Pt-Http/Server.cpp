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

#include <Pt/Http/Server.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Socket.h>

namespace Pt {

namespace Http {

Server::Server(const std::string& ip, unsigned short int port)
: TcpServer(ip, port),
  _readTimeout(5000),
  _writeTimeout(5000),
  _keepAliveTimeout(30000)
{
    _selector.add(*this);
    Pt::connect(connectionPending, *this, &Server::onConnect);
}

void Server::addService(const std::string& url, Service& service)
{
    _service.insert(ServicesType::value_type(url, &service));
}

void Server::removeService(Service& service)
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

Responder* Server::getResponder(const Request& request)
{
    for (ServicesType::const_iterator it = _service.lower_bound(request.url());
        it != _service.end() && it->first == request.url(); ++it)
    {
        Responder* resp = it->second->createResponder(request);
        if (resp)
            return resp;
    }

    return _defaultService.createResponder(request);
}

void Server::onConnect(TcpServer& server)
{
    new Socket(_selector, *this);
}

} // namespace Http

} // namespace Pt
