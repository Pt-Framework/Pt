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
#include "Pt/Deserializer.h"

namespace Pt {

Deserializer::Deserializer()
: _peeking(false)
{ }


Deserializer::~Deserializer()
{ }


SerializationInfo& Deserializer::peek()
{
    if( ! _peeking )
    {
        _stack.push_back( SerializationInfo() );
        Pt::SerializationInfo& si =_stack.back();
        this->read( si );
        _peeking = true;
    }

    _peeking = true;
    return _stack.back();
}


void Deserializer::finish()
{
    std::list<Pt::SerializationInfo>::iterator it;
    for(it = _stack.begin(); it != _stack.end(); ++it)
    {
        this->fixup(*it);
    }

    _peeking = false;
    _objects.clear();
    _stack.clear();
}


Pt::SerializationInfo& Deserializer::get()
{
    if( ! _peeking )
    {
        _stack.push_back( SerializationInfo() );
        Pt::SerializationInfo& si =_stack.back();
        this->read( si );
    }

    _peeking = false;
    return _stack.back();
}


void Deserializer::markFixup(Pt::SerializationInfo& si, void* type, Fixup fixup)
{
    if( ! si.id().empty() )
    {
        _objects[ si.id() ] = type;
        _fixups[ si.id() ] = fixup;
    }
}


void Deserializer::fixup(const Pt::SerializationInfo& si)
{
    Pt::SerializationInfo::ConstIterator it;
    for(it = si.begin(); it != si.end(); ++it)
    {
        if(it->category() == Pt::SerializationInfo::Reference)
        {
            void* obj = _objects[ it->toValue<std::string>() ];
            void* fixme = it->fixupAddr();
            Fixup fixupHandler = _fixups[ it->toValue<std::string>() ];
            fixupHandler( (void**)(&fixme), it->fixupInfo(), obj);
        }

        if(it->category() == Pt::SerializationInfo::Object)
        {
            this->fixup(*it);
        }
    }
}

} // namespace Pt
