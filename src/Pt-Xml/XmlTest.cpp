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
#include "Pt/Xml/CData.h"

#include "Pt/Text/Utf8Codec.h"
#include "Pt/Text/TextStream.h"

#include "Pt/System/Clock.h"
#include "Pt/System/TimeValue.h"
#include "Pt/System/Process.h"

#include "Pt/Char.h"

#include <string>
#include <sstream>

using namespace Pt;
using namespace Pt::Xml;
using namespace std;



class XmlTest : public Pt::Unit::TestSuite
{
    public:
        XmlTest()
        : Pt::Unit::TestSuite("XmlTest")
        {

            this->registerMethod("MissingXmlDeclaration" , *this, &XmlTest::MissingXmlDeclaration);
            this->registerMethod("EmptyDocument" , *this, &XmlTest::EmptyDocument);
            this->registerMethod("DoctypeDeclaration" , *this, &XmlTest::DoctypeDeclaration);
            this->registerMethod("EmptyElementTag" , *this, &XmlTest::EmptyElementTag);
            this->registerMethod("InvalidTag1" , *this, &XmlTest::InvalidTag1);
            this->registerMethod("InvalidTag2" , *this, &XmlTest::InvalidTag2);
            this->registerMethod("NextElement" , *this, &XmlTest::NextElement);
            this->registerMethod("NextTag" , *this, &XmlTest::NextTag);
            this->registerMethod("ElementWithContent" , *this, &XmlTest::ElementWithContent);
            this->registerMethod("DefaultEntities" , *this, &XmlTest::DefaultEntities);
            this->registerMethod("AttributeWithSimpleText" , *this, &XmlTest::AttributeWithSimpleText);
            this->registerMethod("AttributeWithUTF8" , *this, &XmlTest::AttributeWithUTF8);
            this->registerMethod("MultipleAttributesIteration" , *this, &XmlTest::MultipleAttributesIteration);
            //this->registerMethod("CDATA " , *this, &XmlTest::CDATA );
            //CPPUNIT_TEST( testDoctypeDeclaration );

            //CPPUNIT_TEST( testTagMissingCloseTag );
            //CPPUNIT_TEST( testErrorDoubleCloseCharacter );
            //CPPUNIT_TEST( testErrorDoubleOpenCharacter1 );
            //CPPUNIT_TEST( testErrorDoubleOpenCharacter2 );
            //CPPUNIT_TEST( testErrorMissingAttributeValue1 );
            //CPPUNIT_TEST( testErrorMissingAttributeValue2 );
            //CPPUNIT_TEST( testErrorMissingAttributeValue3 );
            //CPPUNIT_TEST( testErrorIncorrectAttribute );
            //CPPUNIT_TEST( testEmptyAttribute );
            // TODO Currently removed, as PIs are not supported yet. Aktivate this again as soon as PIs are supported.
            // CPPUNIT_TEST( testProcessingInstructions );

            //CPPUNIT_TEST( testComments1 );
            //CPPUNIT_TEST( testComments2 );
            //CPPUNIT_TEST( testComments3 );
            //CPPUNIT_TEST( testComments4 );
            //CPPUNIT_TEST( testComments5 );
            //CPPUNIT_TEST( testPerf );

        }


protected:
	void MissingXmlDeclaration();
	void EmptyDocument();
	void DoctypeDeclaration();
    void EmptyElementTag();
    void InvalidTag1();
    void InvalidTag2();
    void NextElement();
    void NextTag();
	void ElementWithContent();

	void testTagMissingCloseTag();
	void testErrorDoubleCloseCharacter();
	void testErrorDoubleOpenCharacter1();
	void testErrorDoubleOpenCharacter2();
	void testErrorMissingAttributeValue1();
	void testErrorMissingAttributeValue2();
	void testErrorMissingAttributeValue3();
	void testErrorIncorrectAttribute();
	void testEmptyAttribute();
	void AttributeWithSimpleText();
	void AttributeWithUTF8();
	void MultipleAttributesIteration();
	void testProcessingInstructions();
	void CDATA();
	void DefaultEntities();
	void testComments1();
	void testComments2();
	void testComments3();
	void testComments4();
	void testComments5();
	void testPerf();
};

