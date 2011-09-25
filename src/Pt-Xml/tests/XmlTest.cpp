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

class XmlTest : public Pt::Unit::TestSuite
{
    public:
        XmlTest()
        : Pt::Unit::TestSuite("XmlTest")
        {
            this->registerMethod("MissingXmlDeclaration", *this, &XmlTest::MissingXmlDeclaration);
            this->registerMethod("EmptyXmlDeclaration", *this, &XmlTest::EmptyXmlDeclaration);
            this->registerMethod("EmptyDocument", *this, &XmlTest::EmptyDocument);
            this->registerMethod("DoctypeDeclaration", *this, &XmlTest::DoctypeDeclaration);
            this->registerMethod("EmptyElementTag", *this, &XmlTest::EmptyElementTag);
            this->registerMethod("InvalidTag1", *this, &XmlTest::InvalidTag1);
            this->registerMethod("InvalidTag2", *this, &XmlTest::InvalidTag2);
            this->registerMethod("InvalidTag3", *this, &XmlTest::InvalidTag3);
            this->registerMethod("InvalidTag4", *this, &XmlTest::InvalidTag4);
            this->registerMethod("InvalidTag5", *this, &XmlTest::InvalidTag5);
            this->registerMethod("NextElement", *this, &XmlTest::NextElement);
            this->registerMethod("NextTag", *this, &XmlTest::NextTag);
            this->registerMethod("ElementWithContent", *this, &XmlTest::ElementWithContent);
            this->registerMethod("DefaultEntities", *this, &XmlTest::DefaultEntities);
            this->registerMethod("InvalidAttribute1", *this, &XmlTest::InvalidAttribute1);
            this->registerMethod("InvalidAttribute2", *this, &XmlTest::InvalidAttribute2);
            this->registerMethod("InvalidAttribute3", *this, &XmlTest::InvalidAttribute3);
            this->registerMethod("InvalidAttribute4", *this, &XmlTest::InvalidAttribute4);
            this->registerMethod("EmptyAttribute", *this, &XmlTest::EmptyAttribute);
            this->registerMethod("AttributeWithSimpleText", *this, &XmlTest::AttributeWithSimpleText);
            this->registerMethod("AttributeWithUTF8", *this, &XmlTest::AttributeWithUTF8);
            this->registerMethod("MultipleAttributesIteration", *this, &XmlTest::MultipleAttributesIteration);
            this->registerMethod("CDATA", *this, &XmlTest::CDATA );
            this->registerMethod("CommentInProlog", *this, &XmlTest::CommentInProlog );
            this->registerMethod("CommentInElement", *this, &XmlTest::CommentInElement );
            this->registerMethod("CommentInEpilog", *this, &XmlTest::CommentInEpilog );
            this->registerMethod("EmptyComment", *this, &XmlTest::EmptyComment );
            this->registerMethod("CommentBeforeRoot", *this, &XmlTest::CommentBeforeRoot );
            this->registerMethod("MissingCloseTag", *this, &XmlTest::MissingCloseTag );
            this->registerMethod("ProcessingInstructionInProlog", *this, &XmlTest::ProcessingInstructionInProlog );
            this->registerMethod("ProcessingInstructionInElement", *this, &XmlTest::ProcessingInstructionInElement );
            this->registerMethod("ProcessingInstructionInEpilog", *this, &XmlTest::ProcessingInstructionInEpilog );
        }

    protected:
        void MissingXmlDeclaration();
        void EmptyXmlDeclaration();
        void EmptyDocument();
        void DoctypeDeclaration();
        void EmptyElementTag();
        void InvalidTag1();
        void InvalidTag2();
        void InvalidTag3();
        void InvalidTag4();
        void InvalidTag5();
        void NextElement();
        void NextTag();
        void ElementWithContent();
        void AttributeWithSimpleText();
        void AttributeWithUTF8();
        void MultipleAttributesIteration();
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

Pt::Unit::RegisterTest<XmlTest> register_XmlTest;


void XmlTest::MissingXmlDeclaration()
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


void XmlTest::EmptyXmlDeclaration()
{
    std::stringstream input;
    input << "<?xml ?>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& n = *it;

    PT_UNIT_ASSERT(n.type() == Pt::Xml::Node::EndDocument);
}


void XmlTest::EmptyDocument()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& n = *it;

