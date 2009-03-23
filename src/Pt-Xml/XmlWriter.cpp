/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "Pt/Xml/XmlWriter.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Text/Utf8Codec.h"
#include <iostream>


namespace Pt {

namespace Xml {

XmlWriter::XmlWriter()
: _tos(new Text::Utf8Codec)
{
}


XmlWriter::XmlWriter(std::ostream& os)
: _tos(os, new Text::Utf8Codec)
{
    _tos << Pt::String(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>") << std::endl;
}


XmlWriter::~XmlWriter()
{
}


void XmlWriter::begin(std::ostream& os)
{
    _tos.attach(os);
    _tos << Pt::String(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>") << std::endl;
}


void XmlWriter::writeStartElement(const Pt::String& prefix, const Pt::String& localName, const Pt::String& ns)
{
}


void XmlWriter::writeStartElement(const Pt::String& localName)
{
    this->writeStartElement(localName, 0, 0);
}


void XmlWriter::writeStartElement(const Pt::String& localName, const Attribute* attr, size_t attrCount)
{
    for(size_t n = 0; n < _elements.size(); ++n)
    {
        _tos << Pt::String(L"  ");
    }

    _tos << Pt::Char(L'<') << localName;

    for(size_t n = 0; n < attrCount; ++n)
    {
        _tos << Pt::Char(' ') << attr[n].name() << Pt::String(L"=\"") << attr[n].value() << Pt::Char('"');
    }

    _tos << Pt::Char(L'>');
    this->endl();

    _elements.push(localName);
}


void XmlWriter::writeEndElement()
{
    if( _elements.empty() )
        return;

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
    this->writeElement(localName, 0, 0, content);
}


void XmlWriter::writeElement(const Pt::String& localName, const Attribute* attr, size_t attrCount, const Pt::String& content)
{
    for(size_t n = 0; n < _elements.size(); ++n)
    {
        _tos << Pt::String(L"  ");
    }

    _tos << Pt::Char(L'<') << localName;

    for(size_t n = 0; n < attrCount; ++n)
    {
        _tos << Pt::Char(' ') << attr[n].name() << Pt::String(L"=\"") << attr[n].value() << Pt::Char('"');
    }

    _tos << Pt::Char(L'>');

    this->writeCharacters(content);
    _tos << Pt::Char(L'<') << Pt::Char(L'/') << localName << Pt::Char(L'>');  
    this->endl();
}


void XmlWriter::writeCharacters(const Pt::String& text)
{
    static const Pt::Char lt[] = { '&', 'l', 't', ';', 0 };
    static const Pt::Char gt[] = { '&', 'g', 't', ';', 0 };
    static const Pt::Char amp[] = { '&', 'a', 'm', 'p', ';', 0 };

    Pt::String::const_iterator it;
    for(it = text.begin(); it != text.end(); ++it)
    {
        switch( it->value() )
        {
            case '<':
                _tos << lt;
                break;

            case '>':
                _tos << gt;
                break;

            case '&':
                _tos << amp;
                break;

            default:
                _tos << *it;
        }
    }
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
