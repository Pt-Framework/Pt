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
#include <vector>
#include <cstddef>

namespace Pt {

namespace Xml {

/** @brief A namespace used in an XML document.
  
   A namespace consists of a name, normally a namespace URI, a locally used
   prefix and the element depth which indicates the scope of the namespace.
*/
class Namespace 
{
    public:
        /** @brief Constructs a Namespace with scope depth, name and prefix.
        */
        Namespace(std::size_t depth, const String& prefix, const String& uri)
        : _depth(depth)
        , _prefix(prefix)
        , _uri(uri)
        { }

        /** @brief Returns the scope depth of the namespace.
        */
        std::size_t depth() const
        { return _depth; }
        
        /** @brief Returns the prefix of this namespace.
        */
        const String& prefix() const
        { return _prefix; }

        /** @brief Sets the prefix of this namespace.
        */
        void setPrefix(const String& prefix)
        { _prefix = prefix; }

        /** @brief Returns the namespace URI.
        */
        const String& namespaceUri() const
        { return _uri; }

        /** @brief Sets the URI of the namespace.
        */
        void setNamespaceUri(const String& uri)
        { _uri = uri; }

        /** @brief Returns true if this is the default namespace.
        */
        bool isDefaultNamespace() const
        { return _prefix.empty(); }

        /** @brief Returns true if explicitly unset.
        */
        bool isUnset() const
        { return _uri.empty(); }

    private:
        std::size_t _depth;
        String _prefix;
        String _uri;
};


/** @brief A list of namespaces, mapped or umapped to a prefix.
*/
class NamespaceMapping
{
    public:
        class Entry
        {
            public:
                Entry(const Namespace& ns, bool mapped)
                : _ns(&ns)
                , _mapped(mapped)
                {}

                bool isMapped() const
                { return _mapped; }

                bool isUnmapped() const
                { return ! _mapped; }

                const String& namespaceUri() const
                { return _ns->namespaceUri(); }

                const String& prefix() const
                { return _ns->prefix(); }
            
            private:
                const Namespace* _ns;
                bool _mapped;
        };

        typedef const Entry* ConstIterator;
        typedef Entry* Iterator;

    public:
        NamespaceMapping()
        {}

        void clear()
        { _entries.clear(); }

        std::size_t size() const
        { return _entries.size(); }

        std::size_t empty() const
        { return _entries.empty(); }

        ConstIterator begin() const
        { return _entries.empty() ? 0 : &_entries[0]; }

        ConstIterator end() const
        { return _entries.empty() ? 0 : &_entries[ _entries.size() ]; }
               
        Iterator begin()
        { return _entries.empty() ? 0 : &_entries[0]; }

        Iterator end()
        { return _entries.empty() ? 0 : &_entries[ _entries.size() ]; }

        void addMapped(const Namespace& ns)
        { _entries.push_back( Entry(ns, true) ); }

        void addUnmapped(const Namespace& ns)
        { _entries.push_back( Entry(ns, false) ); }

    private:
        std::vector<Entry> _entries;
};

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_Namespace_h
