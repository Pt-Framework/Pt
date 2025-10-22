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
#include "Pt/Xml/XmlResolver.h"
#include "Pt/Xml/DocTypeValidator.h"
#include "Pt/Xml/InputSource.h"
#include <Pt/Xml/Entity.h>
#include <Pt/Xml/Notation.h>
#include "Pt/Xml/StartDocument.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/Comment.h"
#include "Pt/Xml/Entity.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/EndDocument.h"
#include "Pt/Xml/DocType.h"
#include "Pt/Xml/DocTypeDefinition.h"
#include "Pt/Xml/ProcessingInstruction.h"
#include "Pt/System/Clock.h"
#include "Pt/String.h"
#include <string>
#include <map>
#include <sstream>
#include <iostream>


class XmlTestResolver : public Pt::Xml::XmlResolver
{
    public:
        XmlTestResolver()
        {}

        virtual ~XmlTestResolver()
        {}

        void addInput(const Pt::String& id, const Pt::String& txt)
        {
            _input.insert( std::make_pair(id, txt) );
        }

    protected:
        virtual Pt::Xml::InputSource* onResolveInput(const Pt::String& publicId, 
                                                     const Pt::String& systemId)
        {
            std::map<Pt::String, Pt::String>::iterator it;
            
            it = _input.find(publicId);
            if( it != _input.end() )
                return new Pt::Xml::StringInputSource(it->second);

            it = _input.find(systemId);
            if( it != _input.end() )
                return new Pt::Xml::StringInputSource(it->second);

            return 0;
        }

        virtual void onReleaseInput(Pt::Xml::InputSource* is)
        {
            if(is)
            {
                delete is;
            }
        }

        Pt::TextCodec<Pt::Char, char>* onResolveEncoding(const Pt::Xml::ByteorderMark& bom,
                                                         const Pt::Xml::XmlDeclaration& decl)
        { 
            return new Pt::Utf8Codec(); 
        }
        
    private:
        std::map<Pt::String, Pt::String> _input;
};

class XmlReaderTest : public Pt::Unit::TestSuite
{
    public:
        XmlReaderTest()
        : Pt::Unit::TestSuite("XmlReaderTest")
        {
            this->registerMethod("MissingXmlDeclaration", *this, &XmlReaderTest::MissingXmlDeclaration);
            this->registerMethod("EmptyXmlDeclaration", *this, &XmlReaderTest::EmptyXmlDeclaration);

            this->registerMethod("ByteorderMarkUtf8", *this, &XmlReaderTest::ByteorderMarkUtf8);
            this->registerMethod("ByteorderMarkUtf16LE", *this, &XmlReaderTest::ByteorderMarkUtf16LE);
            this->registerMethod("ByteorderMarkUtf16BE", *this, &XmlReaderTest::ByteorderMarkUtf16BE);
            this->registerMethod("ByteorderMarkGeneric16LE", *this, &XmlReaderTest::ByteorderMarkGeneric16LE);
            this->registerMethod("ByteorderMarkGeneric16BE", *this, &XmlReaderTest::ByteorderMarkGeneric16BE);

            this->registerMethod("ByteorderMarkUtf32LE", *this, &XmlReaderTest::ByteorderMarkUtf32LE);
            this->registerMethod("ByteorderMarkUtf32BE", *this, &XmlReaderTest::ByteorderMarkUtf32BE);
            this->registerMethod("ByteorderMarkGeneric32LE", *this, &XmlReaderTest::ByteorderMarkGeneric32LE);
            this->registerMethod("ByteorderMarkGeneric32BE", *this, &XmlReaderTest::ByteorderMarkGeneric32BE);
            
            this->registerMethod("DtdEmptyDocument", *this, &XmlReaderTest::DtdEmptyDocument);
            this->registerMethod("DtdExternalSubsetPublicId", *this, &XmlReaderTest::DtdExternalSubsetPublicId);
            this->registerMethod("DtdExternalSubsetSystemId", *this, &XmlReaderTest::DtdExternalSubsetSystemId);
            this->registerMethod("DtdExternalAndInternalSubset", *this, &XmlReaderTest::DtdExternalAndInternalSubset);
            this->registerMethod("DtdValidateAttributes", *this, &XmlReaderTest::DtdValidateAttributes);
            this->registerMethod("DtdValidateEnumAttributes", *this, &XmlReaderTest::DtdValidateEnumAttributes);
            this->registerMethod("DtdValidateIDAttributes", *this, &XmlReaderTest::DtdValidateIDAttributes);
            this->registerMethod("DtdValidateEntityAttributes", *this, &XmlReaderTest::DtdValidateEntityAttributes);
            this->registerMethod("DtdValidateNotationAttributes", *this, &XmlReaderTest::DtdValidateNotationAttributes);
            this->registerMethod("DtdValidateElementContent", *this, &XmlReaderTest::DtdValidateElementContent);
            this->registerMethod("DtdAnyElementContent", *this, &XmlReaderTest::DtdAnyElementContent);
            this->registerMethod("DtdValidateWithNamespace", *this, &XmlReaderTest::DtdValidateWithNamespace);
            this->registerMethod("DtdNotations", *this, &XmlReaderTest::DtdNotations);
            this->registerMethod("DtdInclude", *this, &XmlReaderTest::DtdInclude);
            this->registerMethod("DtdIgnore", *this, &XmlReaderTest::DtdIgnore);
            this->registerMethod("DtdComment", *this, &XmlReaderTest::DtdComment);
            this->registerMethod("DtdProcesssingInstruction", *this, &XmlReaderTest::DtdProcesssingInstruction);
            
            this->registerMethod("EmptyDocument", *this, &XmlReaderTest::EmptyDocument);
            this->registerMethod("EmptyElementTag", *this, &XmlReaderTest::EmptyElementTag);
            this->registerMethod("InvalidTag1", *this, &XmlReaderTest::InvalidTag1);
            this->registerMethod("InvalidTag2", *this, &XmlReaderTest::InvalidTag2);
            this->registerMethod("InvalidTag3", *this, &XmlReaderTest::InvalidTag3);
            this->registerMethod("InvalidTag4", *this, &XmlReaderTest::InvalidTag4);
            this->registerMethod("InvalidTag5", *this, &XmlReaderTest::InvalidTag5);
            this->registerMethod("UnmatchedElement", *this, &XmlReaderTest::UnmatchedElement);
            this->registerMethod("ElementWithContent", *this, &XmlReaderTest::ElementWithContent);
            this->registerMethod("MaxCharacters", *this, &XmlReaderTest::MaxCharacters);

            this->registerMethod("ElementWithNamespace", *this, &XmlReaderTest::ElementWithNamespace);
            this->registerMethod("AttributeWithNamespace", *this, &XmlReaderTest::AttributeWithNamespace);
            this->registerMethod("DefaultNamespace", *this, &XmlReaderTest::DefaultNamespace);
            
            this->registerMethod("DefaultEntities", *this, &XmlReaderTest::DefaultEntities);
            this->registerMethod("CustomEntities", *this, &XmlReaderTest::CustomEntities);
            this->registerMethod("EntitySelfReference", *this, &XmlReaderTest::EntitySelfReference);
            this->registerMethod("ExternalEntities", *this, &XmlReaderTest::ExternalEntities);
            this->registerMethod("ParameterEntities", *this, &XmlReaderTest::ParameterEntities);
            this->registerMethod("ParameterEntitySelfReference", *this, &XmlReaderTest::ParameterEntitySelfReference);
            this->registerMethod("MaxEntityRecursion", *this, &XmlReaderTest::MaxEntityRecursion);
            
            this->registerMethod("InvalidAttribute1", *this, &XmlReaderTest::InvalidAttribute1);
            this->registerMethod("InvalidAttribute2", *this, &XmlReaderTest::InvalidAttribute2);
            this->registerMethod("InvalidAttribute3", *this, &XmlReaderTest::InvalidAttribute3);
            this->registerMethod("InvalidAttribute4", *this, &XmlReaderTest::InvalidAttribute4);
            this->registerMethod("EmptyAttribute", *this, &XmlReaderTest::EmptyAttribute);
            this->registerMethod("AttributeWithSimpleText", *this, &XmlReaderTest::AttributeWithSimpleText);
            this->registerMethod("AttributeWithUTF8", *this, &XmlReaderTest::AttributeWithUTF8);
            this->registerMethod("MultipleAttributesIteration", *this, &XmlReaderTest::MultipleAttributesIteration);
            this->registerMethod("NormalizeAttributes", *this, &XmlReaderTest::NormalizeAttributes);

            this->registerMethod("IgnorableWhitespace", *this, &XmlReaderTest::IgnorableWhitespace);
            this->registerMethod("CDATAAsCharacters", *this, &XmlReaderTest::CDATAAsCharacters);
            this->registerMethod("CDATA", *this, &XmlReaderTest::CDATA );
            this->registerMethod("MaxCDATA", *this, &XmlReaderTest::MaxCDATA);
            this->registerMethod("StartDocument", *this, &XmlReaderTest::StartDocument);
            this->registerMethod("CommentInProlog", *this, &XmlReaderTest::CommentInProlog );
            this->registerMethod("CommentInElement", *this, &XmlReaderTest::CommentInElement );
            this->registerMethod("CommentInEpilog", *this, &XmlReaderTest::CommentInEpilog );
            this->registerMethod("EmptyComment", *this, &XmlReaderTest::EmptyComment );
            this->registerMethod("CommentBeforeRoot", *this, &XmlReaderTest::CommentBeforeRoot );
            this->registerMethod("MissingCloseTag", *this, &XmlReaderTest::MissingCloseTag );
            this->registerMethod("ProcessingInstructionInProlog", *this, &XmlReaderTest::ProcessingInstructionInProlog );
            this->registerMethod("ProcessingInstructionInElement", *this, &XmlReaderTest::ProcessingInstructionInElement );
            this->registerMethod("ProcessingInstructionInEpilog", *this, &XmlReaderTest::ProcessingInstructionInEpilog );

            this->registerMethod("ZZZ1_Benchmark", *this, &XmlReaderTest::Benchmark);
            //this->registerMethod("ZZZ2_Benchmark", *this, &XmlReaderTest::Benchmark);
            //this->registerMethod("ZZZ3_Benchmark", *this, &XmlReaderTest::Benchmark);
            //this->registerMethod("ZZZ4_Benchmark", *this, &XmlReaderTest::Benchmark);
            //this->registerMethod("ZZZ5_Benchmark", *this, &XmlReaderTest::Benchmark);
            //this->registerMethod("ZZZ6_Benchmark", *this, &XmlReaderTest::Benchmark);
            //this->registerMethod("ZZZ7_Benchmark", *this, &XmlReaderTest::Benchmark);
            //this->registerMethod("ZZZ8_Benchmark", *this, &XmlReaderTest::Benchmark);
            //this->registerMethod("ZZZ9_Benchmark", *this, &XmlReaderTest::Benchmark);
            //this->registerMethod("ZZZ0_Benchmark", *this, &XmlReaderTest::Benchmark);
        }

