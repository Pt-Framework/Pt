#include "Pt/Xml/NamespaceContext.h"
#include "Pt/String.h"

#include <iostream>
using namespace std;


namespace Pt {

namespace Xml {

NamespaceContext::NamespaceContext()
{

}


NamespaceContext::~NamespaceContext()
{

}


const String& NamespaceContext::namespaceUri(const String& prefix) const
{
    static const String null;

    std::multimap<String, Namespace>::const_iterator it;
    for( it = _namespaceScopes.begin(); it != _namespaceScopes.end(); ++it) {
        if(it->second.prefix() == prefix) {
            return it->second.namespaceUri();
        }
    }

    return null;
}


const String& NamespaceContext::prefix(const String& namespaceUri) const
{
    static const String null;

    std::multimap<String, Namespace>::const_iterator it;
    for( it = _namespaceScopes.begin(); it != _namespaceScopes.end(); ++it) {
        if(it->second.namespaceUri() == namespaceUri) {
            return it->second.prefix();
        }
    }

    return null;
}


void NamespaceContext::addNamespace(const String& elementName, const Namespace& ns)
{
    _namespaceScopes.insert( make_pair(elementName, ns) );
}


void NamespaceContext::removeNamespace(const String& elementName)
{
    _namespaceScopes.erase(elementName);
}

} // namespace Xml

} // namespace Pt

