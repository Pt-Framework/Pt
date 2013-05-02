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

/** @brief A single attribute of a start element.
*/
class PT_XML_API Attribute
{
    public:
        //! Default constructor.
        Attribute()
        : _namespace(0)
        { }

        /** @brief Returns the qualified name.
        */
        const QName& qname() const
        { return _qname; }

        /** @brief Returns the qualified name.
        */
        QName& qname()
        { return _qname; }

        /** @brief Returns the namespace prefix.
        */
        const String& prefix() const
        { return _qname.prefix(); }

        /** @brief Returns the name of this attribute.
        */
        const String& name() const
        { return _qname.name(); }

        const String& namespaceUri() const
        { return _namespace ? _namespace->namespaceUri() : _qname.prefix(); }
        
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

        void normalize();

        void clear()
        { 
            _qname.clear(); 
            _value.clear(); 
            _namespace = 0;
        }

    private:
        QName _qname;
        String _value;
        const Namespace* _namespace;
};


class PT_XML_API AttributeList : private NonCopyable
{
    public:
        typedef Attribute* Iterator;
        typedef const Attribute* ConstIterator;

    public:
        AttributeList();
        
        bool empty() const;
        
        void clear();

        Attribute& push();

        void pop();
        
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

    private:
        std::vector<Attribute> _container;
        Attribute* _begin;
        Attribute* _end;
        std::size_t _size;
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
        /** Constructs a StartElement object with no name and an empty attribute list.
        */
        StartElement()
        : Node(Node::StartElement)
        , _namespace(0)
        { }

        /** @brief Resets the start element.
        */
        void clear()
        {
            _nameRef.clear();
            _attributes.clear();
            _namespace = 0;
        }

        /** @brief Returns the qualified name.
        */
        const QNameRef& qname() const
        { return _nameRef; }

        QNameRef& qname()
        { return _nameRef; }

        /** @brief Returns the namespace prefix.
        */
        const Char* prefixPtr() const
        { return _nameRef.prefix(); }
        
        /** @brief Returns the local name.
        */
        const Char* namePtr() const
        { return _nameRef.name(); }

        /** @brief Returns the namespace Uri for this element name
        */
        const String& namespaceUri() const
        { return _namespace->namespaceUri(); }
        
        /** @brief Sets the namespace context for this element
        */      
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
        QNameRef _nameRef;
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
