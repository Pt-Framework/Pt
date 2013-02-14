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

#include "DocTypeDefinition.h"
#include "ContentModel.h"
#include <Pt/Xml/ElementDeclaration.h>
#include <algorithm>
#include <memory>
#include <cassert>

namespace {

bool lessThanName(const std::pair<Pt::String, Pt::Xml::ElementDeclaration*>& e, const Pt::String& name)
{
    return e.first < name;
}

}

namespace Pt {

namespace Xml {

DocTypeDefinition::DocTypeDefinition()
: Node(Node::DocTypeDefinition)
, _match(0)
{
    _match = new MatchParticle();
}


DocTypeDefinition::~DocTypeDefinition()
{
    assert(_pool.empty());
    assert(_elemDecls.empty());

    delete _match;
}


ElementDeclaration& DocTypeDefinition::declareElement(const Pt::String& name)
{ 
    ElementDeclarationTable::iterator lbound;
    lbound = std::lower_bound(_elemDecls.begin(), _elemDecls.end(), name, lessThanName);
    
    if( lbound != _elemDecls.end() && lbound->first == name)
    {
        return *lbound->second;
    }

    std::auto_ptr<ElementDeclaration> ep( new ElementDeclaration() );
    std::pair<Pt::String, ElementDeclaration*> entry( name, ep.get() );
    _elemDecls.insert(lbound, entry);
    return *ep.release();
}


ElementDeclaration* DocTypeDefinition::findElementDeclaration(const Pt::String& name)
{
    ElementDeclarationTable::iterator lbound;
    lbound = std::lower_bound(_elemDecls.begin(), _elemDecls.end(), name, lessThanName);
    
    if( lbound != _elemDecls.end() && lbound->first == name)
    {
        return lbound->second;
    }

    return 0;
}


void DocTypeDefinition::clear()
{
    for(ElementDeclarationTable::iterator it = _elemDecls.begin(); it != _elemDecls.end(); ++it)
    {
        delete it->second;
    }

    _elemDecls.clear();


    _entities.clear();
    _paramEntities.clear();
     
    for(unsigned n = 0; n < _pool.size() ; ++n)
    {
        delete _pool[n];
    }

    _pool.clear();
}


Entity* DocTypeDefinition::addEntity(const Pt::String& name)
{
    return _entities.addEntity(name);
}


const Entity* DocTypeDefinition::resolveEntity(const Pt::String& name) const
{
    return _entities.resolveEntity(name);
}


Entity* DocTypeDefinition::addParamEntity(const Pt::String& name)
{
    return _paramEntities.addEntity(name);
}


const Entity* DocTypeDefinition::resolveParamEntity(const Pt::String& name) const
{
    return _paramEntities.resolveEntity(name);
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
    return *_match; 
}

} // namespace Xml

} // namespace Pt
