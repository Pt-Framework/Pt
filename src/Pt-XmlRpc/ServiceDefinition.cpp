/*
 * Copyright (C) 2009-2013 by Dr. Marc Boris Duerner
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

#include <Pt/XmlRpc/ServiceDefinition.h>

namespace Pt {

namespace XmlRpc {

ServiceDefinition::ServiceDefinition()
{ 
}


ServiceDefinition::~ServiceDefinition()
{
    System::MutexLock lock(_mtx);

    ProcedureMap::iterator it;
    for(it = _procedures.begin(); it != _procedures.end(); ++it)
    {
        delete it->second;
    }
}


ServiceProcedure* ServiceDefinition::getProcedure(const std::string& name, Responder& resp)
{
    System::MutexLock lock(_mtx);

    ServiceProcedure* proc = 0;

    ProcedureMap::iterator it = _procedures.find( name );
    if( it != _procedures.end() )
    {
        proc = it->second->createProcedure(resp);
    }

    return proc;
}


void ServiceDefinition::releaseProcedure(ServiceProcedure* proc)
{
    delete proc;
}


void ServiceDefinition::registerProcedure(const std::string& name, ServiceProcedureDef* procDef)
{
    System::MutexLock lock(_mtx);

    ProcedureMap::iterator it = _procedures.find( name );
    if (it == _procedures.end())
    {
        std::pair<const std::string, ServiceProcedureDef*> p( name, procDef );
        _procedures.insert( p );
    }
    else
    {
        delete it->second;
        it->second = procDef;
    }
}

} // namespace XmlRpc

} // namespace Pt
