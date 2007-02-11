#include "Pt/Xml/DocTypeDeclaration.h"


namespace Pt {

namespace Xml {


DocTypeDeclaration::DocTypeDeclaration(const String& content)
: Node(Node::DocType), _content(content) {

}


DocTypeDeclaration::~DocTypeDeclaration() {
}


const String& DocTypeDeclaration::content() const
{
    return _content;
}


void DocTypeDeclaration::setContent(const String& content)
{
    _content = content;
}

} // namespace Xml

} // namespace Pt


