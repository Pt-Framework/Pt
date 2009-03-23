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
#ifndef PTV_Xml_XmlWriter_h
#define PTV_Xml_XmlWriter_h

#include <Pt/Xml/Api.h>
#include <Pt/Xml/EntityResolver.h>
#include <Pt/String.h>
#include <Pt/Text/TextStream.h>
#include <stack>

namespace Pt {

namespace Xml {

    class Attribute;

    class PT_XML_API XmlWriter
    {
        public:
            XmlWriter(std::ostream& os);

            ~XmlWriter();

            void writeStartElement(const Pt::String& prefix, const Pt::String& localName, const Pt::String& ns);

            void writeStartElement(const Pt::String& localName);

            void writeStartElement(const Pt::String& localName, const Attribute* attr, size_t attrCount);

            void writeEndElement();

            void writeElement(const Pt::String& localName, const Pt::String& content);

            void writeElement(const Pt::String& localName, const Attribute* attr, size_t attrCount, const Pt::String& content);

            void writeCharacters(const Pt::String& text);

            void flush();

            void endl();

        private:
            EntityResolver _resolver;
            Text::TextOStream _tos;
            std::stack<Pt::String> _elements;
    };

}

}

#endif
