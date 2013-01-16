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
#include "Pt/Unit/Assertion.h"
#include "Pt/Unit/TestSuite.h"
#include "Pt/Unit/RegisterTest.h"
#include "Pt/Xml/XmlReader.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/Comment.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/EndDocument.h"
#include "Pt/Xml/DocTypeDeclaration.h"
#include "Pt/Xml/ProcessingInstruction.h"
#include "Pt/System/Clock.h"
#include "Pt/String.h"
#include <string>
#include <sstream>
#include <iostream>

class XmlReaderTest : public Pt::Unit::TestSuite
{
    public:
        XmlReaderTest()
        : Pt::Unit::TestSuite("XmlReaderTest")
        {
            this->registerMethod("MissingXmlDeclaration", *this, &XmlReaderTest::MissingXmlDeclaration);
            this->registerMethod("EmptyXmlDeclaration", *this, &XmlReaderTest::EmptyXmlDeclaration);
            this->registerMethod("DtdValidateAttributes", *this, &XmlReaderTest::DtdValidateAttributes);
            this->registerMethod("DtdValidateElementContent", *this, &XmlReaderTest::DtdValidateElementContent);
            this->registerMethod("EmptyDocument", *this, &XmlReaderTest::EmptyDocument);
            this->registerMethod("EmptyElementTag", *this, &XmlReaderTest::EmptyElementTag);
            this->registerMethod("InvalidTag1", *this, &XmlReaderTest::InvalidTag1);
            this->registerMethod("InvalidTag2", *this, &XmlReaderTest::InvalidTag2);
            this->registerMethod("InvalidTag3", *this, &XmlReaderTest::InvalidTag3);
            this->registerMethod("InvalidTag4", *this, &XmlReaderTest::InvalidTag4);
            this->registerMethod("InvalidTag5", *this, &XmlReaderTest::InvalidTag5);
            this->registerMethod("ElementWithContent", *this, &XmlReaderTest::ElementWithContent);
            this->registerMethod("ElementWithNamespace", *this, &XmlReaderTest::ElementWithNamespace);
            this->registerMethod("AttributeWithNamespace", *this, &XmlReaderTest::AttributeWithNamespace);
            this->registerMethod("DefaultNamespace", *this, &XmlReaderTest::DefaultNamespace);
            this->registerMethod("DefaultEntities", *this, &XmlReaderTest::DefaultEntities);
            this->registerMethod("InvalidAttribute1", *this, &XmlReaderTest::InvalidAttribute1);
            this->registerMethod("InvalidAttribute2", *this, &XmlReaderTest::InvalidAttribute2);
            this->registerMethod("InvalidAttribute3", *this, &XmlReaderTest::InvalidAttribute3);
            this->registerMethod("InvalidAttribute4", *this, &XmlReaderTest::InvalidAttribute4);
            this->registerMethod("EmptyAttribute", *this, &XmlReaderTest::EmptyAttribute);
            this->registerMethod("AttributeWithSimpleText", *this, &XmlReaderTest::AttributeWithSimpleText);
            this->registerMethod("AttributeWithUTF8", *this, &XmlReaderTest::AttributeWithUTF8);
            this->registerMethod("MultipleAttributesIteration", *this, &XmlReaderTest::MultipleAttributesIteration);
            this->registerMethod("IgnorableWhitespace", *this, &XmlReaderTest::IgnorableWhitespace);
            this->registerMethod("CDATA", *this, &XmlReaderTest::CDATA );
            this->registerMethod("CommentInProlog", *this, &XmlReaderTest::CommentInProlog );
            this->registerMethod("CommentInElement", *this, &XmlReaderTest::CommentInElement );
            this->registerMethod("CommentInEpilog", *this, &XmlReaderTest::CommentInEpilog );
            this->registerMethod("EmptyComment", *this, &XmlReaderTest::EmptyComment );
            this->registerMethod("CommentBeforeRoot", *this, &XmlReaderTest::CommentBeforeRoot );
            this->registerMethod("MissingCloseTag", *this, &XmlReaderTest::MissingCloseTag );
            this->registerMethod("ProcessingInstructionInProlog", *this, &XmlReaderTest::ProcessingInstructionInProlog );
            this->registerMethod("ProcessingInstructionInElement", *this, &XmlReaderTest::ProcessingInstructionInElement );
            this->registerMethod("ProcessingInstructionInEpilog", *this, &XmlReaderTest::ProcessingInstructionInEpilog );
        }

