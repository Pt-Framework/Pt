#include "Pt/Xml/XmlStream.h"
#include "Pt/Xml/Resolver.h"
#include <Pt/Xml/XmlDeclaration.h>
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/CData.h"
#include "Pt/Xml/ProcessingInstruction.h"
#include "Pt/Xml/Comment.h"
#include "Pt/Xml/EndDocument.h"
using namespace Pt::Xml;

#include "Pt/String.h"
#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf8Codec.h"
using namespace Pt;

#include <sstream>
#include <iostream>
using namespace std;


XmlStreamBuffer::~XmlStreamBuffer() throw()
{
    for(int n = 0; n < _inputBuffer.size(); ++n) {
        delete _inputBuffer[n];
    }
    _inputBuffer.clear();
}


std::auto_ptr<Xml::Node> XmlStreamBuffer::next()
{
    if( !_inputBuffer.empty() ) {
        Xml::Node* node = _inputBuffer.back();
        _inputBuffer.pop_back();
        return std::auto_ptr<Xml::Node>(node);
    }

    XmlToken token;
    size_t size = this->readToken(token);

    if( token.type() == XmlToken::EndDocument )
        return std::auto_ptr<Xml::Node>(new EndDocument);

    Xml::Node::Type type = this->analyse(token);
    switch(type) {
        case Node::Characters: {
            return std::auto_ptr<Xml::Node>( new Characters( token.buffer() ) );
        }
        case Node::StartElement: {
            auto_ptr<StartElement> node( new StartElement() );
            this->parseStartElement(token, *node);
            return std::auto_ptr<Xml::Node>(node);
        }
        case Node::EndElement: {
            auto_ptr<EndElement> node( new EndElement() );
            this->parseEndElement(token, *node);
            return std::auto_ptr<Xml::Node>(node);
        }
        case Node::ProcessingInstruction: {
            auto_ptr<ProcessingInstruction> node( new ProcessingInstruction() );
            this->parseProcessingInstruction(token, *node);
            return std::auto_ptr<Xml::Node>(node);
        }
        case Node::Comment: {
            auto_ptr<Comment> node( new Comment() );
            this->parseComment(token, *node);
            return std::auto_ptr<Xml::Node>(node);
        }
        case Node::CData: {
            if(token.buffer().size() < 12)
                throw ParseError("Invalid CDATA.", PT_SOURCEINFO);

            return std::auto_ptr<Xml::Node>( new CData( token.buffer().substr(9, token.buffer().size()-12) ) );
        }
    }

    return std::auto_ptr<Xml::Node>(new EndDocument);
}


void XmlStreamBuffer::putback(std::auto_ptr<Xml::Node>& node)
{
    _inputBuffer.push_back( node.release() );
}


size_t XmlStreamBuffer::readToken(XmlToken& token)
{
    Pt::TextIStream::int_type eof = std::char_traits<Pt::Char>::eof();

    Pt::TextIStream::int_type ch = this->sbumpc();
    if( std::char_traits<Pt::Char>::eq_int_type( ch, eof ) ) {
        token.setType( XmlToken::EndDocument );
        return 0;
    }

    token.buffer() += std::char_traits<Pt::Char>::to_char_type(ch);
    if(ch == '<') {
        token.setType(XmlToken::Tag);
        readTag(token);
    }
    else {
        token.setType(XmlToken::Characters);
        readCharacters(token);
    }

    return token.buffer().size();
}


void XmlStreamBuffer::readTag(XmlToken& token)
{
    Char eof = char_traits<Pt::Char>::eof();
    Char ch = this->sgetc();

    while( ch != eof && ch != '>') {
        token.buffer() += ch;
        ch = this->snextc();

        if(token.buffer().size() == 9) {
            if(token.buffer() == "<![CDATA[") {
                readCData(token);
                return;
            }

            break;
        }
    }

    while( ch != eof && ch != '>') {
        token.buffer() += ch;
        ch = this->snextc();
    }

    if( ch == '>' ) {
        token.buffer() += ch;
        this->sbumpc();
    }

    return;
}