Pt::Unit::RegisterTest<XmlTest> register_XmlTest;



void XmlTest::MissingXmlDeclaration()
{
	stringstream input;
	input << "<a/>";

    try {
        XmlReader reader(input);
        PT_UNIT_ASSERT_MSG(false, "Expected exception when xml declaration is missing");
    }
    catch(...)
    {
    }
}


void XmlTest::EmptyDocument()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& n = *it;

	PT_UNIT_ASSERT(n.type() == Node::EndDocument);
}


void XmlTest::DoctypeDeclaration()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<!DOCTYPE ressourcen SYSTEM \"ressourcen.dtd\">";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& docTypeNode = *it;

	PT_UNIT_ASSERT(docTypeNode.type() == Node::DocType);

	const Xml::DocTypeDeclaration* docTypeDeclaration = dynamic_cast<const Xml::DocTypeDeclaration*>(&docTypeNode);
	PT_UNIT_ASSERT(docTypeDeclaration->content().narrow() == "DOCTYPE ressourcen SYSTEM \"ressourcen.dtd\"");

	++it;
	PT_UNIT_ASSERT((*it).type() == Node::EndDocument);
}


void XmlTest::EmptyElementTag()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a/>";

	XmlReader reader( input );
	PT_UNIT_ASSERT( reader.depth() == 0)
	
	XmlReader::Iterator it = reader.current();
	const Xml::Node& startNode = *it;

	PT_UNIT_ASSERT(startNode.type() == Node::StartElement != 0);
	PT_UNIT_ASSERT(dynamic_cast<const Xml::StartElement*>(&startNode)->name().narrow() == "a");
	PT_UNIT_ASSERT( reader.depth() == 1)

	++it;
	const Xml::Node& endNode = *it;
	PT_UNIT_ASSERT(endNode.type() == Node::EndElement);
	PT_UNIT_ASSERT(dynamic_cast<const Xml::EndElement*>(&endNode)->name().narrow() == "a");
	PT_UNIT_ASSERT( reader.depth() == 0)

	++it;
	const Xml::Node& endDocument = *it;
	PT_UNIT_ASSERT(endDocument.type() == Node::EndDocument);
	PT_UNIT_ASSERT( reader.depth() == 0)
}


void XmlTest::InvalidTag1()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<>";

	XmlReader reader( input );

	// Expecting exception because empty tags are not allowed.
	PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlTest::InvalidTag2()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "</>";

	XmlReader reader( input );

	// Expecting exception because empty tags are not allowed.
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlTest::NextElement()
{
    stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a><b><c>5</c></b></a>";

    XmlReader reader( input );
    const Node& node = reader.get();
    PT_UNIT_ASSERT(node.type() == Node::StartElement);

    reader.nextElement();
    const StartElement& se = reader.nextElement();
    PT_UNIT_ASSERT(se.name().narrow() == "c");

    PT_UNIT_ASSERT_THROW(reader.nextElement(), std::exception);
}


void XmlTest::NextTag()
{
    stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a><b><c>5</c></b></a>";

    XmlReader reader( input );
    const Node& node = reader.get();
    PT_UNIT_ASSERT(node.type() == Node::StartElement);

    reader.nextTag();
    reader.nextTag();
    reader.nextTag();
    const Node& node2 = reader.nextTag();
    PT_UNIT_ASSERT(node2.type() == Node::EndElement);

    const EndElement& ee = static_cast<const EndElement&>(node2);
    PT_UNIT_ASSERT(ee.name().narrow() == "b");

    reader.nextTag();
    PT_UNIT_ASSERT_THROW(reader.nextTag(), std::exception);
}