    PT_UNIT_ASSERT(n.type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT(reader.documentVersion() == L"1.0");
    PT_UNIT_ASSERT(reader.documentEncoding() == L"UTF-8");
    PT_UNIT_ASSERT(reader.standaloneDocument() == true);
}


void XmlTest::DoctypeDeclaration()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    input << "<!DOCTYPE ressourcen SYSTEM \"ressourcen.dtd\">\n";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::DocType);

    const Pt::Xml::Node& node = *it;
    const Pt::Xml::DocTypeDeclaration* docType = dynamic_cast<const Pt::Xml::DocTypeDeclaration*>(&node);
    PT_UNIT_ASSERT(docType->content().narrow() == "DOCTYPE ressourcen SYSTEM \"ressourcen.dtd\"");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlTest::EmptyElementTag()
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


void XmlTest::InvalidTag1()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<>";

    Pt::Xml::XmlReader reader( input );

    // Expecting exception because empty tags are not allowed.
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlTest::InvalidTag2()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "</>";

    Pt::Xml::XmlReader reader( input );

    // Expecting exception because empty tags are not allowed.
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}



void XmlTest::InvalidTag3()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>>b</a>";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();

    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);
    PT_UNIT_ASSERT_THROW(++it; ++it; ++it;, std::exception);
}


void XmlTest::InvalidTag4()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<<a>b</a>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlTest::InvalidTag5()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<ab<cd>>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlTest::NextElement()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a><b><c>5</c></b></a>";

    Pt::Xml::XmlReader reader( input );
    const Pt::Xml::Node& node = reader.get();
    PT_UNIT_ASSERT(node.type() == Pt::Xml::Node::StartElement);

    reader.nextElement();
    const Pt::Xml::StartElement& se = reader.nextElement();
    PT_UNIT_ASSERT(se.name().narrow() == "c");

    PT_UNIT_ASSERT_THROW(reader.nextElement(), std::exception);
}


void XmlTest::NextTag()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a><b><c>5</c></b></a>";

    Pt::Xml::XmlReader reader( input );
    const Pt::Xml::Node& node = reader.get();
    PT_UNIT_ASSERT(node.type() == Pt::Xml::Node::StartElement);

    reader.nextTag();
    reader.nextTag();
    reader.nextTag();
    const Pt::Xml::Node& node2 = reader.nextTag();
    PT_UNIT_ASSERT(node2.type() == Pt::Xml::Node::EndElement);

    const Pt::Xml::EndElement& ee = static_cast<const Pt::Xml::EndElement&>(node2);
    PT_UNIT_ASSERT(ee.name().narrow() == "b");

    reader.nextTag();
    PT_UNIT_ASSERT_THROW(reader.nextTag(), std::exception);
}


void XmlTest::ElementWithContent()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>?!:=b</a>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    // <a>
    PT_UNIT_ASSERT(startNode.type() == Pt::Xml::Node::StartElement);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::StartElement*>(&startNode)->name().narrow() == "a");
    PT_UNIT_ASSERT( reader.depth() == 1);

    // b
    ++it;
    const Pt::Xml::Node& charactersNode = *it;

    PT_UNIT_ASSERT(charactersNode.type() == Pt::Xml::Node::Characters);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::Characters*>(&charactersNode)->content().narrow() == "?!:=b");
    PT_UNIT_ASSERT( reader.depth() == 1);

    // </a>
    ++it;
    const Pt::Xml::Node& endNode = *it;

    PT_UNIT_ASSERT(endNode.type() == Pt::Xml::Node::EndElement);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::EndElement*>(&endNode)->name().narrow() == "a");
    PT_UNIT_ASSERT( reader.depth() == 0);

    // End of document
    ++it;
    const Pt::Xml::Node& endDocument = *it;
    PT_UNIT_ASSERT(endDocument.type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT( reader.depth() == 0);
}