    protected:
        void MissingXmlDeclaration();
        void EmptyXmlDeclaration();
        void DtdValidateAttributes();
        void DtdValidateElementContent();
        void EmptyDocument();
        void DoctypeDeclaration();
        void EmptyElementTag();
        void InvalidTag1();
        void InvalidTag2();
        void InvalidTag3();
        void InvalidTag4();
        void InvalidTag5();
        void ElementWithContent();
        void ElementWithNamespace();
        void AttributeWithNamespace();
        void DefaultNamespace();
        void AttributeWithSimpleText();
        void AttributeWithUTF8();
        void MultipleAttributesIteration();
        void IgnorableWhitespace();
        void CDATA();
        void DefaultEntities();
        void CommentInProlog();
        void CommentInElement();
        void CommentInEpilog();
        void EmptyComment();
        void CommentBeforeRoot();
        void MissingCloseTag();
        void InvalidAttribute1();
        void InvalidAttribute2();
        void InvalidAttribute3();
        void InvalidAttribute4();
        void EmptyAttribute();

        void ProcessingInstructionInProlog();
        void ProcessingInstructionInElement();
        void ProcessingInstructionInEpilog();
        void CheckPerformance();
};

Pt::Unit::RegisterTest<XmlReaderTest> register_XmlTest;


void XmlReaderTest::MissingXmlDeclaration()
{
    std::stringstream input;
    input << "<a/>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT( reader.depth() == 0);

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    PT_UNIT_ASSERT(startNode.type() == Pt::Xml::Node::StartElement);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::StartElement*>(&startNode)->name().narrow() == "a");
    PT_UNIT_ASSERT( reader.depth() == 1);

    ++it;
    const Pt::Xml::Node& endNode = *it;
    PT_UNIT_ASSERT(endNode.type() == Pt::Xml::Node::EndElement);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::EndElement*>(&endNode)->name().narrow() == "a");
    PT_UNIT_ASSERT( reader.depth() == 0);

    ++it;
    const Pt::Xml::Node& endDocument = *it;
    PT_UNIT_ASSERT(endDocument.type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT( reader.depth() == 0);
}


void XmlReaderTest::EmptyXmlDeclaration()
{
    std::stringstream input;
    input << "<?xml ?>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& n = *it;

    PT_UNIT_ASSERT(n.type() == Pt::Xml::Node::EndDocument);
}


void XmlReaderTest::DtdValidateAttributes()
{
    try
    {
        std::stringstream input;
        input << "<!DOCTYPE test [\n";
        input << "<!ELEMENT test EMPTY>\n";
        input << "<!ATTLIST test a1 CDATA #REQUIRED\n";
        input << "          a2 CDATA #IMPLIED\n";
        input << "          a3 CDATA #FIXED \"A3def\"\n";
        input << "          a4 CDATA \"A4def\"\n>";
        input << "]>\n";
        input << "<test a1='A1' a2='A2' a4='A3def'></test>";

        Pt::Xml::XmlReader reader(input);
        
        Pt::Xml::XmlReader::Iterator it;
        for(it = reader.current(); it != reader.end(); ++it)
        {
            Pt::Xml::StartElement* se = toStartElement(&*it);
            if(se && se->name() == L"test")
            {
                PT_UNIT_ASSERT( se->attributes().has(L"a3") );
                PT_UNIT_ASSERT( se->attributes().find(L"a3")->value() == L"A3def" );

                PT_UNIT_ASSERT( se->attributes().has(L"a4") );
            }
        }
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::DtdValidateElementContent()
{
    try
    {
        std::stringstream input;
        input << "<!DOCTYPE test [\n";
        input << "<!ELEMENT test (a|b)+> \n";
        input << "<!ELEMENT a (#PCDATA|x|y|z)* >\n";
        input << "<!ELEMENT b EMPTY>\n";
        input << "]>\n";
        input << "<test><a>hello<x></x>abc</a><b></b><a>world</a><b></b></test>";

        Pt::Xml::XmlReader reader(input);
            
        Pt::Xml::XmlReader::Iterator it;
        for(it = reader.current(); it != reader.end(); ++it)
            ;
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::EmptyDocument()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& n = *it;

    PT_UNIT_ASSERT(n.type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT(reader.version() == L"1.0");
    PT_UNIT_ASSERT(reader.encoding() == L"UTF-8");
    PT_UNIT_ASSERT(reader.isStandalone() == true);
}

void XmlReaderTest::EmptyElementTag()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a/>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT( reader.depth() == 0);

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    PT_UNIT_ASSERT(startNode.type() == Pt::Xml::Node::StartElement);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::StartElement*>(&startNode)->name().narrow() == "a");
    PT_UNIT_ASSERT( reader.depth() == 1);

    ++it;
    const Pt::Xml::Node& endNode = *it;
    PT_UNIT_ASSERT(endNode.type() == Pt::Xml::Node::EndElement);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::EndElement*>(&endNode)->name().narrow() == "a");
    PT_UNIT_ASSERT( reader.depth() == 0);

    ++it;
    const Pt::Xml::Node& endDocument = *it;
    PT_UNIT_ASSERT(endDocument.type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT( reader.depth() == 0);
}


void XmlReaderTest::InvalidTag1()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<>";

    Pt::Xml::XmlReader reader( input );

    // Expecting exception because empty tags are not allowed.
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::InvalidTag2()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "</>";

    Pt::Xml::XmlReader reader( input );

    // Expecting exception because empty tags are not allowed.
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}



void XmlReaderTest::InvalidTag3()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>>b</a>";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();

    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);
    PT_UNIT_ASSERT_THROW(++it; ++it; ++it;, std::exception);
}