    protected:
        void MissingXmlDeclaration();
        void EmptyXmlDeclaration();

        void ByteorderMarkUtf8();
        void ByteorderMarkUtf16LE();
        void ByteorderMarkUtf16BE();
        void ByteorderMarkGeneric16LE();
        void ByteorderMarkGeneric16BE();
        void ByteorderMarkUtf32LE();
        void ByteorderMarkUtf32BE();
        void ByteorderMarkGeneric32LE();
        void ByteorderMarkGeneric32BE();

        void DtdEmptyDocument();
        void DtdExternalSubsetPublicId();
        void DtdExternalSubsetSystemId();
        void DtdExternalAndInternalSubset();
        void DtdValidateAttributes();
        void DtdValidateEnumAttributes();
        void DtdValidateIDAttributes();
        void DtdValidateEntityAttributes();
        void DtdValidateNotationAttributes();
        void DtdValidateElementContent();
        void DtdAnyElementContent();
        void DtdValidateWithNamespace();
        void DtdNotations();
        void DtdInclude();
        void DtdIgnore();
        void DtdComment();
        void DtdProcesssingInstruction();
        
        void EmptyDocument();
        void EmptyElementTag();
        void InvalidTag1();
        void InvalidTag2();
        void InvalidTag3();
        void InvalidTag4();
        void InvalidTag5();
        void UnmatchedElement();
        void ElementWithContent();
        void MaxCharacters();

        void ElementWithNamespace();
        void AttributeWithNamespace();
        void DefaultNamespace();
        void AttributeWithSimpleText();
        void AttributeWithUTF8();
        void MultipleAttributesIteration();
        void NormalizeAttributes();
        void IgnorableWhitespace();
        void CDATAAsCharacters();
        void CDATA();
        void MaxCDATA();
        void StartDocument();
        
        void DefaultEntities();
        void CustomEntities();
        void EntitySelfReference();
        void ParameterEntities();
        void ExternalEntities();
        void ParameterEntitySelfReference();
        void MaxEntityRecursion();
        
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
        void Benchmark();
};

Pt::Unit::RegisterTest<XmlReaderTest> register_XmlTest;


void XmlReaderTest::MissingXmlDeclaration()
{
    std::stringstream input;
    input << "<a/>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT( reader.depth() == 0);

    Pt::Xml::InputIterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    PT_UNIT_ASSERT(startNode.type() == Pt::Xml::Node::StartElement);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::StartElement*>(&startNode)->name().name() == "a");
    PT_UNIT_ASSERT( reader.depth() == 1);

    ++it;
    const Pt::Xml::Node& endNode = *it;
    PT_UNIT_ASSERT(endNode.type() == Pt::Xml::Node::EndElement);
    // TODO: PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::EndElement*>(&endNode)->name().narrow() == "a");
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

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    Pt::Xml::InputIterator it = reader.current();
    const Pt::Xml::Node& n = *it;

    PT_UNIT_ASSERT(n.type() == Pt::Xml::Node::EndDocument);
}


void XmlReaderTest::ByteorderMarkUtf8()
{
    std::stringstream input;
    input << char(0xef) << char(0xbb) << char(0xbf) << "<?xml version=\"1.0\" encoding=\"UTF-8\"?><a/>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT(reader.depth() == 0);

    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::StartElement* se = Pt::Xml::toStartElement(&*it);
    PT_UNIT_ASSERT(se);
    PT_UNIT_ASSERT(se->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 1);

    ++it;
    Pt::Xml::EndElement* ee = Pt::Xml::toEndElement(&*it);
    PT_UNIT_ASSERT(ee);
    PT_UNIT_ASSERT(ee->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 0);

    ++it;
    Pt::Xml::EndDocument* endDoc = Pt::Xml::toEndDocument(&*it);
    PT_UNIT_ASSERT(endDoc);
    PT_UNIT_ASSERT(reader.depth() == 0);
}


void XmlReaderTest::ByteorderMarkUtf16LE()
{
    std::stringstream input;
    input << char(0xff) << char(0xfe) 
          << char(0x3c) << char(0)
          << char(0x61) << char(0)
          << char(0x2f) << char(0)
          << char(0x3e) << char(0);

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT(reader.depth() == 0);

    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::StartElement* se = Pt::Xml::toStartElement(&*it);
    PT_UNIT_ASSERT(se);
    PT_UNIT_ASSERT(se->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 1);

    ++it;
    Pt::Xml::EndElement* ee = Pt::Xml::toEndElement(&*it);
    PT_UNIT_ASSERT(ee);
    PT_UNIT_ASSERT(ee->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 0);

    ++it;
    Pt::Xml::EndDocument* endDoc = Pt::Xml::toEndDocument(&*it);
    PT_UNIT_ASSERT(endDoc);
    PT_UNIT_ASSERT(reader.depth() == 0);
}


void XmlReaderTest::ByteorderMarkUtf16BE()
{
    std::stringstream input;
    input << char(0xfe) << char(0xff) 
          << char(0) << char(0x3c)  
          << char(0) << char(0x61) 
          << char(0) << char(0x2f) 
          << char(0) << char(0x3e);

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT(reader.depth() == 0);

    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::StartElement* se = Pt::Xml::toStartElement(&*it);
    PT_UNIT_ASSERT(se);
    PT_UNIT_ASSERT(se->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 1);

    ++it;
    Pt::Xml::EndElement* ee = Pt::Xml::toEndElement(&*it);
    PT_UNIT_ASSERT(ee);
    PT_UNIT_ASSERT(ee->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 0);

    ++it;
    Pt::Xml::EndDocument* endDoc = Pt::Xml::toEndDocument(&*it);
    PT_UNIT_ASSERT(endDoc);
    PT_UNIT_ASSERT(reader.depth() == 0);
}


void XmlReaderTest::ByteorderMarkGeneric16LE()
{
    std::stringstream input;
    input << char(0x3c) << char(0)
          << char(0x61) << char(0)
          << char(0x2f) << char(0)
          << char(0x3e) << char(0);

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT(reader.depth() == 0);

    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::StartElement* se = Pt::Xml::toStartElement(&*it);
    PT_UNIT_ASSERT(se);
    PT_UNIT_ASSERT(se->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 1);

    ++it;
    Pt::Xml::EndElement* ee = Pt::Xml::toEndElement(&*it);
    PT_UNIT_ASSERT(ee);
    PT_UNIT_ASSERT(ee->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 0);

    ++it;
    Pt::Xml::EndDocument* endDoc = Pt::Xml::toEndDocument(&*it);
    PT_UNIT_ASSERT(endDoc);
    PT_UNIT_ASSERT(reader.depth() == 0);
}


void XmlReaderTest::ByteorderMarkGeneric16BE()
{
    std::stringstream input;
    input << char(0) << char(0x3c)  
          << char(0) << char(0x61) 
          << char(0) << char(0x2f) 
          << char(0) << char(0x3e);

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT(reader.depth() == 0);

    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::StartElement* se = Pt::Xml::toStartElement(&*it);
    PT_UNIT_ASSERT(se);
    PT_UNIT_ASSERT(se->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 1);

    ++it;
    Pt::Xml::EndElement* ee = Pt::Xml::toEndElement(&*it);
    PT_UNIT_ASSERT(ee);
    PT_UNIT_ASSERT(ee->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 0);

    ++it;
    Pt::Xml::EndDocument* endDoc = Pt::Xml::toEndDocument(&*it);
    PT_UNIT_ASSERT(endDoc);
    PT_UNIT_ASSERT(reader.depth() == 0);
}


