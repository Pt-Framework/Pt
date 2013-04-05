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

        bool isFormatting() const;
        
        void setFormatting(bool value);

        const Pt::String& indent() const;

        void setIndent(const Pt::String& indent);

        Pt::Char quote() const;

        void setQuote(Pt::Char ch);

        /** @brief Clears the writer state and output.

            Ther output stream is removed and the writer state is reset to
            write a new document. The formatting options are not changed.
        */
        void reset();

        void reset(std::basic_ostream<Char>& os);

        std::basic_ostream<Char>* output();

        std::size_t depth() const;

        void setDefaultNamespace(const Pt::String& ns);

        void setNamespacePrefix(const Pt::String& prefix, const Pt::String& ns);

        void writeStartDocument(const Pt::Char* version, std::size_t versionSize, 
                                const Pt::Char* encoding, std::size_t encodingSize, bool standalone = false);

        void writeStartDocument(const Pt::String& version, const Pt::String& encoding, bool standalone = false);

        void writeEndDocument();

        void writeDocType(const Pt::Char* dtd, std::size_t n);

        void writeDocType(const Pt::String& dtd);

        void writeStartElement(const Pt::Char* localName, std::size_t localNameSize);

        void writeStartElement(const Pt::String& localName);

        void writeStartElement(const Char* ns, std::size_t nsSize,
                               const Char* localName, std::size_t localNameSize);

        void writeStartElement(const Pt::String& ns, const Pt::String& localName);

        void writeAttribute(const Char* localName, std::size_t localNameSize,
                            const Char* value, std::size_t valueSize);

        void writeAttribute(const Pt::String& localName, const Pt::String& value);

        void writeAttribute(const Char* ns, std::size_t nsSize,
                            const Char* localName, std::size_t localNameSize,
                            const Char* value, std::size_t valueSize);

        void writeAttribute(const Pt::String& ns, const Pt::String& localName, const Pt::String& value);

        void writeEmptyElement(const Pt::Char* localName, std::size_t localNameSize);

        void writeEmptyElement(const Pt::String& localName);

        void writeEmptyElement(const Char* ns, std::size_t nsSize,
                               const Char* localName, std::size_t localNameSize);

        void writeEmptyElement(const Pt::String& ns, const Pt::String& localName);

        void writeEndElement();

        void writeCharacters(const Pt::Char* text, std::size_t n);

        void writeCharacters(const Pt::String& text);

        void writeEntityReference(const Pt::Char* name, std::size_t n);

        void writeEntityReference(const Pt::String& name);

        void writeCData(const Pt::Char* text, std::size_t n);

        void writeCData(const Pt::String& text);

        void writeComment(const Pt::Char* text, std::size_t n);
        
        void writeComment(const Pt::String& text);

        void writeProcessingInstruction(const Pt::Char* target, std::size_t targetSize,
                                        const Pt::Char* data, std::size_t dataSize);
        
        void writeProcessingInstruction(const Pt::String& text, const Pt::String& data);

        void writeStartTag(const Pt::Char* name);

        void writeEndTag(const Pt::Char* name);

        void writeIndent();

    private:
        class XmlWriterImpl* _impl;
};


inline void XmlWriter::writeStartDocument(const Pt::String& version, const Pt::String& encoding, bool standalone)
{
    this->writeStartDocument(version.c_str(), version.size(),
                             encoding.c_str(), encoding.size(), standalone);
}


inline void XmlWriter::writeDocType(const Pt::String& dtd)
{
    this->writeDocType(dtd.c_str(), dtd.size());
}


inline void XmlWriter::writeStartElement(const Pt::String& localName)
{
    this->writeStartElement(localName.c_str(), localName.size());
}


inline void XmlWriter::writeStartElement(const Pt::String& ns, const Pt::String& localName)
{
    this->writeStartElement(ns.c_str(), ns.size(), 
                            localName.c_str(), localName.size());
}


inline void XmlWriter::writeAttribute(const Pt::String& localName, const Pt::String& value)
{
    this->writeAttribute( localName.c_str(), localName.size(), 
                          value.c_str(), value.size() );
}


inline void XmlWriter::writeAttribute(const Pt::String& ns, const Pt::String& localName, const Pt::String& value)
{
    this->writeAttribute( ns.c_str(), ns.size(), 
                          localName.c_str(), localName.size(), 
                          value.c_str(), value.size() );
}


inline void XmlWriter::writeEmptyElement(const Pt::String& localName)
{
    this->writeEmptyElement(localName.c_str(), localName.size());
}


inline void XmlWriter::writeEmptyElement(const Pt::String& ns, const Pt::String& localName)
{
    this->writeEmptyElement(ns.c_str(), ns.size(), 
                            localName.c_str(), localName.size());
}


inline void XmlWriter::writeCharacters(const Pt::String& text)
{
    this->writeCharacters(text.c_str(), text.size());
}


inline void XmlWriter::writeEntityReference(const Pt::String& name)
{
    this->writeEntityReference(name.c_str(), name.size());
}


inline void XmlWriter::writeCData(const Pt::String& text)
{
    this->writeCData(text.c_str(), text.size());
}


inline void XmlWriter::writeComment(const Pt::String& text)
{
    this->writeComment(text.c_str(), text.size());
}


inline void XmlWriter::writeProcessingInstruction(const Pt::String& target, const Pt::String& data)
{
    this->writeProcessingInstruction( target.c_str(), target.size(), 
                                      data.c_str(), data.size() );
}

} // namespace Xml

} // namespace Pt

#endif
