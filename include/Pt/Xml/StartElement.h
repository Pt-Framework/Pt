/*
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
#include <Pt/Xml/Namespace.h>
#include <Pt/NonCopyable.h>
#include <Pt/String.h>
#include <vector>

namespace Pt {

namespace Xml {

/** @brief A single attribute of a start element.
*/
class Attribute
{
    public:
        //! Default constructor.
        Attribute()
        : _namespace(0)
        { }

        String& prefix() 
        { return _prefix; }

        const String& prefix() const
        { return _prefix; }

        /** @brief Returns the name of this attribute.
        */
        const String& name() const
        { return _name; }

        String& name()
        { return _name; }

        /** @brief Sets the name of this attribute.
        */
        void setName(const String& name)
        { _name = name; }

        const String& namespaceUri() const
        { return _namespace ? _namespace->name() : _prefix; }
        
        void setNamespace(const Namespace& ns)
        { _namespace = &ns; }

        /** @brief Returns the value of this attribute.
        */
        const String& value() const
        { return _value; }

        String& value()
        { return _value; }

        /** @brief Sets the value of this attribute.
        */
        void setValue(const String& value)
        { _value = value; }

        void clear()
        { 
            _name.clear(); 
            _value.clear(); 
            _prefix.clear(); 
            _namespace = 0;
        }

    private:
        String _prefix;
        String _name;
        String _value;
        const Namespace* _namespace;
};

class PT_XML_API AttributeList : private NonCopyable
{
    public:
        typedef std::vector<Attribute> Container;
        typedef Container::iterator Iterator;
        typedef Container::const_iterator ConstIterator;

    public:
        AttributeList()
        {}

        void clear()
        { _container.clear(); }

        void add(const Attribute& attr)
        { _container.push_back(attr); }
        
        ConstIterator find(const String& attributeName) const;

        ConstIterator find(const String& nsUri, const String& name) const;

        bool has(const String& name) const
        { return find(name) != end();}

        bool has(const String& nsUri, const String& name) const
        { return find(nsUri, name) != end();}

        Iterator begin()
        { return _container.begin(); }

        Iterator end()
        { return _container.end(); }

        ConstIterator begin() const
        { return _container.begin(); }

        ConstIterator end() const
        { return _container.end(); }

    private:
        std::vector<Attribute> _container;
};

/** @brief Represents an opening tag in an XML document.
  
    A start element is created when the parser reaches a start tag. It contains
    the name of the tag, its namespace information, and the attributes of
    the tag.
  */
class PT_XML_API StartElement : public Node
                              , private NonCopyable
{
    public:
        //! Constructs a new StartElement object with no name and an empty attribute list.
        StartElement()
        : Node(Node::StartElement)
        , _namespace(0)
        { }

        void clear()
        {
            _name.clear();
            _prefix.clear();
            _attributes.clear();
            _namespace = 0;
        }

        /** @brief Returns the namespace prefix.
        */
        String& prefix() 
        { return _prefix; }

        /** @brief Returns the namespace prefix.
        */
        const String& prefix() const
        { return _prefix; }

        /** @brief Sets the namespace prefix.
        */
        void setPrefix(const String& prefix)
        { _prefix = prefix; }

        /** @brief Returns the local name.
        */
        String& name() 
        { return _name; }

        /** @brief Returns the local name.
        */
        const String& name() const
        { return _name; }

        /** @brief Sets the local name.
        */
        void setName(const String& name)
        { _name = name; }

        const String& namespaceUri() const
        { return _namespace ? _namespace->name() : _prefix; }
        
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

        /** @brief Returns the value of an attribute.
        */
        const String& attribute(const String& name) const;

        /** @brief Returns the value of an attribute.
        */
        const String& attribute(const String& nsUri, const String& name) const;

        inline static const Node::Type nodeId()
        { return Node::StartElement; }

    private:
        String _prefix;
        String _name;
        AttributeList _attributes;
        const Namespace* _namespace;
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
