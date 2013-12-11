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

#ifndef Pt_Xml_EndElement_h
#define Pt_Xml_EndElement_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/Xml/QName.h>
#include <Pt/Xml/Namespace.h>
#include <Pt/NonCopyable.h>

namespace Pt {

namespace Xml {

/** @brief Represents a closing element tag in an XML document.
*/
class EndElement : public Node 
                 , private NonCopyable
{
    public:
        /** @brief Constructs an empty EndElement.
        */
        EndElement()
        : Node(Node::EndElement)
        , _name(0)
        , _namespace(0)
        { }

        /** @brief Clears all content.
        */
        void clear()
        { 
            _name = 0; 
            _namespace = 0; 
            _nsmap.clear(); 
        }

        /** @brief Returns the qualified element name.
        */
        const QName& name() const
        { return *_name; }

        /** @brief Returns the qualified element name.
        */
        void setName(const QName& name, const Namespace& ns)
        { 
            _name = &name; 
            _namespace = &ns; 
        }

        /** @brief Returns the namespace Uri for this element name
        */
        const String& namespaceUri() const
        { return _namespace->namespaceUri(); }

        NamespaceMapping& namespaceMapping()
        { return _nsmap; }

        const NamespaceMapping& namespaceMapping() const
        { return _nsmap; }
        
        //! @internal
        inline static const Node::Type nodeId()
        { return Node::EndElement; }

    private:
        const QName* _name;
        const Namespace* _namespace;
        NamespaceMapping _nsmap;
};

/** @brief Casts a generic node to an EndElement node.
*/
inline EndElement* toEndElement(Node* node)
{
    return nodeCast<EndElement>(node);
}

/** @brief Casts a generic node to an EndElement node.
*/
inline const EndElement* toEndElement(const Node* node)
{
    return nodeCast<EndElement>(node);
}

/** @brief Casts a generic node to an EndElement node.
*/
inline EndElement& toEndElement(Node& node)
{
    return nodeCast<EndElement>(node);
}

/** @brief Casts a generic node to an EndElement node.
*/
inline const EndElement& toEndElement(const Node& node)
{
    return nodeCast<EndElement>(node);
}

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_EndElement_h
