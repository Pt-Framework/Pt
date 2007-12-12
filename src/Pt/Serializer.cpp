/***************************************************************************
 *   Copyright (C) 2007 by Marc Boris Duerner                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 **************************************************************************/
#include "Pt/Serializer.h"

namespace Pt {

Serializer::Serializer()
{ }


Serializer::~Serializer()
{ }


SerializationInfo& Serializer::append(const void* obj)
{
    _stack.resize( _stack.size() + 1 );
    SerializationInfo& si = _stack.back();

    _objects[obj] = &si;
    return si;
}


void Serializer::fixdown()
{
    std::list<Pt::SerializationInfo>::iterator it;
    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        this->fixdown(*it);
    }

    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        this->write( *it );
    }

    _objects.clear();
    _stack.clear();
}


void Serializer::fixdown(Pt::SerializationInfo& si)
{
    if(si.category() == Pt::SerializationInfo::Reference)
    {
        const void* p = si.toValue<void*>();
        Pt::SerializationInfo* pointee = _objects[p];
        pointee->setId( convert<std::string>(pointee) );
        si.setReference( pointee );
    }
    else if(si.category() == Pt::SerializationInfo::Object)
    {
        Pt::SerializationInfo::Iterator it;
        for(it = si.begin(); it != si.end(); ++it)
        {
            this->fixdown(*it);
        }
    }
}

} // namespace Pt
