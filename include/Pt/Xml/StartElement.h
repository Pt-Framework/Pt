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

#ifndef Pt_Xml_StartElement_h
#define Pt_Xml_StartElement_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/Node.h>
#include <Pt/Xml/QName.h>
#include <Pt/Xml/Namespace.h>
#include <Pt/NonCopyable.h>
#include <Pt/String.h>
#include <vector>

namespace Pt {

namespace Xml {

class NamespaceContext;

/** @brief A single attribute of a start element.
*/
class Attribute
{
    public:
        /** @brief Default constructor.
        */
        Attribute()
        : _namespace(0)
        { }

        /** @brief Returns the qualified name.
        */
        const QName& name() const
        { return *_name; }

        /** @brief Sets the qualified name and namespace.
        */
        void setName(const QName& name, const Namespace& ns)
        {
            _name = &name;
            _namespace = &ns;
        }

        /** @brief Returns the namespaceUri.
        */
        const String& namespaceUri() const
        { return _namespace->namespaceUri(); }

        /** @brief Sets the namespace.
        */
        void setNamespace(const Namespace& ns)
        { _namespace = &ns; }

        /** @brief Returns the value of this attribute.
        */
        const String& value() const
        { return _value; }

        /** @brief Returns the value of this attribute.
        */
        String& value()
        { return _value; }

        /** @brief Clears the value of this attribute.
        */
        void clear()
        { _value.clear(); }

    private:
        const QName* _name;
        const Namespace* _namespace;
        String _value;
};

/** @brief An attribute list of an XML element.
*/
class PT_XML_API AttributeList : private NonCopyable
{
    public:
        typedef Attribute* Iterator;
        typedef const Attribute* ConstIterator;

    public:
        /** @brief Construct with namespace context.
        */
        explicit AttributeList(NamespaceContext& nsctx)
        : _begin(0)
        , _end(0)
        , _size(0)
        , _nsctx(&nsctx)
        { }
        
        /** @brief Clears all content.
        */
        void clear()
        { 
            _begin = 0;
            _end = 0;
            _size = 0;
        }

        /** @brief Appends a new element to the end of the list.

            A new attribute is added to the end of the list with the name
            @a name in the namespace @a ns
        */
        Attribute& append(const QName& name, const Namespace& ns);

		/** @brief Gets the value of an attribute.

            The value of the attribute with the local name @a localName is
            returned. If no attribute by that name is present, an
            exception of type XmlError is thrown.
        */
		const Pt::String& get(const String& localName) const;

		/** @brief Gets the value of an attribute.

            The value of the attribute with the local name @a localName is
            returned. If no attribute by that name is present, an
            exception of type XmlError is thrown.
        */
		const Pt::String& get(const Char* localName) const;

        /** @brief Gets the value of an attribute.

            The value of the attribute with the namespace @a nsUri and the
            local name @a localName is returned. If no attribute by that name is present, an
            exception of type XmlError is thrown.
        */
        const Pt::String& get(const String& nsUri, const String& localName) const;

        /** @brief Gets the value of an attribute.

            The value of the attribute with the namespace @a nsUri and the
            local name @a localName is returned. If no attribute by that name is present, an
            exception of type XmlError is thrown.
        */
        const Pt::String& get(const Char* nsUri, const Char* localName) const;

        /** @brief Finds an attribute by name.

            An iterator to the the attribute with the local name @a localName is
            returned. If no attribute by that name is present, an iterator to
            the end of the sequence is returned.
        */
        Iterator find(const String& localName);

        /** @brief Finds an attribute by name.

            An iterator to the the attribute with the local name @a localName is
            returned. If no attribute by that name is present, an iterator to
            the end of the sequence is returned.
        */
        Iterator find(const Char* localName);

        /** @brief Finds an attribute by name.

            An iterator to the the attribute with the the namespace @a nsUri 
            and the local name @a localName is returned. If no attribute by
            that name is present, an iterator to the end of the sequence is
            returned.
        */
        Iterator find(const String& nsUri, const String& localName);

        /** @brief Finds an attribute by name.

            An iterator to the the attribute with the the namespace @a nsUri 
            and the local name @a localName is returned. If no attribute by
            that name is present, an iterator to the end of the sequence is
            returned.
        */
        Iterator find(const Char* nsUri, const Char* localName);

        /** @brief Finds an attribute by name.

            An iterator to the the attribute with the local name @a localName is
            returned. If no attribute by that name is present, an iterator to
            the end of the sequence is returned.
        */
        ConstIterator find(const String& localName) const;

        /** @brief Finds an attribute by name.

            An iterator to the the attribute with the local name @a localName is
            returned. If no attribute by that name is present, an iterator to
            the end of the sequence is returned.
        */
        ConstIterator find(const Char* localName) const;

