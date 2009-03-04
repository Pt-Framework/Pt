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
#include "Pt/Xml/ParseError.h"
#include "Pt/String.h"
#include "Pt/Text/TextStream.h"
#include "Pt/Text/Utf8Codec.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>
#include <sstream>
#include <iostream>

using namespace std;


namespace Pt {

namespace Xml {

struct XmlParseState
{
    virtual ~XmlParseState()
    {}

    XmlParseState* onChar(Pt::Char c, XmlReader& reader)
    {
        //std::cerr << "onChar: " << c.narrow('_') << std::endl;

        if( c == std::char_traits<Pt::Char>::to_char_type( std::char_traits<Pt::Char>::eof() ) )
        {
            return this->onEof(c, reader);
        }

        switch( c.value() )
        {
                case '\n':
                case ' ':
                case '\t':
                case '\r':
                    return this->onSpace(c, reader);

                case '<':
                    return this->onOpenBracket(c, reader);

                case '>':
                    return this->onCloseBracket(c, reader);

                case ':':
                    return this->onColon(c, reader);

                case '/':
                    return this->onSlash(c, reader);

                case '=':
                    return this->onEqual(c, reader);

                case '"':
                case '\'':
                    return this->onQoute(c, reader);

                case '!':
                    return this->onExclam(c, reader);

                case '?':
                    return this->onQuest(c, reader);

                default:
                    return this->onAlpha(c, reader);
        }

        this->syntaxError(reader.line());
        return 0;
    }

    virtual XmlParseState* onSpace(Pt::Char c, XmlReader& reader)
    {
        this->syntaxError( reader.line() );
        return this;
    }

    virtual XmlParseState* onOpenBracket(Pt::Char c, XmlReader& reader)
    {
        this->syntaxError(reader.line());
        return this;
    }

    virtual XmlParseState* onCloseBracket(Pt::Char c, XmlReader& reader)
    {
        this->syntaxError(reader.line());
        return this;
    }

    virtual XmlParseState* onColon(Pt::Char c, XmlReader& reader)
    {
        this->syntaxError(reader.line());
        return this;
    }

    virtual XmlParseState* onSlash(Pt::Char c, XmlReader& reader)
    {
        this->syntaxError(reader.line());
        return this;
    }

    virtual XmlParseState* onEqual(Pt::Char c, XmlReader& reader)
    {
        this->syntaxError(reader.line());
        return this;
    }

    virtual XmlParseState* onQoute(Pt::Char c, XmlReader& reader)
    {
        this->syntaxError(reader.line());
        return this;
    }

    virtual XmlParseState* onExclam(Pt::Char c, XmlReader& reader)
    {
        this->syntaxError(reader.line());
        return this;
    }

    virtual XmlParseState* onQuest(Pt::Char c, XmlReader& reader)
    {
        this->syntaxError(reader.line());
        return this;
    }

    virtual XmlParseState* onAlpha(Pt::Char c, XmlReader& reader)
    {
        this->syntaxError(reader.line());
        return this;
    }

    virtual XmlParseState* onEof(Pt::Char c, XmlReader& reader)
    {
        this->syntaxError(reader.line());
        return this;
    }

    void syntaxError(unsigned line)
    {
        std::cerr << "error in line " << line << std::endl;
        throw ParseError("syntax error", line);
    }
};


struct XmlParser {

struct OnEmptyElement : public XmlParseState
{
    virtual XmlParseState* onSpace(Pt::Char c, XmlReader& reader)
    {
        std::cerr << PT_SOURCEINFO.func()  << c.narrow('_') << std::endl;
        return this;
    }

    virtual XmlParseState* onCloseBracket(Pt::Char c, XmlReader& reader)
    {
        std::cerr << PT_SOURCEINFO.func() << c.narrow('_') << std::endl;
        reader._endElem.name() = reader._startElem.name();
        reader._current = &(reader._endElem);
        reader._depth--;
        return BeforeRootElement::instance();
    }

    static XmlParseState* instance()
    {
        static OnEmptyElement _state;
        return &_state;
    }
};


struct OnElementName : public XmlParseState
{
    virtual XmlParseState* onSpace(Pt::Char c, XmlReader& reader)
    {
        std::cerr << "not implemented"<< std::endl;
        return 0;
    }

