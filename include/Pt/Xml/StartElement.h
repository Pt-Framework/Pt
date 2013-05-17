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
        //! Default constructor.
        Attribute()
        : _namespace(0)
        { }

        /** @brief Returns the qualified name.
        */
        const QName& name() const
        { return *_name; }

        /** @brief Sets the qualified name and namespace.
        */
        void set(const QName& name, const Namespace& ns)
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

/** @brief An attribute list of a start element.
*/
class PT_XML_API AttributeList : private NonCopyable
{
    public:
        typedef Attribute* Iterator;
        typedef const Attribute* ConstIterator;

    public:
        explicit AttributeList(NamespaceContext& nsctx)
        : _begin(0)
        , _end(0)
        , _size(0)
        , _nsctx(&nsctx)
        { }

        void clear()
        { 
            _begin = 0;
            _end = 0;
            _size = 0;
        }

        Attribute& append(const QName& name, const Namespace& ns);
        
        ConstIterator find(const String& localName) const;

        ConstIterator find(const String& nsUri, const String& localName) const;

        bool has(const String& name) const;

        bool has(const String& nsUri, const String& name) const;

        Iterator begin()
        { return _begin; }

        Iterator end()
        { return _end; }

        ConstIterator begin() const
        { return _begin; }

        ConstIterator end() const
        { return _end; }

        bool empty() const
        { return _size == 0; }

        std::size_t size() const
        { return _size; }

        NamespaceContext& namespaceContext()
        { return *_nsctx; }

    private:
        std::vector<Attribute> _container;
        Attribute* _begin;
        Attribute* _end;
        std::size_t _size;
        NamespaceContext* _nsctx;
};

/** @brief Represents an opening tag in an XML document.
  
    A start element is created when the parser reaches a start tag. It contains
    the name of the tag, its namespace information, and the attributes of
    the tag.
*/
class StartElement : public Node
                   , private NonCopyable
{
    public:
        /** Constructs an empty StartElement.
        */
        StartElement(NamespaceContext& nsctx)
        : Node(Node::StartElement)
        , _name(0)
        , _namespace(0)
        , _attributes(nsctx)
        { }

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

        /** @brief Returns the namespace Uri for this element name
        */
        const String& namespaceUri() const
        { return _namespace->namespaceUri(); }

        void setNamespace(const Namespace& ns)
        { _namespace = &ns; }
        
        /** @brief Returns the attributes of the tag.
        */
        const AttributeList& attributes() const
        { return _attributes; }

        /** @brief Returns the attributes of the tag.
        */
        AttributeList& attributes()
        { return _attributes; }

        //! @internal
        inline static const Node::Type nodeId()
        { return Node::StartElement; }

    private:
        const QName* _name;
        const Namespace* _namespace;
        AttributeList _attributes;
};


inline StartElement* toStartElement(Node* node)
{
    return nodeCast<StartElement>(node);
}


inline const StartElement* toStartElement(const Node* node)
{
    return nodeCast<StartElement>(node);
}


inline StartElement& toStartElement(Node& node)
{
    return nodeCast<StartElement>(node);
}


inline const StartElement& toStartElement(const Node& node)
{
    return nodeCast<StartElement>(node);
}

} // namespace Xml

} // namespace Pt

#endif
