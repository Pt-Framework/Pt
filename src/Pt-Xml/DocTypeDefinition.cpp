/*
 * Copyright (C) 2012 by Marc Boris Duerner
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

#include "ElementModel.h"
#include <Pt/Xml/DocTypeDefinition.h>
#include <Pt/Xml/Entity.h>
#include <Pt/Xml/Notation.h>
#include <algorithm>
#include <iterator>
#include <memory>
#include <cassert>

namespace {

template<class Iter, class T, class Pred> 
inline Iter lowerBound(Iter first, Iter last, const T& value, Pred pred)
{	
    typedef std::iterator_traits<Iter> TraitsType;

    typename TraitsType::difference_type count = std::distance(first, last);
	
    for( ; count > 0; )
	{	
		typename TraitsType::difference_type count2 = count / 2;
		
        Iter mid = first;
		std::advance(mid, count2);

		if( pred(*mid, value) ) // upper half
		{	
		    first = ++mid;
		    count -= count2 + 1;
		}
		else // lower half
        {
			count = count2;
        }
	}
	
    return first;
}

bool lessElementName(Pt::Xml::ElementModel* e, const Pt::Xml::QName& name)
{
    return e->qname() < name;
}

bool lessNotationName(Pt::Xml::Notation* n, const Pt::String& name)
{
    return n->name() < name;
}

bool lessEntityName(Pt::Xml::Entity* e, const Pt::String& name)
{
    return e->name() < name;
}

}

namespace Pt {

namespace Xml {

DocTypeDefinition::DocTypeDefinition()
{
}


DocTypeDefinition::~DocTypeDefinition()
{
    clear();
}


void DocTypeDefinition::clear()
{
    _rootName.clear();

    for(Elements::iterator it = _elements.begin(); it != _elements.end(); ++it)
    {
        delete *it;
    }
    _elements.clear();

    for(Entities::iterator it = _entities.begin(); it != _entities.end(); ++it)
    {
        delete *it;
    }
    _entities.clear();

    for(Entities::iterator it = _paramEntities.begin(); it != _paramEntities.end(); ++it)
    {
        delete *it;
    }
    _paramEntities.clear();

    for(Notations::iterator it = _notations.begin(); it != _notations.end(); ++it)
    {
        delete *it;
    }
    _notations.clear();
}


bool DocTypeDefinition::isDefined() const
{ 
    return ! _rootName.name().empty(); 
}


const QName& DocTypeDefinition::rootName() const
{ 
    return _rootName; 
}


QName& DocTypeDefinition::rootName()
{ 
    return _rootName; 
}


Entity* DocTypeDefinition::declareEntity(const Pt::String& name)
{
    Entities::iterator lbound;
    lbound = lowerBound(_entities.begin(), _entities.end(), name, lessEntityName);
    
    if( lbound != _entities.end() && (*lbound)->name() == name)
    {
        // return 0 for duplicates
        return 0;
    }

    std::auto_ptr<Entity> ep( new Entity(name) );
    _entities.insert(lbound, ep.get());
    return ep.release();
}


const Entity* DocTypeDefinition::findEntity(const Pt::String& name) const
{
    Entities::const_iterator lbound;
    lbound = lowerBound(_entities.begin(), _entities.end(), name, lessEntityName);

    if( lbound != _entities.end() && (*lbound)->name() == name)
    {
        return *lbound;
    }

    return 0;
}


void DocTypeDefinition::removeEntity(const Pt::String& name)
{
    Entities::iterator lbound;
    lbound = lowerBound(_entities.begin(), _entities.end(), name, lessEntityName);

    if( lbound != _entities.end() && (*lbound)->name() == name )
    {
        delete *lbound;
        _entities.erase(lbound);
    }
}


Entity* DocTypeDefinition::declareParamEntity(const Pt::String& name)
{
    Entities::iterator lbound;
    lbound = lowerBound(_paramEntities.begin(), _paramEntities.end(), name, lessEntityName);
    
    if( lbound != _paramEntities.end() && (*lbound)->name() == name)
    {
        // return 0 for duplicates
        return 0;
    }

    std::auto_ptr<Entity> ep( new Entity(name) );
    _paramEntities.insert(lbound, ep.get());
    return ep.release();
}


const Entity* DocTypeDefinition::findParamEntity(const Pt::String& name) const
{
    Entities::const_iterator lbound;
    lbound = lowerBound(_paramEntities.begin(), _paramEntities.end(), name, lessEntityName);

    if( lbound != _paramEntities.end() && (*lbound)->name() == name)
    {
        return *lbound;
    }

    return 0;
}


void DocTypeDefinition::removeParamEntity(const Pt::String& name)
{
    Entities::iterator lbound;
    lbound = lowerBound(_paramEntities.begin(), _paramEntities.end(), name, lessEntityName);

    if( lbound != _paramEntities.end() && (*lbound)->name() == name )
    {
        delete *lbound;
        _paramEntities.erase(lbound);
    }
}


Notation* DocTypeDefinition::declareNotation(const Pt::String& name)
{
    Notations::iterator lbound;
    lbound = lowerBound(_notations.begin(), _notations.end(), name, lessNotationName);
    
    if( lbound != _notations.end() && (*lbound)->name() == name)
    {
        // return 0 for duplicates
        return 0;
    }

    std::auto_ptr<Notation> ep( new Notation(name) );
    _notations.insert(lbound, ep.get());
    return ep.release();
}


const Notation* DocTypeDefinition::findNotation(const Pt::String& name) const
{
    Notations::const_iterator lbound;
    lbound = lowerBound(_notations.begin(), _notations.end(), name, lessNotationName);

    if( lbound != _notations.end() && (*lbound)->name() == name)
    {
        return *lbound;
    }

    return 0;
}


void DocTypeDefinition::removeNotation(const Pt::String& name)
{
    Notations::iterator lbound;
    lbound = lowerBound(_notations.begin(), _notations.end(), name, lessNotationName);

    if( lbound != _notations.end() && (*lbound)->name() == name )
    {
        delete *lbound;
        _notations.erase(lbound);
    }
}


ContentModel& DocTypeDefinition::declareContent(const QName& name)
{ 
    Elements::iterator lbound;
    lbound = lowerBound(_elements.begin(), _elements.end(), name, lessElementName);
    
    if( lbound != _elements.end() && (*lbound)->qname() == name)
    {
        return (*lbound)->content();
    }

    std::auto_ptr<ElementModel> ep( new ElementModel(name) );
    _elements.insert(lbound, ep.get());

    ElementModel* elemDecl = ep.release();
    return elemDecl->content();
}


AttributeListModel& DocTypeDefinition::declareAttributeList(const QName& name)
{ 
    Elements::iterator lbound;
    lbound = lowerBound(_elements.begin(), _elements.end(), name, lessElementName);
    
    if( lbound != _elements.end() && (*lbound)->qname() == name)
    {
        return (*lbound)->attributes();
    }

    std::auto_ptr<ElementModel> ep( new ElementModel(name) );
    _elements.insert(lbound, ep.get());
    
    ElementModel* elemDecl = ep.release();
    return elemDecl->attributes();
}


ElementModel* DocTypeDefinition::findElement(const QName& name)
{
    Elements::iterator lbound;
    lbound = lowerBound(_elements.begin(), _elements.end(), name, lessElementName);
    
    if( lbound != _elements.end() && (*lbound)->qname() == name)
    {
        return *lbound;
    }

    return 0;
}


AttributeListModel* DocTypeDefinition::findAttributes(const QName& name)
{
    Elements::iterator lbound;
    lbound = lowerBound(_elements.begin(), _elements.end(), name, lessElementName);
    
    if( lbound != _elements.end() && (*lbound)->qname() == name)
    {
        AttributeListModel& attList = (*lbound)->attributes();
        return &attList;
    }

    return 0;
}

} // namespace Xml

} // namespace Pt