    virtual XmlParseState* onSlash(Pt::Char c, XmlReader& reader)
    {
        std::cerr << PT_SOURCEINFO.func()  << c.narrow('_') << std::endl;
        reader._current = &(reader._startElem);
        reader._depth++;
        return OnEmptyElement::instance();
    }

    virtual XmlParseState* onAlpha(Pt::Char c, XmlReader& reader)
    {
       std::cerr << PT_SOURCEINFO.func()  << c.narrow('_') << std::endl;
        reader._startElem.name() += c;
        return this;
    }

    virtual XmlParseState* onCloseBracket(Pt::Char c, XmlReader& reader)
    {
        return 0; /// TODO normal start element
    }

    static XmlParseState* instance()
    {
        static OnElementName _state;
        return &_state;
    }
};


struct OnElement : public XmlParseState
{
    virtual XmlParseState* onSpace(Pt::Char c, XmlReader& reader)
    {
        std::cerr << PT_SOURCEINFO.func()  << c.narrow('_') << std::endl;
        return this;
    }

    virtual XmlParseState* onAlpha(Pt::Char c, XmlReader& reader)
    {
        std::cerr << PT_SOURCEINFO.func()  << c.narrow('_') << std::endl;
        reader._startElem.clear();
        reader._startElem.name() += c;
        return OnElementName::instance();
    }

    static XmlParseState* instance()
    {
        static OnElement _state;
        return &_state;
    }
};


struct BeforeRootElement : public XmlParseState
{
    virtual XmlParseState* onSpace(Pt::Char c, XmlReader& reader)
    {   std::cerr << PT_SOURCEINFO.func() << c.narrow('_') << std::endl;
        return this;
    }

    virtual XmlParseState* onOpenBracket(Pt::Char c, XmlReader& reader)
    {   std::cerr << PT_SOURCEINFO.func() << c.narrow('_') << std::endl;
        return OnElement::instance();
    }

    virtual XmlParseState* onEof(Pt::Char c, XmlReader& reader)
    {   std::cerr << PT_SOURCEINFO.func() << c.narrow('_') << std::endl;
        reader._current = &( reader.documentEnd() );
        return this;
    }

    static XmlParseState* instance()
    {
        static BeforeRootElement _state;
        return &_state;
    }
};


struct OnProcessingInstructionEnd : public XmlParseState
{
    virtual XmlParseState* onCloseBracket(Pt::Char c, XmlReader& reader)
    {
        reader._current = &(reader._procInstr);
        return BeforeRootElement::instance();
    }

    static XmlParseState* instance()
    {
        static OnProcessingInstructionEnd _state;
        return &_state;
    }
};


struct OnXmlDeclValue : public XmlParseState
{
    virtual XmlParseState* onQoute(Pt::Char c, XmlReader& reader)
    {
        static const Pt::Char version[] = { 'v', 'e', 'r', 's', 'i', 'o', 'n', 0};
        static const Pt::Char encoding[] = { 'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g',  0};
        static const Pt::Char standalone[] = { 's', 't', 'a', 'n', 'd', 'a', 'l', 'o', 'n','e', 0};
        static const Pt::Char trueval[] = { 't', 'r', 'u', 'e', 0};

        if(reader._attr.name() == version)
        {
            reader._version = reader._attr.value();
        }
        else if(reader._attr.name() == encoding)
        {
            reader._encoding = reader._attr.value();
        }
        else if(reader._attr.name() == standalone)
        {
            if(reader._attr.value() == trueval)
                reader._standalone = true;
        }

        return OnXmlDeclBeforeAttr::instance();
    }

    virtual XmlParseState* onAlpha(Pt::Char c, XmlReader& reader)
    {
        reader._attr.value() += c;;
        return this;
    }

    static XmlParseState* instance()
    {
        static OnXmlDeclValue _state;
        return &_state;
    }
};


struct OnXmlDeclBeforeValue : public XmlParseState
{
    virtual XmlParseState* onSpace(Pt::Char c, XmlReader& reader)
    {
        return this;
    }

    virtual XmlParseState* onQoute(Pt::Char c, XmlReader& reader)
    {
        return OnXmlDeclValue::instance();
    }

    static XmlParseState* instance()
    {
        static OnXmlDeclBeforeValue _state;
        return &_state;
    }
};


struct OnXmlDeclAfterName : public XmlParseState
{
    virtual XmlParseState* onSpace(Pt::Char c, XmlReader& reader)
    {
        return this;
    }

