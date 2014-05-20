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
// ParameterDefinition
///////////////////////////////////////////////////////////////////////////////

ParameterDefinition::ParameterDefinition()
: _parent(0)
{ 
}


ParameterDefinition::~ParameterDefinition()
{
}

///////////////////////////////////////////////////////////////////////////////
// IntegerParameter
///////////////////////////////////////////////////////////////////////////////

IntegerParameter::IntegerParameter()
{ 
}


IntegerParameter::~IntegerParameter()
{
}


const ParameterDefinition* IntegerParameter::parse(const Xml::Node& node, Composer*& composer) const
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
// StructParameter
///////////////////////////////////////////////////////////////////////////////

StructParameter::StructParameter()
{ 
}


StructParameter::~StructParameter()
{
    ParameterMap::iterator it;
    for(it = _params.begin(); it != _params.end(); ++it)
    {
        delete it->second;
    }
}


void StructParameter::addParameter(const std::string& name, ParameterDefinition* param)
{
    ParameterMap::iterator it = _params.find( name );
    if( it == _params.end() )
    {
        std::pair<const std::string, ParameterDefinition*> p( name, param );
        _params.insert( p );
    }
    else
    {
        delete it->second;
        it->second = param;
    }

    param->setParent(this);
}


const ParameterDefinition* StructParameter::parse(const Xml::Node& node, Composer*& composer) const
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
// StructParameter
///////////////////////////////////////////////////////////////////////////////

ArrayParameter::ArrayParameter(ParameterDefinition* elem)
: _elem(elem)
{ 
    _elem->setParent(this);
}


ArrayParameter::~ArrayParameter()
{
    delete _elem;
}


void ArrayParameter::setElement(ParameterDefinition* param)
{
    delete _elem;
    _elem = param;
    _elem->setParent(this);
}


const ParameterDefinition* ArrayParameter::parse(const Xml::Node& node, Composer*& composer) const
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
    return _elem;
}

///////////////////////////////////////////////////////////////////////////////
// ProcedureDefinition
///////////////////////////////////////////////////////////////////////////////

ProcedureDefinition::ProcedureDefinition()
{ 
}


ProcedureDefinition::~ProcedureDefinition()
{
    ParameterMap::iterator it;
    for(it = _params.begin(); it != _params.end(); ++it)
    {
        delete it->second;
    }
}


void ProcedureDefinition::addParameter(const std::string& name, ParameterDefinition* param)
{
    ParameterMap::iterator it = _params.find( name );
    if( it == _params.end() )
    {
        std::pair<const std::string, ParameterDefinition*> p( name, param );
        _params.insert( p );
    }
    else
    {
        delete it->second;
        it->second = param;
    }
}


const ParameterDefinition* ProcedureDefinition::parse(Xml::Node& node, const std::string& procName)
{ 
    Xml::EndElement* endElem = Xml::toEndElement(&node);
    if(endElem && endElem->name().local() == procName.c_str())
    {
        return 0;
    }

    Xml::StartElement* startElem = Xml::toStartElement(&node);
    if(startElem)
    {
        ParameterMap::iterator it;
        it = _params.find( startElem->name().local().narrow() );
        if(it == _params.end())
            throw std::runtime_error("invalid parameter");
        
        return it->second;
    }

    return 0; 
}

const ParameterDefinition* ProcedureDefinition::getParameter(const std::string& name) const
{ 
    ParameterMap::const_iterator it;
    it = _params.find(name);
    if(it == _params.end())
        throw std::runtime_error("invalid parameter");
        
    return it->second;
}

///////////////////////////////////////////////////////////////////////////////
// Position
///////////////////////////////////////////////////////////////////////////////

struct Position
{
    Position()
    : longitude(0), latitude(0)
    {}
    
    int longitude;
    int latitude;
};


void operator >>=(const SerializationInfo& si, Position& p)
{
    si.getMember("longitude") >>= p.longitude;
    si.getMember("latitude") >>= p.latitude;
}