void XmlStreamBuffer::readCharacters(XmlToken& token)
{
    Char eof = char_traits<Pt::Char>::eof();
    Char ch = this->sgetc();

    while( ch != eof && ch != '<') {
        token.buffer() += ch;
        ch = this->snextc();
    }

    return;
}


void XmlStreamBuffer::readCData(XmlToken& token)
{
    Char eof = char_traits<Pt::Char>::eof();
    Char ch = this->sgetc();

    while( ch != eof) {
        if( token.buffer().find("]]>", token.buffer().size()-3) != String::npos )
            break;

        token.buffer() += ch;
        ch = this->snextc();
    }

    return;
}


Xml::Node::Type XmlStreamBuffer::analyse(XmlToken& token)
{
    Pt::String& text = token.buffer();

    if( text.at(0) != '<' ) {
        return Xml::Node::Characters;
    }

    if( text.at(1) == '/' ) {
        return Xml::Node::EndElement;
    }
    else if ( text.at(1) == '!' ) {
        if( text.find("<!--") == 0 )
            return Xml::Node::Comment;
        else if( text.find("<![CDATA[") == 0 )
            return Xml::Node::CData;
    }
    else if ( text.at(1) == '?' ) {
        return Xml::Node::ProcessingInstruction;
    }
    else {
        return Xml::Node::StartElement;
    }
}


void XmlStreamBuffer::parseStartElement(const XmlToken& token, StartElement& element) throw(ParseError)
{
    //std::cerr << "Parsing StartElement: " << token.buffer() << endl;

    // Element Name
    size_t beginName = token.buffer().find_first_not_of("< \t\n\r");
    if(beginName == String::npos) {
        throw ParseError("Invalid XML start element.", PT_SOURCEINFO);
    }

    size_t endName = token.buffer().find_first_of("> \t\n\r", beginName);
    if(endName == String::npos) {
        element.setName( token.buffer().substr(beginName) );
        //cerr << "ElementName: '" << element.name() << "'" << endl;
        return;
    }

    element.setName( token.buffer().substr(beginName, endName - beginName) );
    //cerr << "ElementName: '" << element.name() << "'" << endl;

    // Attribute processing
    while( (beginName = token.buffer().find_first_not_of("/> \t\n\r", endName + 1)) != String::npos) {
        String attributeName;
        String attributeValue;

        endName = token.buffer().find_first_of("= \t\n\r", beginName);
        if(endName == String::npos) {
            throw ParseError("Invalid attribute value in XML start element (0).", PT_SOURCEINFO);        
        }

        attributeName = token.buffer().substr(beginName, endName - beginName);
        if (attributeName.empty() ) {
            throw ParseError("Invalid attribute name in XML start element.", PT_SOURCEINFO);
        }
        //cerr << "Attribute Name: '"<< attributeName << "'"<< endl;

        size_t indexEqual = token.buffer().find_first_not_of(" \t\n\r", endName);
        if( token.buffer().at(indexEqual) != '=') {
            throw ParseError("Invalid attribute in XML start element. Expected '=' in attribute.", PT_SOURCEINFO);
        }

        beginName = token.buffer().find_first_of("\'\"", indexEqual + 1);
        if(beginName == String::npos) {
            throw ParseError("Invalid attribute value in XML start element (1).", PT_SOURCEINFO);        
        }

        endName = token.buffer().find_first_of("\'\"", beginName + 1);
        if(endName == String::npos) {
            throw ParseError("Invalid attribute value in XML start element (2).", PT_SOURCEINFO);        
        }

        attributeValue = token.buffer().substr(beginName + 1, endName - beginName - 1);
        if (attributeValue.empty() ) {
            throw ParseError("Invalid attribute value in XML start element (3).", PT_SOURCEINFO);
        }

        //cerr << "Attribute Value: '"<< attributeValue << "'" << endl;
        element.addAttribute( Attribute(attributeName, attributeValue) );
    }

    return;
}