void XmlReaderTest::ByteorderMarkUtf32LE()
{
    std::stringstream input;
    input << char(0xff) << char(0xfe) << char(0) << char(0)
          << char(0x3c) << char(0) << char(0) << char(0)
          << char(0x61) << char(0) << char(0) << char(0)
          << char(0x2f) << char(0) << char(0) << char(0)
          << char(0x3e) << char(0) << char(0) << char(0);

    XmlTestResolver resolver;
    Pt::Xml::BinaryInputSource is(resolver, input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT(reader.depth() == 0);

    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::StartElement* se = Pt::Xml::toStartElement(&*it);

    PT_UNIT_ASSERT(se);
    PT_UNIT_ASSERT(se->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 1);

    ++it;
    Pt::Xml::EndElement* ee = Pt::Xml::toEndElement(&*it);
    PT_UNIT_ASSERT(ee);
    PT_UNIT_ASSERT(ee->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 0);

    ++it;
    Pt::Xml::EndDocument* endDoc = Pt::Xml::toEndDocument(&*it);
    PT_UNIT_ASSERT(endDoc);
    PT_UNIT_ASSERT(reader.depth() == 0);
}


void XmlReaderTest::ByteorderMarkUtf32BE()
{
    std::stringstream input;
    input << char(0) << char(0) << char(0xfe) << char(0xff) 
          << char(0) << char(0) << char(0) << char(0x3c)  
          << char(0) << char(0) << char(0) << char(0x61) 
          << char(0) << char(0) << char(0) << char(0x2f) 
          << char(0) << char(0) << char(0) << char(0x3e);


    XmlTestResolver resolver;
    Pt::Xml::BinaryInputSource is(resolver, input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT(reader.depth() == 0);

    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::StartElement* se = Pt::Xml::toStartElement(&*it);

    PT_UNIT_ASSERT(se);
    PT_UNIT_ASSERT(se->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 1);

    ++it;
    Pt::Xml::EndElement* ee = Pt::Xml::toEndElement(&*it);
    PT_UNIT_ASSERT(ee);
    PT_UNIT_ASSERT(ee->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 0);

    ++it;
    Pt::Xml::EndDocument* endDoc = Pt::Xml::toEndDocument(&*it);
    PT_UNIT_ASSERT(endDoc);
    PT_UNIT_ASSERT(reader.depth() == 0);
}


void XmlReaderTest::ByteorderMarkGeneric32LE()
{
    std::stringstream input;
    input << char(0x3c) << char(0) << char(0) << char(0)
          << char(0x61) << char(0) << char(0) << char(0)
          << char(0x2f) << char(0) << char(0) << char(0)
          << char(0x3e) << char(0) << char(0) << char(0);

    XmlTestResolver resolver;
    Pt::Xml::BinaryInputSource is(resolver, input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT(reader.depth() == 0);

    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::StartElement* se = Pt::Xml::toStartElement(&*it);

    PT_UNIT_ASSERT(se);
    PT_UNIT_ASSERT(se->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 1);

    ++it;
    Pt::Xml::EndElement* ee = Pt::Xml::toEndElement(&*it);
    PT_UNIT_ASSERT(ee);
    PT_UNIT_ASSERT(ee->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 0);

    ++it;
    Pt::Xml::EndDocument* endDoc = Pt::Xml::toEndDocument(&*it);
    PT_UNIT_ASSERT(endDoc);
    PT_UNIT_ASSERT(reader.depth() == 0);
}


void XmlReaderTest::ByteorderMarkGeneric32BE()
{
    std::stringstream input;
    //input << char(0) << char(0) << char(0) << '<' 
    //      << char(0) << char(0) << char(0) << '?'
    //      << char(0) << char(0) << char(0) << 'x' 
    //      << char(0) << char(0) << char(0) << 'm' 
    //      << char(0) << char(0) << char(0) << 'l' 
    //      << char(0) << char(0) << char(0) << ' ' 
    //      << char(0) << char(0) << char(0) << '?' 
    //      << char(0) << char(0) << char(0) << '>' 
    //      << char(0) << char(0) << char(0) << char(0x3c)  
    //      << char(0x61) 
    //      << char(0x2f) 
    //      << char(0x3e);

    input << char(0) << char(0) << char(0) << char(0x3c)  
          << char(0) << char(0) << char(0) << char(0x61) 
          << char(0) << char(0) << char(0) << char(0x2f) 
          << char(0) << char(0) << char(0) << char(0x3e);

    XmlTestResolver resolver;
    Pt::Xml::BinaryInputSource is(resolver, input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT(reader.depth() == 0);

    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::StartElement* se = Pt::Xml::toStartElement(&*it);

    PT_UNIT_ASSERT(se);
    PT_UNIT_ASSERT(se->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 1);

    ++it;
    Pt::Xml::EndElement* ee = Pt::Xml::toEndElement(&*it);
    PT_UNIT_ASSERT(ee);
    PT_UNIT_ASSERT(ee->name().name() == "a");
    PT_UNIT_ASSERT(reader.depth() == 0);

    ++it;
    Pt::Xml::EndDocument* endDoc = Pt::Xml::toEndDocument(&*it);
    PT_UNIT_ASSERT(endDoc);
    PT_UNIT_ASSERT(reader.depth() == 0);
}


void XmlReaderTest::DtdEmptyDocument()
{
    XmlTestResolver resolver;
    resolver.addInput("external.dtd", "<!ENTITY e1 \"e1External\">");

    std::stringstream input;
    input << "<!DOCTYPE test SYSTEM \"external.dtd\" [\n";
    input << "<!ELEMENT test EMPTY>\n";
    input << "]>";

    Pt::Xml::BinaryInputSource is(input);

    Pt::Xml::XmlReader reader(resolver, is);
    reader.reportDocType(true);

    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::DocType& docType = Pt::Xml::toDocType(*it);
    PT_UNIT_ASSERT_EQUALS(docType.publicId(), "");
    PT_UNIT_ASSERT_EQUALS(docType.systemId(), "external.dtd");
    PT_UNIT_ASSERT_EQUALS(docType.isInternal(), true);
    
    ++it;
    
    Pt::Xml::EndDocType& endDocInternal = Pt::Xml::toEndDocType(*it);
    PT_UNIT_ASSERT_EQUALS(endDocInternal.isInternal(), true);
    PT_UNIT_ASSERT_EQUALS(endDocInternal.isExternal(), false);
    PT_UNIT_ASSERT_EQUALS(reader.dtd().rootName().name(), "test");
    PT_UNIT_ASSERT_EQUALS(reader.depth(), 0);

    ++it;
    
    Pt::Xml::EndDocType& endDocExternal = Pt::Xml::toEndDocType(*it);
    PT_UNIT_ASSERT_EQUALS(endDocExternal.isInternal(), false);
    PT_UNIT_ASSERT_EQUALS(endDocExternal.isExternal(), true);
    PT_UNIT_ASSERT_EQUALS(reader.dtd().rootName().name(), "test");
    PT_UNIT_ASSERT_EQUALS(reader.depth(), 0);

    ++it;
    Pt::Xml::EndDocument* endDoc = Pt::Xml::toEndDocument(&*it);
    PT_UNIT_ASSERT(endDoc);

    // used size is names and values from DTD
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 32);
}


void XmlReaderTest::DtdExternalSubsetPublicId()
{
    XmlTestResolver resolver;
    resolver.addInput("external.dtd", "<!ELEMENT test EMPTY>");

    std::stringstream input;
    input << "<!DOCTYPE test PUBLIC \"pubid\" \"external.dtd\">";
    input << "<test></test>";

    Pt::Xml::BinaryInputSource is(input);

    Pt::Xml::XmlReader reader(resolver, is);
    reader.reportDocType(true);

    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::DocType& docType = Pt::Xml::toDocType(*it);
    PT_UNIT_ASSERT_EQUALS(docType.publicId(), "pubid");
    PT_UNIT_ASSERT_EQUALS(docType.systemId(), "external.dtd");
    PT_UNIT_ASSERT_EQUALS(docType.isInternal(), false);

    PT_UNIT_ASSERT_EQUALS(reader.depth(), 0);

    Pt::Xml::DocTypeValidator validator( reader.dtd() );
    for(it = reader.current(); it != reader.end(); ++it)
    {
        PT_UNIT_ASSERT( validator.validate(*it) );
    }

    // used size is names and values from DTD
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 25);
}


void XmlReaderTest::DtdExternalSubsetSystemId()
{
    XmlTestResolver resolver;
    resolver.addInput("external.dtd", "<!ELEMENT test EMPTY>");

    std::stringstream input;
    input << "<!DOCTYPE test SYSTEM \"external.dtd\">\n";
    input << "<test></test>";

    Pt::Xml::BinaryInputSource is(input);

    Pt::Xml::XmlReader reader(resolver, is);
    reader.reportDocType(true);

    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::DocType& docType = Pt::Xml::toDocType(*it);
    PT_UNIT_ASSERT_EQUALS(docType.publicId(), "");
    PT_UNIT_ASSERT_EQUALS(docType.systemId(), "external.dtd");
    PT_UNIT_ASSERT_EQUALS(docType.isInternal(), false);

    PT_UNIT_ASSERT_EQUALS(reader.depth(), 0);

    Pt::Xml::DocTypeValidator validator( reader.dtd() );
    for(it = reader.current(); it != reader.end(); ++it)
    {
        PT_UNIT_ASSERT( validator.validate(*it) );
    }

    // used size is names and values from DTD
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 20);
}


void XmlReaderTest::DtdExternalAndInternalSubset()
{
    try
    {
        XmlTestResolver resolver;
        resolver.addInput("external.dtd", "<!ENTITY e1 \"e1External\">\n<!ENTITY e2 \"e2External\">\n");

        Pt::Xml::StringInputSource is(
            "<!DOCTYPE test SYSTEM \"external.dtd\" [\n"
            "<!ELEMENT test (#PCDATA)>\n"
            "<!ENTITY e1 \"e1Internal\">\n"
            "]>\n"
            "<test>&e1; &e2;</test>"
        );

        Pt::Xml::XmlReader reader(resolver, is);
        reader.reportDocType(true);

        Pt::Xml::InputIterator it = reader.current();
        
        Pt::Xml::DocType& docType = Pt::Xml::toDocType(*it);
        PT_UNIT_ASSERT_EQUALS(docType.publicId(), "");
        PT_UNIT_ASSERT_EQUALS(docType.systemId(), "external.dtd");
        PT_UNIT_ASSERT_EQUALS(docType.isInternal(), true);

        ++it;
        
        Pt::Xml::EndDocType* endDoc = Pt::Xml::toEndDocType(&(*it));
        PT_UNIT_ASSERT(endDoc);
        PT_UNIT_ASSERT_EQUALS(reader.dtd().rootName().name(), "test");
        PT_UNIT_ASSERT( reader.dtd().findEntity("e1") );
        PT_UNIT_ASSERT( reader.dtd().findEntity("e2") == 0 );
        PT_UNIT_ASSERT_EQUALS(reader.depth(), 0);

        Pt::String content;
        for(it = reader.current(); it != reader.end(); ++it)
        {
            Pt::Xml::Characters* chars = toCharacters(&*it);
            if(chars)
                content = chars->content();
        }

        PT_UNIT_ASSERT_EQUALS(content, "e1Internal e2External");
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::DtdValidateAttributes()
{
    try
    {
        std::stringstream input;
        input << "<!DOCTYPE test [\n";
        input << "<!ATTLIST test a1 CDATA #REQUIRED\n";
        input << "          a2 CDATA #IMPLIED\n";
        input << "          a3 NMTOKEN #FIXED \"A3def\"\n";
        input << "          a4 NMTOKENS \"A4def\"\n>";
        input << "<!ELEMENT test EMPTY>\n";
        input << "]>\n";
        input << "<test a1='A1' a2='A2' a4='A3def'></test>";

        Pt::Xml::BinaryInputSource is(input);

        Pt::Xml::XmlReader reader(is);
        Pt::Xml::DocTypeValidator validator( reader.dtd() );
        
        Pt::Xml::InputIterator it;
        for(it = reader.current(); it != reader.end(); ++it)
        {
            PT_UNIT_ASSERT( validator.validate(*it) );

            Pt::Xml::StartElement* se = toStartElement(&*it);
            if(se && se->name().name() == "test")
            {
                PT_UNIT_ASSERT( se->attributes().has("a3") );
                PT_UNIT_ASSERT( se->attributes().find("a3")->value() == "A3def" );

                PT_UNIT_ASSERT( se->attributes().has("a4") );
            }
        }

        // used size is names and values from DTD
        PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 26);
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::DtdValidateEnumAttributes()
{
    try
    {
        std::stringstream input;
        input << "<!DOCTYPE test [\n";
        input << "<!ELEMENT test EMPTY>\n";
        input << "<!ATTLIST test a1 (aaa|bbb) #REQUIRED>\n";
        input << "]>\n";
        input << "<test a1='bbb'></test>";

        Pt::Xml::BinaryInputSource is(input);

        Pt::Xml::XmlReader reader(is);
        Pt::Xml::DocTypeValidator validator( reader.dtd() );
        
        Pt::Xml::InputIterator it;
        for(it = reader.current(); it != reader.end(); ++it)
        {
            PT_UNIT_ASSERT( validator.validate(*it) );
        }
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::DtdValidateIDAttributes()
{
    try
    {
        std::stringstream input;
        input << "<!DOCTYPE test [\n";
        input << "<!ELEMENT test (first, second)>\n";
        input << "<!ELEMENT first EMPTY>\n";
        input << "<!ELEMENT second EMPTY>\n";
        input << "<!ATTLIST test id ID #REQUIRED>\n";
        input << "<!ATTLIST first refs IDREFS #REQUIRED\n>";
        input << "<!ATTLIST second id ID #REQUIRED\n";
        input << "                 ref IDREF #REQUIRED\n>";
        input << "]>\n";
        input << "<test id='A1'>\n";
        input << "    <first refs='A1 A2'/>\n";
        input << "    <second id='A2' ref='A1' />\n";
        input << "</test>";

        Pt::Xml::BinaryInputSource is(input);

        Pt::Xml::XmlReader reader(is);
        Pt::Xml::DocTypeValidator validator( reader.dtd() );
        
        Pt::Xml::InputIterator it;
        for(it = reader.current(); it != reader.end(); ++it)
        {
            PT_UNIT_ASSERT( validator.validate(*it) );
        }

        // used size is names and values from DTD
        PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 41);
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::DtdValidateEntityAttributes()
{
    try
    {
        std::stringstream input;
        input << "<!DOCTYPE test [\n";
        input << "<!ELEMENT test EMPTY>\n";
        input << "<!ATTLIST test a1 ENTITY #REQUIRED\n";
        input << "               a2 ENTITIES #REQUIRED>\n";
        input << "<!ENTITY e1 SYSTEM 'e1.txt' NDATA txt>\n";
        input << "<!ENTITY e2 SYSTEM 'e1.txt' NDATA txt>\n";
        input << "]>\n";
        input << "<test a1='e1' a2='e1 e2'></test>";

        Pt::Xml::BinaryInputSource is(input);

        Pt::Xml::XmlReader reader(is);
        Pt::Xml::DocTypeValidator validator( reader.dtd() );
        
        Pt::Xml::InputIterator it = reader.current();
        for(; it != reader.end(); ++it)
        {
            PT_UNIT_ASSERT( validator.validate(*it) );
        }
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::DtdValidateNotationAttributes()
{
    try
    {
        std::stringstream input;
        input << "<!DOCTYPE test [\n";
        input << "<!ELEMENT test EMPTY>\n";
        input << "<!ATTLIST test a1 NOTATION (jpeg) #REQUIRED\n";
        input << "               a2 NOTATION (jpeg|gif) #REQUIRED>\n";
        input << "<!NOTATION jpeg SYSTEM 'image/jpeg'>\n";
        input << "<!NOTATION gif SYSTEM 'image/gif'>\n";
        input << "]>\n";
        input << "<test a1='jpeg' a2='gif'></test>";

        Pt::Xml::BinaryInputSource is(input);

        Pt::Xml::XmlReader reader(is);
        Pt::Xml::DocTypeValidator validator( reader.dtd() );
        
        Pt::Xml::InputIterator it = reader.current();
        for(; it != reader.end(); ++it)
        {
            PT_UNIT_ASSERT( validator.validate(*it) );
        }

        // used size is names and values from DTD
        PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 49);
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
        input << "<!ELEMENT test (b?|a)*> \n";
        input << "<!ELEMENT a (#PCDATA|x|y|z)* >\n";
        input << "<!ELEMENT b EMPTY>\n";
        input << "<!ELEMENT x EMPTY>\n";
        input << "<!ELEMENT y EMPTY>\n";
        input << "<!ELEMENT z EMPTY>\n";
        input << "]>\n";
        input << "<test>\n"
                 "  <a>hello<x></x>world</a>\n"
                 "  <b></b>\n"
                 "  <a></a>\n"
                 "  <a>hello</a>\n"
                 "  <b></b>\n"
                 "</test>\n";

        Pt::Xml::BinaryInputSource is(input);

        Pt::Xml::XmlReader reader(is);
        Pt::Xml::DocTypeValidator validator( reader.dtd() );

        Pt::Xml::InputIterator it;
        for(it = reader.current(); it != reader.end(); ++it)
        {
            PT_UNIT_ASSERT( validator.validate(*it) );
        }

        // used size is names and values from DTD
        PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 25);
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::DtdAnyElementContent()
{
    try
    {
        std::stringstream input;
        input << "<!DOCTYPE test [\n";
        input << "<!ELEMENT test ANY> \n";
        input << "<!ELEMENT a ANY >\n";
        input << "<!ELEMENT b ANY>\n";
        input << "]>\n";
        input << "<test>\n"
                 "  <a>hello <b/> world</a>\n"
                 "  <b></b>\n"
                 "</test>\n";

        Pt::Xml::BinaryInputSource is(input);

        Pt::Xml::XmlReader reader(is);
        Pt::Xml::DocTypeValidator validator( reader.dtd() );
        
        Pt::Xml::InputIterator it;
        for(it = reader.current(); it != reader.end(); ++it)
        {
            bool valid = validator.validate(*it);
            PT_UNIT_ASSERT(valid);
        }

        // used size is names and values from DTD
        PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 10);
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::DtdValidateWithNamespace()
{
    try
    {
        std::stringstream input;
        input << "<!DOCTYPE pt:test [\n";
        input << "<!ELEMENT pt:test (pt:first)>\n";
        input << "<!ELEMENT pt:first EMPTY>\n";
        input << "<!ATTLIST pt:test pt:a1 CDATA #REQUIRED>\n";
        input << "]>\n";
        input << "<pt:test xmlns:pt='http://www.pt-framework.org' pt:a1='A1'>\n";
        input << "<pt:first/>\n";
        input << "</pt:test>\n";

        Pt::Xml::BinaryInputSource is(input);

        Pt::Xml::XmlReader reader(is);
        Pt::Xml::DocTypeValidator validator( reader.dtd() );
        
        Pt::Xml::InputIterator it;
        for(it = reader.current(); it != reader.end(); ++it)
        {
            PT_UNIT_ASSERT( validator.validate(*it) );

            Pt::Xml::StartElement* se = toStartElement(&*it);
            if(se && se->name().name() == "test" )
            {
                PT_UNIT_ASSERT( se->attributes().has("a1") );
            }
        }

        // used size is names and values from DTD
        PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 31);
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::DtdNotations()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    input << "<!DOCTYPE test [\n";
    input << "<!ELEMENT test EMPTY>\n";
    input << "<!NOTATION notation1 SYSTEM \"n1\">\n";
    input << "<!NOTATION notation2 PUBLIC \"http://www.pt-framework.org/n2\">\n";
    input << "<!NOTATION notation3 PUBLIC \"http://www.pt-framework.org/n3\" \"n3\">\n";
    input << "]>\n";

    input << "<test></test>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    reader.reportDocType(true);
    
    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::DocType* docType = Pt::Xml::toDocType(&*it);
    PT_UNIT_ASSERT(docType);

    ++it;

    Pt::Xml::EndDocType* endDocType = Pt::Xml::toEndDocType(&*it);
    PT_UNIT_ASSERT(endDocType);

    const Pt::Xml::Notation* notation = reader.dtd().findNotation("notation1");
    PT_UNIT_ASSERT(notation);
    PT_UNIT_ASSERT_EQUALS(notation->publicId(), "");
    PT_UNIT_ASSERT_EQUALS(notation->systemId(), "n1");

    notation = reader.dtd().findNotation("notation2");
    PT_UNIT_ASSERT(notation);
    PT_UNIT_ASSERT_EQUALS(notation->publicId(), "http://www.pt-framework.org/n2");
    PT_UNIT_ASSERT_EQUALS(notation->systemId(), "");

    notation = reader.dtd().findNotation("notation3");
    PT_UNIT_ASSERT(notation);
    PT_UNIT_ASSERT_EQUALS(notation->publicId(), "http://www.pt-framework.org/n3");
    PT_UNIT_ASSERT_EQUALS(notation->systemId(), "n3");

    Pt::Xml::DocTypeValidator validator( reader.dtd() );
    for(; it != reader.end(); ++it)
    {
        PT_UNIT_ASSERT( validator.validate(*it) );
    }

    // used size is names and values from DTD
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 99);
}


void XmlReaderTest::DtdInclude()
{
    try
    {
        Pt::String externalDtd;
        externalDtd += "<![INCLUDE[]]>\n";
        externalDtd += "<![ INCLUDE [ ]]>\n";
        externalDtd += "<![%include;[\n";
        externalDtd += "<![INCLUDE[ <!ENTITY e2 \"entity2\"> ]]>";
        externalDtd += "  <!ENTITY e1 \"entity1\">\n";
        externalDtd += "]]>\n";

        XmlTestResolver resolver;
        resolver.addInput("external.dtd", externalDtd);

        std::stringstream input;
        input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        input << "<!DOCTYPE test SYSTEM \"external.dtd\" [\n";
        input << "<!ENTITY % include \"INCLUDE\">\n";
        input << "<!ELEMENT test EMPTY>\n";
        input << "]>\n";
        input << "<test></test>";

        Pt::Xml::BinaryInputSource is(input);

        Pt::Xml::XmlReader reader( resolver, is );

        Pt::Xml::InputIterator it = reader.current();
        for(; it != reader.end(); ++it)
            ;

        const Pt::Xml::Entity* entity = reader.dtd().findEntity("e1");
        PT_UNIT_ASSERT(entity);
        PT_UNIT_ASSERT_EQUALS(entity->value(), "entity1");

        entity = reader.dtd().findEntity("e2");
        PT_UNIT_ASSERT(entity);
        PT_UNIT_ASSERT_EQUALS(entity->value(), "entity2");
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::DtdIgnore()
{
    try
    {
        Pt::String externalDtd;
        externalDtd += "<![IGNORE[]]>\n";
        externalDtd += "<![ IGNORE [ ]]>\n";
        externalDtd += "<![%ignore;[\n";
        externalDtd += "  <!ENTITY e1 \"entity1\">\n";
        externalDtd += "]]>\n";
        externalDtd += "<![IGNORE[ <!ENTITY e2 \"entity2\"> ]]>";

        std::stringstream input;
        input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        input << "<!DOCTYPE test SYSTEM \"external.dtd\" [\n";
        input << "<!ENTITY % ignore \"IGNORE\">\n";
        input << "<!ELEMENT test EMPTY>\n";
        input << "]>\n";
        input << "<test></test>";

        XmlTestResolver resolver;
        resolver.addInput("external.dtd", externalDtd);

        Pt::Xml::BinaryInputSource is(input);
        Pt::Xml::XmlReader reader( resolver, is );

        Pt::Xml::InputIterator it = reader.current();
        for(; it != reader.end(); ++it)
            ;

        const Pt::Xml::Entity* entity = reader.dtd().findEntity("e1");
        PT_UNIT_ASSERT( ! entity);

        entity = reader.dtd().findEntity("e2");
        PT_UNIT_ASSERT( ! entity);
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::DtdComment()
{
    try
    {
        XmlTestResolver resolver;
        resolver.addInput("external.dtd", "<!-- some comment external -->\n");

        std::stringstream input;
        input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        input << "<!DOCTYPE test SYSTEM \"external.dtd\" [\n";
        input << "<!-- some comment internal -->\n";
        input << "<!ELEMENT test EMPTY>\n";
        input << "]>\n";
        input << "<test></test>";

        Pt::Xml::BinaryInputSource is(input);
        Pt::Xml::XmlReader reader( resolver, is );
        Pt::Xml::DocTypeValidator validator( reader.dtd() );

        Pt::Xml::InputIterator it = reader.current();
        for(; it != reader.end(); ++it)
        {
            PT_UNIT_ASSERT( validator.validate(*it) );
        }
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::DtdProcesssingInstruction()
{
    try
    {
        XmlTestResolver resolver;
        resolver.addInput("external.dtd", "<?pi2 external processing instruction?>\n");

        std::stringstream input;
        input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        input << "<!DOCTYPE test SYSTEM \"external.dtd\" [\n";
        input << "<?pi1 internal processing instruction?>\n";
        input << "<!ELEMENT test EMPTY>\n";
        input << "]>\n";
        input << "<test></test>";

        Pt::Xml::BinaryInputSource is(input);
        Pt::Xml::XmlReader reader( resolver, is );
        reader.reportProcessingInstructions(true);

        Pt::Xml::InputIterator it = reader.current();
        const Pt::Xml::ProcessingInstruction* pi = Pt::Xml::toProcessingInstruction(&*it);
        PT_UNIT_ASSERT(pi);
        PT_UNIT_ASSERT_EQUALS(pi->target(), "pi1");
        PT_UNIT_ASSERT_EQUALS(pi->data(), "internal processing instruction");

        ++it;
        pi = Pt::Xml::toProcessingInstruction(&*it);
        PT_UNIT_ASSERT(pi);
        PT_UNIT_ASSERT_EQUALS(pi->target(), "pi2");
        PT_UNIT_ASSERT_EQUALS(pi->data(), "external processing instruction");
        
        Pt::Xml::DocTypeValidator validator( reader.dtd() );

        for(; it != reader.end(); ++it)
        {
            PT_UNIT_ASSERT( validator.validate(*it) );
        }
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

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    Pt::Xml::InputIterator it = reader.current();
    const Pt::Xml::Node& n = *it;

    PT_UNIT_ASSERT(n.type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT( reader.input() );
    
    const Pt::Xml::XmlDeclaration* xmlDecl = reader.input()->declaration();
    PT_UNIT_ASSERT(xmlDecl);
    PT_UNIT_ASSERT_EQUALS(xmlDecl->version(), "1.0");
    PT_UNIT_ASSERT_EQUALS(xmlDecl->encoding(), "UTF-8");
    PT_UNIT_ASSERT_EQUALS(xmlDecl->isStandalone(), false);
}


void XmlReaderTest::EmptyElementTag()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a/>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT( reader.depth() == 0);

    Pt::Xml::InputIterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    PT_UNIT_ASSERT(startNode.type() == Pt::Xml::Node::StartElement);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::StartElement*>(&startNode)->name().name() == "a");
    PT_UNIT_ASSERT( reader.depth() == 1);

    ++it;
    const Pt::Xml::Node& endNode = *it;
    PT_UNIT_ASSERT(endNode.type() == Pt::Xml::Node::EndElement);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::EndElement*>(&endNode)->name().name() == "a");
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

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    // Expecting exception because empty tags are not allowed.
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::InvalidTag2()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "</>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    // Expecting exception because empty tags are not allowed.
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}



void XmlReaderTest::InvalidTag3()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>>b</a>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    Pt::Xml::InputIterator it = reader.current();

    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);
    PT_UNIT_ASSERT_THROW(++it; ++it; ++it;, std::exception);
}


void XmlReaderTest::InvalidTag4()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<<a>b</a>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::InvalidTag5()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<ab<cd>>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::UnmatchedElement()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    input << "<test>\n";
    input << "  <aaa>\n</aa>\n";
    input << "</test>\n";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    reader.next();
    reader.next();
    reader.next();
    reader.next();
    PT_UNIT_ASSERT_THROW(reader.next(), Pt::Xml::SyntaxError);
}


void XmlReaderTest::ElementWithContent()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>?!:=b</a>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    Pt::Xml::InputIterator it = reader.current();

    // <a>
    PT_UNIT_ASSERT(Pt::Xml::toStartElement(*it).name().name() == "a");
    PT_UNIT_ASSERT( reader.depth() == 1);

    // b
    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(*it).isSpace() == false);
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(*it).content() == "?!:=b");
    PT_UNIT_ASSERT( reader.depth() == 1);

    // </a>
    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toEndElement(*it).name().name() == "a");
    PT_UNIT_ASSERT( reader.depth() == 0);

    // End of document
    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT( reader.depth() == 0);

    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::MaxCharacters()
{
    std::stringstream input;
    input << "<a>0123456789</a>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    reader.setChunkSize(5);
    
    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    Pt::Xml::Characters* chars = Pt::Xml::toCharacters(&*it);
    PT_UNIT_ASSERT(chars);
    PT_UNIT_ASSERT_EQUALS(chars->content(), "01234");

    ++it;
    chars = Pt::Xml::toCharacters(&*it);
    PT_UNIT_ASSERT(chars);
    PT_UNIT_ASSERT_EQUALS(chars->content(), "56789");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::ElementWithNamespace()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<my:a xmlns:my=\"http://www.my1.net\">";
    input <<   "<my:a xmlns:my=\"http://www.my2.net\">b</my:a>";
    input << "</my:a>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    Pt::Xml::InputIterator it = reader.current();

    // <my:a>
    const Pt::Xml::StartElement& startA = Pt::Xml::toStartElement(*it);
    PT_UNIT_ASSERT(startA.name().prefix() == "my");
    PT_UNIT_ASSERT(startA.name().name() == "a");
    PT_UNIT_ASSERT(startA.namespaceUri() == "http://www.my1.net");
    PT_UNIT_ASSERT(startA.namespaceMapping().size() == 1);
    PT_UNIT_ASSERT(startA.namespaceMapping().begin()->isMapped() == true);
    PT_UNIT_ASSERT(startA.namespaceMapping().begin()->prefix() == "my");
    PT_UNIT_ASSERT(startA.namespaceMapping().begin()->namespaceUri() == "http://www.my1.net");
    PT_UNIT_ASSERT(reader.depth() == 1);

    // <my:a>
    ++it;
    const Pt::Xml::StartElement& startA2 = Pt::Xml::toStartElement(*it);
    PT_UNIT_ASSERT(startA2.name().prefix() == "my");
    PT_UNIT_ASSERT(startA2.name().name() == "a");
    PT_UNIT_ASSERT(startA2.namespaceUri() == "http://www.my2.net");
    PT_UNIT_ASSERT(startA2.namespaceMapping().size() == 1);
    PT_UNIT_ASSERT(startA2.namespaceMapping().begin()->isMapped() == true);
    PT_UNIT_ASSERT(startA2.namespaceMapping().begin()->prefix() == "my");
    PT_UNIT_ASSERT(startA2.namespaceMapping().begin()->namespaceUri() == "http://www.my2.net");
    PT_UNIT_ASSERT(reader.depth() == 2);

    // b
    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(*it).isSpace() == false);
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(*it).content() == "b");
    PT_UNIT_ASSERT(reader.depth() == 2);

    // </my:a>
    ++it;
    const Pt::Xml::EndElement& endA2 = Pt::Xml::toEndElement(*it);
    PT_UNIT_ASSERT(endA2.name().name() == "a");
    PT_UNIT_ASSERT(endA2.name().prefix() == "my");
    PT_UNIT_ASSERT(endA2.namespaceUri() == "http://www.my2.net");
    PT_UNIT_ASSERT(endA2.namespaceMapping().size() == 1);
    PT_UNIT_ASSERT(endA2.namespaceMapping().begin()->isMapped() == true);
    PT_UNIT_ASSERT(endA2.namespaceMapping().begin()->prefix() == "my");
    PT_UNIT_ASSERT(endA2.namespaceMapping().begin()->namespaceUri() == "http://www.my1.net");
    PT_UNIT_ASSERT(reader.depth() == 1);

    // </my:a>
    ++it;
    const Pt::Xml::EndElement& endA = Pt::Xml::toEndElement(*it);

    PT_UNIT_ASSERT(endA.name().name() == "a");
    PT_UNIT_ASSERT(endA.name().prefix() == "my");
    PT_UNIT_ASSERT(endA.namespaceUri() == "http://www.my1.net");
    PT_UNIT_ASSERT(endA.namespaceMapping().size() == 1);
    PT_UNIT_ASSERT(endA.namespaceMapping().begin()->isMapped() == false);
    PT_UNIT_ASSERT(endA.namespaceMapping().begin()->prefix() == "my");
    PT_UNIT_ASSERT(endA.namespaceMapping().begin()->namespaceUri() == "http://www.my1.net");
    PT_UNIT_ASSERT( reader.depth() == 0);

    // End of document
    ++it;
    const Pt::Xml::Node& endDocument = *it;
    PT_UNIT_ASSERT(endDocument.type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT( reader.depth() == 0);

    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}

void XmlReaderTest::AttributeWithNamespace()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a my:attr=\"Hallo\" xmlns:my=\"http://www.my.net\">b</a>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    Pt::Xml::InputIterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    // <a>
    PT_UNIT_ASSERT(startNode.type() == Pt::Xml::Node::StartElement);
    PT_UNIT_ASSERT(Pt::Xml::toStartElement(&startNode)->name().name() == "a");
    PT_UNIT_ASSERT( Pt::Xml::toStartElement(&startNode)->attributes().has("attr") );
    PT_UNIT_ASSERT( Pt::Xml::toStartElement(&startNode)->attributes().begin()->name().prefix() == "my" );
    PT_UNIT_ASSERT( Pt::Xml::toStartElement(&startNode)->attributes().begin()->namespaceUri() == "http://www.my.net");
    PT_UNIT_ASSERT( reader.depth() == 1);

    // b
    ++it;
    const Pt::Xml::Characters& text = Pt::Xml::toCharacters(*it);
    PT_UNIT_ASSERT(text.content() == "b");
    PT_UNIT_ASSERT(reader.depth() == 1);

    // </a>
    ++it;
    const Pt::Xml::Node& endNodeA = *it;

    PT_UNIT_ASSERT(endNodeA.type() == Pt::Xml::Node::EndElement);
    // TODO: PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::EndElement*>(&endNodeA)->name() == "a");
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::EndElement*>(&endNodeA)->namespaceUri() == "");
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

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    Pt::Xml::InputIterator it = reader.current();

    // <my:a>
    const Pt::Xml::StartElement& startA = Pt::Xml::toStartElement(*it);
    PT_UNIT_ASSERT(startA.name().prefix() == "");
    PT_UNIT_ASSERT(startA.name().name() == "a");
    PT_UNIT_ASSERT(startA.namespaceUri() == "http://www.my1.net");
    PT_UNIT_ASSERT(reader.depth() == 1);

    // <my:a>
    ++it;
    const Pt::Xml::StartElement& startA2 = Pt::Xml::toStartElement(*it);
    PT_UNIT_ASSERT(startA2.name().prefix() == "");
    PT_UNIT_ASSERT(startA2.name().name() == "a");
    PT_UNIT_ASSERT(startA2.namespaceUri() == "http://www.my2.net");
    PT_UNIT_ASSERT(reader.depth() == 2);

    // b
    ++it;
    const Pt::Xml::Node& charactersNode = *it;
    PT_UNIT_ASSERT(charactersNode.type() == Pt::Xml::Node::Characters);
    PT_UNIT_ASSERT(dynamic_cast<const Pt::Xml::Characters*>(&charactersNode)->content() == "b");
    PT_UNIT_ASSERT( reader.depth() == 2);

    // </my:a>
    ++it;
    const Pt::Xml::EndElement& endA2 = Pt::Xml::toEndElement(*it);
    PT_UNIT_ASSERT(endA2.name().prefix() == "");
    PT_UNIT_ASSERT(endA2.name().name() == "a");
    PT_UNIT_ASSERT(endA2.namespaceUri() == "http://www.my2.net");
    PT_UNIT_ASSERT(reader.depth() == 1);

    // </my:a>
    ++it;
    const Pt::Xml::EndElement& endA = Pt::Xml::toEndElement(*it);

    PT_UNIT_ASSERT(endA.name().prefix() == "");
    PT_UNIT_ASSERT(endA.name().name() == "a");
    PT_UNIT_ASSERT(endA.namespaceUri() == "http://www.my1.net");
    PT_UNIT_ASSERT( reader.depth() == 0);

    // End of document
    ++it;
    const Pt::Xml::Node& endDocument = *it;
    PT_UNIT_ASSERT(endDocument.type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT( reader.depth() == 0);

    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::MissingCloseTag()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a><b></a>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    Pt::Xml::InputIterator it = reader.current();

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

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::InvalidAttribute2()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a blub=/>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::InvalidAttribute3()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a blub=\"/>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::InvalidAttribute4()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a blub=bla=\"b\"/>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    PT_UNIT_ASSERT_THROW(reader.current(), std::exception);
}


void XmlReaderTest::EmptyAttribute()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a b=\"\"/>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    Pt::Xml::InputIterator it = reader.current();
    const Pt::Xml::Node& node = *it;

    PT_UNIT_ASSERT(node.type() == Pt::Xml::Node::StartElement);
    const Pt::Xml::StartElement* tag = dynamic_cast<const Pt::Xml::StartElement*>(&node);

    PT_UNIT_ASSERT(tag->attributes().has("b"));
    PT_UNIT_ASSERT(tag->attributes().find("b")->value().narrow() == "");
}


void XmlReaderTest::AttributeWithSimpleText()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a value=\"a bcdefghijklmnopqrstuvwxyz\"/>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    Pt::Xml::InputIterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);
    const Pt::Xml::StartElement* tag = dynamic_cast<const Pt::Xml::StartElement*>(&startNode);

    PT_UNIT_ASSERT(tag->attributes().has("value"));
    PT_UNIT_ASSERT(tag->attributes().find("value")->value().narrow() == "a bcdefghijklmnopqrstuvwxyz");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::AttributeWithUTF8()
{
    std::stringstream input;

    // The attribute value is a UTF-8 encoded 5-character word with greek characters.
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a b=\"" << (char)0xce << (char)0xba << (char)0xe1 << (char)0xbd << (char)0xb9 << (char)0xcf <<
                          (char)0x83 << (char)0xce << (char)0xbc << (char)0xce << (char)0xb5 << "\"/>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    Pt::Xml::InputIterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    PT_UNIT_ASSERT(startNode.type() == Pt::Xml::Node::StartElement);
    const Pt::Xml::StartElement* tag = dynamic_cast<const Pt::Xml::StartElement*>(&startNode);

    PT_UNIT_ASSERT(tag->attributes().has("b"));

    // This is the same 5-character word with greek characters as above, but this time not encoded but
    // directly in Unicode. If the UTF-8 decoding works this must be stored in the Attribute's String now.
    Pt::Char c[] = { 954, 8057, 963, 956, 949, 0 };
    PT_UNIT_ASSERT(tag->attributes().find("b")->value() == c);
}


void XmlReaderTest::MultipleAttributesIteration()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a b=\"123\" c=\"456\" d=\"789\"/>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    Pt::Xml::InputIterator it = reader.current();
    const Pt::Xml::Node& startNode = *it;

    const Pt::Xml::StartElement* tag = Pt::Xml::toStartElement(&startNode);
    PT_UNIT_ASSERT(tag);
    PT_UNIT_ASSERT(tag->attributes().has("b"));
    PT_UNIT_ASSERT(tag->attributes().has("c"));
    PT_UNIT_ASSERT(tag->attributes().has("d"));

    const Pt::Xml::AttributeList& attributes = tag->attributes();
    Pt::Xml::AttributeList::ConstIterator attributeIter = attributes.begin();

    PT_UNIT_ASSERT(attributeIter != attributes.end());

    Pt::Xml::Attribute attributeB = *attributeIter;
    PT_UNIT_ASSERT(attributeB.name().name() == "b");
    PT_UNIT_ASSERT(attributeB.value().narrow() == "123");

    attributeIter++;
    PT_UNIT_ASSERT(attributeIter != attributes.end());

    Pt::Xml::Attribute attributeC = *attributeIter;
    PT_UNIT_ASSERT(attributeC.name().name() == "c");
    PT_UNIT_ASSERT(attributeC.value().narrow() == "456");

    attributeIter++;
    PT_UNIT_ASSERT(attributeIter != attributes.end());

    Pt::Xml::Attribute attributeD = *attributeIter;
    PT_UNIT_ASSERT(attributeD.name().name() == "d");
    PT_UNIT_ASSERT(attributeD.value().narrow() == "789");

    attributeIter++;
    PT_UNIT_ASSERT(attributeIter == attributes.end());
}


void XmlReaderTest::NormalizeAttributes()
{
    try
    {
        std::stringstream input;
        input << "<!DOCTYPE test [\n";
        input << "<!ELEMENT test EMPTY>\n";
        input << "<!ATTLIST test a1 CDATA #REQUIRED\n";
        input << "               a2 NMTOKEN #REQUIRED\n";
        input << "               a3 NMTOKEN #REQUIRED\n";
        input << "               a4 NMTOKEN #REQUIRED\n";
        input << "               a5 NMTOKEN #REQUIRED\n>";
        input << "]>\n";
        input << "<test a1=' a ' a2='' a3='a' a4=' a ' a5=' a b \r\n\t c'></test>";

        Pt::Xml::BinaryInputSource is(input);
        Pt::Xml::XmlReader reader(is);
        
        Pt::Xml::InputIterator it;
        for(it = reader.current(); it != reader.end(); ++it)
        {
            if(it->type() == Pt::Xml::Node::StartElement)
                break;
        }

        Pt::Xml::StartElement* se = toStartElement(&*it);
        PT_UNIT_ASSERT(se);
        PT_UNIT_ASSERT( se->name().name() == "test" );
        
        PT_UNIT_ASSERT( se->attributes().has("a1") );
        PT_UNIT_ASSERT( se->attributes().find("a1")->value() == " a " );

        PT_UNIT_ASSERT( se->attributes().has("a2") );
        PT_UNIT_ASSERT( se->attributes().find("a2")->value() == "" );

        PT_UNIT_ASSERT( se->attributes().has("a3") );
        PT_UNIT_ASSERT( se->attributes().find("a3")->value() == "a" );

        PT_UNIT_ASSERT( se->attributes().has("a4") );
        PT_UNIT_ASSERT( se->attributes().find("a4")->value() == "a" );

        PT_UNIT_ASSERT( se->attributes().has("a5") );
        PT_UNIT_ASSERT( se->attributes().find("a5")->value() == "a b c" );
    }
    catch(const Pt::Xml::SyntaxError& error)
    {
        std::cerr << error.what() << ": " << error.line() << std::endl;
        throw;
    }
}


void XmlReaderTest::ProcessingInstructionInProlog()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<?xml-stylesheet type=\"text/css\" href=\"styles.css\"?>";
    input << "<a/>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    reader.reportProcessingInstructions(true);
    
    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::ProcessingInstruction);
    const Pt::Xml::ProcessingInstruction& pi = dynamic_cast<const Pt::Xml::ProcessingInstruction&>(*it);
    PT_UNIT_ASSERT(pi.data() == "type=\"text/css\" href=\"styles.css\"");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::ProcessingInstructionInElement()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>123";
    input << "<?xml-stylesheet type=\"text/css\" href=\"styles.css\"?>";
    input << "456</a>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    reader.reportProcessingInstructions(true);

    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::ProcessingInstruction);
    const Pt::Xml::ProcessingInstruction& pi = dynamic_cast<const Pt::Xml::ProcessingInstruction&>(*it);
    PT_UNIT_ASSERT(pi.data() == "type=\"text/css\" href=\"styles.css\"");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::Characters);
    const Pt::Xml::Characters& chars = dynamic_cast<const Pt::Xml::Characters&>(*it);
    PT_UNIT_ASSERT(chars.content() == "123456");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::ProcessingInstructionInEpilog()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    input << "<a/>\n";
    input << "<?xml-stylesheet type=\"text/css\" href=\"styles.css\"?>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    reader.reportProcessingInstructions(true);

    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::ProcessingInstruction);
    const Pt::Xml::ProcessingInstruction& pi = dynamic_cast<const Pt::Xml::ProcessingInstruction&>(*it);
    PT_UNIT_ASSERT(pi.data() == "type=\"text/css\" href=\"styles.css\"");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::IgnorableWhitespace()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>";
    input << "   <b></b>\n";
    input << "\t<c></c>\n";
    input << "</a>\n";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
        
    Pt::Xml::InputIterator it;
    for(it = reader.current(); it != reader.end(); ++it)
    {
        Pt::Xml::Characters* chars = toCharacters(&*it);
        if(chars)
        {
            PT_UNIT_ASSERT( chars->isSpace() );
        }
    }
}