    virtual XmlParseState* onEqual(Pt::Char c, XmlReader& reader)
    {
        return OnXmlDeclBeforeValue::instance();
    }

    static XmlParseState* instance()
    {
        static OnXmlDeclAfterName _state;
        return &_state;
    }
};


struct OnXmlDeclAttr : public XmlParseState
{
    virtual XmlParseState* onSpace(Pt::Char c, XmlReader& reader)
    {
        return OnXmlDeclAfterName::instance();
    }

    virtual XmlParseState* onEqual(Pt::Char c, XmlReader& reader)
    {
        return OnXmlDeclBeforeValue::instance();
    }

    virtual XmlParseState* onAlpha(Pt::Char c, XmlReader& reader)
    {
        reader._attr.name() += c;
        return this;
    }

    static XmlParseState* instance()
    {
        static OnXmlDeclAttr _state;
        return &_state;
    }
};


struct OnXmlDeclEnd : public XmlParseState
{
    virtual XmlParseState* onCloseBracket(Pt::Char c, XmlReader& reader)
    {
        return BeforeRootElement::instance();
    }

    static XmlParseState* instance()
    {
        static OnXmlDeclEnd _state;
        return &_state;
    }
};


struct OnXmlDeclBeforeAttr : public XmlParseState
{
    virtual XmlParseState* onSpace(Pt::Char c, XmlReader& reader)
    {
        return this;
    }

    virtual XmlParseState* onAlpha(Pt::Char c, XmlReader& reader)
    {
        reader._attr.clear();
        reader._attr.name() += c;
        return OnXmlDeclAttr::instance();
    }

    virtual XmlParseState* onQuest(Pt::Char c, XmlReader& reader)
    {
        return OnXmlDeclEnd::instance();
    }

    static XmlParseState* instance()
    {
        static OnXmlDeclBeforeAttr _state;
        return &_state;
    }
};


struct OnXmlDeclName : public XmlParseState
{
    virtual XmlParseState* onSpace(Pt::Char c, XmlReader& reader)
    {
        static const Pt::Char xml[] = { 'x', 'm', 'l', 0};

        if( reader._procInstr.target() == xml )
            return OnXmlDeclBeforeAttr::instance();

        return 0; /// TODO it is a normal processing instruction
    }

    virtual XmlParseState* onAlpha(Pt::Char c, XmlReader& reader)
    {
        reader._procInstr.target() += c;
        return this;
    }

    static XmlParseState* instance()
    {
        static OnXmlDeclName _state;
        return &_state;
    }
};


struct OnXmlDeclQMark : public XmlParseState
{
    virtual XmlParseState* onAlpha(Pt::Char c, XmlReader& reader)
    {
        reader._procInstr.target().clear();
        reader._procInstr.target() += c;
        return OnXmlDeclName::instance();
    }

    static XmlParseState* instance()
    {
        static OnXmlDeclQMark _state;
        return &_state;
    }
};


struct OnXmlDecl : public XmlParseState
{
    virtual XmlParseState* onQuest(Pt::Char c, XmlReader& reader)
    {
        return OnXmlDeclQMark::instance();
    }

    static XmlParseState* instance()
    {
        static OnXmlDecl _state;
        return &_state;
    }
};


struct OnDocumentBegin : public XmlParseState
{
    virtual XmlParseState* onSpace(Pt::Char c, XmlReader& reader)
    {
        return BeforeRootElement::instance();
    }

    virtual XmlParseState* onOpenBracket(Pt::Char c, XmlReader& reader)
    {
        return OnXmlDecl::instance();
    }

