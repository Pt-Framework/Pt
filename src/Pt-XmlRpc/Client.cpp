/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 * Copyright (C) 2009 by Tommi Meakitalo
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

#include "Pt/XmlRpc/Client.h"
#include "Pt/XmlRpc/RemoteProcedure.h"
#include "ClientImpl.h"

namespace Pt {

namespace XmlRpc {

Client::Client()
: _impl(0)
{
    _impl = new ClientImpl();
}

Client::~Client()
{
    delete _impl;
}


SerializationContext& Client::context()
{
    return _ctx;
}


void Client::beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _impl->beginCall(r, method, argv, argc);

    this->onInvoke();
}


void Client::endCall()
{
    if( _impl->isError() )
    {
        _impl->setError(false);
        onError();
    }

    if( _impl->isFault() )
    {
        _impl->throwFault();
    }
}


void Client::call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _impl->beginCall(r, method, argv, argc);

    this->onCall();
}


void Client::cancel()
{
    _impl->cancel();

    this->onCancel();
}


const IRemoteProcedure* Client::activeProcedure() const
{
    return _impl->activeProcedure();
}


bool Client::isFailed() const
{
    return _impl->isError() || _impl->isFault();
}


void Client::beginMessage(std::ostream& os)
{
    _impl->beginMessage(os);
}


bool Client::advanceMessage()
{
    return _impl->advanceMessage();
}


void Client::finishMessage()
{
    _impl->finishMessage();
}


void Client::beginResult(std::istream& is)
{
    _impl->beginResult(is);
}


bool Client::parseResult()
{
    return _impl->parseResult();
}


void Client::setFault(int rc, const char* msg)
{
    _impl->setFault(rc, msg);
}


void Client::setError()
{
    _impl->setError();
}


void Client::finishResult()
{
    if( _impl->activeProcedure() )
    {
        _impl->finishResult();
    }
    else if( _impl->isError() )
    {
        _impl->setError(false);
        onError();
    }
}


void Client::processResult(std::istream& is)
{
    _impl->processResult(is);
}

} // namespace XmlRpc

} // namespace Pt