void XmlTest::ElementWithContent()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a>b</a>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& startNode = *it;

	// <a>
	PT_UNIT_ASSERT(startNode.type() == Node::StartElement);
	PT_UNIT_ASSERT(dynamic_cast<const Xml::StartElement*>(&startNode)->name().narrow() == "a");
	PT_UNIT_ASSERT( reader.depth() == 1)

	// b
	++it;
	const Xml::Node& charactersNode = *it;

	PT_UNIT_ASSERT(charactersNode.type() == Node::Characters);
	PT_UNIT_ASSERT(dynamic_cast<const Xml::Characters*>(&charactersNode)->content().narrow() == "b");
	PT_UNIT_ASSERT( reader.depth() == 1)

	// </a>
	++it;
	const Xml::Node& endNode = *it;

	PT_UNIT_ASSERT(endNode.type() == Node::EndElement);
	PT_UNIT_ASSERT(dynamic_cast<const Xml::EndElement*>(&endNode)->name().narrow() == "a");
	PT_UNIT_ASSERT( reader.depth() == 0)

	// End of document
	++it;
	const Xml::Node& endDocument = *it;
	PT_UNIT_ASSERT(endDocument.type() == Node::EndDocument);
	PT_UNIT_ASSERT( reader.depth() == 0)
}


void XmlTest::testTagMissingCloseTag()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a><b></a>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& startNodeA = *it;


	// <a>
	PT_UNIT_ASSERT(startNodeA.type() == Node::StartElement);
	PT_UNIT_ASSERT(dynamic_cast<const Xml::StartElement*>(&startNodeA)->name().narrow() == "a");


	// <b>
	++it;
	const Xml::Node& startNodeB = *it;

	PT_UNIT_ASSERT(startNodeB.type() == Node::StartElement);
	PT_UNIT_ASSERT(dynamic_cast<const Xml::StartElement*>(&startNodeB)->name().narrow() == "b");


	// </a>
	++it;
	const Xml::Node& endNode = *it;

	PT_UNIT_ASSERT(endNode.type() == Node::EndElement);
	PT_UNIT_ASSERT(dynamic_cast<const Xml::EndElement*>(&endNode)->name().narrow() == "a");


	// End of document
	++it;
	const Xml::Node& endDocument = *it;

	PT_UNIT_ASSERT(endDocument.type() == Node::EndDocument);
}


void XmlTest::testErrorDoubleCloseCharacter()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a>>b</a>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& startNodeA = *it;


	// <a>
	PT_UNIT_ASSERT(startNodeA.type() == Node::StartElement);
	PT_UNIT_ASSERT(dynamic_cast<const Xml::StartElement*>(&startNodeA)->name().narrow() == "a");

	// >  -  An exception has to happen as a single ">" is not valid XML.
	//PT_UNIT_ASSERT_THROW(++it, LogicError);
}


void XmlTest::testErrorDoubleOpenCharacter1()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<<a>b</a>";

	// An exception has to happen as "<<a>" is not valid XML.
	//PT_UNIT_ASSERT_THROW({ XmlReader reader( input ); XmlReader::Iterator it = reader.current(); }, LogicError);
}


void XmlTest::testErrorDoubleOpenCharacter2()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<ab<cd>>";

	// An exception has to happen as "<ab<cd>>" is not valid XML.
	//PT_UNIT_ASSERT_THROW({ XmlReader reader( input ); XmlReader::Iterator it = reader.current(); }, LogicError);
}


void XmlTest::testErrorMissingAttributeValue1()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a blub/>";

	XmlReader reader( input );

	//PT_UNIT_ASSERT_THROW(reader.current(), LogicError);
}


void XmlTest::testErrorMissingAttributeValue2()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a blub=/>";

	XmlReader reader( input );

	//PT_UNIT_ASSERT_THROW(reader.current(), LogicError);
}


void XmlTest::testErrorMissingAttributeValue3()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a blub=\"/>";

	XmlReader reader( input );

	//PT_UNIT_ASSERT_THROW(reader.current(), LogicError);
}


void XmlTest::testErrorIncorrectAttribute()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a blub=bla=\"b\"/>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();

	// TODO Aktuell wird hier fälschlicherweise ein Attribut "blub" mit dem Wert "b" gelesen.
	//PT_UNIT_ASSERT_THROW(reader.current(), LogicError);
}


