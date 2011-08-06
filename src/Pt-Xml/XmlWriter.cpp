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
#include "Pt/Utf8Codec.h"
#include <iostream>


namespace Pt {

namespace Xml {

XmlWriter::XmlWriter()
: _tos(new Utf8Codec)
, _flags(UseXmlDeclaration | UseIndent | UseEndl)
{
}


XmlWriter::XmlWriter(std::ostream& os, int flags)
: _tos(os, new Utf8Codec)
, _flags(flags)
{
    static const Pt::Char declstr[] = { '<', '?', 'x', 'm', 'l', ' ', 'v', 
        'e', 'r', 's', 'i', 'o', 'n', '=', '"', '1', '.', '0', '"', ' ', 
        'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g', '=', '"', 'U', 'T', 'F', 
        '-', '8', '"', '?', '>' };

    if( useXmlDeclaration() )
    {
        _tos.write(declstr, sizeof(declstr)/sizeof(Pt::Char));

        if( useEndl() )
            this->endl();
    }
}


XmlWriter::~XmlWriter()
{
}


void XmlWriter::begin(std::ostream& os)
{
    static const Pt::Char declstr[] = { '<', '?', 'x', 'm', 'l', ' ', 'v', 
        'e', 'r', 's', 'i', 'o', 'n', '=', '"', '1', '.', '0', '"', ' ', 
        'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g', '=', '"', 'U', 'T', 'F', 
        '-', '8', '"', '?', '>' };

    _tos.attach(os);

    if( useXmlDeclaration() )
        _tos.write(declstr, sizeof(declstr)/sizeof(Pt::Char));

    if( useEndl() )
        this->endl();
}


void XmlWriter::writeStartTag(const Pt::Char* name)
{
    _tos << Pt::Char('<') << name;
    _tos << Pt::Char('>');

    if( useEndl() )
        this->endl();
}


void XmlWriter::writeEndTag(const Pt::Char* name)
{
    _tos << Pt::Char('<') << Pt::Char('/') << name << Pt::Char('>');

    if( useEndl() )
        this->endl();
}


void XmlWriter::writeStartElement(const Pt::String& prefix, const Pt::String& localName, const Pt::String& ns)
{
}


void XmlWriter::writeStartElement(const Pt::Char* localName, const Attribute* attr, size_t attrCount)
{
    this->writeStartElement(Pt::String(localName), attr, attrCount);
}


void XmlWriter::writeStartElement(const Pt::String& localName, const Attribute* attr, size_t attrCount)
{
    if (useIndent())
    {
        for(size_t n = 0; n < _elements.size(); ++n)
        {
            _tos << Pt::Char(' ') << Pt::Char(' ');
        }
    }

    _tos << Pt::Char('<') << localName;

    for(size_t n = 0; n < attrCount; ++n)
    {
        _tos << Pt::Char(' ') << attr[n].name() << Pt::Char('=') << Pt::Char('"');
        writeCharacters( attr[n].value() );
        _tos << Pt::Char('"');
    }

    _tos << Pt::Char('>');

    if (useEndl())
        this->endl();

    _elements.push(localName);
}


void XmlWriter::writeEndElement()
{
    if( _elements.empty() )
        return;

    if (useIndent())
    {
        for(size_t n = 1; n < _elements.size(); ++n)
        {
            _tos << Pt::Char(' ') << Pt::Char(' ');
        }
    }

    _tos << Pt::Char('<') << Pt::Char('/') << _elements.top() << Pt::Char('>');

    if (useEndl())
        this->endl();

    _elements.pop();
}


void XmlWriter::writeElement(const Pt::String& localName, const Pt::String& content, const Attribute* attr, size_t attrCount)
{
    if( useIndent() )
    {
        for(size_t n = 0; n < _elements.size(); ++n)
        {
            _tos << Pt::Char(' ') << Pt::Char(' ');
        }
    }

    _tos << Pt::Char('<') << localName;

    for(size_t n = 0; n < attrCount; ++n)
    {
        _tos << Pt::Char(' ') << attr[n].name() << Pt::Char('=') << Pt::Char('"');
        writeCharacters( attr[n].value() );
        _tos << Pt::Char('"');
    }

    _tos << Pt::Char('>');

    this->writeCharacters(content);
    _tos << Pt::Char('<') << Pt::Char('/') << localName << Pt::Char('>');

    if( useEndl() )
        this->endl();
}


void XmlWriter::writeElement(const Pt::Char* localName, const Pt::Char* content, const Attribute* attr, size_t attrCount)
{
    if( useIndent() )
    {
        for(size_t n = 0; n < _elements.size(); ++n)
        {
            _tos << Pt::Char(' ') << Pt::Char(' ');
        }
    }

    _tos << Pt::Char('<') << localName;

    for(size_t n = 0; n < attrCount; ++n)
    {
        _tos << Pt::Char(' ') << attr[n].name() << Pt::Char('=') << Pt::Char('"');
        writeCharacters( attr[n].value() );
        _tos << Pt::Char('"');
    }

    _tos << Pt::Char('>');

    this->writeCharacters(content);
    _tos << Pt::Char('<') << Pt::Char('/') << localName << Pt::Char('>');

    if( useEndl() )
        this->endl();
}


void XmlWriter::writeCharacters(const Pt::Char* text)
{
    _resolver.getEntity(_tos, text);
}


void XmlWriter::writeCharacters(const Pt::String& text)
{
    _resolver.getEntity(_tos, text.c_str());
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
