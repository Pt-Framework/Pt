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
#include <algorithm>
#include <memory>
#include <cassert>

namespace {

bool lessThanNameooo(const std::pair<Pt::Xml::QName, Pt::Xml::ElementModel*>& e, const Pt::Xml::QName& name)
{
    return e.first < name;
}

bool lessThanName(Pt::Xml::ElementModel* e, const Pt::Xml::QName& name)
{
    return e->qname() < name;
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
    for(DocumentModel::iterator it = _docModel.begin(); it != _docModel.end(); ++it)
    {
        delete *it;
    }

    _rootName.clear();
    _docModel.clear();
    _entities.clear();
    _paramEntities.clear();
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
    Entities::iterator it = _entities.lower_bound(name);

    // return 0 for duplicates
    if(it != _entities.end() && it->first == name)
        return 0;

    // insert new Entity
    Entities::value_type elem(name, Entity());
    it = _entities.insert(it, elem);
    return &it->second;
}


const Entity* DocTypeDefinition::findEntity(const Pt::String& name) const
{
    Entities::const_iterator it = _entities.find(name);
    if(it == _entities.end() )
        return 0;

    return &(it->second);
}


Entity* DocTypeDefinition::declareParamEntity(const Pt::String& name)
{

    Entities::iterator it = _paramEntities.lower_bound(name);

    // return 0 for duplicates
    if(it != _paramEntities.end() && it->first == name)
        return 0;

    // insert new Entity
    Entities::value_type elem(name, Entity());
    it = _paramEntities.insert(it, elem);
    return &it->second;
}


const Entity* DocTypeDefinition::findParamEntity(const Pt::String& name) const
{
    Entities::const_iterator it = _paramEntities.find(name);
    if(it == _paramEntities.end() )
        return 0;

    return &(it->second);
}


Notation* DocTypeDefinition::declareNotation(const Pt::String& name)
{
    Notations::iterator it = _notations.lower_bound(name);

    // return 0 for duplicates
    if(it != _notations.end() && it->first == name)
        return 0;

    // insert new Notation
    Notations::value_type elem(name, Notation());
    it = _notations.insert(it, elem);
    return &it->second;
}


const Notation* DocTypeDefinition::findNotation(const Pt::String& name) const
{
    Notations::const_iterator it = _notations.find(name);
    if(it == _notations.end() )
        return 0;

    return &(it->second);
}


ElementModel* DocTypeDefinition::declareElement(const QName& name)
{ 
    DocumentModel::iterator lbound;
    lbound = std::lower_bound(_docModel.begin(), _docModel.end(), name, lessThanName);
    
    if( lbound != _docModel.end() && (*lbound)->qname() == name)
    {
        ElementModel* e = *lbound;
        return e->isUndeclared() ? e : 0;
    }

    std::auto_ptr<ElementModel> ep( new ElementModel(name) );
    _docModel.insert(lbound, ep.get());
    return ep.release();
}


ElementModel* DocTypeDefinition::findElement(const QName& name)
{
    DocumentModel::iterator lbound;
    lbound = std::lower_bound(_docModel.begin(), _docModel.end(), name, lessThanName);
    
    if( lbound != _docModel.end() && (*lbound)->qname() == name)
    {
        return *lbound;
    }

    return 0;
}


AttributeListModel& DocTypeDefinition::declareAttributeList(const QName& name)
{ 
    DocumentModel::iterator lbound;
    lbound = std::lower_bound(_docModel.begin(), _docModel.end(), name, lessThanName);
    
    if( lbound != _docModel.end() && (*lbound)->qname() == name)
    {
        return (*lbound)->attributes();
    }

    std::auto_ptr<ElementModel> ep( new ElementModel(name) );
    _docModel.insert(lbound, ep.get());
    
    ElementModel* elemDecl = ep.release();
    return elemDecl->attributes();
}


AttributeListModel* DocTypeDefinition::findAttributeList(const QName& name)
{
    DocumentModel::iterator lbound;
    lbound = std::lower_bound(_docModel.begin(), _docModel.end(), name, lessThanName);
    
    if( lbound != _docModel.end() && (*lbound)->qname() == name)
    {
        ElementModel* e = *lbound;
        return &e->attributes();
    }

    return 0;
}

} // namespace Xml

} // namespace Pt