void XmlReaderTest::CDATAAsCharacters()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<test>";
    input << "<a>";
    input << "hello";
    input << "<![CDATA[<Element>pure &gt; data</Element>]]>";
    input << "world!";
    input << "</a>";
    input << "<b>";
    input << "<![CDATA[bbb]]>";
    input << "</b>";
    input << "</test>\n";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    Pt::Xml::Characters* chars = Pt::Xml::toCharacters(&*it);
    PT_UNIT_ASSERT(chars);
    PT_UNIT_ASSERT_EQUALS(chars->content(), "hello<Element>pure &gt; data</Element>world!");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    chars = Pt::Xml::toCharacters(&*it);
    PT_UNIT_ASSERT(chars);
    PT_UNIT_ASSERT(chars->content().narrow() == "bbb");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlReaderTest::CDATA()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<test>";
    input << "<a>";
    input << "hello";
    input << "<![CDATA[<Element>pure &gt; data</Element>]]>";
    input << "world!";
    input << "</a>";
    input << "<b>";
    input << "<![CDATA[bbb]]>";
    input << "</b>";
    input << "</test>\n";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    reader.reportCData(true);

    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    Pt::Xml::Characters* chars = Pt::Xml::toCharacters(&*it);
    PT_UNIT_ASSERT(chars);
    PT_UNIT_ASSERT(chars->content().narrow() == "hello");

    ++it;
    chars = Pt::Xml::toCharacters(&*it);
    PT_UNIT_ASSERT(chars);
    PT_UNIT_ASSERT(chars->isCData());
    PT_UNIT_ASSERT(chars->content().narrow() == "<Element>pure &gt; data</Element>");

    ++it;
    chars = Pt::Xml::toCharacters(&*it);
    PT_UNIT_ASSERT(chars);
    PT_UNIT_ASSERT_EQUALS(chars->content().narrow(), "world!");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    chars = Pt::Xml::toCharacters(&*it);
    PT_UNIT_ASSERT(chars);
    PT_UNIT_ASSERT(chars->isCData());
    PT_UNIT_ASSERT(chars->content().narrow() == "bbb");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::MaxCDATA()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>";
    input << "<![CDATA[0123456789]]>";
    input << "</a>\n";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    reader.setChunkSize(5);
    reader.reportCData(true);

    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    Pt::Xml::Characters* chars = Pt::Xml::toCharacters(&*it);
    PT_UNIT_ASSERT(chars);
    PT_UNIT_ASSERT(chars->isCData());
    PT_UNIT_ASSERT_EQUALS(chars->content(), "01234");

    ++it;
    chars = Pt::Xml::toCharacters(&*it);
    PT_UNIT_ASSERT(chars);
    PT_UNIT_ASSERT(chars->isCData());
    PT_UNIT_ASSERT_EQUALS(chars->content(), "56789");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::StartDocument()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
    input << "<test>\n";
    input << "</test>\n";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    reader.reportStartDocument(true);
        
    Pt::Xml::InputIterator it = reader.current();

    Pt::Xml::StartDocument* startDoc = Pt::Xml::toStartDocument(&*it);
    PT_UNIT_ASSERT(startDoc);

    const Pt::Xml::XmlDeclaration* xmlDecl = reader.input()->declaration();
    PT_UNIT_ASSERT(xmlDecl);
    PT_UNIT_ASSERT_EQUALS(xmlDecl->version(), "1.0");
    PT_UNIT_ASSERT_EQUALS(xmlDecl->encoding(), "UTF-8");
    PT_UNIT_ASSERT_EQUALS(xmlDecl->isStandalone(), true);

    ++it;
    Pt::Xml::StartElement* startElem = Pt::Xml::toStartElement(&*it);
    PT_UNIT_ASSERT(startElem);

    ++it;
    Pt::Xml::Characters* chars = Pt::Xml::toCharacters(&*it);
    PT_UNIT_ASSERT(chars);

    ++it;
    Pt::Xml::EndElement* endElem = Pt::Xml::toEndElement(&*it);
    PT_UNIT_ASSERT(endElem);

    ++it;
    Pt::Xml::EndDocument* endDoc = Pt::Xml::toEndDocument(&*it);
    PT_UNIT_ASSERT(endDoc);
}