void XmlReaderTest::InvalidTag4()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<<a>b</a>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::InvalidTag5()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<ab<cd>>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::ElementWithContent()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>?!:=b</a>";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();

    // <a>
    PT_UNIT_ASSERT(Pt::Xml::toStartElement(*it).name() == L"a");
    PT_UNIT_ASSERT( reader.depth() == 1);

    // b
    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(*it).isIgnorable() == false);
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(*it).content() == L"?!:=b");
    PT_UNIT_ASSERT( reader.depth() == 1);

    // </a>
    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toEndElement(*it).name() == L"a");
    PT_UNIT_ASSERT( reader.depth() == 0);

    // End of document
    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT( reader.depth() == 0);
}


void XmlReaderTest::ElementWithNamespace()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<my:a xmlns:my=\"http://www.my1.net\">";
    input << "<my:a xmlns:my=\"http://www.my2.net\">b</my:a>";
    input << "</my:a>";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();

    // <my:a>
    const Pt::Xml::StartElement& startA = Pt::Xml::toStartElement(*it);
    PT_UNIT_ASSERT(startA.name() == L"a");
    PT_UNIT_ASSERT(startA.prefix() == L"my");
    PT_UNIT_ASSERT(startA.namespaceUri() == L"http://www.my1.net");
    PT_UNIT_ASSERT(reader.depth() == 1);

    // <my:a>
    ++it;
    const Pt::Xml::StartElement& startA2 = Pt::Xml::toStartElement(*it);
    PT_UNIT_ASSERT(startA2.name() == L"a");
    PT_UNIT_ASSERT(startA2.prefix() == L"my");
    PT_UNIT_ASSERT(startA2.namespaceUri() == "http://www.my2.net");
    PT_UNIT_ASSERT(reader.depth() == 2);

    // b
    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(*it).isIgnorable() == false);
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(*it).content() == L"b");
    PT_UNIT_ASSERT(reader.depth() == 2);

    // </my:a>
    ++it;
    const Pt::Xml::EndElement& endA2 = Pt::Xml::toEndElement(*it);
    PT_UNIT_ASSERT(endA2.name() == L"a");
    PT_UNIT_ASSERT(endA2.prefix() == L"my");
    PT_UNIT_ASSERT(endA2.namespaceUri() == L"http://www.my2.net");
    PT_UNIT_ASSERT(reader.depth() == 1);

    // </my:a>
    ++it;
    const Pt::Xml::EndElement& endA = Pt::Xml::toEndElement(*it);

    PT_UNIT_ASSERT(endA.name() == L"a");
    PT_UNIT_ASSERT(endA.prefix() == L"my");
    PT_UNIT_ASSERT(endA.namespaceUri() == L"http://www.my1.net");
    PT_UNIT_ASSERT( reader.depth() == 0);

    // End of document
    ++it;
    const Pt::Xml::Node& endDocument = *it;
    PT_UNIT_ASSERT(endDocument.type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT( reader.depth() == 0);
}

