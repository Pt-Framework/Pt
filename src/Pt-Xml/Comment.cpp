#include "Pt/Xml/Comment.h"

#include <iostream>
using namespace std;


namespace Pt {

namespace Xml {

Comment::Comment(const String& text) 
: Node(Node::Comment)
, _text(text)
{

}


Comment::~Comment()
{
}


String& Comment::text()
{
	return _text;
}


const String& Comment::text() const
{
	return _text;
}


void Comment::setText(const String text)
{
	_text = text;
}

} // namespace Xml

} // namespace Pt
