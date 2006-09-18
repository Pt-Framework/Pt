#include "Pt/Xml/StartElement.h"

#include <iostream>
using namespace std;


namespace Pt {

namespace Xml {

Attribute::Attribute()
{
}


Attribute::Attribute(const String& name, const String& value) 
: _name(name), _value(value)
{
}


Attribute::~Attribute()
{
}





StartElement::StartElement() 
: Node(Node::StartElement)
{

}


StartElement::StartElement(const String& name)
: Node(Node::StartElement),
  _name(name)
{
}


StartElement::~StartElement()
{
}


const String& StartElement::attribute(const String attributeName) const
{
	static const String null;
	
	for(std::list<Attribute>::const_iterator it = _attributes.begin(); it != _attributes.end(); ++it) {
		if(it->name() == attributeName) {
			return it->value();
		}
	}
	
	return null;
}


bool StartElement::hasAttribute(const String attributeName) const
{
	for(std::list<Attribute>::const_iterator it = _attributes.begin(); it != _attributes.end(); ++it) {
		if(it->name() == attributeName) {
			return true;
		}
	}
	
	return false;
}


bool StartElement::operator==(const Node& node) const
{
	const StartElement* se = dynamic_cast<const StartElement*>(&node);
	if(!se) return false;

	return ( se->name() == this->name() );
}

} // namespace Xml

} // namespace Pt
