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

#ifndef PT_JSON_NODE_H
#define PT_JSON_NODE_H

#include <Pt/Json/Api.h>
#include <Pt/Json/JsonError.h>

namespace Pt {

namespace Json {

/** @brief JSON document node.

    The JsonReader reports the content of a JSON document as nodes. A node
    might be a start object, an end object, string, a start ducument or an
    end document. The specialized node classes have methods and data members
    to process the information specific to the node. So, this class serves
    more or less as an anchor for casting and as a common token type. 
    
    To cast to a derived node, a static_cast can be done after checking the
    node type, or the helper functions such as toStartObject() can be used.
*/
class Node 
{
    public:
        enum Type 
        {
            Unknown = 0,
            StartDocument = 1,
            EndDocument = 2,
            Null = 3,
            Boolean = 4,
            String = 5,
            Integer = 6,
            Float = 7,
            StartArray = 8,
            EndArray = 9,
            StartObject = 10,
            Member = 11,
            EndObject = 12
        };

        //! @brief Destructor.
        virtual ~Node()
        {}

        /** @brief Returns the type of the node.
        */
        Type type() const
        { return _type; }

    protected:
        /** @brief Constructs a new Node object with the specified node type
        */
        explicit Node(Type type)
        : _type(type)
        { }

    private:
        Type _type;
};

//! @internal
template <typename T>
T* nodeCast(Node* node)
{
    T* e = 0;
        
    if( node->type() == T::nodeId() )
        e = static_cast<T*>(node);

    return e;
}

//! @internal
template <typename T>
const T* nodeCast(const Node* node)
{
    const T* e = 0;
        
    if( node->type() == T::nodeId() )
        e = static_cast<const T*>(node);

    return e;
}

//! @internal
template <typename T>
T& nodeCast(Node& node)
{
    if( node.type() != T::nodeId() )
        throw JsonError("unexpected node type");

    return static_cast<T&>(node);
}

//! @internal
template <typename T>
const T& nodeCast(const Node& node)
{
    if( node.type() != T::nodeId() )
        throw JsonError("unexpected node type");

    return static_cast<const T&>(node);
}

} // namespace

} // namespace

#endif // nclude guard
