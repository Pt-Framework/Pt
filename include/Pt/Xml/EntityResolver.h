/*
 * Copyright (C) 2009-2012 Marc Boris Duerner
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
#ifndef Pt_Xml_EntityResolver_h
#define Pt_Xml_EntityResolver_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <map>

namespace Pt {

namespace Xml {

/** @brief Handles character and entity references.
*/
class PT_XML_API EntityResolver
{
    typedef std::map<String, String> EntityMap;

    public:
        /** @brief Constructs with the XML default entities.
        */
        EntityResolver();

        //! @brief Destructor.
        ~EntityResolver();

        /** @brief Resets to use the XML default entities.
        */
        void clear();

        /** @brief Adds an entity reference to the lookup.
        */
        void addEntity(const String& entity, const String& token);

        /** @brief Replaces the entity with its string value.
        */
        bool resolveEntity(String& entity) const;

        /** @brief Returns the entity reference for a character.

            If no entity reference is found, a null pointer is returned.
        */
        const Pt::Char* findEntity(Char ch) const;

        /** @brief Replaces characters with entities.
            
            If characters are found in @a str, which are represented by an
            entity reference, they will be replaced. The result is written
            to the output stream @a os.
        */
        void getEntity(std::basic_ostream<Char>& os, const Pt::Char* str) const;

    private:
        EntityMap _entityMap;
};

} // namespace Xml

} // namespace Pt

#endif
