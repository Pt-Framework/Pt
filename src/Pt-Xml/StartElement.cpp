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

Attribute& AttributeList::append(const QName& name, const Namespace& ns)
{
    if( _size >= _container.size() )
    {
        _container.push_back( Attribute() ); 
    }
    else
    {
        _container[_size].clear();
    }
    
    std::size_t backPos = _size;
    ++_size;

    _begin = &_container[0];
    _end = _begin + (_size);

    Attribute& attr = _container[backPos];
    attr.setName(name, ns);
    return attr;
}


const Pt::String& AttributeList::get(const String& localName) const
{
    ConstIterator it = find(localName);
            
	if(it == end())
		throw XmlError("Attribute not found");

		return it->value();
}


const Pt::String& AttributeList::get(const Char* localName) const
{
    ConstIterator it = find(localName);
            
	if(it == end())
		throw XmlError("Attribute not found");

		return it->value();
}


const Pt::String& AttributeList::get(const String& nsUri, const String& localName) const
{
    ConstIterator it = find(nsUri, localName);
          	
	if(it == end())
		throw XmlError("Attribute not found");

	return it->value();
}


const Pt::String& AttributeList::get(const Char* nsUri, const Char* localName) const
{
    ConstIterator it = find(nsUri, localName);
          	
	if(it == end())
		throw XmlError("Attribute not found");

	return it->value();
}


AttributeList::Iterator AttributeList::find(const String& name)
{
    Iterator it;
    for(it = begin(); it != end(); ++it) 
    {
        if(it->name().name() == name) 
        {
            break;
        }
    }
    
    return it;
}


AttributeList::Iterator AttributeList::find(const Char* name)
{
    Iterator it;
    for(it = begin(); it != end(); ++it) 
    {
        if(it->name().name() == name) 
        {
            break;
        }
    }
    
    return it;
}


AttributeList::Iterator AttributeList::find(const String& nsUri, const String& name)
{
    Iterator it;
    for(it = begin(); it != end(); ++it) 
    {
        if(it->name().name() == name && it->namespaceUri() == nsUri) 
        {
            break;
        }
    }
    
    return it;
}


AttributeList::Iterator AttributeList::find(const Char* nsUri, const Char* name)
{
    Iterator it;
    for(it = begin(); it != end(); ++it) 
    {
        if(it->name().name() == name && it->namespaceUri() == nsUri) 
        {
            break;
        }
    }
    
    return it;
}


AttributeList::ConstIterator AttributeList::find(const String& name) const
{
    ConstIterator it;
    for(it = begin(); it != end(); ++it) 
    {
        if(it->name().name() == name) 
        {
            break;
        }
    }
    
    return it;
}


AttributeList::ConstIterator AttributeList::find(const Char* name) const
{
    ConstIterator it;
    for(it = begin(); it != end(); ++it) 
    {
        if(it->name().name() == name) 
        {
            break;
        }
    }
    
    return it;
}


AttributeList::ConstIterator AttributeList::find(const String& nsUri, const String& name) const
{
    ConstIterator it;
    for(it = begin(); it != end(); ++it) 
    {
        if(it->name().name() == name && it->namespaceUri() == nsUri) 
        {
            break;
        }
    }
    
    return it;
}


AttributeList::ConstIterator AttributeList::find(const Char* nsUri, const Char* name) const
{
    ConstIterator it;
    for(it = begin(); it != end(); ++it) 
    {
        if(it->name().name() == name && it->namespaceUri() == nsUri) 
        {
            break;
        }
    }
    
    return it;
}

} // namespace Xml

} // namespace Pt
