#include "Pt/Xml/XmlWriter.h"
#include <Pt/Text/Utf8Codec.h>
#include <iostream>


namespace Pt {

namespace Xml {

XmlWriter::XmlWriter(std::ostream& os)
: _tos(os, new Text::Utf8Codec)
{
    _tos << Pt::String(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>") << std::endl;
}


XmlWriter::~XmlWriter()
{
}


void XmlWriter::writeStartElement(const Pt::String& prefix, const Pt::String& localName, const Pt::String& ns)
{
}


void XmlWriter::writeStartElement(const Pt::String& localName)
{
    for(size_t n = 0; n < _elements.size(); ++n)
    {
        _tos << Pt::String(L"  ");
    }
    
    _tos << Pt::Char(L'<') << localName << Pt::Char(L'>');
    this->endl();

    _elements.push(localName);
}


void XmlWriter::writeEndElement()
{
    for(size_t n = 0; n < _elements.size()-1; ++n)
    {
        _tos << Pt::String(L"  ");
    }

    _tos << Pt::Char(L'<') << Pt::Char(L'/') << _elements.top() << Pt::Char(L'>');
    this->endl();
    _elements.pop();
}


void XmlWriter::writeElement(const Pt::String& localName, const Pt::String& content)
{
    for(size_t n = 0; n < _elements.size(); ++n)
    {
        _tos << Pt::String(L"  ");
    }
    
    _tos << Pt::Char(L'<') << localName << Pt::Char(L'>');
    this->writeCharacters(content);
    _tos << Pt::Char(L'<') << Pt::Char(L'/') << localName << Pt::Char(L'>');  
    this->endl();
}


void XmlWriter::writeCharacters(const Pt::String& text)
{
    _tos << text;
}


void XmlWriter::flush()
{
    _tos.flush();
}


void XmlWriter::endl()
{
    _tos << Pt::Char('\n');
}

} // namespace Xml

} // namespace Pt
