/*
 * Copyright (C) 2013 Marc Boris Duerner
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
 

#include "ContentParticle.h"
#include "ElementModel.h"
#include "AttributeModel.h"
#include <Pt//Xml/DocTypeValidator.h>
#include <Pt/Xml/DocTypeDefinition.h>
#include <Pt/Xml/Characters.h>
#include <stack>

namespace Pt {

namespace Xml {

class ElementValidator
{
    public:
        //! @brief A validator for an undeclared element.
        ElementValidator()
        : _elemDecl(0)
        { }

        explicit ElementValidator(const ElementModel& elemDecl)
        : _particles( elemDecl.content().expression(), elemDecl.content().size() )
        , _elemDecl(&elemDecl)
        { }

        bool validateNode(Node& node);
        
        bool isValid() const;

    private:
        ContentParticleList _particles;
        const ElementModel* _elemDecl;
};


bool ElementValidator::validateNode(Node& node)
{
    // handle ignorable WS and EMPTY separately, so indentation in XML
    // documents does not lead to costly state transitions. 
    if( Pt::Xml::Characters* chars = Pt::Xml::toCharacters(&node) )
    {
        if( chars->isSpace() )
        {
            // special rule for EMPTY, not even WS is allowed
            if( _elemDecl && _elemDecl->content().isEmpty() )
                return false;

            // all other cases ignore WS
            return true;
        }
    }

    if( _elemDecl && _elemDecl->content().isAny() )
        return true;

    // invalid or EMPTY will not have any further states, so we will
    // return with false eventually
    return _particles.advance(node);
}
        

bool ElementValidator::isValid() const
{ 
    // if the element was undeclared, empty or any content is allowed
    // we do not expect more content
    if( ! _elemDecl || ! _elemDecl->content().isExpression() )
        return true;
            
    return _particles.isValid();
}


class DocTypeValidatorImpl
{
    public:
        explicit DocTypeValidatorImpl(DocTypeDefinition& dtd);

        void reset();

        bool validate(Node& node);

    private:
        DocTypeDefinition* _dtd;
        std::stack<ElementValidator> _decls;
        AttributeValidator _attrValidator;
};


DocTypeValidatorImpl::DocTypeValidatorImpl(DocTypeDefinition& dtd)
: _dtd(&dtd)
{
}


void DocTypeValidatorImpl::reset()
{ 
    _attrValidator.reset();

    while( ! _decls.empty() )
        _decls.pop();
}


bool DocTypeValidatorImpl::validate(Node& node)
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
            else // first start element
            {
                valid = _dtd->rootName() == se.name();
            }
                    
            ElementModel* decl = _dtd->findElement( se.name() );
            if(decl)
            {
                ElementValidator validator( *decl );
                _decls.push(validator);

                if( ! _attrValidator.validate(se.attributes(), decl->attributes()) )
                    valid = false;
            }
            else
            {
                ElementValidator validator;
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
            valid = _decls.top().isValid();
            _decls.pop();

            if( _decls.empty() )
                if( ! _attrValidator.isValid() )
                    valid = false;
                    
            break;
        }

        default:
            break;
    }

    return valid;
}


DocTypeValidator::DocTypeValidator(DocTypeDefinition& dtd)
: _impl( new DocTypeValidatorImpl(dtd) )
{}


DocTypeValidator::~DocTypeValidator()
{
    delete _impl;
}


void DocTypeValidator::reset()
{ 
    _impl->reset();
}


bool DocTypeValidator::validate(Node& node)
{
    return _impl->validate(node);
}

} // namespace Xml

} // namespace Pt
