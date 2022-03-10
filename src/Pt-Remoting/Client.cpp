/*
 * Copyright (C) 2009-2014 by Dr. Marc Boris Duerner
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

#include <Pt/Remoting/Client.h>
#include <Pt/Remoting/RemoteProcedure.h>
#include <Pt/System/Logger.h>
#include <cassert>

PT_LOG_DEFINE("Pt.Remoting.Client")

namespace Pt {

namespace Remoting {

Client::Client()
: _method(0)
{
}


Client::~Client()
{
}


void Client::beginCall(Composer& r, RemoteCall& method, Decomposer** argv, unsigned argc)
{
    try
    {
        _method = &method;
        this->onBeginCall(r, method, argv, argc);
    }
    catch(...)
    {
        _method = 0;
        throw;
    }
}


void Client::call(Composer& r, RemoteCall& method, Decomposer** argv, unsigned argc)
{
    try
    {
        _method = &method;
        this->onCall(r, method, argv, argc);
        _method = 0;
    }
    catch(...)
    {
        _method = 0;
        throw;
    }
}


void Client::endCall()
{
    try
    {
        this->onEndCall();
        _method = 0;
    }
    catch(...)
    {
        _method = 0;
        throw;
    }
}


void Client::cancelCall()
{
    this->onCancel();
    _method = 0;
}


void Client::cancel()
{
    if(_method)
        _method->cancel();
    else
        cancelCall();

    _method = 0;
}


void Client::setReady()
{
    assert( _method );

    if( _method )
        _method->setReady();
}

} // namespace Remoting

} // namespace Pt
