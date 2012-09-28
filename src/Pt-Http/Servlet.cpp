/*
 * Copyright (C) 2011 by Marc Boris Duerner
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

#include <Pt/Http/Servlet.h>
#include <Pt/Http/Server.h>
#include <Pt/Http/Authentication.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>

namespace Pt {

namespace Http {

class MapUrl2 : public Servlet2::Mapping
{
    public:
        MapUrl2(const std::string& url)
        : _url(url)
        {}

        bool map(const Request& request)
        { return request.url() == _url; }

    private:
        std::string _url;
};

Servlet2::Servlet2(const std::string& url, Service& s)
: _server(0)
, _mapping(0)
, _service(0)
, _auth(0)
, _useCount(0)
{
    _mapping = new MapUrl2(url);
}


Servlet2::~Servlet2()
{
    delete _mapping;

    detach();
}


void setShutdown(bool shutdown)
{
    // if(_server)
    // {
    //     _server->setServletShutdown(*this);
    // } 
}


bool Servlet2::isIdle()
{ 
    // if(_server)
    //     return _server->isServletIdle(*this);

    return false; 
}


void Servlet2::detach()
{ 
    // if(_server)
    //     _server->removeServlet(*this);
}


void Servlet2::registerServer(Server& server)
{
    _server = &server;
}
        

void Servlet2::unregisterServer(Server& server)
{
    _server = 0;
}

} // namespace Http

} // namespace Pt
