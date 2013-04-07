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

#include "ElementDeclaration.h"
#include "AttributeDeclaration.h"

namespace Pt {

namespace Xml {

ContentModel::ContentModel()
: _start(0)
, _size(0)
, _type(Invalid)
{}
        

ContentModel::~ContentModel()
{
}


const AttributeListModel& ContentModel::attributeList() const
{ 
    return _attrs; 
}


AttributeListModel& ContentModel::attributeList()
{ 
    return _attrs; 
}
        

bool ContentModel::isEmpty() const
{ 
    return _type == Empty; 
}


void ContentModel::setEmpty()
{ 
    _start = 0;
    _size = 0;
    _type = Empty;
}


bool ContentModel::isAny() const
{ 
    return _type == Any; 
}


void ContentModel::setAny()
{ 
    _start = 0;
    _size = 0;
    _type = Any;
}


bool ContentModel::isExpression() const
{ 
    return _type == Expression; 
}


void ContentModel::setExpression(ContentParticle& start, unsigned n)
{ 
    _start = &start; 
    _size = n;
    _type = Expression;
}


const ContentParticle* ContentModel::content() const
{ 
    return _start; 
}


std::size_t ContentModel::contentSize() const
{ 
    return _size; 
}

} // namespace Xml

} // namespace Pt
