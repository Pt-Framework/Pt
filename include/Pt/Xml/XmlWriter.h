/*
 * Copyright (C) 2005-2013 Marc Boris Duerner
 *
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

#ifndef Pt_Xml_XmlWriter_h
#define Pt_Xml_XmlWriter_h

#include <Pt/Xml/Api.h>
#include <Pt/String.h>
#include <Pt/TextStream.h>

namespace Pt {

namespace Xml {

class Attribute;

class PT_XML_API XmlWriter
{
    public:
        XmlWriter();

        XmlWriter(std::basic_ostream<Char>& os);

        ~XmlWriter();

        void begin(std::basic_ostream<Char>& os);

        void writeStartDocument(const Pt::Char* version, const Pt::Char* encoding);

        void writeStartElement(const Pt::String& prefix, const Pt::String& localName, const Pt::String& ns);

        void writeStartTag(const Pt::Char* name);

        void writeEndTag(const Pt::Char* name);

        void writeStartElement(const Pt::Char* localName, const Attribute* attr = 0, size_t attrCount = 0);

        void writeStartElement(const Pt::String& localName, const Attribute* attr = 0, size_t attrCount = 0);

        void writeEndElement();

        void writeElement(const Pt::String& localName, const Pt::String& content, const Attribute* attr = 0, size_t attrCount = 0);

        void writeElement(const Pt::Char* localName, const Char* content, const Attribute* attr = 0, size_t attrCount = 0);

        void writeCharacters(const Pt::Char* text);

        void writeCharacters(const Pt::String& text);

        void flush();

        void endl();

        void useIndent(bool sw);

        void useEndl(bool sw);

    private:
        class XmlWriterImpl* _impl;
};

}

}

#endif
