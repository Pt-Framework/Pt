/*
 * Copyright (C) 2009 Marc Boris Duerner
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

/** @brief Entity resolver class which associates entities to resolved entity values.
 
    Entities can be added to this class using the method addEntity(). This method takes
    the entity and the resolved entity value. To resolve the resolves value for an entity
    the method resolveEntity() can be used.
 */
class PT_XML_API EntityResolver
{
    typedef std::map<String, String> EntityMap;

    public:
        /** @brief Initializes to use the XML default entities.
        */
        EntityResolver();

        //! @brief Destructor.
        ~EntityResolver();

        /** @brief Resets to use the XML default entities.
        */
        void clear();

        /** @brief Adds an entity to the lookup.

            To determine the resolved entity value of a entity the method resolveEntity() can be used.

            @param entity The entity name.
            @param token The characters the entity represents.
        */
        void addEntity(const String& entity, const String& token);

        /** @brief Replaces the entity by the the character it represents.

            @param entity The entity name to be replaced.
            @return True if the entity could be replaced.
        */
        bool resolveEntity(String& entity) const;

        /** @brief Returns the entity name for a character.

            If no entity is found, a null pointer is returned.
        */
        const Pt::Char* findEntity(String& str) const;

        /** @brief Returns the entity name for a string.

            If no entity is found, a null pointer is returned.
        */
        const Pt::Char* findEntity(Char ch) const;

        /** @brief Replaces characters with entities.
            
            If characters are found in @q str, which are represented by an
            entity, they will be replaced. The result is written to the output
            stream @ os.
        */
        void getEntity(std::basic_ostream<Char>& os, const Pt::Char* str) const;

    private:
        EntityMap _entityMap;
};

}

}

#endif