void XmlStreamBuffer::parseEndElement(const XmlToken& token, EndElement& element) throw(ParseError)
{
    //std::cerr << "Parsing EndElement: " << token.buffer() << endl;

    if( token.buffer().at(0) != '<' || token.buffer().at(1) != '/' ) {
        throw ParseError("Invalid XML end element." , PT_SOURCEINFO);
    }

    size_t tokenBegin = token.buffer().find_first_not_of("</ \t\n\r");
    size_t tokenEnd = token.buffer().find_first_of("> \t\n\r", tokenBegin + 1);
    element.setName( token.buffer().substr(tokenBegin, tokenEnd - tokenBegin) );
    //cerr << "EndElement: '"<< element.name() << "'" << endl;

    return;
}


void XmlStreamBuffer::parseComment(const XmlToken& token, Comment& comment) throw(ParseError)
{
    //std::cerr << "Parsing Comment: " << token.buffer() << endl;
    
    if(token.buffer().size() < 7 ||
       token.buffer().at(0) != '<' || 
       token.buffer().at(1) != '!' ||
       token.buffer().at(2) != '-' ||         
       token.buffer().at(3) != '-' ||
       token.buffer().at(token.buffer().size()-3) != '-' ||             
       token.buffer().at(token.buffer().size()-2) != '-' ||         
       token.buffer().at(token.buffer().size()-1) != '>') {
        stringstream ss;
        ss << token.buffer();
        throw ParseError( string("Invalid XML comment:" + ss.str()), PT_SOURCEINFO);
    }

    static const size_t beginToken = 5;
    comment.setText( token.buffer().substr(5, token.buffer().size() - 8)  );
}


void XmlStreamBuffer::parseProcessingInstruction(const XmlToken& token, ProcessingInstruction& procInst) throw(ParseError)
{
    //std::cerr << "Parsing ProcessingInstruction: " << token.buffer() << endl;

    // Target
    size_t beginToken = token.buffer().find_first_not_of("<? \t\n\r");
    if(beginToken == String::npos) {
        throw ParseError("Invalid XML processing instruction.", PT_SOURCEINFO);        
    }

    size_t endToken = token.buffer().find_first_of("> \t\n\r", beginToken);
    if(endToken == String::npos) {
        throw ParseError("Invalid XML processing instruction.", PT_SOURCEINFO);
    }

    procInst.setTarget( token.buffer().substr(beginToken, endToken - beginToken)  );
    //cerr << "ProcessingInstruction for target: '"<< procInst.target() << '\'' << endl;

    beginToken = endToken;
    endToken = token.buffer().find('?', beginToken);
    if(endToken == String::npos) {
        throw ParseError("Invalid XML processing instruction.", PT_SOURCEINFO);
    }    

    procInst.setData( token.buffer().substr(beginToken, endToken - beginToken)  );
    //cerr << "ProcessingInstruction data: '"<< procInst.data() << '\'' << endl;
}


void XmlStreamBuffer::parseXmlDeclaration(const XmlToken& token, XmlDeclaration& xmlDecl) throw(ParseError)
{
    //std::cerr << "Parsing StartElement: " << token.buffer() << endl;

    size_t beginName = 5;
    size_t endName = 5;
    // Attribute processing
    while( (beginName = token.buffer().find_first_not_of("?> \t\n\r", endName + 1)) != String::npos) {
        String attributeName;
        String attributeValue;

        endName = token.buffer().find_first_of("= \t\n\r", beginName);
        if(endName == String::npos) {
            throw ParseError("Invalid XML declaration.", PT_SOURCEINFO);        
        }

        attributeName = token.buffer().substr(beginName, endName - beginName);
        if (attributeName.empty() ) {
            throw ParseError("Invalid XML declaration.", PT_SOURCEINFO);
        }
        //cerr << "Attribute Name: '"<< attributeName << "'"<< endl;

        size_t indexEqual = token.buffer().find_first_not_of(" \t\n\r", endName);
        if( token.buffer().at(indexEqual) != '=') {
            throw ParseError("Invalid XML declaration. Expected '=' in attribute.", PT_SOURCEINFO);
        }

        beginName = token.buffer().find_first_of("\'\"", indexEqual + 1);
        if(beginName == String::npos) {
            throw ParseError("Invalid XML declaration.", PT_SOURCEINFO);
        }

        endName = token.buffer().find_first_of("\'\"", beginName + 1);
        if(endName == String::npos) {
            throw ParseError("Invalid XML declaration.", PT_SOURCEINFO);
        }

        attributeValue = token.buffer().substr(beginName + 1, endName - beginName - 1);
        if (attributeValue.empty() ) {
            throw ParseError("Invalid XML declaration.", PT_SOURCEINFO);
        }

        if(attributeName == "version") {
            xmlDecl.setVersion(attributeValue);
        }
        else if(attributeName == "encoding") {
            xmlDecl.setEncoding(attributeValue);
        }
        else if(attributeName == "standalone") {
            if(attributeValue == "true") {
                xmlDecl.setStandalone(true);
            }
            else {
                xmlDecl.setStandalone(false);
            }
        }
        else {
            throw ParseError("Invalid value in XML declaration.", PT_SOURCEINFO);
        }
    }

    return;
}





