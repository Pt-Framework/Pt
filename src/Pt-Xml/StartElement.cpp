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

#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/XmlError.h"

namespace Pt {

namespace Xml {

void Attribute::normalize()
{
    Pt::String& str = value();

    Pt::String::iterator p1 = str.begin();
    Pt::String::iterator p2 = str.begin();
    int spaces = 1;
    bool normalized = false;
    
    for(; p2 != str.end(); ++p2)
    {
        if( Pt::isspace(*p2) )
        {
            switch(spaces)
            {
                case 0:
                    *p1 = *p2;
                    ++p1;
                    break;

                case 1:
                    normalized = true;
            };

              ++spaces;
        }
        else
        {
            spaces = 0;
                    
            if(normalized)
                *p1 = *p2;
                    
            ++p1;
        }
    }
           
    if(p1 != p2)
    {               
        str.erase( p1, str.end() );
    }

    if( spaces != 0 && ! str.empty() )
    {
        str.erase(str.size() - 1);
    }
}

AttributeList::ConstIterator AttributeList::find(const String& name) const
{
    ConstIterator it;
    for(it = _container.begin(); it != _container.end(); ++it) 
    {
        if(it->name() == name) 
        {
            break;
        }
    }
    
    return it;
}


AttributeList::ConstIterator AttributeList::find(const String& nsUri, const String& name) const
{
    ConstIterator it;
    for(it = _container.begin(); it != _container.end(); ++it) 
    {
        if(it->name() == name && it->namespaceUri() == nsUri) 
        {
            break;
        }
    }
    
    return it;
}


const String& StartElement::attribute(const String& name) const
{
    AttributeList::ConstIterator it = attributes().find(name);
    if( it == _attributes.end() )
        throw NoSuchAttribute(name);

    return it->value();
}


const String& StartElement::attribute(const String& nsUri, const String& name) const
{
    AttributeList::ConstIterator it = attributes().find(nsUri, name);
    if( it == _attributes.end() )
        throw NoSuchAttribute(name);

    return it->value();
}

} // namespace Xml

} // namespace Pt
