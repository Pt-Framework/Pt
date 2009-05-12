/*
 * Copyright (C) 2008 by Marc Boris Duerner
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
#include "Pt/Serializer.h"
#include "Pt/Formatter.h"

namespace Pt {

void ISerializer::fixdownEach(Pt::SerializationInfo& si, SerializationContext& context)
{
    if(si.category() == Pt::SerializationInfo::Reference)
    {
        const void* p = si.refAddr();
        ISerializer* pointee = context.find(p);
        pointee->setId( convert<std::string>(pointee) );
        si.setReference( pointee ); // TODO setRefId()
        //std::cerr << "fixdown " << p << " to " << pointee << std::endl;
    }
    else if(si.category() == Pt::SerializationInfo::Object)
    {
        Pt::SerializationInfo::Iterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            fixdownEach(*it, context);
        }
    }
}


void ISerializer::formatEach(const Pt::SerializationInfo& si, Formatter& formatter)
{
    if(si.category() == SerializationInfo::Value)
    {
        formatter.addValue( si.name(), si.typeName(), si.toString(), si.id() );
    }
    else if(si.category() == SerializationInfo::Object)
    {
        formatter.beginObject( si.name(), si.id() );

        SerializationInfo::ConstIterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            formatter.beginMember( it->name() );
            formatEach(*it, formatter);
            formatter.finishMember();
        }

        formatter.finishObject();
    }
    else if(si.category() == Pt::SerializationInfo::Reference)
    {
    	Pt::String addr = convert<Pt::String>( si.refAddr() );
        formatter.addReference( si.name(), addr);
    }
    else if(si.category() == Pt::SerializationInfo::Array)
    {
        formatter.beginArray( si.name(), si.id() );

        SerializationInfo::ConstIterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            formatEach(*it, formatter);
        }

        formatter.finishArray();
    }
}


SerializationContext::SerializationContext()
{
}


SerializationContext::~SerializationContext()
{
    this->clear();
}


void SerializationContext::clear()
{
    //
    // Serialisation
    //
    _omap.clear();

    std::vector<ISerializer*>::iterator it;
    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        delete *it;
    }
    _stack.clear();

    //
    // Deserialisation
    //
    _targets.clear();
    _pointers.clear();
}


//
// Serialization specific
//
ISerializer* SerializationContext::find(const void* p) const
{
    std::map<const void*, ISerializer*>::const_iterator it;
    it = _omap.find(p);
    if(it == _omap.end())
        return 0;

    return it->second;
}


void SerializationContext::fixdown(Formatter& formatter)
{
    std::vector<ISerializer*>::iterator it;
    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        ISerializer* serializer = *it;
        serializer->fixdown(*this);
    }

    _omap.clear();

    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        (*it)->format(formatter);
    }
}


void SerializationContext::do_begin(const void* target, ISerializer* serializer)
{
    _omap[target] = serializer;
    _stack.push_back(serializer);
}

//
// Deserialization specific
//
void SerializationContext::addObject(const std::string& id, void* obj, const std::type_info& fixupInfo)
{
    FixupInfo fi;
    fi.address = obj;
    fi.type = &fixupInfo;
    _targets[id] = fi;
}


void SerializationContext::addFixup(const std::string& id, void* obj, const std::type_info& fixupInfo)
{
    FixupInfo fi;
    fi.address = obj;
    fi.type = &fixupInfo;
    _pointers[id] = fi;
}


void SerializationContext::fixup()
{
    std::map<std::string, FixupInfo>::iterator it;
    for(it = _pointers.begin(); it != _pointers.end(); ++it)
    {
        void* fixme = it->second.address;
        const std::type_info* fixupType = it->second.type;
        std::string id = it->first;
        void* target = _targets[id].address;
        const std::type_info* targetType = _targets[id].type ;

        //std::cerr << "FIXING: " << fixme << " to " << target << std::endl;
        bool fixupAllowed = this->checkFixup(*fixupType, *targetType);
        if( ! fixupAllowed )
            throw SerializationError("reference fixup failed, type mismatch");

        void** vp =(void**)(fixme);
        *vp = target;
    }

    clear();
}


bool SerializationContext::checkFixup(const std::type_info& from, const std::type_info& to)
{
    return from == to;
}

} // namespace Pt
