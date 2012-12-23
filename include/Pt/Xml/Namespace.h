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
#ifndef Pt_Xml_Namespace_h
#define Pt_Xml_Namespace_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>

namespace Pt {

namespace Xml {

/** @brief A namespace used in an XML document.
  
   A namespace consists of a name, normally a namespace URI, a locally used
   prefix and the element depth which indicates te scope of the namespace.
  
   @see NamespaceContext
  */
class Namespace 
{
    public:
        /** @brief Constructs a Namespace with scope depth, name and prefix.
        */
        Namespace(unsigned depth, const String& prefix, const String& name)
        : _depth(depth)
        , _prefix(prefix)
        , _name(name)
        { }

        /** @brief Returns the scope depth of the namespace.
        */
        unsigned depth() const
        { return _depth; }
        
        /** @brief Returns the prefix of this namespace.
        */
        const String& prefix() const
        { return _prefix; }

        /** @brief Sets the prefix of this namespace.
        */
        void setPrefix(const String& prefix)
        { _prefix = prefix; }

        /** @brief Returns the namespace name.
        */
        const String& namespaceUri() const
        { return _name; }

        /** @brief Sets the name of the namespace.
        */
        void setNamespaceUri(const String& name)
        { _name = name; }

        /** @brief Returns true if this is the default namespace.
        */
        bool isDefaultNamespace() const
        { return _prefix.empty(); }

    private:
        unsigned _depth;
        String _prefix;
        String _name;
};

}

}

#endif
