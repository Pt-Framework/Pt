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

   A Node may for example be a opening tag, a closing tag, a comment or a doctype declaration.
   The supported node types are contained in the enum Type. To determine the type of a Node the
   method type() can be used.

   For every supported node type (except "Unknown") a specialized class exists that is derived
   from this Node class. Those classes contain more data and access methods to allow the user
   to determine the information specific to the node, for example the tag name for a StartElement.

   This class mainly provides the method type() to determine the type of the Node. The user
   may use this information to determine to which specialized class that is associated
   with the type this object can be cast; for the Node::StartElement type the Node object can be
   cast to StartElement, for example.
*/
class Node 
{
    public:
        enum Type 
        {
            //! Unknown Node type (may not currently be supported)
            Unknown = 0,

            //! Xml declaration
            StartDocument = 1,

            //! Doctype
            DocType = 2,

            //! End of the document
            EndDocument = 3,

            //! Start element aka opening tag
            StartElement = 4,

            //! End element aka closing tag
            EndElement = 5,

            //! Parsed content of a tag's body
            Characters = 6,

            //! Comment
            Comment = 7,

            //! Processing instruction
            ProcessingInstruction = 8
        };

        //! @brief Destructor.
        virtual ~Node()
        {}

        /** @brief Returns the type of the Node.

            This information may be used to determine to which specialized Node class that is associated
            with the type, this Node object can be cast; for the Node::StartElement type the Node object
            can be cast to StartElement, for example.

            @return The type of this node.
          */
        Type type() const
        { return _type; }

    protected:
        /** @brief Constructs a new Node object with the specified node type
        */
        Node(Type type)
        : _type(type)
        { }

    private:
        Type _type;
};


template <typename T>
T* nodeCast(Node* node)
{
    T* e = 0;
        
    if( node->type() == T::nodeId() )
        e = static_cast<T*>(node);

    return e;
}


template <typename T>
const T* nodeCast(const Node* node)
{
    const T* e = 0;
        
    if( node->type() == T::nodeId() )
        e = static_cast<const T*>(node);

    return e;
}


template <typename T>
T& nodeCast(Node& node)
{
    if( node.type() != T::nodeId() )
        throw XmlError("unexpected node type");

    return static_cast<T&>(node);
}


template <typename T>
const T& nodeCast(const Node& node)
{
    if( node.type() != T::nodeId() )
        throw XmlError("unexpected node type");

    return static_cast<const T&>(node);
}

} // namespace Xml

} // namespace Pt

#endif