void XmlReaderTest::AttributeWithNamespace()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a my:attr=\"Hallo\" xmlns:my=\"http://www.my.net\">b</a>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    // <a>
    PT_UNIT_ASSERT(startNode.type() == Pt::Xml::Node::StartElement);
    PT_UNIT_ASSERT(Pt::Xml::toStartElement(&startNode)->name() == L"a");
    PT_UNIT_ASSERT( Pt::Xml::toStartElement(&startNode)->attributes().has(L"attr") );
    PT_UNIT_ASSERT( Pt::Xml::toStartElement(&startNode)->attributes().begin()->prefix() == L"my" );
    PT_UNIT_ASSERT( Pt::Xml::toStartElement(&startNode)->attributes().begin()->namespaceUri() == L"http://www.my.net");
    PT_UNIT_ASSERT( reader.depth() == 1);

    // b
    ++it;
    const Pt::Xml::Characters& text = Pt::Xml::toCharacters(*it);
    PT_UNIT_ASSERT(text.content() == L"b");
    PT_UNIT_ASSERT(reader.depth() == 1);

    // </a>
    ++it;
    const Pt::Xml::Node& endNodeA = *it;

    PT_UNIT_ASSERT(endNodeA.type() == Pt::Xml::Node::EndElement);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::EndElement*>(&endNodeA)->name() == L"a");
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::EndElement*>(&endNodeA)->namespaceUri() == L"");
    PT_UNIT_ASSERT( reader.depth() == 0);

    // End of document
    ++it;
    const Pt::Xml::Node& endDocument = *it;
    PT_UNIT_ASSERT(endDocument.type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT( reader.depth() == 0);
}


void XmlReaderTest::DefaultNamespace()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a xmlns=\"http://www.my1.net\">";
    input << "<a xmlns=\"http://www.my2.net\">b</a>";
    input << "</a>";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();

    // <my:a>
    const Pt::Xml::StartElement& startA = Pt::Xml::toStartElement(*it);
    PT_UNIT_ASSERT(startA.name() == L"a");
    PT_UNIT_ASSERT(startA.prefix() == L"");
    PT_UNIT_ASSERT(startA.namespaceUri() == L"http://www.my1.net");
    PT_UNIT_ASSERT(reader.depth() == 1);

    // <my:a>
    ++it;
    const Pt::Xml::StartElement& startA2 = Pt::Xml::toStartElement(*it);
    PT_UNIT_ASSERT(startA2.name() == L"a");
    PT_UNIT_ASSERT(startA2.prefix() == L"");
    PT_UNIT_ASSERT(startA2.namespaceUri() == "http://www.my2.net");
    PT_UNIT_ASSERT(reader.depth() == 2);

    // b
    ++it;
    const Pt::Xml::Node& charactersNode = *it;
    PT_UNIT_ASSERT(charactersNode.type() == Pt::Xml::Node::Characters);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::Characters*>(&charactersNode)->content() == L"b");
    PT_UNIT_ASSERT( reader.depth() == 2);

    // </my:a>
    ++it;
    const Pt::Xml::EndElement& endA2 = Pt::Xml::toEndElement(*it);
    PT_UNIT_ASSERT(endA2.name() == L"a");
    PT_UNIT_ASSERT(endA2.prefix() == L"");
    PT_UNIT_ASSERT(endA2.namespaceUri() == L"http://www.my2.net");
    PT_UNIT_ASSERT(reader.depth() == 1);

    // </my:a>
    ++it;
    const Pt::Xml::EndElement& endA = Pt::Xml::toEndElement(*it);

    PT_UNIT_ASSERT(endA.name() == L"a");
    PT_UNIT_ASSERT(endA.prefix() == L"");
    PT_UNIT_ASSERT(endA.namespaceUri() == L"http://www.my1.net");
    PT_UNIT_ASSERT( reader.depth() == 0);

    // End of document
    ++it;
    const Pt::Xml::Node& endDocument = *it;
    PT_UNIT_ASSERT(endDocument.type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT( reader.depth() == 0);
}


void XmlReaderTest::MissingCloseTag()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a><b></a>";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();

    // <a>
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    // <b>
    PT_UNIT_ASSERT_THROW(++it; ++it; ++it;, std::exception);
}


