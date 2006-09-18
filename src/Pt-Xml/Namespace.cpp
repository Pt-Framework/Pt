#include "Pt/Xml/Namespace.h"

#include <iostream>
using namespace std;


namespace Pt {

namespace Xml {

Namespace::Namespace(const String& namespaceUri, const String& prefix)
: _prefix(prefix), _namespaceUri(namespaceUri)
{

}


Namespace::~Namespace()
{

}

} // namespace Xml

} // namespace Pt
