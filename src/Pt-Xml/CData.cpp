#include "Pt/Xml/CData.h"

using namespace Pt;
using namespace Pt::Xml;


CData::CData(const String& content)
: Node(Node::CData), _content(content) {

}


CData::~CData() {
}


bool CData::empty() const
{
    return _content.empty();
}


String& CData::content()
{
    return _content;
}


const String& CData::content() const
{
    return _content;
}


void CData::setContent(const String& content)
{
    _content = content;
}


bool CData::operator==(const Node& node) const
{
    const CData* cdata = dynamic_cast<const CData*>(&node);
    if( !cdata ) return false;

    return ( cdata->content() == this->content() );
}


