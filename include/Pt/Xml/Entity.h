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

#ifndef Pt_Xml_Entity_h
#define Pt_Xml_Entity_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/NonCopyable.h>
#include <Pt/String.h>
#include <map>

namespace Pt {

namespace Xml {

/** @brief An entity declaration in a DTD.
*/
class Entity
{
    public:
        /** @brief Constructs with entity name.
        */
        explicit Entity(const Pt::String& name)
        : _name(name)
        , _ndata(false)
        {}

        /** @brief Returns the name of the entity.
        */
        const Pt::String& name() const
        { return _name; }

        /** @brief Returns true if the entity in external.
        */
        bool isExternal() const
        { return ! _publicId.empty() || ! _systemId.empty(); }

        /** @brief Returns true if the entity in internal.
        */
        bool isInternal() const
        { return _publicId.empty() && _systemId.empty(); }

        /** @brief Returns the value of the entity.
        */
        const Pt::String& value() const
        { return _value; }

        /** @brief Returns the value of the entity.
        */
        Pt::String& value()
        { return _value; }

        /** @brief Sets the value of the entity.
        */
        void setValue(const Pt::String& val)
        {  _value = val; }

        /** @brief Returns the public ID of the entity.
        */
        const Pt::String& publicId() const
        { return _publicId; }

        /** @brief Sets the public ID of the entity.
        */
        void setPublicId(const Pt::String& pubId)
        { _publicId = pubId; }

        /** @brief Returns the system ID of the entity.
        */
        const Pt::String& systemId() const
        { return _systemId; }

        /** @brief Sets the system ID of the entity.
        */
        void setSystemId(const Pt::String& sysId)
        { _systemId = sysId; }

        /** @brief Indicates if the entity is unparsed (NDATA).
        */
        bool isUnparsed() const
        { return _ndata; }

        /** @brief Sets the notation of an unparsed entity (NDATA).
        */
        void setUnparsed(const Pt::String& notation)
        {   
            _ndata = true;
            _value = notation; 
        }

        /** @brief Returns the name of the notation.
        */
        const Pt::String& notationName() const
        { return _value; }

    private:
        Pt::String _name;
        Pt::String _publicId;
        Pt::String _systemId;
        Pt::String _value;
        bool _ndata;
};

/** @brief An entity reference XML node.

    The XmlReader normally reports only unresolved entity references, but
    can be configured to report all entity references. They can be ignored
    or treated as an error depending on the application.
*/
class EntityReference : public Node
                      , private NonCopyable 
{
    public:
        /** @brief Creates an EntityReference object.
        */
        EntityReference()
        : Node(Node::EntityReference)
        , _entity(0)
        { }

        /** @brief Clears all content.
        */
        void clear()
        {
            _name.clear();
            _entity = 0;
        }

        /** @brief Returns the name of the entity this reference refers to.
        */
        const Pt::String& name() const
        { return _name; }

        /** @brief Returns the name of the entity this reference refers to.
        */
        Pt::String& name()
        { return _name; }

        /** @brief Sets the name of the entity this reference refers to.
        */
        void setName(const Pt::String& name)
        { _name = name; }

        /** @brief Returns the entity this reference refers to or a nullptr.
        */
        const Entity* get() const
        { return _entity; }

        /** @brief Sets the entity this reference refers to.
        */
        void setEntity(const Entity* entity)
        { _entity = entity; }

        //! @internal
        inline static Node::Type nodeId()
        { return Node::EntityReference; }

    private:
        Pt::String _name;
        const Entity* _entity;
};

/** @brief Casts a generic node to an EntityReference node.
*/
inline EntityReference* toEntityReference(Node* node)
{
    return nodeCast<EntityReference>(node);
}

/** @brief Casts a generic node to an EntityReference node.
*/
inline const EntityReference* toEntityReference(const Node* node)
{
    return nodeCast<EntityReference>(node);
}

/** @brief Casts a generic node to an EntityReference node.
*/
inline EntityReference& toEntityReference(Node& node)
{
    return nodeCast<EntityReference>(node);
}

/** @brief Casts a generic node to an EntityReference node.
*/
inline const EntityReference& toEntityReference(const Node& node)
{
    return nodeCast<EntityReference>(node);
}

/** @internal @brief Replaces the entity with its string value.
*/
bool resolveDefaultEntity(String& entity);
        
/** @internal @brief Replaces the entity with its string value.
*/
bool resolveCharacterEntity(String& entity);

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_Entity_h
