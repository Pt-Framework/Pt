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

#include <sstream>

class XmlWriterTest : public Pt::Unit::TestSuite
{
    public:
        XmlWriterTest()
        : Pt::Unit::TestSuite("XmlWriterTest")
        {
            this->registerMethod("Element" , *this, &XmlWriterTest::Element);
            this->registerMethod("TextElement" , *this, &XmlWriterTest::TextElement);
        }

    protected:

	    void Element()
	    {
	        std::stringstream ss;
	        Pt::Xml::XmlWriter writer(ss);
	        writer.writeStartElement(L"first");
	        writer.writeEndElement();
	        writer.flush();

	        std::stringstream result;
	        result << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
            result << "<first>" << std::endl;
            result << "</first>" << std::endl;

		    PT_UNIT_ASSERT( result.str() == ss.str());
	    }

	    void TextElement()
	    {
	        std::stringstream ss;
	        Pt::Xml::XmlWriter writer(ss);
            writer.writeElement(L"fourth", L"Hello world!");
            writer.flush();

	        std::stringstream result;
	        result << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
            result << "<fourth>Hello world!</fourth>" << std::endl;

		    PT_UNIT_ASSERT( result.str() == ss.str());
	    }
};

Pt::Unit::RegisterTest<XmlWriterTest> register_XmlWriterTest;
