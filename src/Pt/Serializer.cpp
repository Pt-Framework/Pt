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
#include "Pt/SerializationContext.h"
#include "Pt/Formatter.h"

namespace Pt {

void ISerializer::fixdownEach(Pt::SerializationInfo& si, SerializationContext& context)
{
    if(si.category() == Pt::SerializationInfo::Reference)
    {
        const void* p = si.refAddr();
        context.makeId(p);
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


void ISerializer::formatEach2(Pt::SerializationInfo& si, const void* type,
                              SerializationContext& context, Formatter& formatter)
{
    unsigned* id = context.getId(type);
    if(id)
    {
        si.setId( convert<std::string>(*id) );
    }

    if(si.category() == SerializationInfo::Value)
    {
        formatter.addValue( si.name(), si.typeName(), si.toString(), si.id() );
    }
    else if(si.category() == SerializationInfo::Object)
    {
        formatter.beginObject( si.name(), si.id() );

        SerializationInfo::Iterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            formatter.beginMember( it->name() );
            formatEach2(*it, 0, context, formatter);
            formatter.finishMember();
        }

        formatter.finishObject();
    }
    else if(si.category() == Pt::SerializationInfo::Reference)
    {
        unsigned* id = context.getId( si.refAddr() );
        if( ! id )
            throw std::runtime_error("no such id for reference");

        Pt::String addr = convert<Pt::String>( *id );
        formatter.addReference( si.name(), addr);
    }
    else if(si.category() == Pt::SerializationInfo::Array)
    {
        formatter.beginArray( si.name(), si.id() );

        SerializationInfo::Iterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            formatEach2(*it, 0, context, formatter);
        }

        formatter.finishArray();
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

} // namespace Pt
