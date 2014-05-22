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

// SOAP includes
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Convert.h>
#include <iterator>

namespace Pt {

namespace XmlRpc {

///////////////////////////////////////////////////////////////////////////////
// Type
///////////////////////////////////////////////////////////////////////////////

Type::Type()
: _parent(0)
{ 
}


Type::~Type()
{
}

///////////////////////////////////////////////////////////////////////////////
// IntegerType
///////////////////////////////////////////////////////////////////////////////

IntegerType::IntegerType()
{ 
}


IntegerType::~IntegerType()
{
}


const Type* IntegerType::parse(const Xml::Node& node, Composer*& composer) const
{
    const Xml::EndElement* ee = Xml::toEndElement(&node);
    if(ee)
    {
        composer = composer->finish();
        return parent();
    }

    const Xml::StartElement* se = Xml::toStartElement(&node);
    if(se)
        return this;

    const Xml::Characters* c = Xml::toCharacters(&node);
    if(c)
    {
        Pt::int32_t number = 0;
        bool ok = false;
        parseInt( c->content().begin(), c->content().end(), number, ok);

        if( ! ok )
            throw std::runtime_error("invalid integer parameter");

        composer->setInt(number);
    }

    return this; 
}

///////////////////////////////////////////////////////////////////////////////
// IntegerType
///////////////////////////////////////////////////////////////////////////////

StringType::StringType()
{ 
}


StringType::~StringType()
{
}


const Type* StringType::parse(const Xml::Node& node, Composer*& composer) const
{
    const Xml::EndElement* ee = Xml::toEndElement(&node);
    if(ee)
    {
        composer = composer->finish();
        return parent();
    }

    const Xml::StartElement* se = Xml::toStartElement(&node);
    if(se)
        return this;

    const Xml::Characters* c = Xml::toCharacters(&node);
    if(c)
    {
        composer->setString( c->content() );
    }

    return this; 
}

///////////////////////////////////////////////////////////////////////////////
// StructType
///////////////////////////////////////////////////////////////////////////////

StructType::StructType()
{ 
}


StructType::~StructType()
{

}


void StructType::addParameter(const std::string& name, Type& param)
{
    _params[name] = &param;
    param.setParent(this);
}


const Type* StructType::parse(const Xml::Node& node, Composer*& composer) const
{
    const Xml::EndElement* ee = Xml::toEndElement(&node);
    if(ee)
    {
        composer = composer->finish();
        return parent();
    }

    const Xml::StartElement* se = Xml::toStartElement(&node);
    if( ! se)
        return this;

    ParameterMap::const_iterator it;
    it = _params.find( se->name().local().narrow() );
    if( it == _params.end() )
        throw std::runtime_error("invalid struct parameter");

    composer = composer->beginMember(it->first);
    return it->second;
}

///////////////////////////////////////////////////////////////////////////////
// StructType
///////////////////////////////////////////////////////////////////////////////

ArrayType::ArrayType()
{ 
}


ArrayType::ArrayType(const std::string& name, Type& elem)
: _elem(name, elem)
{ 
    _elem.set(name, elem);
    _elem.setParent(this);
}


ArrayType::~ArrayType()
{
}


void ArrayType::setElement(const std::string& name, Type& elem)
{
    _elem.set(name, elem);
    _elem.setParent(this);
}


const Type* ArrayType::parse(const Xml::Node& node, Composer*& composer) const
{
    const Xml::EndElement* ee = Xml::toEndElement(&node);
    if(ee)
    {
        composer = composer->finish();
        return parent();
    }

    const Xml::StartElement* se = Xml::toStartElement(&node);
    if( ! se)
        return this;

    composer = composer->beginElement();
    return _elem.type();
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


void PortType::setOutput(const std::string& name, Type& type)
{
    _out.set(name, type);
}


const Parameter* PortType::getOutput() const
{
    return &_out;
}


void PortType::addInput(const std::string& name, Type& param)
{
    _params[name] = &param;
}


const Type* PortType::getInput(const std::string& name) const
{ 
    ParameterMap::const_iterator it;
    it = _params.find(name);
    if(it == _params.end())
        throw std::runtime_error("invalid parameter");
        
    return it->second;
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
