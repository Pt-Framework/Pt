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

#ifndef Pt_Xml_NamespaceContext_h
#define Pt_Xml_NamespaceContext_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Namespace.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>
#include <vector>

namespace Pt {

namespace Xml {

/** @brief Manages all namespaces used in an XML document.
        
    @see Namespace
*/
class NamespaceContext : private NonCopyable
{
    public:
        /** Default constructor.
        */
        NamespaceContext();

        /** Destructor.
        */
        ~NamespaceContext();

        /** @brief Removes all namespaces.
        */
        void clear();

        /** @brief Returns the current default namespace.
        */
        const Namespace& getDefaultNamespace() const;

        /** @brief Returns the namespace for a prefix.
        */
        const Namespace* findPrefix(const String& prefix) const;

        /** @brief Returns the namespace for a prefix.
        */
        const Namespace* findPrefix(const Char* prefix, std::size_t n) const;

        /** @brief Returns the prefix for a namespace URI.
        */
        const Namespace* findUri(const String& ns) const;

        const Namespace* findUri(const Char* ns, std::size_t n) const;

        /** @brief Adds a namespace to the stack.
        */
        void pushNamespace(std::size_t depth, const String& prefix, const String& name);

        /** @brief Adds a default namespace to the stack.
        */
        void pushDefaultNamespace(std::size_t depth, const String& name);

        /** @brief Unsets a namespace.
        */
        void unsetNamespace(std::size_t depth, const String& prefix);

        /** @brief Unsets the default namespace.
        */
        void unsetDefaultNamespace(std::size_t depth);

        const Namespace* startElement(std::size_t depth, NamespaceMapping& nsmap, const String& prefix) const;

        const Namespace* endElement(std::size_t depth, NamespaceMapping& nsmap, const String& prefix) const;

        /** @brief Removes all namespaces at greater or equal depth.
        */
        std::size_t popNamespace(std::size_t depth);

        const Namespace& emptyNamespace() const
        { return _empty; }

    private:
        Namespace _empty;
        std::vector<Namespace> _namespaces;
        Namespace _xmlNamespace;
};

} // namespace Xml

} // namespace Pt

#endif
