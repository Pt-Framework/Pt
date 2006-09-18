#include "Pt/Xml/XmlDeclaration.h"

#include <iostream>
using namespace std;


namespace Pt {

namespace Xml {

XmlDeclaration::XmlDeclaration()
: Node(Node::XmlDeclaration),
  _version( L"1.0" ),
  _encoding( L"UTF-8" ),
  _standalone(false)
{

}


XmlDeclaration::~XmlDeclaration() {
}


const String& XmlDeclaration::version() const {
	return _version;
}


void XmlDeclaration::setVersion(const String& version) {
	_version = version;
}


const String& XmlDeclaration::encoding() const {
	return _encoding;
}


void XmlDeclaration::setEncoding(const String& encoding) {
	_encoding = encoding;
}


bool XmlDeclaration::standalone() const {
	return _standalone;
}


void XmlDeclaration::setStandalone(bool standalone) {
	_standalone = standalone;
}


} // namespace Xml

} // namespace Pt
