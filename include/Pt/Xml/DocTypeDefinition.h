/*
 * Copyright (C) 2012 by Marc Boris Duerner
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

#ifndef Pt_Xml_DocTypeDefinition_h
#define Pt_Xml_DocTypeDefinition_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/Xml/QName.h>
#include <Pt/String.h>
#include <Pt/Types.h>
#include <Pt/NonCopyable.h>
#include <vector>

namespace Pt {

namespace Xml {

class ElementModel;
class ContentModel;
class AttributeListModel;
class Entity;
class Notation;

class DocTypeDefinition;

/** @brief The %DocTypeDefinition of an XML document.

    A %DocTypeDefinition contains all entity, notation, element and attribute
    list declarations from the external and internal DTD subsets.
*/
class PT_XML_API DocTypeDefinition : private NonCopyable
{
    public:
        /** @brief Construct an empty DocType node.
        */
        DocTypeDefinition();

        /** @brief Destructor.
        */
        ~DocTypeDefinition();

        /** @brief Clears all content.
        */
        void clear();

        /** @brief Returns true if a DTD was defined in the document.
        */
        bool isDefined() const;

        /** @brief Returns the documents root element name.
        */
        const QName& rootName() const;

        /** @brief Returns the documents root element name.
        */
        QName& rootName();

        //! @brief Returns the entity or a nullptr if already declared.
        Entity* declareEntity(const Pt::String& name);

        //! @brief Returns the entity or a nullptr if not declared.
        const Entity* findEntity(const Pt::String& name) const;

        //! @brief Removes the entity with the given name.
        void removeEntity(const Pt::String& name);

        //! @brief Returns the entity or a nullptr if already declared.
        Entity* declareParamEntity(const Pt::String& name);

        //! @brief Returns the entity or a nullptr if not declared.
        const Entity* findParamEntity(const Pt::String& name) const;

        //! @brief Removes the entity with the given name.
        void removeParamEntity(const Pt::String& name);

        //! @brief Returns the notation or a nullptr if already declared.
        Notation* declareNotation(const Pt::String& name);

        //! @brief Returns the notation or a nullptr if not declared.
        const Notation* findNotation(const Pt::String& name) const;

        //! @brief Removes the notation with the given name.
        void removeNotation(const Pt::String& name);

    public:
        //! @internal @brief Returns null if already declared.
        ContentModel& declareContent(const QName& name);

        //! @internal @brief Returns the attribute list for an element.
        AttributeListModel& declareAttributeList(const QName& name);

        //! @internal @brief Returns null if not declared.
        ElementModel* findElement(const QName& name);

        //! @internal @brief Returns null if not declared.
        AttributeListModel* findAttributes(const QName& name);

    private:
        typedef std::vector<ElementModel*> Elements;
        typedef std::vector<Notation*> Notations;
        typedef std::vector<Entity*> Entities;

        QName _rootName;
        Elements _elements;
        Entities _entities;
        Entities _paramEntities;
        Notations _notations;
        Pt::varint_t _r1; // allocator
        Pt::varint_t _r2;
};

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_DocTypeDefinition_h