void XmlTest::testEmptyAttribute()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a b=\"\"/>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& startNode = *it;

	PT_UNIT_ASSERT(startNode.type() == Node::StartElement);
	const Xml::StartElement* tag = dynamic_cast<const Xml::StartElement*>(&startNode);

	PT_UNIT_ASSERT(tag->hasAttribute(L"b"));
	PT_UNIT_ASSERT(tag->attribute(L"b").narrow() == "");
}


void XmlTest::AttributeWithSimpleText()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a b=\"abcdefghijklmnopqrstuvwxyz\"/>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& startNode = *it;

	PT_UNIT_ASSERT(startNode.type() == Node::StartElement);
	const Xml::StartElement* tag = dynamic_cast<const Xml::StartElement*>(&startNode);

	PT_UNIT_ASSERT(tag->hasAttribute(L"b"));
	PT_UNIT_ASSERT(tag->attribute(L"b").narrow() == "abcdefghijklmnopqrstuvwxyz");
}


void XmlTest::AttributeWithUTF8()
{
	stringstream input;

	// The attribute value is a UTF-8 encoded 5-character word with greek characters.
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a b=\"" << (char)0xce << (char)0xba << (char)0xe1 << (char)0xbd << (char)0xb9 << (char)0xcf <<
	                      (char)0x83 << (char)0xce << (char)0xbc << (char)0xce << (char)0xb5 << "\"/>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& startNode = *it;

	PT_UNIT_ASSERT(startNode.type() == Node::StartElement);
	const Xml::StartElement* tag = dynamic_cast<const Xml::StartElement*>(&startNode);

	PT_UNIT_ASSERT(tag->hasAttribute(L"b"));

	// This is the same 5-character word with greek characters as above, but this time not encoded but
	// directly in Unicode. If the UTF-8 decoding works this must be stored in the Attribute's String now.
	Char c[] = { 954, 8057, 963, 956, 949, 0 };
	PT_UNIT_ASSERT(tag->attribute(L"b") == c);
}


void XmlTest::MultipleAttributesIteration()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a b=\"123\" c=\"456\" d=\"789\"/>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& startNode = *it;

	PT_UNIT_ASSERT(startNode.type() == Node::StartElement);
	const Xml::StartElement* tag = dynamic_cast<const Xml::StartElement*>(&startNode);

	PT_UNIT_ASSERT(tag->hasAttribute(L"b"));
	PT_UNIT_ASSERT(tag->hasAttribute(L"c"));
	PT_UNIT_ASSERT(tag->hasAttribute(L"d"));

	std::list<Attribute> attributes = tag->attributes();
	std::list<Attribute>::const_iterator attributeIter = attributes.begin();


	PT_UNIT_ASSERT(attributeIter != attributes.end());

	Attribute attributeB = *attributeIter;
	PT_UNIT_ASSERT(attributeB.name().narrow() == "b");
	PT_UNIT_ASSERT(attributeB.value().narrow() == "123");


	attributeIter++;
	PT_UNIT_ASSERT(attributeIter != attributes.end());

	Attribute attributeC = *attributeIter;
	PT_UNIT_ASSERT(attributeC.name().narrow() == "c");
	PT_UNIT_ASSERT(attributeC.value().narrow() == "456");


	attributeIter++;
	PT_UNIT_ASSERT(attributeIter != attributes.end());

	Attribute attributeD = *attributeIter;
	PT_UNIT_ASSERT(attributeD.name().narrow() == "d");
	PT_UNIT_ASSERT(attributeD.value().narrow() == "789");


	attributeIter++;
	PT_UNIT_ASSERT(attributeIter == attributes.end());
}

void XmlTest::testProcessingInstructions()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<?xml-stylesheet type=\"text/css\" href=\"styles.css\"?>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& processingInstructionNode = *it;

	PT_UNIT_ASSERT(processingInstructionNode.type() == Node::ProcessingInstruction);
	//const Xml::ProcessingInstruction* pi = dynamic_cast<const Xml::ProcessingInstruction*>(&processingInstructionNode);

	// TODO Add more testing as soon as XmlReader supports ProcessingInstructions.
}