    static XmlParseState* instance()
    {
        static OnDocumentBegin _state;
        return &_state;
    }
};

}; // XmlParser


XmlReader::XmlReader(std::istream& is)
: _textBuffer(0)
, _buffer(0)
, _depth(0)
, _line(1)
, _standalone(true)
, _current(0)
{
    _state = XmlParser::OnDocumentBegin::instance();

    _buffer = new Text::TextBuffer( &is, new Pt::Text::Utf8Codec() );
    _textBuffer = _buffer;
}


XmlReader::XmlReader(Text::TextStream& is)
: _textBuffer(is.rdbuf())
, _buffer( 0 )
, _depth(0)
, _line(1)
, _standalone(true)
, _current(0)
{
    _state = XmlParser::OnDocumentBegin::instance();
}


XmlReader::~XmlReader()
{
    delete _buffer;
}


const Node& XmlReader::get()
{
    if( ! _current )
    {
        this->next();
    }

    return *_current;
}


const Node& XmlReader::next()
{
    const Pt::Char eof = std::char_traits<char>::eof();

    _current = 0;
    Pt::Char ch = 0;
    do
    {
        ch = _textBuffer->sbumpc();
        _state = _state->onChar(ch, *this);

        if(ch == '\n')
        {
            ++_line;
        }
    }
    while ( !_current && ch != eof);

    return *_current;
}


bool XmlReader::advance()
{
    const Pt::Char eof = std::char_traits<char>::eof();

    _current = 0;
    Pt::Char ch = 0;
    while( ! _current && _textBuffer->in_avail() > 0 )
    {
        ch = _textBuffer->sbumpc();
        _state = _state->onChar(ch, *this);

        if(ch == '\n')
        {
            ++_line;
        }
    }

    return _current != 0;
}


const StartElement& XmlReader::nextElement()
{
    bool found = false;
    while( !found )
    {
        const Node& node = this->next();
        switch( node.type() )
        {
            case Node::EndDocument:
                throw std::logic_error("End of document" + PT_SOURCEINFO);

            case Node::StartElement:
                found = true;
                break;

            default:
                break;
        }

    }

    return static_cast<const StartElement&>( this->get() );
}


const Node& XmlReader::nextTag()
{
    bool found = false;
    while( !found )
    {
        const Node& node = this->next();
        switch( node.type() )
        {
            case Node::EndDocument:
                throw std::logic_error("End of document" + PT_SOURCEINFO);

            case Node::StartElement:
            case Node::EndElement:
                found = true;
                break;

            default:
                break;
        }

    }

    return this->get();
}

/*
XmlReader& XmlReader::operator>>(StartElement& to)
{
    return *this;
}


XmlReader& XmlReader::operator>>(EndElement& to)
{
    return *this;
}


XmlReader& XmlReader::operator>>(Characters& to)
{

    return *this;
}
*/




/*
bool XmlReader::parseAttribute(String& name, String& value)
{
    enum AttributeParseState
    {
        BeforeName  = 0,
        OnName      = 1,
        BeforeEqual = 2,
        BeforeValue = 3,
        OnValue     = 4,
        AfterValue  = 5
    };

    AttributeParseState state = BeforeName;

    // stay on '/' or '<' when done.
    typedef std::char_traits<Pt::Char> CharTraits;
    const Char eof = CharTraits::to_char_type( CharTraits::eof() );
    for( Char ch = _textBuffer->sgetc(); eof != ch; ch = _textBuffer->snextc() )
    {
        switch(state)
        {
            case BeforeName:
            {
               if( ch == '>' || ch == '/')
               {
                   return false;
               }
               else if( isspace(ch) )
               {
                   continue;
               }
               else
               {
                   name += ch;
                   state = OnName;
               }

               break;
            }

            case OnName:
            {
               if( isspace(ch) )
               {
                   state = BeforeEqual;
               }
               else if( ch == '=')
               {
                   state = BeforeValue;
               }
               else
               {
                   name += ch;
               }

               break;
            }

            case BeforeEqual:
            {
               if( isspace(ch) )
               {
                   continue;
               }
               else if( ch == '=')
               {
                   state = BeforeValue;
               }
               else
                   throw  std::runtime_error("Invalid XML attribute" + PT_SOURCEINFO);

               break;
            }

            case BeforeValue:
            {
               if( isspace(ch) )
               {
                   continue;
               }
               else if( ch == '\'' || ch == '"')
               {
                   state = OnValue;
               }
               else
               {
                   throw  std::runtime_error("Invalid XML attribute" + PT_SOURCEINFO);
                }

               break;
            }

            case OnValue:
            {
               if( ch == '\'' || ch == '"')
               {
                   state = AfterValue;
               }
               else
                   value += ch;

               break;
            }

            case AfterValue:
            {
               if( isspace(ch) )
               {
                   continue;
               }
               else
               {
                   return true;
               }

               break;
            }

            default:
                throw  std::runtime_error("Invalid XML attribute" + PT_SOURCEINFO);
        }
    }

    return true;
}*/











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