void XmlReaderTest::InvalidAttribute1()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a blub/>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::InvalidAttribute2()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a blub=/>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::InvalidAttribute3()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a blub=\"/>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::InvalidAttribute4()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a blub=bla=\"b\"/>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::EmptyAttribute()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a b=\"\"/>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& node = *it;

    PT_UNIT_ASSERT(node.type() == Pt::Xml::Node::StartElement);
    const Pt::Xml::StartElement* tag = dynamic_cast<const Pt::Xml::StartElement*>(&node);

    PT_UNIT_ASSERT(tag->attributes().has(L"b"));
    PT_UNIT_ASSERT(tag->attribute(L"b").narrow() == "");
}


void XmlReaderTest::AttributeWithSimpleText()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a b=\"a bcdefghijklmnopqrstuvwxyz\"/>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    PT_UNIT_ASSERT(startNode.type() == Pt::Xml::Node::StartElement);
    const Pt::Xml::StartElement* tag = dynamic_cast<const Pt::Xml::StartElement*>(&startNode);

    PT_UNIT_ASSERT(tag->attributes().has(L"b"));
    PT_UNIT_ASSERT(tag->attribute(L"b").narrow() == "a bcdefghijklmnopqrstuvwxyz");
}


void XmlReaderTest::AttributeWithUTF8()
{
    std::stringstream input;

    // The attribute value is a UTF-8 encoded 5-character word with greek characters.
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a b=\"" << (char)0xce << (char)0xba << (char)0xe1 << (char)0xbd << (char)0xb9 << (char)0xcf <<
                          (char)0x83 << (char)0xce << (char)0xbc << (char)0xce << (char)0xb5 << "\"/>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    PT_UNIT_ASSERT(startNode.type() == Pt::Xml::Node::StartElement);
    const Pt::Xml::StartElement* tag = dynamic_cast<const Pt::Xml::StartElement*>(&startNode);

    PT_UNIT_ASSERT(tag->attributes().has(L"b"));

    // This is the same 5-character word with greek characters as above, but this time not encoded but
    // directly in Unicode. If the UTF-8 decoding works this must be stored in the Attribute's String now.
    Pt::Char c[] = { 954, 8057, 963, 956, 949, 0 };
    PT_UNIT_ASSERT(tag->attribute(L"b") == c);
}


void XmlReaderTest::MultipleAttributesIteration()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a b=\"123\" c=\"456\" d=\"789\"/>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    const Pt::Xml::StartElement* tag = Pt::Xml::toStartElement(&startNode);
    PT_UNIT_ASSERT(tag);
    PT_UNIT_ASSERT(tag->attributes().has(L"b"));
    PT_UNIT_ASSERT(tag->attributes().has(L"c"));
    PT_UNIT_ASSERT(tag->attributes().has(L"d"));

    const Pt::Xml::AttributeList& attributes = tag->attributes();
    std::vector<Pt::Xml::Attribute>::const_iterator attributeIter = attributes.begin();

    PT_UNIT_ASSERT(attributeIter != attributes.end());

    Pt::Xml::Attribute attributeB = *attributeIter;
    PT_UNIT_ASSERT(attributeB.name().narrow() == "b");
    PT_UNIT_ASSERT(attributeB.value().narrow() == "123");

    attributeIter++;
    PT_UNIT_ASSERT(attributeIter != attributes.end());

    Pt::Xml::Attribute attributeC = *attributeIter;
    PT_UNIT_ASSERT(attributeC.name().narrow() == "c");
    PT_UNIT_ASSERT(attributeC.value().narrow() == "456");

    attributeIter++;
    PT_UNIT_ASSERT(attributeIter != attributes.end());

    Pt::Xml::Attribute attributeD = *attributeIter;
    PT_UNIT_ASSERT(attributeD.name().narrow() == "d");
    PT_UNIT_ASSERT(attributeD.value().narrow() == "789");

    attributeIter++;
    PT_UNIT_ASSERT(attributeIter == attributes.end());
}


