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
#ifndef Pt_Xml_DtdValidator_h
#define Pt_Xml_DtdValidator_h

#include "DocTypeDefinition.h"
#include "ContentModel.h"
#include <Pt/Xml/ElementDeclaration.h>

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>

#include <vector>
#include <stack>
#include <cassert>

namespace Pt {

namespace Xml {

// TODO: rename DocTypeValidator
class DtdValidator : private NonCopyable
{
    public:
        DtdValidator(DocTypeDefinition& dtd)
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
                        valid = _decls.top().validate(se);
                    }
                    
                    ElementDeclaration* decl = _dtd->findElementDeclaration( se.name() );
                    if(decl)
                    {
                        ContentValidator validator( *decl );
                        _decls.push(validator);
                        
                        if( ! decl->attlist().validate( se.attributes() ) )
                            valid = false; 
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
                        valid = _decls.top().validate(chars);
                    }

                    break;
                }

                case Node::EndElement:
                {
                    valid = _decls.top().isComplete();
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