void XmlReaderTest::DefaultEntities()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a>&lt;&gt;&amp;&quot;&apos;&#1234;</a>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    Pt::Xml::InputIterator it = reader.current();
    const Pt::Xml::Node& startNodeA = *it;

    PT_UNIT_ASSERT(startNodeA.type() == Pt::Xml::Node::StartElement);

    ++it;
    const Pt::Xml::Node& characterNode = *it;
    PT_UNIT_ASSERT(characterNode.type() == Pt::Xml::Node::Characters);

    Pt::Char compare[] = { '<', '>', '&', '\"', '\'', 1234, 0 };

    const Pt::Xml::Characters* text = dynamic_cast<const Pt::Xml::Characters*>(&characterNode);
    PT_UNIT_ASSERT(text->content() == compare);
}


void XmlReaderTest::CustomEntities()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    input << "<!DOCTYPE a [\n";
    input << "<!ELEMENT a (#PCDATA)>\n";
    input << "<!ENTITY entity1 \"Hello World!\">\n";
    input << "<!ENTITY entity2 SYSTEM \"e1.ext\" NDATA ext>\n";
    input << "]>\n";

    input << "<a>&entity1; &undeclared;&entity2;</a>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    Pt::Xml::InputIterator it = reader.current();

    PT_UNIT_ASSERT(Pt::Xml::toStartElement(&*it));

    ++it;
    Pt::Xml::EntityReference* ent = Pt::Xml::toEntityReference(&*it);
    PT_UNIT_ASSERT(ent);
    PT_UNIT_ASSERT(ent->name() == "undeclared" );

    Pt::Xml::StringInputSource entval("resolved");
    reader.addInput(entval);

    ++it;
    ent = Pt::Xml::toEntityReference(&*it);
    PT_UNIT_ASSERT(ent);
    PT_UNIT_ASSERT(ent->name() == "entity2" );

    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(&*it));
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(*it).content() == "Hello World! resolved");

    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toEndElement(&*it));

    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toEndDocument(&*it));

    // used size is names and values from DTD
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 44);
}