void XmlReaderTest::ProcessingInstructionInProlog()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<?xml-stylesheet type=\"text/css\" href=\"styles.css\"?>";
    input << "<a/>";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::ProcessingInstruction);
    const Pt::Xml::ProcessingInstruction& pi = dynamic_cast<const Pt::Xml::ProcessingInstruction&>(*it);
    PT_UNIT_ASSERT(pi.data() == L"type=\"text/css\" href=\"styles.css\"");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlReaderTest::ProcessingInstructionInElement()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>123";
    input << "<?xml-stylesheet type=\"text/css\" href=\"styles.css\"?>";
    input << "456</a>";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::ProcessingInstruction);
    const Pt::Xml::ProcessingInstruction& pi = dynamic_cast<const Pt::Xml::ProcessingInstruction&>(*it);
    PT_UNIT_ASSERT(pi.data() == L"type=\"text/css\" href=\"styles.css\"");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::Characters);
    const Pt::Xml::Characters& chars = dynamic_cast<const Pt::Xml::Characters&>(*it);
    PT_UNIT_ASSERT(chars.content() == L"123456");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlReaderTest::ProcessingInstructionInEpilog()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    input << "<a/>\n";
    input << "<?xml-stylesheet type=\"text/css\" href=\"styles.css\"?>";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::ProcessingInstruction);
    const Pt::Xml::ProcessingInstruction& pi = dynamic_cast<const Pt::Xml::ProcessingInstruction&>(*it);
    PT_UNIT_ASSERT(pi.data() == L"type=\"text/css\" href=\"styles.css\"");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlReaderTest::IgnorableWhitespace()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>";
    input << "   <b></b>\n";
    input << "\t<c></c>\n";
    input << "</a>\n";

    Pt::Xml::XmlReader reader(input);
        
    Pt::Xml::XmlReader::Iterator it;
    for(it = reader.current(); it != reader.end(); ++it)
    {
        Pt::Xml::Characters* chars = toCharacters(&*it);
        if(chars)
        {
            PT_UNIT_ASSERT( chars->isIgnorable() );
        }
    }
}


void XmlReaderTest::CDATA()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>";
    input << "<![CDATA[<Element>pure &gt; data</Element>]]>";
    input << "</a>\n";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    const Pt::Xml::Node& node = *it;
    PT_UNIT_ASSERT(node.type() == Pt::Xml::Node::Characters);
    const Pt::Xml::Characters* chars = dynamic_cast<const Pt::Xml::Characters*>(&node);
    PT_UNIT_ASSERT(chars->content().narrow() == "<Element>pure &gt; data</Element>");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlReaderTest::DefaultEntities()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>&lt;&gt;&amp;&quot;&apos;&#1234;</a>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& startNodeA = *it;

    PT_UNIT_ASSERT(startNodeA.type() == Pt::Xml::Node::StartElement);

    ++it;
    const Pt::Xml::Node& characterNode = *it;
    PT_UNIT_ASSERT(characterNode.type() == Pt::Xml::Node::Characters);

    Pt::Char compare[] = { '<', '>', '&', '\"', '\'', 1234, 0 };

    const Pt::Xml::Characters* text = dynamic_cast<const Pt::Xml::Characters*>(&characterNode);
    PT_UNIT_ASSERT(text->content() == compare);
}


void XmlReaderTest::CommentInProlog()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<!--</a>-->";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlReaderTest::CommentBeforeRoot()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<!-- - ab --><a/>";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);
}


void XmlReaderTest::CommentInElement()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    input << "<a>123<!--a-->456</a>";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::Characters);
    const Pt::Xml::Characters& text = dynamic_cast<const Pt::Xml::Characters&>(*it);
    PT_UNIT_ASSERT(text.content() == L"123456");
}


void XmlReaderTest::CommentInEpilog()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a/><!--a-->";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlReaderTest::EmptyComment()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<!---->";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlReaderTest::CheckPerformance()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    for(int i = 0; i < 1000000; ++i)
    {
        input << "<testelem x=\"abc\">";
        input << "0123456789abcdefghijklmnopqrstuvwxyz";
        input << "</testelem>";
    }

    std::cerr << "PrefTest: ";
    Pt::Xml::XmlReader reader( input );

    //clock_t begin = clock();
    Pt::System::Clock c;
    c.start();


    for(Pt::Xml::XmlReader::Iterator it = reader.current(); it != reader.end(); ++it)
    {}
    Pt::Timespan ts = c.stop();
    std::cerr << "msecs: " << ts.toMSecs() << std::endl;
}
