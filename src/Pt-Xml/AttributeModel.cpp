/*
 * Copyright (C) 2013 by Marc Boris Duerner
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

#include "AttributeModel.h"
#include "AttributeListModel.h"
#include "NamespaceContext.h"
#include <Pt/Xml/DocTypeDefinition.h>
#include <Pt/Xml/Entity.h>
#include <Pt/Xml/Notation.h>
#include <Pt/StringStream.h>

namespace Pt {

namespace Xml {

AttributeValidator::AttributeValidator()
{}

void AttributeValidator::reset()
{
    _ids.clear();
    _idrefs.clear();
}


bool AttributeValidator::validate(AttributeList& attrs, const AttributeListModel& decls)
{
    //TODO: use fixed array[N], use vector when decls.size() > N

    std::vector<const Pt::Xml::AttributeModel*> attrDecls;
    std::copy(decls.begin(), decls.end(), std::back_inserter(attrDecls));

    // TODO: do not allow two ID type attributes

    // match attributes against declarations and remove declarations
    // that match an attribute
    Pt::Xml::AttributeList::ConstIterator attr;
    for(attr = attrs.begin(); attr != attrs.end(); ++attr)
    {
        std::vector<const Pt::Xml::AttributeModel*>::iterator it;
                 
        for(it = attrDecls.begin(); it != attrDecls.end(); ++it)
        {
            if( (*it)->qname() == attr->name() )
            {
                break;
            }
        }

        if( it == attrDecls.end() )
            return false;

        if( ! (*it)->validate( *this, *attr) )
            return false;

        attrDecls.erase(it);
    }

    // post process unmatched declarations e.g. get default values
    // and check for missing required attributes
    std::vector<const Pt::Xml::AttributeModel*>::iterator decl;
    for(decl = attrDecls.begin(); decl != attrDecls.end(); ++decl)
    {
        if( ! (*decl)->fixup(*this, attrs) )
            return false;
    }

    return true;
}


bool AttributeValidator::isValid() const
{
    bool valid = true;
            
    std::vector<Pt::String>::const_iterator it;
    for(it =_idrefs.begin(); it != _idrefs.end(); ++it)
    {
        if( _ids.find(*it) == _ids.end() )
        {
            valid = false;
        }
    }

    return valid;
}

bool AttributeValidator::addId(const Pt::String& id)
{
    if( _ids.find(id) != _ids.end() )
        return false;

    _ids.insert(id);
    return true;
}

void AttributeValidator::addRef(const Pt::String& id)
{
    _idrefs.push_back(id);
}




bool AttributeModel::validate(AttributeValidator& validator, const Attribute& attr) const
{
    if(mode() == Fixed && attr.value() != defaultValue() )
        return false;

    return onValidate(validator, attr);
}
      

bool AttributeModel::fixup(AttributeValidator& validator, AttributeList& list) const
{
    switch(_mode)
    {
        case Required:
            return false;

        case Implied:
            return true;

        case Fixed:
        case Default:
            break;
    };

    NamespaceContext& nsctx = list.namespaceContext();

    const Namespace* ns = nsctx.findPrefix( _qname.prefix() );
    if( ! ns )
        return false;

    Attribute& attr = list.append(_qname, *ns);
    attr.value() = _default;

    return onValidate(validator, attr);
}




bool CDataAttributeModel::onValidate(AttributeValidator& validator, const Attribute& attr) const
{ 
    // TODO: check for non-CDATA characters in value           
    return true; 
}

void CDataAttributeModel::onNormalize(Attribute&) const
{
}


bool NMTokenAttributeModel::onValidate(AttributeValidator& validator, const Attribute& attr) const
{ 
    // TODO: check for non-CDATA characters in value           
    return true; 
}


bool NMTokensAttributeModel::onValidate(AttributeValidator& validator, const Attribute& attr) const
{ 
    // TODO: check for non-CDATA characters in value           
    return true; 
}


bool EnumAttributeModel::onValidate(AttributeValidator& validator, const Attribute& attr) const
{           
    return _enumValues.find( attr.value() ) != _enumValues.end(); 
}


bool IDAttributeModel::onValidate(AttributeValidator& validator, const Attribute& attr) const
{          
    // TODO: attribute value must be an XML name

    return validator.addId( attr.value() );
}


bool IDRefAttributeModel::onValidate(AttributeValidator& validator, const Attribute& attr) const
{          
    // TODO: attribute value must be an XML name

    validator.addRef( attr.value() );
    return true; 
}


bool IDRefsAttributeModel::onValidate(AttributeValidator& validator, const Attribute& attr) const
{          
    // TODO: attribute value must be an XML name

    Pt::IStringStream iss( attr.value() );
    Pt::String idref;
    
    while(iss >> idref)
    {
        validator.addRef(idref);
    }

    return true; 
}


bool EntityAttributeModel::onValidate(AttributeValidator& validator, const Attribute& attr) const
{          
    const Entity* ent = _dtd->findEntity(attr.value());
    if( ent && ent->isUnparsed() )
    {
        return true;
    }

    return false; 
}


bool EntitiesAttributeModel::onValidate(AttributeValidator& validator, const Attribute& attr) const
{          
    Pt::IStringStream iss( attr.value() );
    Pt::String name;
    
    while(iss >> name)
    {
        const Entity* ent = _dtd->findEntity(name);
        if( ! ent || ! ent->isUnparsed() )
        {
            return false;
        }
    }

    return true; 
}


bool NotationAttributeModel::onValidate(AttributeValidator& validator, const Attribute& attr) const
{          
    if( _notations.find( attr.value() ) == _notations.end() )
    {
        return false;
    }

    if( ! _dtd->findNotation(attr.value()) )
    {
        return false;
    }
    
    return true; 
}

} // namespace Xml

} // namespace Pt
