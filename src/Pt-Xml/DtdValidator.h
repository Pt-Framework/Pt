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

#ifndef Pt_Xml_DocTypeValidator_h
#define Pt_Xml_DocTypeValidator_h

#include "ContentModel.h"
#include "ElementDeclaration.h"
#include "AttributeDeclaration.h"

#include <Pt/Xml/Api.h>
#include <Pt/Xml/DocTypeDefinition.h>
#include <Pt/Xml/Characters.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>

#include <iterator>
#include <vector>
#include <stack>
#include <cassert>

namespace Pt {

namespace Xml {

class AttributeValidator
{
    public:
        AttributeValidator()
        {}

        bool validate(AttributeList& attrs, const AttributeListDeclaration& decls) const
        {
            std::vector<const AttributeDeclaration*> attrDecls;
            std::copy(decls.begin(), decls.end(), std::back_inserter(attrDecls));

            // match attributes against declarations and remove declarations
            // that match an attribute
            AttributeList::ConstIterator attr;
            for(attr = attrs.begin(); attr != attrs.end(); ++attr)
            {
                std::vector<const AttributeDeclaration*>::iterator it;
                 
                for(it = attrDecls.begin(); it != attrDecls.end(); ++it)
                {
                    if( (*it)->name() == attr->name() )
                    {
                        break;
                    }
                }

                if( it == attrDecls.end() )
                    return false;

                if( ! (*it)->match( *attr) )
                    return false;

                attrDecls.erase(it);
            }

            // post process unmatched declarations e.g. get default values
            // and check for missing required attributes
            std::vector<const AttributeDeclaration*>::iterator decl;
            for(decl = attrDecls.begin(); decl != attrDecls.end(); ++decl)
            {
                if( ! (*decl)->validate(attrs) )
                    return false;
            }

            return true;
        }
};


class ContentValidator : public ValidationContext
{
    public:
        //!@brief A validator for an undeclared element.
        ContentValidator()
        : _elemDecl(0)
        {}

        ContentValidator(const ElementDeclaration& elemDecl)
        : ValidationContext( elemDecl.contentSize() )
        , _elemDecl(&elemDecl)
        {
            // if only an ATTLIST was declared, this can be null
            if( elemDecl.content() )
            {
                elemDecl.content()->get(*this);
            }
        }

        bool validateNode(Node& node)
        {
            // handle ignorable WS and EMPTY separately, so indentation in XML
            // documents does not lead to costly state transitions. 
            if( Pt::Xml::Characters* chars = Pt::Xml::toCharacters(&node) )
            {
                if( chars->isIgnorable() )
                {
                    // special rule for EMPTY, not even WS is allowed
                    if( _elemDecl && _elemDecl->isEmpty() )
                        return false;

                    // all other cases ignore WS
                    return true;
                }
            }

            if( _elemDecl && _elemDecl->isAny() )
                return true;

            // invalid or EMPTY will not have any further states, so we will
            // return with false eventually
            _next = ValidationContext::next();
            this->clear();

            for(unsigned n = 0; n < _next.size(); ++n)
            {
                _next[n]->eval(*this, node);
            }

            // no follow up particles means validation error
            return ! this->next().empty();
        }
        
        bool isCompleteNode() const
        { 
            // if the element was undeclared, empty or any content is allowed
            // we do not expect more content
            if( ! _elemDecl || ! _elemDecl->isExpression() )
                return true;
            
            // at the end of the validation, at least one current particle
            // must be a match particle, otherwise there was more content
            // expected to come
            for(unsigned n = 0; n < this->next().size(); ++n)
            {
                if( this->next()[n]->isValid() )
                    return true;
            }
            
            return false; 
        }

    private:
        const ElementDeclaration* _elemDecl;
        std::vector<const ContentParticle*> _next;
};


class DocTypeValidator : private NonCopyable
{
    public:
        DocTypeValidator(DocTypeDefinition& dtd)
        : _dtd(&dtd)
        {}

        void clear()
        {
            while( ! _decls.empty() )
                _decls.pop();
        }

        bool validate(Node& node)
        {
            bool valid = true;

            switch( node.type() )
            {
                case Node::StartElement:
                {
                    StartElement& se = static_cast<StartElement&>(node);
                    if( ! _decls.empty() )
                    {
                        valid = _decls.top().validateNode(se);
                    }
                    
                    ElementDeclaration* decl = _dtd->findElement( se.name() );
                    if(decl)
                    {
                        ContentValidator validator( *decl );
                        _decls.push(validator);
                        
                        AttributeValidator attributeValidator;
                        if( ! attributeValidator.validate( se.attributes(), decl->attributeList() ) )
                            valid = false;

                        //TODO: push empty ContentValidator if only ATTLIST is declared
                    }
                    else
                    {
                        ContentValidator validator;
                        _decls.push(validator);
                        valid = false;
                    }

                    break;
                }
                
                case Node::Characters:
                {
                    Characters& chars = static_cast<Characters&>(node);

                    if( ! _decls.empty() )
                    {
                        valid = _decls.top().validateNode(chars);
                    }

                    break;
                }

                case Node::EndElement:
                {
                    valid = _decls.top().isCompleteNode();
                    _decls.pop();
                    break;
                }

                default:
                    break;
            }

            return valid;
        }

    private:
        DocTypeDefinition* _dtd;
        std::stack<ContentValidator> _decls;
};

} // namespace Xml

} // namespace Pt

#endif