void XmlReaderTest::EntitySelfReference()
{
    std::stringstream input;
    input << "<!DOCTYPE test [\n";
    input << "<!ENTITY ha \"&ha;\">\n";
    input << "<!ELEMENT test ANY>\n";

    input << "]>\n";
    input << "<test></test>\n";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    PT_UNIT_ASSERT_THROW(reader.next(), Pt::Xml::SyntaxError);
}


void XmlReaderTest::ExternalEntities()
{
    XmlTestResolver resolver;
    resolver.addInput("Pt-Xml-test-e2", "World");

    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    input << "<!DOCTYPE a [\n";
    input << "<!ELEMENT a (#PCDATA)>\n";
    input << "<!ENTITY MyEntity1 SYSTEM \"Pt-Xml-test-e1\">\n";
    input << "<!ENTITY MyEntity2 PUBLIC \"http://www.pt-framework.org/e2\" \"Pt-Xml-test-e2\">\n";
    input << "<!ENTITY MyEntity3 PUBLIC \"http://www.pt-framework.org/e3\">\n";
    input << "]>\n";

    input << "<a>&MyEntity1; &MyEntity2;&MyEntity3;</a>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader( resolver, is );
    Pt::Xml::InputIterator it = reader.current();

    PT_UNIT_ASSERT(Pt::Xml::toStartElement(&*it));

    ++it;
    Pt::Xml::EntityReference* ent = Pt::Xml::toEntityReference(&*it);
    PT_UNIT_ASSERT(ent);
    PT_UNIT_ASSERT(ent->name() == "MyEntity1" );
    PT_UNIT_ASSERT(ent->get());
    PT_UNIT_ASSERT(ent->get()->publicId().empty());
    PT_UNIT_ASSERT(ent->get()->systemId() == "Pt-Xml-test-e1" );

    Pt::Xml::StringInputSource entval1( "Hello");
    reader.addInput( entval1);

    ++it;
    ent = Pt::Xml::toEntityReference(&*it);
    PT_UNIT_ASSERT(ent);
    PT_UNIT_ASSERT(ent->name() == "MyEntity3" );
    PT_UNIT_ASSERT(ent->get());
    PT_UNIT_ASSERT(ent->get()->publicId() == "http://www.pt-framework.org/e3");
    PT_UNIT_ASSERT(ent->get()->systemId().empty() );

    Pt::Xml::StringInputSource entval3( "!");
    reader.addInput(entval3);

    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(&*it));
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(*it).content() == "Hello World!");

    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toEndElement(&*it));

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);

    // used size is names and values from DTD
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 124);
}


