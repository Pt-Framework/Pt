/*
 * Copyright (C) 2004-2010 by Marc Boris Duerner
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
#include "Pt/Reflex/GenericType.h"

namespace Pt {

namespace Reflex {


GenericType::~GenericType()
{
    TypeMap::iterator it;
    for(it = _types.begin(); it != _types.end(); ++it)
    {
        delete it->second;
    }
}


Type* GenericType::getType(Pt::Reflex::Type& t1)
{
    TypeMap::iterator it = _types.find( t1.name() );
    if( it != _types.end() )
        return it->second;

    Type* type = this->makeType(t1);

    _types[ t1.name() ] = type;
    return type;
}


void GenericType::setType(Pt::Reflex::Type& t1, Pt::Reflex::Type& type)
{
    _types[ t1.name() ] = & type;
}

}

}
