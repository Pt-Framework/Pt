#include "Pt/Xml/XmlReader.h"
#include "Pt/Xml/Resolver.h"
#include <Pt/Xml/XmlDeclaration.h>
#include <Pt/Xml/DocTypeDeclaration.h>
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/CData.h"
#include "Pt/Xml/ProcessingInstruction.h"
#include "Pt/Xml/Comment.h"
#include "Pt/Xml/EndDocument.h"

#include "Pt/String.h"
#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf8Codec.h"

#include <sstream>
#include <iostream>

using namespace std;


namespace Pt {

namespace Xml {


XmlReader::XmlReader(std::istream& is)
: _textBuffer(0)
, _buffer(0)
, _tokenMax(512)
{
    _buffer = new Text::TextBuffer( is.rdbuf(), new Pt::Text::Utf8Codec() );
    _textBuffer = _buffer;

    this->init();
}


XmlReader::XmlReader(Text::TextStream& is)
: _textBuffer(is.rdbuf())
, _buffer( 0 )
, _tokenMax(512)
{
    this->init();
}


void XmlReader::init()
{
    const std::char_traits<Pt::Char>::int_type eof = std::char_traits<Pt::Char>::eof();

    // read magic bytes
    Char firstBytes[8];
    size_t size = _textBuffer->sgetn(firstBytes, 8);
    String start(firstBytes, size);

    // see if byte order mark and a valid XML declaration is present
    static const String xmlDeclStart(L"<?xml");
    size_t pos = start.find(xmlDeclStart);
    if(pos != 0)
        throw std::logic_error("Invalid XML declaration." + PT_SOURCEINFO);

    // read the whole XML declaration
    while( true )
    {
        Char ch = _textBuffer->sbumpc();

        if(ch.value() == eof)
            throw std::logic_error("Invalid XML declaration." + PT_SOURCEINFO);

        if(ch == '>')
            break;

        start.append( 1, ch );
    }
}


XmlReader::~XmlReader()
{
    while( !_nodeBuffer.empty() )
    {
        delete _nodeBuffer.front();
        _nodeBuffer.pop();
    }

    delete _buffer;
}


XmlReader::Iterator XmlReader::current()
{
    return Iterator(*this);
}


XmlReader::Iterator XmlReader::end() const
{
    return Iterator();
}


const Node& XmlReader::get()
{
    if( _nodeBuffer.empty() )
        return this->next();

    return *_nodeBuffer.front();
}


const Node& XmlReader::next()
{
    if( _nodeBuffer.size() > 0 )
    {
        delete _nodeBuffer.front();
        _nodeBuffer.pop();
    }

    if( !_nodeBuffer.empty() )
    {
        return *_nodeBuffer.front();
    }

    switch( _textBuffer->sgetc() )
    {
        case uint32_t(-1): // EOF
        {
            return this->documentEnd();
        }

        case '<':
        {
            switch( _textBuffer->snextc() )
            {
                case '/':
                    _textBuffer->snextc();
                    this->onEndElement();
                    break;

                case '!':
                    switch( _textBuffer->snextc() )
                    {
                        case 'D': this->onDocType(); break;
                        case '-': this->onComment(); break;
                    }

                    break;

                case '?':
                    throw std::logic_error("Processing instruction not yet supported" + PT_SOURCEINFO);

                default:
                    this->onStartElement();
                    break;
            }

            break;
        }

        default:
            this->onTextElement();
            break;
    }

    if( _nodeBuffer.empty() )
    {
        return this->documentEnd();
    }

    return *_nodeBuffer.front();
}


XmlReader& XmlReader::operator>>(StartElement& to)
{
    // check if there is a node in the buffer
    if( !_nodeBuffer.empty() )
    {
        const StartElement* elem = dynamic_cast<const StartElement*>( _nodeBuffer.front() );
        if(elem)
        {
            to = *elem;
            delete _nodeBuffer.front();
            _nodeBuffer.pop();
            return *this;
        }

        throw std::logic_error("Requested XML element is not a start element." + PT_SOURCEINFO);
    }

    if( '<' != _textBuffer->sgetc() )
        throw std::logic_error("Requested XML element is not a start element." + PT_SOURCEINFO);

    if( '/' != _textBuffer->snextc() )
        throw std::logic_error("Requested XML element is not a start element." + PT_SOURCEINFO);

    _textBuffer->snextc();

    this->parseStartElement(to);

    return *this;
}


XmlReader& XmlReader::operator>>(EndElement& to)
{
    // check if there is a node in the buffer
    if( !_nodeBuffer.empty() )
    {
        const EndElement* elem = dynamic_cast<const EndElement*>( _nodeBuffer.front() );
        if(elem)
        {
            to = *elem;
            delete _nodeBuffer.front();
            _nodeBuffer.pop();
            return *this;
        }

        throw std::logic_error("Requested XML element is not an end element." + PT_SOURCEINFO);
    }

    if( '<' != _textBuffer->sgetc() )
        throw std::logic_error("Requested XML element is not an end element." + PT_SOURCEINFO);

    if( '/' == _textBuffer->snextc() )
        throw std::logic_error("Requested XML element is not an end element." + PT_SOURCEINFO);

    this->parseEndElement(to);
    return *this;
}


XmlReader& XmlReader::operator>>(Characters& to)
{
    // check if there is a node in the buffer
    if( !_nodeBuffer.empty() )
    {
        const Characters* elem = dynamic_cast<const Characters*>( _nodeBuffer.front() );
        if(elem)
        {
            to = *elem;
            delete _nodeBuffer.front();
            _nodeBuffer.pop();
            return *this;
        }

        throw std::logic_error("Requested XML element is not a text element." + PT_SOURCEINFO);
    }

    if( '<' == _textBuffer->sgetc() )
        throw std::logic_error("Requested XML element is not text element." + PT_SOURCEINFO);

    this->parseTextElement(to);
    return *this;
}


void XmlReader::onDocType()
{
    Char buffer[7];
    _textBuffer->sgetn(buffer, 7);
    String content(buffer, 7);

    if( content != String(L"DOCTYPE") )
        throw std::logic_error("Invalid DOCTYPE declaration." + PT_SOURCEINFO);

    // read the whole DOCTYPE declaration
    this->getUntil(content, L">");
    _nodeBuffer.push( new DocTypeDeclaration(content) );

    _textBuffer->snextc();
}


void XmlReader::onStartElement()
{
    std::auto_ptr<Xml::StartElement> elem( new Xml::StartElement() );
    bool isStandalone = this->parseStartElement(*elem);

    _nodeBuffer.push( elem.get() );

    if (isStandalone)
    {
        _nodeBuffer.push( new Xml::EndElement(elem->name()) );
    }

    elem.release();
}


void XmlReader::onEndElement()
{
    std::auto_ptr<Xml::EndElement> elem( new Xml::EndElement() );
    this->parseEndElement(*elem);
    _nodeBuffer.push( elem.release() );
}


void XmlReader::onTextElement()
{
    std::auto_ptr<Xml::Characters> elem( new Xml::Characters() );
    this->parseTextElement(*elem);
    _nodeBuffer.push( elem.release() );
}


void XmlReader::onComment()
{
    static const String commentEnd(L">");

    String text;
    this->getUntil(text, commentEnd);
    _textBuffer->snextc();
    _nodeBuffer.push( new Comment(text) );
}


bool XmlReader::parseAttribute(String& name, String& value)
{
    typedef std::char_traits<Pt::Char> CharTraits;
    static const uint32_t eof = CharTraits::eof();

    if( _textBuffer->sgetc() == '>' || _textBuffer->sgetc() == '/') {
        return false;
    }

    static const String attributeNameBegin(L"> /");
    Pt::Char last = this->findNotOf(attributeNameBegin);

    if( last == '>' || last == '/') {
        return false;
    }

    static const String attributeNameEnd(L">/= ");
    this->getUntil(name, attributeNameEnd);

    if( _textBuffer->sgetc() == '>' || _textBuffer->sgetc() == '/' ) {
        throw  std::logic_error("Invalid XML attribute" + PT_SOURCEINFO);
    }

    static const String attributeValueBegin(L">/\"\'");
    this->findOf(attributeValueBegin);
    if( _textBuffer->sgetc() == '>' || _textBuffer->sgetc() == '/') {
        throw  std::logic_error("Invalid XML attribute" + PT_SOURCEINFO);
    }

    _textBuffer->snextc();

    static const String attributeValueEnd(L"\"\'>/");
    this->getUntil(value, attributeValueEnd);
    if( _textBuffer->sgetc() == '>' || _textBuffer->sgetc() == '/' ) {
        throw  std::logic_error("Invalid XML attribute" + PT_SOURCEINFO);
    }

    _textBuffer->snextc();

    return true;
}



bool XmlReader::parseStartElement(StartElement& to)
{
    bool isStandalone = false;

    static const String startElementBegin(L">/ \t");

    Pt::Char last = this->findNotOf(startElementBegin);
    if( last == '>' || last == '/') {
        throw std::logic_error("Invalid XML end element: no name" + PT_SOURCEINFO);
    }

    static const String startElementEnd(L"> \t/");
    this->getUntil(to.name(), startElementEnd);

    while(true)
    {
        String attrName, attrValue;
        if( false == this->parseAttribute( attrName, attrValue ) )
            break;

        to.addAttribute( Attribute( attrName, attrValue ) );
    }

    if( _textBuffer->sgetc() == '/' )
    {
        isStandalone = true;
        _textBuffer->snextc();
    }

    _textBuffer->snextc();

    return isStandalone;
}


void XmlReader::parseEndElement(EndElement& to)
{
    static const String endElementBegin(L">/ \t");

    Pt::Char last = this->findNotOf(endElementBegin);

    if( last == '>' || last == '/') {
        throw std::logic_error("Invalid XML end element: no name" + PT_SOURCEINFO);
    }

    static const String endElementEnd(L"> \t/");
    this->getUntil(to.name(), endElementEnd);

    _textBuffer->snextc();
}


void XmlReader::parseTextElement(Characters& to)
{
    static const String textElementEnd(L"<");
    this->getUntil(to.content(), textElementEnd);
    this->resolveEntities(to.content());
}


/*
const Char* XmlReader::refill(const Char* current)
{
    const std::char_traits<Pt::Char>::int_type eof = std::char_traits<Pt::Char>::eof();

    // refill buffer if neccessary
    if( current == _textBuffer->in_end() )
    {
        _textBuffer->in_bump( _textBuffer->in_avail() );
        if( eof == _textBuffer->sgetc() )
            throw std::logic_error("Reached EOF within elemwnt" + PT_SOURCEINFO);

        current = _textBuffer->in_begin();
    }

    return current;
}
*/

void XmlReader::findOf(const String& str)
{
    typedef std::char_traits<Pt::Char> CharTraits;
    const Char eof = CharTraits::to_char_type( CharTraits::eof() );
    for( Char ch = _textBuffer->sgetc(); eof != ch; ch = _textBuffer->snextc() )
    {
        if( String::npos != str.find( ch ) )
            break;
    }
}


Pt::Char XmlReader::findNotOf(const String& str)
{
    Pt::Char last = _textBuffer->sgetc();
    typedef std::char_traits<Pt::Char> CharTraits;
    const Char eof = CharTraits::to_char_type( CharTraits::eof() );

    for( Char ch = _textBuffer->sgetc(); eof != ch; ch = _textBuffer->snextc() )
    {
        if( String::npos == str.find( ch ) )
        {
            return last;
        }
        last = ch;
    }

    return last;
}


void XmlReader::getUntil(String& buffer, const String& stop)
{
/*
    const Char* current = _textBuffer->in_begin();
    const Char* begin = _textBuffer->in_begin();
    const Char* end = 0;

    // move to end of name
    for(; ; ++current)
    {
        // refill buffer if neccessary
        if( current == _textBuffer->in_end() )
        {
            // save what was read so far
            buffer.append( begin, current - begin );
            current = this->refill(current);
            begin = current;
        }

        if( stop.find( *current ) != String::npos )
        {
            end = current;
            break;
        }
    }

    // copy to target buffer
    buffer.append(begin, end - begin);

    // adjust buffer pointers
    _textBuffer->in_bump( current - _textBuffer->in_begin() );
*/

    typedef std::char_traits<Pt::Char> CharTraits;
    const Char eof = CharTraits::to_char_type( CharTraits::eof() );

    size_t n = 0;
    for( _token[n] = _textBuffer->sgetc(); eof != _token[n]; _token[n] = _textBuffer->snextc() )
    {
        if( stop.find( _token[n] ) != String::npos )
            break;

        if(n == _tokenMax - 1)
        {
            buffer.append(_token, n+1);
            n = 0;
        }
        else
        {
            ++n;
        }
    }

    buffer.append(_token, n);
}


void XmlReader::resolveEntities(String& str)
{
    size_t entityBegin = 0;
    size_t entityEnd = 0;

    while( (entityBegin = str.find('&', entityBegin)) != string::npos) {
        entityEnd = str.find(';', entityBegin);
        if(entityEnd == string::npos)
        {
            throw logic_error( "Invalid XML entitiy reference" + PT_SOURCEINFO );
        }

        String ref = str.substr(entityBegin+1, entityEnd-entityBegin-1);

        String resolved = _resolver.resolveEntity( str.substr(entityBegin+1, entityEnd-entityBegin-1) );

        str.replace(entityBegin, entityEnd-entityBegin+1, resolved);

        ++entityBegin;
    }
}


} // namespace Xml

} // namespace Pt
