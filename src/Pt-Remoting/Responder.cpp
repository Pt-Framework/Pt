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

#include <Pt/Remoting/Responder.h>
#include <Pt/Remoting/ServiceDefinition.h>
#include <Pt/Remoting/Fault.h>
#include <Pt/System/Logger.h>
#include <cassert>

PT_LOG_DEFINE("Pt.Remoting.Responder")

namespace Pt {

namespace Remoting {

Responder::Responder(ServiceDefinition& serviceDef)
: _serviceDef(&serviceDef)
, _proc(0)
{
}


Responder::~Responder()
{
    if(_proc)
        _serviceDef->releaseProcedure(_proc);
}


Pt::Composer** Responder::setProcedure(const std::string& name)
{
    if(_proc)
        _serviceDef->releaseProcedure(_proc);

    _proc = _serviceDef->getProcedure( name, *this );

   Composer** args = _proc ? _proc->beginArgs() : 0;
   
    return args;
}


void Responder::beginCall(System::EventLoop& loop)
{
    if( ! _proc )
    {
        throw Fault("invalid XML-RPC");
    }

    //if( _args && *_args )
    //{
    //    throw Fault("expected more arguments", 5);
    //}

    _proc->beginCall(loop); // throws Fault
}


Pt::Decomposer* Responder::endCall()
{
    if( ! _proc )
    {
        throw Fault("invalid XML-RPC");
    }

    return _proc->endCall(); // throws Fault
}


Pt::Decomposer* Responder::call()
{
    if( ! _proc )
    {
        throw Fault("invalid JSON-RPC");
    }

    return _proc->call();
}


void Responder::cancel()
{
    this->onCancel();
    
    if(_proc)
        _serviceDef->releaseProcedure(_proc);
    
    _proc = 0;
    //_args = 0;
}

} // namespace Remoting

} // namespace Pt
