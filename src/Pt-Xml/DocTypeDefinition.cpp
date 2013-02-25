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

bool lessThanName(const std::pair<Pt::String, Pt::Xml::ElementDeclaration*>& e, const Pt::String& name)
{
    return e.first < name;
}

}

namespace Pt {

namespace Xml {

DocTypeDefinition::DocTypeDefinition(DocTypeContext& ctx)
: Node(Node::DocTypeDefinition)
, _ctx(&ctx)
{
}


DocTypeDefinition::~DocTypeDefinition()
{
    clear();
}


void DocTypeDefinition::clear()
{
    for(ElementDeclarationList::iterator it = _elemDecls.begin(); it != _elemDecls.end(); ++it)
    {
        delete it->second;
    }

    _elemDecls.clear();
    _entities.clear();
    _paramEntities.clear();
}


ElementDeclaration* DocTypeDefinition::declareElement(const Pt::String& name)
{ 
    ElementDeclarationList::iterator lbound;
    lbound = std::lower_bound(_elemDecls.begin(), _elemDecls.end(), name, lessThanName);
    
    if( lbound != _elemDecls.end() && lbound->first == name)
    {
        return 0;
    }

    std::auto_ptr<ElementDeclaration> ep( new ElementDeclaration() );
    std::pair<Pt::String, ElementDeclaration*> entry( name, ep.get() );
    _elemDecls.insert(lbound, entry);
    return ep.release();
}


ElementDeclaration* DocTypeDefinition::findElement(const Pt::String& name)
{
    ElementDeclarationList::iterator lbound;
    lbound = std::lower_bound(_elemDecls.begin(), _elemDecls.end(), name, lessThanName);
    
    if( lbound != _elemDecls.end() && lbound->first == name)
    {
        return lbound->second;
    }

    return 0;
}


AttributeListDeclaration& DocTypeDefinition::declareAttributeList(const Pt::String& name)
{ 
    ElementDeclarationList::iterator lbound;
    lbound = std::lower_bound(_elemDecls.begin(), _elemDecls.end(), name, lessThanName);
    
    if( lbound != _elemDecls.end() && lbound->first == name)
    {
        return lbound->second->attributeList();
    }

    std::auto_ptr<ElementDeclaration> ep( new ElementDeclaration() );
    std::pair<Pt::String, ElementDeclaration*> entry( name, ep.get() );
    _elemDecls.insert(lbound, entry);
    ElementDeclaration* elemDecl = ep.release();
    return elemDecl->attributeList();
}


AttributeListDeclaration* DocTypeDefinition::attributeList(const Pt::String& name)
{
    ElementDeclarationList::iterator lbound;
    lbound = std::lower_bound(_elemDecls.begin(), _elemDecls.end(), name, lessThanName);
    
    if( lbound != _elemDecls.end() && lbound->first == name)
    {
        return &lbound->second->attributeList();
    }

    return 0;
}


Entity* DocTypeDefinition::declareEntity(const Pt::String& name)
{
    return _entities.declareEntity(name);
}


const Entity* DocTypeDefinition::resolveEntity(const Pt::String& name) const
{
    return _entities.resolveEntity(name);
}


Entity* DocTypeDefinition::declareParamEntity(const Pt::String& name)
{
    return _paramEntities.declareEntity(name);
}


const Entity* DocTypeDefinition::resolveParamEntity(const Pt::String& name) const
{
    return _paramEntities.resolveEntity(name);
}

} // namespace Xml

} // namespace Pt
