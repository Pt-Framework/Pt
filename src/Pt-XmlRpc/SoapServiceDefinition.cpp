/*
 * Copyright (C) 2014 by Dr. Marc Boris Duerner
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

#include <Pt/XmlRpc/SoapServiceDefinition.h>

namespace Pt {

namespace XmlRpc {

///////////////////////////////////////////////////////////////////////////////
// IntegerType
///////////////////////////////////////////////////////////////////////////////

IntegerType::IntegerType()
: Type(Type::Int)
{ 
}


IntegerType::~IntegerType()
{
}

///////////////////////////////////////////////////////////////////////////////
// IntegerType
///////////////////////////////////////////////////////////////////////////////

StringType::StringType()
: Type(Type::String)
{ 
}


StringType::~StringType()
{
}

///////////////////////////////////////////////////////////////////////////////
// StructType
///////////////////////////////////////////////////////////////////////////////

StructType::StructType()
: Type(Type::Struct)
{ 
}


StructType::~StructType()
{

}


void StructType::addParameter(const std::string& name, Type& t)
{
    _params[name] = &t;

    Parameter param(name, t);
    _paramList.push_back(param);
}


const Parameter* StructType::getParameter(std::size_t n) const
{ 
    return n >= _paramList.size() ? 0 : &_paramList[n]; 
}


const Parameter* StructType::getParameter(const std::string& name) const
{ 
    ParameterList::const_iterator it;
    for(it = _paramList.begin(); it != _paramList.end(); ++it)
    {
        if(it->name() == name)
            return &*it;
    }
    
    return 0; 
}

///////////////////////////////////////////////////////////////////////////////
// StructType
///////////////////////////////////////////////////////////////////////////////

ArrayType::ArrayType()
: Type(Type::Array)
{ 
}


ArrayType::ArrayType(const std::string& name, Type& elem)
: Type(Type::Array)
, _elem(name, elem)
{
}


ArrayType::~ArrayType()
{
}


void ArrayType::setElement(const std::string& name, Type& elem)
{
    _elem.set(name, elem);
}


const Parameter* ArrayType::getParameter(std::size_t n) const
{ 
    return &_elem; 
}


const Parameter* ArrayType::getParameter(const std::string& name) const
{ 
    return &_elem; 
}

///////////////////////////////////////////////////////////////////////////////
// PortType
///////////////////////////////////////////////////////////////////////////////

PortType::PortType()
{ 
}


PortType::~PortType()
{

}


void PortType::addInput(const std::string& name, Type& t)
{
    Parameter param(name, t);
    _params.push_back(param);
}


const Parameter* PortType::getInput(const std::string& name) const
{ 
    ParameterList::const_iterator it;
    for(it = _params.begin(); it != _params.end(); ++it)
    {
        if(it->name() == name)
            return &*it;
    }
    
    return 0; 
}


void PortType::setOutput(const std::string& name, Type& type)
{
    _out.set(name, type);
}


const Parameter* PortType::getOutput() const
{
    return &_out;
}

///////////////////////////////////////////////////////////////////////////////
// SoapServiceDefinition
///////////////////////////////////////////////////////////////////////////////

SoapServiceDefinition::SoapServiceDefinition()
{ 
}


SoapServiceDefinition::~SoapServiceDefinition()
{
}


void SoapServiceDefinition::addPort(const std::string& name, PortType& procDef)
{
    System::MutexLock lock( mutex() );
    _procDefs[name] = &procDef;
}


const PortType* SoapServiceDefinition::getPort(const std::string& name) const
{
    System::MutexLock lock( mutex() );

    ProcedureDefinitionMap::const_iterator it = _procDefs.find( name );
    if( it == _procDefs.end() )
    {
        return 0;
    }

    return it->second;
}

} // namespace XmlRpc

} // namespace Pt