void XmlStreamBase::resolveEntities(String& str) throw(ParseError) {
    size_t entityBegin = 0;
    size_t entityEnd = 0;

    while( (entityBegin = str.find('&', entityBegin)) != string::npos) {
        entityEnd = str.find(';', entityBegin);
        if(entityEnd == string::npos) {
            throw ParseError( "Invalid XML entitiy reference", PT_SOURCEINFO );
        }

        String ref = str.substr(entityBegin+1, entityEnd-entityBegin-1);
        //cerr << "Resolving: " << ref << endl;
        String resolved = this->resolver().resolveEntity( str.substr(entityBegin+1, entityEnd-entityBegin-1) );
        //cerr << "Resolved To: " << resolved << endl;
        str.replace(entityBegin, entityEnd-entityBegin+1, resolved);
        //cerr << "New content: " << str << endl;
        entityBegin = entityEnd;
    }
}





void XmlIStream::init()
{
    std::auto_ptr<Xml::Node> node = this->readNode();
    if(node->type() != Xml::Node::ProcessingInstruction) {
        this->buffer()->putback(node);
        return;
    }

    try {
        ProcessingInstruction& procInst = dynamic_cast<ProcessingInstruction&>(*node);

        if(procInst.target() != "xml") {
            throw "No XML declaration.";
        }
        //cerr << "XML declaration: " << procInst.data() << endl;
    }
    catch(...) {
        this->buffer()->putback(node);
        return;
    }
}


XmlIStream::~XmlIStream() throw()
{
}


XmlIStream::operator void*() const
{
    return std::basic_istream<Pt::Char>::operator void*();
}


bool XmlIStream::operator!() const
{
    return std::basic_istream<Pt::Char>::operator!();
}


std::auto_ptr<Xml::Node> XmlIStream::readNode() throw(IO::IOError, ParseError)
{
    std::auto_ptr<Xml::Node> node;
    while(true) {
        node = this->buffer()->next();

        if( XmlStreamBase::flags() & SkipWhitespace && node->type() == Xml::Node::Characters ) {
            Characters& chars = dynamic_cast<Characters&>(*node);
            if( chars.content().find_first_not_of(" \t\n\r") == string::npos )
                continue;
        }
    
        if( XmlStreamBase::flags() & SkipComments && node->type() == Xml::Node::Comment ) {
            continue;
        }

        if( XmlStreamBase::flags() & SkipProcessingInstructions && node->type() == Xml::Node::ProcessingInstruction ) {
            continue;
        }

        break;
    }

    if( node->type() == Xml::Node::Characters ) {
        Characters& chars = dynamic_cast<Characters&>(*node);
        this->resolveEntities(chars.content());
    }
    else if( node->type() == Xml::Node::StartElement &&
             !(XmlStreamBase::flags() & IgnoreNamespaces) ) {
        StartElement& element = dynamic_cast<StartElement&>(*node);

        std::list<Attribute>::const_iterator it;
        for(it = element.attributes().begin(); it != element.attributes().end(); ++it) {
            if(it->name().find("xmlns:") == 0) {
                Namespace ns( it->value(), it->name().substr(6) );
                this->namespaceContext().addNamespace(element.name(), ns);
                //cerr << "Namespace: "<< it->name() << ": " << it->value() << "'" << endl;
            }
        }

        element.setNamespaceContext( this->namespaceContext() );
    }
    else if(node->type() == Xml::Node::EndElement &&
            !(XmlStreamBase::flags() & IgnoreNamespaces) ) {
        EndElement& endElement = dynamic_cast<EndElement&>(*node);
        // remove namespaces that go out of scope.
        this->namespaceContext().removeNamespace( endElement.name() );
    }
    else if( node->type() == Xml::Node::EndDocument ) {
        this->setstate(ios::eofbit);
    }

    return node;
}