void XmlTest::MissingCloseTag()
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


void XmlTest::InvalidAttribute1()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a blub/>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlTest::InvalidAttribute2()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a blub=/>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlTest::InvalidAttribute3()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a blub=\"/>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlTest::InvalidAttribute4()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a blub=bla=\"b\"/>";

    Pt::Xml::XmlReader reader( input );
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlTest::EmptyAttribute()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a b=\"\"/>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& node = *it;

    PT_UNIT_ASSERT(node.type() == Pt::Xml::Node::StartElement);
    const Pt::Xml::StartElement* tag = dynamic_cast<const Pt::Xml::StartElement*>(&node);

    PT_UNIT_ASSERT(tag->hasAttribute(L"b"));
    PT_UNIT_ASSERT(tag->attribute(L"b").narrow() == "");
}


void XmlTest::AttributeWithSimpleText()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a b=\"a bcdefghijklmnopqrstuvwxyz\"/>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    PT_UNIT_ASSERT(startNode.type() == Pt::Xml::Node::StartElement);
    const Pt::Xml::StartElement* tag = dynamic_cast<const Pt::Xml::StartElement*>(&startNode);

    PT_UNIT_ASSERT(tag->hasAttribute(L"b"));
    PT_UNIT_ASSERT(tag->attribute(L"b").narrow() == "a bcdefghijklmnopqrstuvwxyz");
}


void XmlTest::AttributeWithUTF8()
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

    PT_UNIT_ASSERT(tag->hasAttribute(L"b"));

    // This is the same 5-character word with greek characters as above, but this time not encoded but
    // directly in Unicode. If the UTF-8 decoding works this must be stored in the Attribute's String now.
    Pt::Char c[] = { 954, 8057, 963, 956, 949, 0 };
    PT_UNIT_ASSERT(tag->attribute(L"b") == c);
}


void XmlTest::MultipleAttributesIteration()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a b=\"123\" c=\"456\" d=\"789\"/>";

    Pt::Xml::XmlReader reader( input );

    Pt::Xml::XmlReader::Iterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    PT_UNIT_ASSERT(startNode.type() == Pt::Xml::Node::StartElement);
    const Pt::Xml::StartElement* tag = dynamic_cast<const Pt::Xml::StartElement*>(&startNode);

    PT_UNIT_ASSERT(tag->hasAttribute(L"b"));
    PT_UNIT_ASSERT(tag->hasAttribute(L"c"));
    PT_UNIT_ASSERT(tag->hasAttribute(L"d"));

    std::list<Pt::Xml::Attribute> attributes = tag->attributes();
    std::list<Pt::Xml::Attribute>::const_iterator attributeIter = attributes.begin();

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


void XmlTest::ProcessingInstructionInProlog()
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


void XmlTest::ProcessingInstructionInElement()
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


void XmlTest::ProcessingInstructionInEpilog()
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


void XmlTest::CDATA()
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


void XmlTest::DefaultEntities()
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


void XmlTest::CommentInProlog()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<!--</a>-->";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlTest::CommentBeforeRoot()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<!-- - ab --><a/>";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);
}


void XmlTest::CommentInElement()
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


void XmlTest::CommentInEpilog()
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


void XmlTest::EmptyComment()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<!---->";

    Pt::Xml::XmlReader reader( input );
    Pt::Xml::XmlReader::Iterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlTest::CheckPerformance()
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
    std::cerr << "msecs: " << ts.totalMSecs() << std::endl;
}
