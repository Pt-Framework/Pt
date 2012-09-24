/*
 * Copyright (C) 2012 Marc Boris Duerner
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

#include <Pt/Http/Service.h>
#include <Pt/Http/Server.h>
#include <Pt/Http/Responder.h>
#include <cassert>

namespace Pt {

namespace Http {

Service::Service()
: _shutdown(false)
, _responderCount(0)
{ 
}


Service::~Service() 
{
    assert( _servers.empty() );
}


Responder* Service::getResponder(const Request& request)
{
    System::MutexLock lock(_mutex);
    
    if(_shutdown)
        return 0;
    
    Responder* responder = createResponder(request);
    ++_responderCount;    
    
    return responder;
}


void Service::releaseResponder(Responder* responder)
{
    if( ! responder)
        return;

    System::MutexLock lock(_mutex);
    
    destroyResponder(responder);
    --_responderCount;
}


void Service::setShutdown(bool shutdown)
{
    System::MutexLock lock(_mutex);
    _shutdown = shutdown;
}


bool Service::isIdle()
{
    System::MutexLock lock(_mutex);
    return _responderCount == 0;
}


void Service::detach()
{
    while( ! _servers.empty() )
    {
        _servers[0]->removeService(*this);
    }
}


void Service::registerServer(Server& server)
{
    std::vector<Server*>::iterator it;
    for(it = _servers.begin(); it != _servers.end(); ++it)
    {
        if( *it == &server )
            break;
    }

    if( it == _servers.end() )
        _servers.push_back(&server);
}


void Service::unregisterServer(Server& server)
{
    std::vector<Server*>::iterator it;
    for(it = _servers.begin(); it != _servers.end(); ++it)
    {
        if(*it == &server)
            break;
    }

    if(it != _servers.end())
        _servers.erase(it);
}

}

}
