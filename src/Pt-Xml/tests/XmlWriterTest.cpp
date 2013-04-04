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
#undef PT_XML_API_EXPORT

#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"

#include "Pt/Xml/XmlWriter.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/Comment.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/EndDocument.h"

#include "Pt/Utf8Codec.h"

#include <sstream>

class XmlWriterTest : public Pt::Unit::TestSuite
{
    public:
        XmlWriterTest()
        : Pt::Unit::TestSuite("XmlWriterTest")
        {
            this->registerMethod("Element" , *this, &XmlWriterTest::Element);
            this->registerMethod("MixedContent" , *this, &XmlWriterTest::MixedContent);
            this->registerMethod("TextElement" , *this, &XmlWriterTest::TextElement);
            this->registerMethod("Namespaces" , *this, &XmlWriterTest::Namespaces);
        }

    protected:
        void Element()
        {
            std::stringstream ss;

            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.reset(tos);
            writer.writeStartElement(L"first");
            writer.writeEndElement();
            tos.flush();

            std::stringstream result;
            result << "<first></first>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result.str() == ss.str());
        }

        void MixedContent()
        {
            std::stringstream ss;

            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.reset(tos);
            writer.writeStartElement(L"root");
            writer.writeCharacters(L"aaa");
            writer.writeStartElement(L"first");
            writer.writeCharacters(L"bbb");
            writer.writeEndElement();
            writer.writeCharacters(L"ccc");
            writer.writeEndElement();
            tos.flush();

            std::stringstream result;
            result << "<root>aaa<first>bbb</first>ccc</root>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result.str() == ss.str());
        }

        void TextElement()
        {
            std::stringstream ss;
            
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.reset(tos);
            writer.writeStartElement(L"elem");
            writer.writeCharacters(L"Hello world!");
            writer.writeEndElement();

            tos.flush();

            std::stringstream result;
            result << "<elem>Hello world!</elem>";

            PT_UNIT_ASSERT( result.str() == ss.str());
        }

        void Namespaces()
        {
            std::stringstream ss;
            
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);

            Pt::Xml::XmlWriter writer(tos);
            writer.setFormatting(false);
            
            writer.setDefaultNamespace(L"http://pt-framework.org/default");
            writer.setNamespacePrefix(L"pt", L"http://pt-framework.org/pt");
            
            writer.writeStartElement(L"root");
            writer.writeStartElement(L"http://pt-framework.org/pt", L"first");
            writer.writeAttribute(L"http://pt-framework.org/pt", L"a", L"aaa");
            writer.writeStartElement(L"http://pt-framework.org/pt", L"second");
            writer.writeStartElement(L"http://pt-framework.org/default", L"default");
            writer.writeAttribute(L"http://pt-framework.org/default", L"b", L"bbb");
            writer.writeEndElement();
            writer.writeEndElement();
            writer.writeEndElement();

            writer.writeEndElement();
            tos.flush();

            //std::cerr << '\n' << ss.str() << std::endl;
            std::string result = "<root xmlns:pt=\"http://pt-framework.org/pt\""
                                      " xmlns=\"http://pt-framework.org/default\">"
                                   "<pt:first pt:a=\"aaa\">"
                                     "<pt:second>"
                                       "<default b=\"bbb\">"
                                       "</default>"
                                     "</pt:second>"
                                   "</pt:first>"
                                 "</root>";
            PT_UNIT_ASSERT( result == ss.str());
        }
};

Pt::Unit::RegisterTest<XmlWriterTest> register_XmlWriterTest;
