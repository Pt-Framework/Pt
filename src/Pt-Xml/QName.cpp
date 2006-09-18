#include "Pt/Xml/QName.h"

#include <iostream>
#include <sstream>
using namespace std;


namespace Pt {

namespace Xml {

QName::QName()
{
}


QName::QName(const String& localPart) 
: _localPart(localPart)
{
}


QName::QName(const String& namespaceURI, const String& localPart) 
: _localPart(localPart) , _namespaceUri(namespaceURI)
{
}


QName::QName(const String& namespaceURI, const String& localPart, const String& prefix) 
: _prefix(prefix) , _localPart(localPart), _namespaceUri(namespaceURI)
{
}


QName::~QName()
{

}

const String& QName::prefix() const
{
	return _prefix;
}


void QName::setPrefix(const String& prefix) {
	_prefix = prefix;
}


const String& QName::localPart() const {
	return _localPart;
}


void QName::setLocalPart(const String& localPart) {
	_localPart = localPart;
}


const String& QName::namespaceUri() const {
	return _namespaceUri;
}


void QName::setNamespaceUri(const String& namespaceUri) {
	_namespaceUri = namespaceUri;
}

} // namespace Xml

} // namespace Pt