void operator <<=(SerializationInfo& si, const Position& p)
{
    si.addMember("longitude") <<= p.longitude;
    si.addMember("latitude") <<= p.latitude;
}

///////////////////////////////////////////////////////////////////////////////
// testSoap
///////////////////////////////////////////////////////////////////////////////

void testSoap()
{
    ProcedureDefinition proc;
    
    StructParameter* p1 = new StructParameter;
    p1->addParameter("longitude", new IntegerParameter);
    p1->addParameter("latitude", new IntegerParameter);
    proc.addParameter("position", p1);
    
    ArrayParameter* p2 = new ArrayParameter( new IntegerParameter );
    proc.addParameter("numbers", p2);

    Position position;
    BasicComposer<Position> a1;
    a1.begin(position);

    std::vector<int> numbers;
    BasicComposer< std::vector<int> > a2;
    a2.begin(numbers);

    std::stringstream iss;
    iss << "<myMethod>"
           "  <position>"
           "    <longitude>10</longitude>"
           "    <latitude>20</latitude>"
           "  </position>"
           "  <numbers>"
           "    <number>1</number>"
           "    <number>2</number>"
           "    <number>3</number>"
           "  </numbers>"
           "</myMethod>";

    Pt::Xml::BinaryInputSource is(iss);
    Pt::Xml::XmlReader reader(is);

    Pt::Xml::InputIterator it = reader.current();

    for(it = reader.current(); it != reader.end(); ++it)
    {
        Xml::StartElement* startElem = Xml::toStartElement(&*it);
        if(startElem)
        {
            if( startElem->name().local() != "myMethod" )
                throw std::runtime_error("invalid procedure: myMethod" );

            ++it;
            break;
        }
    }
    
    Composer* composer = &a1;
    int paramCount = 0;
    for(it = reader.current(); it != reader.end(); ++it)
    {
        const ParameterDefinition* param = proc.parse(*it, "myMethod");
        if(param)
        {
            ++it;

            for(it = reader.current(); it != reader.end(); ++it)
            {
                param = param->parse(*it, composer);
                if( ! param)
                {
                    paramCount++;
                    composer = &a2;
                    break;
                }
            }

            if(paramCount == 2)
            {
                ++it;
                break;
            }
        }
    }

    for(it = reader.current(); it != reader.end(); ++it)
    {
        Xml::EndElement* endElem = Xml::toEndElement(&*it);
        if(endElem)
        {
            if( endElem->name().local() != "myMethod" )
                throw std::runtime_error("invalid procedure: myMethod");
            
            ++it;
            break;
        }
    }

    std::clog << "\n------------" << std::endl;
    std::clog << "position: " << position.longitude << " " << position.latitude << std::endl;
    std::clog << "numbers[" << numbers.size() << "]: " ;
    std::copy ( numbers.begin(), numbers.end(), std::ostream_iterator<int>(std::clog,", ") );
    std::clog << std::endl;
    std::clog << "------------" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
// SoapServiceDefinition
///////////////////////////////////////////////////////////////////////////////

SoapServiceDefinition::SoapServiceDefinition()
{ 
}


SoapServiceDefinition::~SoapServiceDefinition()
{
    System::MutexLock lock( mutex() );

    ProcedureDefinitionMap::iterator it;
    for(it = _procDefs.begin(); it != _procDefs.end(); ++it)
    {
        delete it->second;
    }
}


void SoapServiceDefinition::addDefinition(const std::string& name, ProcedureDefinition* procDef)
{
    System::MutexLock lock( mutex() );

    ProcedureDefinitionMap::iterator it = _procDefs.find( name );
    if (it == _procDefs.end())
    {
        std::pair<const std::string, ProcedureDefinition*> p( name, procDef );
        _procDefs.insert( p );
    }
    else
    {
        delete it->second;
        it->second = procDef;
    }
}


const ProcedureDefinition* SoapServiceDefinition::getDefinition(const std::string& name) const
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