void XmlReaderTest::ParameterEntities()
{
    std::stringstream input;
    input << "<?xml version='1.0'?>\n";
    input << "<!DOCTYPE test [\n";
    input << "<!ENTITY % test_content \"(#PCDATA)\">\n";
    input << "<!ELEMENT test %test_content; >\n";
    input << "<!ENTITY % xx \"&#37;zz;\">\n";
    input << "<!ENTITY % yy '&#60;!ENTITY tricky \"complicated\" >' >\n";
    input << "<!ENTITY % zz '%yy;' >\n";
    input << "%xx;\n";
    input << "]>\n";
    input << "<test>This sample shows a &tricky; method.</test>\n";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    
    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(Pt::Xml::toStartElement(&*it));

    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(&*it));
    PT_UNIT_ASSERT(Pt::Xml::toCharacters(*it).content() == "This sample shows a complicated method.");

    ++it;
    PT_UNIT_ASSERT(Pt::Xml::toEndElement(&*it));

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlReaderTest::ParameterEntitySelfReference()
{
    std::stringstream input;
    input << "<!DOCTYPE test [\n";
    input << "<!ENTITY % pe \"%pe;\">\n";
    input << "<!ELEMENT test ANY>\n";

    input << "]>\n";
    input << "<test></test>\n";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);

    PT_UNIT_ASSERT_THROW(reader.next(), Pt::Xml::SyntaxError);
}


