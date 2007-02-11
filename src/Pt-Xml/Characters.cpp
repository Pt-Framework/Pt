#include "Pt/Xml/Characters.h"
using namespace Pt;
using namespace Pt::Xml;


Characters::Characters(const String& content)
: Node(Node::Characters), _content(content) {

}


Characters::~Characters() {
}


bool Characters::empty() const
{
    return _content.empty();
}


String& Characters::content()
{
    return _content;
}


const String& Characters::content() const
{
    return _content;
}


void Characters::setContent(const String& content)
{
    _content = content;
}


bool Characters::operator==(const Node& node) const
{
    const Characters* chars = dynamic_cast<const Characters*>(&node);
    if(!chars) return false;

    return ( chars->content() == this->content() );
}


