#include "Pt/Xml/EndElement.h"

#include <iostream>
using namespace std;


namespace Pt {

namespace Xml {


EndElement::EndElement(const String& name)
: Node(Node::EndElement),
  _name(name)
{
}


EndElement::~EndElement()
{
}


String& EndElement::name()
{
	return _name;
}


const String& EndElement::name() const
{
	return _name;
}


void EndElement::setName(const String name)
{
	_name = name;
}


bool EndElement::operator==(const Node& node) const
{
	const EndElement* e = dynamic_cast<const EndElement*>(&node);
	if(!e) return false;

	return ( e->name() == this->name() );
}


} // namespace Xml

} // namespace Pt