        /** @brief Finds an attribute by name.

            An iterator to the the attribute with the the namespace @a nsUri 
            and the local name @a localName is returned. If no attribute by
            that name is present, an iterator to the end of the sequence is
            returned.
        */
        ConstIterator find(const String& nsUri, const String& localName) const;

        /** @brief Finds an attribute by name.

            An iterator to the the attribute with the the namespace @a nsUri 
            and the local name @a localName is returned. If no attribute by
            that name is present, an iterator to the end of the sequence is
            returned.
        */
        ConstIterator find(const Char* nsUri, const Char* localName) const;

        /** @brief Checks if an attribute is present.
        */
        bool has(const String& localName) const
        { 
            return find(localName) != end();
        }

        /** @brief Checks if an attribute is present.
        */
        bool has(const Char* localName) const
        { 
            return find(localName) != end();
        }

        /** @brief Checks if an attribute is present.
        */
        bool has(const String& nsUri, const String& localName) const
        { 
            return find(nsUri, localName) != end();
        }

        /** @brief Checks if an attribute is present.
        */
        bool has(const Char* nsUri, const Char* localName) const
        { 
            return find(nsUri, localName) != end();
        }

        /** @brief An iterator to the begin of the sequence.
        */
        Iterator begin()
        { return _begin; }

        /** @brief An iterator to the end of the sequence.
        */
        Iterator end()
        { return _end; }

        /** @brief An iterator to the begin of the sequence.
        */
        ConstIterator begin() const
        { return _begin; }

        /** @brief An iterator to the end of the sequence.
        */
        ConstIterator end() const
        { return _end; }

        /** @brief Returns true if the list is empty.
        */
        bool empty() const
        { return _size == 0; }

        /** @brief Returns the size of the list.
        */
        std::size_t size() const
        { return _size; }

        /** @brief Returns the namespace context for the attributes.
        */
        NamespaceContext& namespaceContext()
        { return *_nsctx; }

    private:
        std::vector<Attribute> _container;
        Attribute* _begin;
        Attribute* _end;
        std::size_t _size;
        NamespaceContext* _nsctx;
};

/** @brief Represents the start of an element in an XML document.
  
    A start element node is reported when the XML reader parsed the opening
    tag of an XML element. It contains the name of the XML element, its
    namespace information, and the attributes of the XML element.
*/
class StartElement : public Node
                   , private NonCopyable
{
    public:
        /** @brief Constructs an empty %StartElement.
        */
        StartElement(NamespaceContext& nsctx)
        : Node(Node::StartElement)
        , _name(0)
        , _namespace(0)
        , _attributes(nsctx)
        { }

        /** @brief Clears all content.
        */
        void clear()
        { 
            _name = 0; 
            _namespace = 0; 
            _nsmap.clear(); 
        }

        /** @brief Returns the qualified name.
        */
        const QName& name() const
        { return *_name; }

        /** @brief Sets the qualified name.
        */
        void setName(const QName& n, const Namespace& ns)
        {
            _name = &n;
            _namespace = &ns;
        }

        /** @brief Returns the namespace Uri for this element name.
        */
        const String& namespaceUri() const
        { return _namespace->namespaceUri(); }

        /** @brief Sets the namespace.
        */
        void setNamespace(const Namespace& ns)
        { _namespace = &ns; }
        
        /** @brief Returns the attributes of the element.
        */
        const AttributeList& attributes() const
        { return _attributes; }

        /** @brief Returns the attributes of the element.
        */
        AttributeList& attributes()
        { return _attributes; }

        NamespaceMapping& namespaceMapping()
        { return _nsmap; }

        const NamespaceMapping& namespaceMapping() const
        { return _nsmap; }

        //! @internal
        inline static const Node::Type nodeId()
        { return Node::StartElement; }

    private:
        const QName* _name;
        const Namespace* _namespace;
        AttributeList _attributes;
        NamespaceMapping _nsmap;
};

/** @brief Casts a generic node to a %StartElement node.
*/
inline StartElement* toStartElement(Node* node)
{
    return nodeCast<StartElement>(node);
}

/** @brief Casts a generic node to a %StartElement node.
*/
inline const StartElement* toStartElement(const Node* node)
{
    return nodeCast<StartElement>(node);
}

/** @brief Casts a generic node to a %StartElement node.
*/
inline StartElement& toStartElement(Node& node)
{
    return nodeCast<StartElement>(node);
}

/** @brief Casts a generic node to a %StartElement node.
*/
inline const StartElement& toStartElement(const Node& node)
{
    return nodeCast<StartElement>(node);
}

} // namespace Xml

} // namespace Pt

#endif // Pt_Xml_StartElement_h