std::auto_ptr<Xml::Node> XmlIStream::peekNode() throw(IO::IOError, ParseError)
{
    auto_ptr<Xml::Node> node = this->readNode();
    auto_ptr<Xml::Node> ret( node->clone() );
    this->buffer()->putback(node);
    return ret;
}


std::auto_ptr<StartElement> XmlIStream::readStartElement() throw(IO::IOError, ParseError)
{
    auto_ptr<Xml::Node> node = this->readNode();

    StartElement* startElement = dynamic_cast<StartElement*>( node.get() );
    if(!startElement) {
        this->buffer()->putback(node);
        throw ParseError("Could not read XML start element.", PT_SOURCEINFO);
    }

    node.release();
    return auto_ptr<StartElement>(startElement);
}


std::auto_ptr<StartElement> XmlIStream::readStartElement(const String& name) throw(IO::IOError, ParseError)
{
    auto_ptr<StartElement> startElement = readStartElement();
    if(startElement->name() != name) {
        auto_ptr<Xml::Node> node( startElement.release() );
        this->buffer()->putback(node);
        throw ParseError("Could not read XML start element.", PT_SOURCEINFO);
    }

    return startElement;
}


std::auto_ptr<EndElement> XmlIStream::readEndElement() throw(IO::IOError, ParseError)
{
    auto_ptr<Xml::Node> node = this->readNode();

    EndElement* endElement = dynamic_cast<EndElement*>( node.get() );
    if(!endElement) {
        this->buffer()->putback(node);
        throw ParseError("Could not read XML end element.", PT_SOURCEINFO);
    }

    node.release();
    return auto_ptr<EndElement>(endElement);
}


std::auto_ptr<EndElement> XmlIStream::readEndElement(const String& name) throw(IO::IOError, ParseError)
{
    auto_ptr<EndElement> endElement = readEndElement();
    if(endElement->name() != name) {
        auto_ptr<Xml::Node> node( endElement.release() );
        this->buffer()->putback(node);
        throw ParseError("Could not read XML end element.", PT_SOURCEINFO);
    }

    return endElement;
}


std::auto_ptr<Characters> XmlIStream::readCharacters() throw(IO::IOError, ParseError)
{
    auto_ptr<Xml::Node> node = this->readNode();

    Characters* chars = dynamic_cast<Characters*>( node.get() );
    if(!chars) {
        this->buffer()->putback(node);
        throw ParseError("Could not read XML end element.", PT_SOURCEINFO);
    }

    this->resolveEntities( chars->content() );

    node.release();
    return auto_ptr<Characters>(chars);
}


std::auto_ptr<CData> XmlIStream::readCData() throw(IO::IOError, ParseError)
{
    auto_ptr<Xml::Node> node = this->readNode();

    CData* cdata = dynamic_cast<CData*>( node.get() );
    if(!cdata) {
        this->buffer()->putback(node);
        throw ParseError("Could not read XML CDATA element.", PT_SOURCEINFO);
    }

    node.release();
    return auto_ptr<CData>(cdata);
}


XmlOStream& Pt::Xml::operator<<(XmlOStream& os, const StartElement& element) {
    os.writeCharacters("<");
    os.writeCharacters( element.name() );
    os.writeCharacters(" ");
    
    for(std::list<Attribute>::const_iterator it = element.attributes().begin(); it != element.attributes().end(); ++it) {
        os.writeCharacters( it->name() );
        os.writeCharacters( "=\"");
        os.writeCharacters( it->value() );
        os.writeCharacters("\"");
    }
    
    os.writeCharacters(">");
    return os;
}
