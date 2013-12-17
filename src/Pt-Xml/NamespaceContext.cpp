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
#include "NamespaceContext.h"

namespace Pt {

namespace Xml {

NamespaceContext::NamespaceContext()
: _empty(0, String(), String() )
, _xmlNamespace(0, String("xml"), String("http://www.w3.org/XML/1998/namespace"))
{
}


NamespaceContext::~NamespaceContext()
{
}


void NamespaceContext::clear()
{ 
    _namespaces.clear(); 
}


const Namespace& NamespaceContext::getDefaultNamespace() const
{
    std::vector<Namespace>::const_reverse_iterator it;

    for(it = _namespaces.rbegin(); it != _namespaces.rend(); ++it)
    {
      if( it->isDefaultNamespace() )
          return *it;
    }

    return _empty;
}


const Namespace* NamespaceContext::startElement(std::size_t depth, NamespaceMapping& nsmap, const String& prefix) const
{
    std::vector<Namespace>::const_reverse_iterator iter;

    for(iter = _namespaces.rbegin(); iter != _namespaces.rend(); ++iter)
    {
        if( iter->depth() != depth )
            break;

        if( iter->isUnset() )
            nsmap.addUnmapped(*iter);
        else
            nsmap.addMapped(*iter);
    }

    for(iter = _namespaces.rbegin(); iter != _namespaces.rend(); ++iter)
    {
      if( prefix == iter->prefix() && ! iter->isUnset() )
      {
          return &(*iter);
      }
    }

    if( prefix == _xmlNamespace.prefix() )
    {
        return &_xmlNamespace;
    }

    return prefix.empty() ? &_empty : 0;
}


const Namespace* NamespaceContext::findPrefix(const String& prefix) const
{
    std::vector<Namespace>::const_reverse_iterator it;

    for(it = _namespaces.rbegin(); it != _namespaces.rend(); ++it)
    {
      if( prefix == it->prefix() && ! it->isUnset() )
      {
          return &(*it);
      }
    }

    if( prefix == _xmlNamespace.prefix() )
    {
        return &_xmlNamespace;
    }

    return prefix.empty() ? &_empty : 0;
}


const Namespace* NamespaceContext::findPrefix(const Char* prefix, std::size_t n) const
{
    std::vector<Namespace>::const_reverse_iterator it;

    for(it = _namespaces.rbegin(); it != _namespaces.rend(); ++it)
    {
      if( 0 == it->prefix().compare(0, n, prefix) && 
          ! it->isUnset() )
      {
          return &(*it);
      }
    }

    if( 0 == _xmlNamespace.prefix().compare(0, n, prefix) )
    {
        return &_xmlNamespace;
    }

    return n == 0 ? &_empty : 0;
}


const Namespace* NamespaceContext::endElement(std::size_t depth, NamespaceMapping& nsmap, const String& prefix) const
{
    std::vector<Namespace>::const_reverse_iterator iter;

    for(iter = _namespaces.rbegin(); iter != _namespaces.rend(); ++iter)
    {
        if( iter->depth() < depth )
            break;

        std::vector<Namespace>::const_reverse_iterator it = iter;

        for(++it; it != _namespaces.rend(); ++it)
        {
          if( it->prefix() == iter->prefix() && ! it->isUnset() )
          {
              nsmap.addMapped(*it);
              break;
          }
        }

        if(it == _namespaces.rend())
            nsmap.addUnmapped(*iter);
    }

    for(iter = _namespaces.rbegin(); iter != _namespaces.rend(); ++iter)
    {
      if( prefix == iter->prefix() && ! iter->isUnset() )
      {
          return &(*iter);
      }
    }

    if( prefix == _xmlNamespace.prefix() )
    {
        return &_xmlNamespace;
    }

    return prefix.empty() ? &_empty : 0;
}


const Namespace* NamespaceContext::findUri(const String& ns) const
{
    std::vector<Namespace>::const_reverse_iterator it;

    for(it = _namespaces.rbegin(); it != _namespaces.rend(); ++it)
    {
      if( ns == it->namespaceUri() )
      {
          return &(*it);
      }
    }

    if( ns == _xmlNamespace.namespaceUri() )
        return &_xmlNamespace;

    return 0;
}


const Namespace* NamespaceContext::findUri(const Char* ns, std::size_t n) const
{
    std::vector<Namespace>::const_reverse_iterator it;

    for(it = _namespaces.rbegin(); it != _namespaces.rend(); ++it)
    {
      if( 0 == it->namespaceUri().compare(0, n, ns) )
      {
          return &(*it);
      }
    }

    if( 0 == _xmlNamespace.namespaceUri().compare(0, n, ns) )
    {
        return &_xmlNamespace;
    }

    return 0;
}


void NamespaceContext::pushNamespace(std::size_t depth, const String& prefix, const String& name)
{
    _namespaces.push_back( Namespace(depth, prefix, name) );
}


void NamespaceContext::pushDefaultNamespace(std::size_t depth, const String& name)
{
    // Namespace without prefix is default namespace
    _namespaces.push_back( Namespace(depth, String(), name) );
}


void NamespaceContext::unsetNamespace(std::size_t depth, const String& prefix)
{
    // Namespace without name is an explicitly unset namespace
    _namespaces.push_back( Namespace(depth, prefix, String()) );
}


void NamespaceContext::unsetDefaultNamespace(std::size_t depth)
{
    // Namespace without name is an explicitly unset namespace
    _namespaces.push_back( Namespace(depth, String(), String()) );
}


std::size_t NamespaceContext::popNamespace(std::size_t depth)
{
    std::size_t size = 0;
    while( ! _namespaces.empty() && _namespaces.back().depth() >= depth)
    {
        Namespace & ns = _namespaces.back();
        size += ns.namespaceUri().size() + ns.prefix().size();
        _namespaces.pop_back();
    }

    return size;
}

} // namespace Xml

} // namespace Pt

