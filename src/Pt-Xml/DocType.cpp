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

#include "InputStack.h"
#include <Pt/Xml/DocType.h>
#include <Pt/Xml/InputSource.h>
#include <Pt/Xml/DocTypeDefinition.h>

namespace Pt {

namespace Xml {

DocType::DocType(DocTypeDefinition& dtd)
: Node(Node::DocType)
, _dtd(&dtd)
{
    _internal.i = 0;
}


DocType::~DocType()
{
}


void DocType::clear()
{ 
    _publicId.clear();
    _systemId.clear();
    _internal.i = 0;
}


bool DocType::isExternal() const
{ 
    return ! _publicId.empty() || ! _systemId.empty(); 
}


bool DocType::isInternal() const
{ 
    return _internal.i == 1; 
}


void DocType::setInternal(bool hasInternal)
{
    _internal.i = hasInternal ? 1 : 0;
}


const QName& DocType::rootName() const
{ 
    return _dtd->rootName(); 
}


EndDocType::EndDocType()
: Node(Node::EndDocType)
{
    _internal.i = 0;
}


EndDocType::~EndDocType()
{ 
}


void EndDocType::clear()
{ 
    _internal.i = 0;
}


bool EndDocType::isExternal() const
{ 
    return _internal.i != 1;
}


bool EndDocType::isInternal() const
{ 
    return _internal.i == 1; 
}


void EndDocType::setInternal(bool value)
{
    _internal.i = value ? 1 : 0;
}

} // namespace Xml

} // namespace Pt
