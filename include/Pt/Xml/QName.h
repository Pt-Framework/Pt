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
#ifndef Pt_Xml_QName_h
#define Pt_Xml_QName_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Namespace.h>
#include <Pt/String.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Xml {

/** @brief A namespace qualified XML name
*/
class QName 
{
    public:
        /** @brief Constructs an empty qualified name.
        */
        QName()
        : _namespace(0)
        {}

        /** @brief Clears all content.
        */
        void clear()
        {
            _name.clear();
            _prefix.clear();
            _namespace = 0;
        }

        /** @brief Returns the namespace prefix.
        */
        String& prefix() 
        { return _prefix; }

        /** @brief Returns the namespace prefix.
        */
        const String& prefix() const
        { return _prefix; }

        /** @brief Sets the namespace prefix.
        */
        void setPrefix(const String& prefix)
        { _prefix = prefix; }

        /** @brief Returns the local name.
        */
        String& name() 
        { return _name; }

        /** @brief Returns the local name.
        */
        const String& name() const
        { return _name; }

        /** @brief Sets the local name.
        */
        void setName(const String& name)
        { _name = name; }

        /** @brief Returns the namespace Uri for this element name
        */
        const String& namespaceUri() const
        { return _namespace ? _namespace->namespaceUri() : _prefix; }
        
        /** @brief Sets the namespace context for this element
        */
        void setNamespace(const Namespace& ns)
        { _namespace = &ns; }

    private:
        String _prefix;
        String _name;
        const Namespace* _namespace;
};

} // namespace Xml

} // namespace Pt

#endif
