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

#ifndef Pt_Xml_Notation_h
#define Pt_Xml_Notation_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>

namespace Pt {

namespace Xml {

/** @brief A notation declaration in a DTD.
*/
class Notation 
{
    public:
        /** @brief Constructs with notation name.
        */
        explicit Notation(const Pt::String& name)
        : _name(name)
        { }

        /** @brief Returns the name of the notation.
        */
        const Pt::String& name() const
        { return _name; }

        /** @brief Returns the public ID of the notation.
        */
        const Pt::String& publicId() const
        { return _publicId; }

        /** @brief Sets the public ID of the notation.
        */
        void setPublicId(const Pt::String& pubId)
        { _publicId = pubId; }

        /** @brief Returns the system ID of the notation.
        */
        const Pt::String& systemId() const
        { return _systemId; }

        /** @brief Sets the system ID of the notation.
        */
        void setSystemId(const Pt::String& sysId)
        { _systemId = sysId; }

    private:
        Pt::String _name;
        Pt::String _publicId;
        Pt::String _systemId;
};

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_Notation_h