void XmlTest::CDATA()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<![CDATA[<Element>dieses Element wird &gt; nur als Zeichenfolge ausgegeben</Element>]]>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& cDataNode = *it;

	PT_UNIT_ASSERT(cDataNode.type() == Node::CData);
	const Xml::CData* cDataElement = dynamic_cast<const Xml::CData*>(&cDataNode);

	PT_UNIT_ASSERT(cDataElement->content().narrow() == "<Element>dieses Element wird &gt; nur als Zeichenfolge ausgegeben</Element>");


	++it;
	const Xml::Node& endDocument = *it;
	PT_UNIT_ASSERT(endDocument.type() == Node::EndDocument);
}


void XmlTest::DefaultEntities()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<a>&lt;&gt;&amp;&quot;&apos;&#1234;</a>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& startNodeA = *it;

	PT_UNIT_ASSERT(startNodeA.type() == Node::StartElement);

	++it;
	const Xml::Node& characterNode = *it;
	PT_UNIT_ASSERT(characterNode.type() == Node::Characters);

	Char compare[] = { '<', '>', '&', '\"', '\'', 1234, 0 };

	const Xml::Characters* text = dynamic_cast<const Xml::Characters*>(&characterNode);
	PT_UNIT_ASSERT(text->content() == compare);
}


void XmlTest::testComments1()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<!--a-->";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& commentNode = *it;

	const Xml::Comment* comment = dynamic_cast<const Xml::Comment*>(&commentNode);

	PT_UNIT_ASSERT(comment->text().narrow() == "a");

	++it;
	const Xml::Node& endDocument = *it;
	PT_UNIT_ASSERT(endDocument.type() == Node::EndDocument);
}


void XmlTest::testComments2()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<!--a>-->";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& commentNode = *it;

	const Xml::Comment* comment = dynamic_cast<const Xml::Comment*>(&commentNode);

	PT_UNIT_ASSERT(comment->text().narrow() == "a>");

	++it;
	const Xml::Node& endDocument = *it;
	PT_UNIT_ASSERT(endDocument.type() == Node::EndDocument);
}


void XmlTest::testComments3()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<!--a > </a> -->";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& commentNode = *it;

	const Xml::Comment* comment = dynamic_cast<const Xml::Comment*>(&commentNode);

	PT_UNIT_ASSERT(comment->text().narrow() == "a> </a> ");

	++it;
	const Xml::Node& endDocument = *it;
	PT_UNIT_ASSERT(endDocument.type() == Node::EndDocument);
}


void XmlTest::testComments4()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<!---->";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& commentNode = *it;

	const Xml::Comment* comment = dynamic_cast<const Xml::Comment*>(&commentNode);

	PT_UNIT_ASSERT(comment->text().narrow() == "");

	++it;
	const Xml::Node& endDocument = *it;
	PT_UNIT_ASSERT(endDocument.type() == Node::EndDocument);
}


void XmlTest::testComments5()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
	input << "<!-- <!-- ab --><a>";

	XmlReader reader( input );

	XmlReader::Iterator it = reader.current();
	const Xml::Node& commentNode = *it;

	const Xml::Comment* comment = dynamic_cast<const Xml::Comment*>(&commentNode);

	PT_UNIT_ASSERT(comment->text().narrow() == " <!-- ab ");


	++it;
	const Xml::Node& startTagA = *it;
	PT_UNIT_ASSERT(startTagA.type() == Node::StartElement);


	++it;
	const Xml::Node& endDocument = *it;
	PT_UNIT_ASSERT(endDocument.type() == Node::EndDocument);
}


void XmlTest::testPerf()
{
	stringstream input;
	input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

	for(int i = 0; i < 1000000; ++i)
	{
		input << "<testelem x=\"abc\">";
		input << "0123456789abcdefghijklmnopqrstuvwxyz";
		input << "</testelem>";
	}

	cerr << "PrefTest: ";
	XmlReader reader( input );

	//clock_t begin = clock();
	Pt::System::Clock c;
	c.start();


	for(XmlReader::Iterator it = reader.current(); it != reader.end(); ++it)
	{}
	Pt::System::TimeValue v = c.stop();
	cerr << "seconds: " << v.seconds() << "   ms: " << (v.microSeconds() /1000) << endl;
}

