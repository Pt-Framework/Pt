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

void unlinkMember(Pt::SerializationInfo& si, SerializationContext& context)
{
    if(si.category() == Pt::SerializationInfo::Reference)
    {
        const void* p = si.refAddr();
        context.unlinkTarget(p);
    }
    else if(si.category() == Pt::SerializationInfo::Object ||
            si.category() == Pt::SerializationInfo::Array)
    {
        Pt::SerializationInfo::Iterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            unlinkMember(*it, context);
        }
    }
}


void unlinkEach(Pt::SerializationInfo& si, const void* target, SerializationContext& context)
{
    context.beginUnlinkTarget(si.name(), target);
    context.finishUnlinkTarget();

    if(si.category() == Pt::SerializationInfo::Reference)
    {
        const void* p = si.refAddr();
        context.unlinkTarget(p);
    }
    else if(si.category() == Pt::SerializationInfo::Object ||
            si.category() == Pt::SerializationInfo::Array)
    {
        Pt::SerializationInfo::Iterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            unlinkMember(*it, context);
        }
    }
}


void formatEach(Pt::SerializationInfo& si, const void* type,
                SerializationContext& context, Formatter& formatter)
{
    if( context.isUnlinked(type) )
    {
        std::string id = context.getUnlinkId(type);
        si.setId(id);
    }

    // TODO: it might not be necessary to store the id in the si object

    if(si.category() == SerializationInfo::Value)
    {
        formatter.addValue( si.name(), si.typeName(), si.toString(), si.id() );
    }
    else if(si.category() == Pt::SerializationInfo::Reference)
    {
        std::string id = context.getUnlinkId( si.refAddr() );
        formatter.addReference( si.name(), id);
    }
    else if(si.category() == SerializationInfo::Object)
    {
        formatter.beginObject( si.name(), si.id() );

        SerializationInfo::Iterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            formatter.beginMember( it->name() );
            formatEach(*it, 0, context, formatter);
            formatter.finishMember();
        }

        formatter.finishObject();
    }
    else if(si.category() == Pt::SerializationInfo::Array)
    {
        formatter.beginArray( si.name(), si.id() );

        SerializationInfo::Iterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            formatEach(*it, 0, context, formatter);
        }

        formatter.finishArray();
    }
}

} // namespace Pt
