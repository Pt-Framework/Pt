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

namespace Pt {

namespace Xml {

class Characters;
class Attribute;

class Entity
{
    public:
        Entity()
        {}
     
        Entity(Pt::Char val)
        : _value(1, val)
        {}
        
        Entity(const Pt::String& value)
        : _value(value)
        {}

        bool isExternal() const
        { return ! _publicId.empty() || ! _systemId.empty(); }

        bool isInternal() const
        { return _publicId.empty() && _systemId.empty(); }

        const Pt::String& value() const
        { return _value; }

        void setValue(const Pt::String& val)
        {  _value = val; }

        const Pt::String& publicId() const
        { return _publicId; }

        void setPublicId(const Pt::String& pubId)
        { _publicId = pubId; }

        const Pt::String& systemId() const
        { return _systemId; }

        void setSystemId(const Pt::String& sysId)
        { _systemId = sysId; }

    private:
        Pt::String _publicId;
        Pt::String _systemId;
        Pt::String _value;
};


class PT_XML_API EntityReference : public Node
                                 , private NonCopyable 
{
    public:
        /** @brief Creates an EntityReference object.
        */
        EntityReference();

        void set(const Pt::String& name, const Entity* entity, Pt::Xml::Characters* chars);

        void set(const Pt::String& name, const Entity* entity, Attribute* attr);

        const Pt::String& name() const
        { return _name; }
        
        const Entity* get() const
        { return _entity; }
  
        void resolve(const Pt::Char* value) const;

        //! @internal
        inline static Node::Type nodeId()
        { return Node::EntityReference; }

    private:
        Pt::String _name;
        const Entity* _entity;

        // TODO: use common base class
        Pt::Xml::Characters* _chars;
        Attribute* _attr;
};


inline EntityReference* toEntityReference(Node* node)
{
    return nodeCast<EntityReference>(node);
}


inline const EntityReference* toEntityReference(const Node* node)
{
    return nodeCast<EntityReference>(node);
}


inline EntityReference& toEntityReference(Node& node)
{
    return nodeCast<EntityReference>(node);
}


inline const EntityReference& toEntityReference(const Node& node)
{
    return nodeCast<EntityReference>(node);
}

} // namespace Xml

} // namespace Pt

#endif
