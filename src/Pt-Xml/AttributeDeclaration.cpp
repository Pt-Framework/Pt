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

#include "AttributeDeclaration.h"
#include "DocTypeValidator.h"
#include <Pt/StringStream.h>

namespace Pt {

namespace Xml {

bool EnumAttributeDeclaration::onValidate(const Attribute& attr) const
{           
    return _enumValues.find( attr.value() ) != _enumValues.end(); 
}


bool IDAttributeDeclaration::onValidate(const Attribute& attr) const
{          
    // TODO: attribute value must be an XML name

    return _validator->addId( attr.value() );
}


bool IDRefAttributeDeclaration::onValidate(const Attribute& attr) const
{          
    // TODO: attribute value must be an XML name

    _validator->addIdRef( attr.value() );
    return true; 
}


bool IDRefsAttributeDeclaration::onValidate(const Attribute& attr) const
{          
    // TODO: attribute value must be an XML name

    Pt::IStringStream iss( attr.value() );
    Pt::String idref;
    
    while(iss >> idref)
    {
        _validator->addIdRef(idref);
    }

    return true; 
}

} // namespace Xml

} // namespace Pt
