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

#include "ContentModel.h"
#include "ElementDeclaration.h"
#include <Pt/Xml/DocTypeDefinition.h>
#include <algorithm>
#include <memory>
#include <cassert>

namespace {

bool lessThanName(const std::pair<Pt::Xml::QName, Pt::Xml::ContentModel*>& e, const Pt::Xml::QName& name)
{
    return e.first < name;
}

}

namespace Pt {

namespace Xml {

EntityMapping::EntityMapping()
{
}


EntityMapping::~EntityMapping()
{
}


void EntityMapping::clear()
{ 
    _entities.clear(); 
}


Entity* EntityMapping::declareEntity(const Pt::String& name)
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


const Entity* EntityMapping::findEntity(const Pt::String& name) const
{
    Entities::const_iterator it = _entities.find(name);
    if(it == _entities.end() )
        return 0;

    return &(it->second);
}


NotationMapping::NotationMapping()
{
}


NotationMapping::~NotationMapping()
{
}


void NotationMapping::clear()
{ 
    _notations.clear(); 
}


Notation* NotationMapping::declareNotation(const Pt::String& name)
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


const Notation* NotationMapping::findNotation(const Pt::String& name) const
{
    Notations::const_iterator it = _notations.find(name);
    if(it == _notations.end() )
        return 0;

    return &(it->second);
}


// TODO
static MatchParticle match;

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
        delete it->second;
    }

    _rootName.clear();
    _docModel.clear();
    _entities.clear();
    _paramEntities.clear();

    for(unsigned n = 0; n < _pool.size() ; ++n)
    {
        delete _pool[n];
    }

    _pool.clear();
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


ContentModel* DocTypeDefinition::declareElement(const QName& name)
{ 
    DocumentModel::iterator lbound;
    lbound = std::lower_bound(_docModel.begin(), _docModel.end(), name, lessThanName);
    
    if( lbound != _docModel.end() && lbound->first == name)
    {
        return 0;
    }

    std::auto_ptr<ContentModel> ep( new ContentModel() );
    std::pair<QName, ContentModel*> entry( name, ep.get() );
    _docModel.insert(lbound, entry);
    return ep.release();
}


ContentModel* DocTypeDefinition::findElement(const QName& name)
{
    DocumentModel::iterator lbound;
    lbound = std::lower_bound(_docModel.begin(), _docModel.end(), name, lessThanName);
    
    if( lbound != _docModel.end() && lbound->first == name)
    {
        return lbound->second;
    }

    return 0;
}


AttributeListModel& DocTypeDefinition::declareAttributeList(const QName& name)
{ 
    DocumentModel::iterator lbound;
    lbound = std::lower_bound(_docModel.begin(), _docModel.end(), name, lessThanName);
    
    if( lbound != _docModel.end() && lbound->first == name)
    {
        return lbound->second->attributeList();
    }

    std::auto_ptr<ContentModel> ep( new ContentModel() );
    std::pair<QName, ContentModel*> entry( name, ep.get() );
    _docModel.insert(lbound, entry);
    ContentModel* elemDecl = ep.release();
    return elemDecl->attributeList();
}


AttributeListModel* DocTypeDefinition::findAttributeList(const QName& name)
{
    DocumentModel::iterator lbound;
    lbound = std::lower_bound(_docModel.begin(), _docModel.end(), name, lessThanName);
    
    if( lbound != _docModel.end() && lbound->first == name)
    {
        return &lbound->second->attributeList();
    }

    return 0;
}


Entity* DocTypeDefinition::declareEntity(const Pt::String& name)
{
    return _entities.declareEntity(name);
}


const Entity* DocTypeDefinition::findEntity(const Pt::String& name) const
{
    return _entities.findEntity(name);
}


Entity* DocTypeDefinition::declareParamEntity(const Pt::String& name)
{
    return _paramEntities.declareEntity(name);
}


const Entity* DocTypeDefinition::findParamEntity(const Pt::String& name) const
{
    return _paramEntities.findEntity(name);
}


Notation* DocTypeDefinition::declareNotation(const Pt::String& name)
{
    return _notations.declareNotation(name);
}


const Notation* DocTypeDefinition::findNotation(const Pt::String& name) const
{
    return _notations.findNotation(name);
}


LeafParticle& DocTypeDefinition::getLabel(const Pt::String& name)
{
    _pool.reserve(_pool.size() + 1);
    LeafParticle* label = new LeafParticle(name);
    _pool.push_back(label);
    return *label;
}


SplitParticle& DocTypeDefinition::getSplit(ContentParticle& to)
{
    _pool.reserve(_pool.size() + 1);
    SplitParticle* split = new SplitParticle(&to);
    _pool.push_back(split);
    return *split;
}


PcDataParticle& DocTypeDefinition::getPcData()
{
    _pool.reserve(_pool.size() + 1);
    PcDataParticle* node = new PcDataParticle();
    _pool.push_back(node);
    return *node;
}


MatchParticle& DocTypeDefinition::getMatch()
{ 
    return match; 
}

} // namespace Xml

} // namespace Pt
