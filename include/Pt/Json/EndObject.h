/*
   Copyright (C) 2015-2023 by Dr. Marc Boris Duerner
  
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#ifndef PT_JSON_ENDOBJECT_H
#define PT_JSON_ENDOBJECT_H

#include <Pt/Json/Api.h>
#include <Pt/Json/Node.h>

namespace Pt {

namespace Json {

/** @brief Represents the end of an object.
*/
class EndObject : public Node 
{
    public:
        /** @brief Constructor.
        */
        EndObject()
        : Node(Node::EndObject)
        { }

        //! @internal
        inline static const Node::Type nodeId()
        { return Node::EndObject; }
};

/** @brief Casts a generic node to a EndObject node.

    @related EndObject
*/
inline EndObject* toEndObject(Node* node)
{
    return nodeCast<EndObject>(node);
}

/** @brief Casts a generic node to a EndObject node.

    @related EndObject
*/
inline const EndObject* toEndObject(const Node* node)
{
    return nodeCast<EndObject>(node);
}

/** @brief Casts a generic node to a EndObject node.

    @related EndObject
*/
inline EndObject& toEndObject(Node& node)
{
    return nodeCast<EndObject>(node);
}

/** @brief Casts a generic node to a EndObject node.

    @related EndObject
*/
inline const EndObject& toEndObject(const Node& node)
{
    return nodeCast<EndObject>(node);
}

} // namespace

} // namespace

#endif // include guard
