/*
 * Copyright (C) 2012 Marc Boris Duerner
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
 
#include "Pt/Xml/Entity.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/XmlError.h"

namespace Pt {

namespace Xml {

EntityReference::EntityReference()
: Node(Node::EntityReference)
, _entity(0)
{ }


void EntityReference::clear()
{
    _name.clear();
    _entity = 0;
}


void EntityReference::setName(const Pt::String& name)
{
    _name = name; 
}


void EntityReference::setEntity(const Entity* entity)
{
    _entity = entity;
}


const Pt::String& EntityReference::name() const
{
    return _name; 
}
        

const Entity* EntityReference::get() const
{ 
    return _entity; 
}


//void EntityReference::resolve(const Pt::Char* value) const
//{
//    if(_chars)
//    {       
//        _chars->append(value);
//
//        for(const Pt::Char* ch = value; *ch != '\0'; ++ch)
//        {
//            if(*ch != ' ' && *ch != '\n' && *ch != '\r' && *ch != '\t')
//            {
//                _chars->setIgnorable(false);
//                break;
//            }
//        }
//    }
//    else if(_attr)
//    {
//        _attr->value().append(value);
//    }
//}

} // namespace Xml

} // namespace Pt
