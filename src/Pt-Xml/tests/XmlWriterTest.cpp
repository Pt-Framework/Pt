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
            this->registerMethod("XmlDeclaration" , *this, &XmlWriterTest::XmlDeclaration);
            this->registerMethod("DocType" , *this, &XmlWriterTest::DocType);
            this->registerMethod("EmptyElement" , *this, &XmlWriterTest::EmptyElement);
            this->registerMethod("EndDocument" , *this, &XmlWriterTest::EndDocument);
            this->registerMethod("Attributes" , *this, &XmlWriterTest::Attributes);
            this->registerMethod("MixedContent" , *this, &XmlWriterTest::MixedContent);
            this->registerMethod("Characters" , *this, &XmlWriterTest::Characters);
            this->registerMethod("CData" , *this, &XmlWriterTest::CData);
            this->registerMethod("EntityReference" , *this, &XmlWriterTest::EntityReference);
            this->registerMethod("Comment" , *this, &XmlWriterTest::Comment);
            this->registerMethod("ProcessingInstruction" , *this, &XmlWriterTest::ProcessingInstruction);
            this->registerMethod("Namespaces" , *this, &XmlWriterTest::Namespaces);
            this->registerMethod("Indent" , *this, &XmlWriterTest::Indent);
        }

    protected:
        void XmlDeclaration()
        {
            std::stringstream ss;
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.setFormatting(false);

            writer.reset(tos);
            writer.writeStartDocument(L"1.0", L"UTF-8", true);
            writer.writeStartElement(L"first");
            writer.writeEndElement();
            tos.flush();

            std::string result;
            result = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><first/>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result == ss.str());
        }

        void DocType()
        {
            std::stringstream ss;
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.setFormatting(false);

            writer.reset(tos);
            writer.writeDocType(L"first SYSTEM \"test.dtd\"");
            writer.writeStartElement(L"first");
            writer.writeEndElement();
            tos.flush();

            std::string result;
            result = "<!DOCTYPE first SYSTEM \"test.dtd\"><first/>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result == ss.str());
        }

        void EmptyElement()
        {
            std::stringstream ss;
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.setFormatting(false);

            writer.reset(tos);
            writer.writeStartElement(L"first");
            writer.writeEmptyElement(L"second");
            writer.writeEmptyElement(L"third");
            writer.writeEndElement();
            tos.flush();

            std::string result;
            result = "<first><second/><third/></first>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result == ss.str());
        }

        void EndDocument()
        {
            std::stringstream ss;
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.setFormatting(false);

            writer.reset(tos);
            writer.writeStartElement(L"first");
            writer.writeStartElement(L"second");
            writer.writeEmptyElement(L"third");
            writer.writeEndDocument();
            tos.flush();

            std::string result;
            result = "<first><second><third/></second></first>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result == ss.str());
        }

        void Attributes()
        {
            std::stringstream ss;
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.setFormatting(false);

            writer.reset(tos);
            writer.writeStartElement(L"first");
            writer.writeAttribute(L"a1", L"aaa");

            writer.setQuote('\'');
            writer.writeAttribute(L"a2", L"bbb");
            writer.writeEndElement();
            tos.flush();

            std::string result;
            result = "<first a1=\"aaa\" a2='bbb'/>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result == ss.str());
        }

        void MixedContent()
        {
            std::stringstream ss;
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.setFormatting(false);

            writer.reset(tos);
            writer.writeStartElement(L"root");
            writer.writeCharacters(L"aaa");
            writer.writeStartElement(L"first");
            writer.writeCharacters(L"bbb");
            writer.writeEndElement();
            writer.writeCharacters(L"ccc");
            writer.writeEndElement();
            tos.flush();

            std::string result;
            result = "<root>aaa<first>bbb</first>ccc</root>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result == ss.str());
        }

        void Characters()
        {
            std::stringstream ss;
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.setFormatting(false);

            writer.reset(tos);
            writer.writeStartElement(L"first");
            writer.writeCharacters(L"Hello world!");
            writer.writeCharacters(L"<>&'\"");
            writer.writeEndElement();

            tos.flush();

            std::string result;
            result = "<first>Hello world!&lt;&gt;&amp;&apos;&quot;</first>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result == ss.str());
        }

        void EntityReference()
        {
            std::stringstream ss;
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.setFormatting(false);

            writer.reset(tos);
            writer.writeStartElement(L"first");
            writer.writeEntityReference(L"eref");
            writer.writeEndElement();

            tos.flush();

            std::string result;
            result = "<first>&eref;</first>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result == ss.str());
        }

        void CData()
        {
            std::stringstream ss;
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.setFormatting(false);

            writer.reset(tos);
            writer.writeStartElement(L"first");
            writer.writeCData(L"Hello world!");
            writer.writeEndElement();

            tos.flush();

            std::string result;
            result = "<first><![CDATA[Hello world!]]></first>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result == ss.str());
        }

        void Comment()
        {
            std::stringstream ss;
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.setFormatting(false);

            writer.reset(tos);
            writer.writeStartElement(L"first");
            writer.writeComment(L"comment 1");
            
            writer.writeStartElement(L"second");
            writer.writeComment(L"comment 2");
            writer.writeEndElement();
            
            writer.writeStartElement(L"third");
            writer.writeCharacters(L"Hello");
            writer.writeComment(L"comment 3");
            writer.writeCharacters(L"world!");
            writer.writeEndElement();
            
            writer.writeComment(L"comment 4");
            writer.writeEndElement();

            tos.flush();

            std::string result;
            result = "<first>"
                       "<!--comment 1-->"
                         "<second>"
                           "<!--comment 2-->"
                         "</second>"
                         "<third>Hello<!--comment 3-->world!</third>"
                       "<!--comment 4-->"
                     "</first>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result == ss.str());
        }
        
        void ProcessingInstruction()
        {
            std::stringstream ss;
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);
            
            Pt::Xml::XmlWriter writer;
            writer.setFormatting(false);

            writer.reset(tos);
            writer.writeStartElement(L"first");
            writer.writeProcessingInstruction(L"proc", L"some data");
            writer.writeEndElement();

            tos.flush();

            std::string result;
            result = "<first>"
                       "<?proc some data?>"
                     "</first>";

            //std::cerr << '\n' << ss.str() << std::endl;
            PT_UNIT_ASSERT( result == ss.str());
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
                                 "<default b=\"bbb\"/>"
                                 "</pt:second>"
                                 "</pt:first>"
                                 "</root>";
            
            PT_UNIT_ASSERT( result == ss.str());
        }

        void Indent()
        {
            std::stringstream ss;
            Pt::TextOStream tos(ss, new Pt::Utf8Codec);

            Pt::Xml::XmlWriter writer(tos);

            writer.writeStartDocument(L"1.0", L"UTF-8", true);
                       
            writer.writeStartElement(L"root");
            writer.writeStartElement(L"first");
            writer.writeComment(L"comment 1");
            writer.writeProcessingInstruction(L"proc", L"some data");
            writer.writeStartElement(L"second");

            writer.writeStartElement(L"third");
            writer.writeEndElement();

            writer.writeEmptyElement(L"empty");
            
            writer.writeStartElement(L"fourth");
            writer.writeCharacters(L"Hello ");
            writer.writeEntityReference(L"world");
            writer.writeEndElement();
           
            writer.writeEndElement();
            writer.writeEndElement();
            writer.writeEndElement();
            tos.flush();

            //std::cerr << '\n' << ss.str() << std::endl;
            std::stringstream result;
            result << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << std::endl;
            result << "<root>" << std::endl;
            result << "  <first>" << std::endl;
            result << "    <!--comment 1-->" << std::endl;
            result << "    <?proc some data?>" << std::endl;
            result << "    <second>" << std::endl;
            result << "      <third/>" << std::endl;
            result << "      <empty/>" << std::endl;
            result << "      <fourth>Hello &world;</fourth>" << std::endl;
            result << "    </second>" << std::endl;
            result << "  </first>" << std::endl;
            result << "</root>";
            
            PT_UNIT_ASSERT( result.str() == ss.str());
        }
};

Pt::Unit::RegisterTest<XmlWriterTest> register_XmlWriterTest;
