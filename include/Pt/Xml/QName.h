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
#include <Pt/String.h>
#include <cstddef>

namespace Pt {

namespace Xml {

/** @brief A qualified XML name.

    A qualified XML name consists of a prefix separated by a colon from its
    local name. The prefix is used to refer to a namespace. This allows to
    distinguish between to attributes or elements with the same local name.
*/
class QName 
{
    public:
        /** @brief Constructs an empty qualified name.
        */
        QName()
        {}

        /** @brief Clears all content.
        */
        void clear()
        {
            _name.clear();
            _prefix.clear();
        }

        /** @brief Returns true if empty.
        */
        bool empty() const
        {
            return _prefix.empty() && _name.empty(); 
        }

        /** @brief Returns the size of the prefix and local part.
        */
        inline std::size_t size() const
        {
            return _prefix.size() + _name.size(); 
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

        /** @internal @brief Returns the local name.
        */
        String& name() 
        { return _name; }

        /** @internal @brief Returns the local name.
        */
        const String& name() const
        { return _name; }

        /** @brief Returns the local name.
        */
        String& local() 
        { return _name; }

        /** @brief Returns the local name.
        */
        const String& local() const
        { return _name; }

        /** @brief Sets the local name.
        */
        void setLocal(const String& name)
        { _name = name; }

    private:
        String _prefix;
        String _name;
};

/** @brief Returns true if equal.
*/
inline bool operator ==(const QName& a, const QName& b)
{
    return a.prefix() == b.prefix() && a.name() == b.name();
}

/** @brief Returns true if not equal.
*/
inline bool operator !=(const QName& a, const QName& b)
{
    return a.prefix() != b.prefix() || a.name() != b.name();
}

/** @brief Returns true if less.
*/
inline bool operator<(const QName& a, const QName& b)
{
	  return a.prefix() < b.prefix() ||
           ( ! (b.prefix() < a.prefix()) && a.name() < b.name() );
}

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_QName_h
