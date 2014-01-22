/*
 * Copyright (C) 2006-2012 by Marc Boris Duerner
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

#ifndef Pt_Xml_Node_h
#define Pt_Xml_Node_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/XmlError.h>

namespace Pt {

namespace Xml {

/** @brief XML document node.

    The XmlReader reports the content of a XML document as XML nodes. A node
    might be a start element, an end element, characters, a comment or a
    doctype declaration. The specialized node classes have methods and data
    members to process the information specific to the node. So, this class
    serves more or less as an anchor for casting and as a common token type. 
    
    To cast to a derived node, a static_cast can be done after checking the
    node type, or the helper functions such as toStartElement() can be used.
*/
class Node 
{
    public:
        enum Type 
        {
            Unknown = 0,
            DocType = 1,
            EndDocType = 2,
            StartDocument = 3,
            EndDocument = 4,
            StartElement = 5,
            EndElement = 6,
            Characters = 7,
            Comment = 8,
            ProcessingInstruction = 9,
            EntityReference = 10
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
        throw XmlError("unexpected node type");

    return static_cast<T&>(node);
}

//! @internal
template <typename T>
const T& nodeCast(const Node& node)
{
    if( node.type() != T::nodeId() )
        throw XmlError("unexpected node type");

    return static_cast<const T&>(node);
}

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_Node_h
