#ifndef Xml_StartElement_h
#define Xml_StartElement_h

#include <Pt/Xml/Node.h>
#include <Pt/Xml/Namespace.h>
#include <Pt/Xml/NamespaceContext.h>
#include <Pt/Api.h>
#include <Pt/Text/String.h>

#include <list>
#include <iostream>


namespace Pt {

namespace Xml {

	class PT_EXPORT Attribute
	{
		public:
			Attribute();

			Attribute(const String& name, const String& value);

			virtual ~Attribute();

			const String& name() const
			{return _name;}

			void setName(const String name)
			{_name = name;}

			const String& value() const
			{return _value;}

			void setValue(const String value)
			{_value = value;}

		private:
			String _name;
			String _value;
	};


	class PT_EXPORT StartElement : public Node
	{
		public:
			StartElement();

			StartElement(const String& name);

			~StartElement();

			StartElement* clone() const
			{return new StartElement(*this);}

			String& name() {return _name;}

			const String& name() const
			{return _name;}

			void setName(const String name)
			{_name = name;}

			void addAttribute(const Attribute& attr)
			{_attributes.push_back(attr);}

			const std::list<Attribute>& attributes() const
			{return _attributes;}

			const String& attribute(const String attributeName) const;

			bool hasAttribute(const String attributeName) const;

			const NamespaceContext& namespaceContext() const
			{return _namespaceContext;}

			void setNamespaceContext(const NamespaceContext& context)
			{_namespaceContext = context;}

			const String& namespaceUri(const String& prefix) const
			{return _namespaceContext.namespaceUri(prefix);}

			virtual bool operator==(const Node& node) const;

		private:
			String _name;
			std::list<Attribute> _attributes;
			NamespaceContext _namespaceContext;
	};

}

}

#endif
