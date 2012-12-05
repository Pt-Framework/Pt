/*
 * Copyright (C) 2012 Marc Boris Duerner
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
#include "Pt/Xml/NamespaceContext.h"

namespace Pt {

namespace Xml {

// TODO: return null if not found !!!
//       reader has to throw an exception then
const String& NamespaceContext::getNamespace(const String& prefix) const
{
    std::vector<Namespace>::const_reverse_iterator it;

    for(it = _namespaces.rbegin(); it != _namespaces.rend(); ++it)
    {
      if( prefix == it->prefix() )
          return it->name();
    }

    return _defaultNSName;
}


void NamespaceContext::setNamespace(unsigned depth, const String& prefix, const String& name)
{
    _namespaces.push_back( Namespace(depth, prefix, name) );
}


void NamespaceContext::popNamespace(unsigned depth)
{
    while( ! _namespaces.empty() && _namespaces.back().depth() >= depth)
        _namespaces.pop_back();
}

} // namespace Xml

} // namespace Pt