void XmlReaderTest::MaxEntityRecursion()
{
    std::stringstream input;
    input << "<?xml version='1.0'?>\n";
    input << "<!DOCTYPE test [\n";
    input << "<!ENTITY ha1 \"XXX\">\n";
    input << "<!ENTITY ha2 \"&ha1; &ha1; &ha1; &ha1;\">\n";
    input << "<!ENTITY ha3 \"&ha2; &ha2; &ha2; &ha2;\">\n";
    input << "<!ENTITY ha4 \"&ha3; &ha3; &ha3; &ha3;\">\n";
    input << "<!ENTITY ha5 \"&ha4; &ha4; &ha4; &ha4;\">\n";
    input << "<!ENTITY ha6 \"&ha5; &ha5; &ha5; &ha5;\">\n";
    input << "<!ENTITY ha7 \"&ha6; &ha6; &ha6; &ha6;\">\n";
    input << "<!ENTITY ha8 \"&ha7; &ha7; &ha7; &ha7;\">\n";
    input << "<!ELEMENT test ANY>\n";

    input << "]>\n";
    input << "<test>&ha8;</test>\n";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    reader.setMaxInputDepth(4);

    reader.next();
    PT_UNIT_ASSERT_THROW(reader.next(), Pt::Xml::SyntaxError);
}

void XmlReaderTest::CommentInProlog()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<!--</a>-->";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    
    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::CommentBeforeRoot()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<!-- - ab --><a/>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    
    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::CommentInElement()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    input << "<a>123<!--a-->456</a>";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    
    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    const Pt::Xml::Characters* text = Pt::Xml::toCharacters(&*it);
    PT_UNIT_ASSERT(text);
    PT_UNIT_ASSERT(text->content() == "123456");

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::CommentInEpilog()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<a/><!--a-->";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    
    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::StartElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndElement);

    ++it;
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
    PT_UNIT_ASSERT_EQUALS(reader.usedSize(), 0);
}


void XmlReaderTest::EmptyComment()
{
    std::stringstream input;
    input << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    input << "<!---->";

    Pt::Xml::BinaryInputSource is(input);
    Pt::Xml::XmlReader reader(is);
    Pt::Xml::InputIterator it = reader.current();
    PT_UNIT_ASSERT(it->type() == Pt::Xml::Node::EndDocument);
}


void XmlReaderTest::Benchmark()
{
    // compare to rev. 6012

    std::string txt;
    txt += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    txt += "<benchmark>";
    for(int i = 0; i < 50000; ++i)
    {
        txt += "<test>";
        txt += "<number>";
        txt += "3.1415";
        txt += "</number>";
        txt += "<text>";
        txt += "hello world!";
        txt += "</text>";
        txt += "</test>";
    }
    txt += "</benchmark>";

    std::istringstream iss(txt);
    Pt::Xml::BinaryInputSource is(iss);

    Pt::System::Clock c;
    c.start();

    Pt::Xml::XmlReader reader(is);

    int n = 0;
    for(;;)
    {      
        //Pt::Xml::Node& node = reader.next();

        //++n;
        //if(node.type() == Pt::Xml::Node::EndDocument)
            //break;

        Pt::Xml::Node* node = reader.advance();
        if( ! node && iss.rdbuf()->in_avail() <= 0)
        {
            break;
        }

        ++n;
    }

    Pt::Timespan ts = c.stop();
    std::cerr << "parsed " << n << " nodes in " << ts.toMSecs()  << " msecs." << std::endl;
}
