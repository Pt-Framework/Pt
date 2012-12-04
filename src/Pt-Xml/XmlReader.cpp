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
#include <Pt/Xml/EndDocument.h>
#include "Pt/Xml/EntityResolver.h"
#include <Pt/Xml/DocTypeDeclaration.h>
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/ProcessingInstruction.h"
#include "Pt/Xml/Comment.h"
#include "Pt/Xml/XmlError.h"
#include "Pt/TextStream.h"
#include "Pt/Utf8Codec.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>
#include <iostream>

#define log_define(a)
#define log_debug(a)
#define log_info(a)
#define log_warn(a)
#define log_error(a)

log_define("pt.xml.reader")

namespace Pt {

namespace Xml {

class XmlReaderImpl
{
    private:
        void onDocumentBegin(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                _parse = &XmlReaderImpl::onProlog;
            }
            else if( ch == '<')
            {
                _parse = &XmlReaderImpl::onXmlDecl;
            }
            else
            {
                throw XmlError("XML syntax error", _line);
            }
        }

        void onXmlDecl(int c)
        {
            Char ch = notEof(c);

            if(ch == '?')
            {
                _parse = &XmlReaderImpl::onXmlDeclQMark;
                return;
            }

            if(ch == '!')
            {
                _parse = &XmlReaderImpl::onTagExclam;
                return;
            }

            if( isAlpha(ch) )
            {
                _startElem.clear();
                _startElem.name() += ch;
                _parse = &XmlReaderImpl::onStartElement;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onXmlDeclQMark(int c)
        {
            Char ch = notEof(c);
 
            if( isAlpha(ch) )
            {
                _procInstr.clear();
                _procInstr.target() += ch;
                _parse = &XmlReaderImpl::onXmlDeclName;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onXmlDeclName(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                if( _procInstr.target() == L"xml" )
                    _parse =  &XmlReaderImpl::onXmlDeclBeforeAttr;
                else
                    _parse =  &XmlReaderImpl::onProcessingInstructionData;

                return;
            }

            if( isAlpha(ch) || ch == ':' )
            {
                _procInstr.target() += ch;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onXmlDeclBeforeAttr(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( isAlpha(ch) )
            {
                _attr.clear();
                _attr.name() += c;
                _parse =  &XmlReaderImpl::onXmlDeclAttr;
                return;
            }

            if(ch == '?')
            {
                _parse =  &XmlReaderImpl::onXmlDeclEnd;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onXmlDeclAttr(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                _parse =  &XmlReaderImpl::onXmlDeclAfterName;
                return;
            }

            if(ch == '=')
            {
                _parse =  &XmlReaderImpl::onXmlDeclBeforeValue;
                return;
            }

            if( isAlpha(ch) )
            {
                _attr.name() += ch;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onXmlDeclAfterName(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch == '=')
            {
                _parse =  &XmlReaderImpl::onXmlDeclBeforeValue;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onXmlDeclBeforeValue(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( isQoute(ch) )
            {
                _parse =  &XmlReaderImpl::onXmlDeclValue;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onXmlDeclValue(int c)
        {
            Char ch = notEof(c);

            if( isQoute(ch) )
            {
                if(_attr.name() == L"version")
                {
                    _version = _attr.value();
                }
                else if(_attr.name() == L"encoding")
                {
                    _encoding = _attr.value();
                }
                else if(_attr.name() == L"standalone")
                {
                    if(_attr.value() == L"true")
                        _standalone = true;
                }

                _parse =  &XmlReaderImpl::onXmlDeclBeforeAttr;
                return;
            }

            if( isAlpha(ch) )
            {
                _attr.value() += c;;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onXmlDeclEnd(int c)
        {
            Char ch = notEof(c);

            if(ch == '>')
            {
                _parse =  &XmlReaderImpl::onProlog;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onProcessingInstruction(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                _parse = &XmlReaderImpl::onProcessingInstructionData;
                return;
            }

            if( isAlpha(ch) )
            {
                _procInstr.target() += c;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onProcessingInstructionData(int c)
        {
            Char ch = notEof(c);

            if(Pt::isspace(ch) || isAlpha(ch) || isQoute(ch) || 
               ch == ':' || ch == '/' || ch == '!' || ch == '=')
            {
                _procInstr.data() += c;
                return;
            }

            if(ch == '?')
            {
                _parse = &XmlReaderImpl::onProcessingInstructionEnd;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onProcessingInstructionEnd(int c)
        {
            Char ch = notEof(c);

            if(ch == '>')
            {
                _current = &(_procInstr);
                _parse = &XmlReaderImpl::afterTag;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onTag(int c)
        {
            Char ch = notEof(c);
            
            if(ch == '?')
            {
                _procInstr.clear();
                _parse = &XmlReaderImpl::onProcessingInstruction;
                return;
            }

            if(ch == '!')
            {
                _parse = &XmlReaderImpl::onTagExclam;
                return;
            }

            if(ch == '/')
            {
                if( _chars.content().length() )
                {
                    _current = &(_chars);
                }

                _endElem.clear();
                _parse = &XmlReaderImpl::onEndElement;
                return;
            }

            if( isAlpha(ch) )
            {
                if( _chars.content().length() )
                {
                    _current = &(_chars);
                }

                _startElem.clear();
                _startElem.name() += ch;
                _parse = &XmlReaderImpl::onStartElement;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onTagExclam(int c)
        {
            Char ch = notEof(c);

            if(ch == '-')
            {
                _parse = &XmlReaderImpl::beforeComment;
                return;
            }

            if(ch == '[' && depth() > 0)
            {
                _token.clear();
                _token += ch;
                _parse = &XmlReaderImpl::beforeCData;
                return;
            }

            if(ch == 'D' && depth() == 0)
            {
                _docType.clear();
                _docType.content() += ch;
                _parse = &XmlReaderImpl::beforeDocType;
                return;
            }

            throw XmlError("XML syntax error", _line);
        };

        void beforeDocType(int c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) )
            {
                String& token = _docType.content();
                token += c;

                if(token.length() < 7)
                    return;

                if(token == L"DOCTYPE")
                {
                    _parse = &XmlReaderImpl::onDocType;
                    return;
                }
            }

            throw XmlError("XML syntax error", _line);
        }

        void onDocType(int c)
        {
            Char ch = notEof(c);

            if(ch == '<')
                throw XmlError("XML syntax error", _line);

            if(ch == '>')
            {
                _current = &(_docType);
                _parse = &XmlReaderImpl::onProlog;
                return;
            }

            _docType.content() += c;
        };

        void afterTag(int c)
        {
            if( c == std::char_traits<Char>::eof() )
            {
                if( depth() > 0 )
                    throw XmlError("XML syntax error", _line);

                _current = &( _endDoc );
                return;
            }

            Char ch(c);

            if( Pt::isspace(ch) )
            {
                if( depth() == 0 )
                {
                    _parse = &XmlReaderImpl::onProlog;
                    return;
                }

                _chars.content() += c;
                _parse = &XmlReaderImpl::onCharacters;
                return;
            }

            if(ch == '<')
            {
                _parse = &XmlReaderImpl::onTag;
                return;
            }

            if(ch == '>')
            {
                throw XmlError("XML syntax error", _line);
            }

            if(ch == '&')
            {
                _token.clear();
                _parse = &XmlReaderImpl::onEntityReference;
                return;
            }

            appendContent(c);
        }

        void beforeComment(int c)
        {
            if(c == '-')
            {
                _parse = &XmlReaderImpl::onComment;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onComment(int c)
        {
            Char ch = notEof(c);
            
            if(ch == '-')
            {
                _parse = &XmlReaderImpl::afterComment;
                return;
            }
        }

        void afterComment(int c)
        {
            Char ch = notEof(c);

            if(ch == '-')
            {
                _parse = &XmlReaderImpl::onCommentEnd;
                return;
            }

            _parse = &XmlReaderImpl::onComment;
        }

        void onCommentEnd(int c)
        {
            Char ch = notEof(c);
            
            if(ch == '>')
            {
                if(depth() == 0)
                {
                    _parse = &XmlReaderImpl::onProlog;
                    return;
                }

                _parse = &XmlReaderImpl::afterTag;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onStartElement(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                _parse = &XmlReaderImpl::beforeAttribute;
                return;
            }

            if(ch == '/')
            {
                _chars.clear();
                _current = &(_startElem);
                _depth++;

                _parse = &XmlReaderImpl::onEmptyElement;
                return;
            }

            if(ch == ':')
            {
                _startElem.name() += c;
                return;
            }

            if( isAlpha(ch) )
            {
                _startElem.name() += c;
                return;
            }

            if(ch == '>')
            {
                _chars.clear();
                _current = &(_startElem);
                _depth++;

                _parse = &XmlReaderImpl::afterTag;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void beforeAttribute(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch == '/')
            {
                _current = &(_startElem);
                _depth++;

                _parse = &XmlReaderImpl::onEmptyElement;
                return;
            }

            if( isAlpha(ch) )
            {
                _attr.clear();
                _attr.name() += c;

                _parse = &XmlReaderImpl::onAttributeName;
                return;
            }

            if(ch == '>')
            {
                _chars.clear();
                _current = &(_startElem);
                _depth++;

                _parse = &XmlReaderImpl::afterTag;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onAttributeName(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                _parse = &XmlReaderImpl::afterAttributeName;
                return;
            }

            if(ch == '=')
            {
                _parse = &XmlReaderImpl::beforeAttributeValue;
                return;
            }

            if( isAlpha(ch) )
            {
                _attr.name() += c;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void afterAttributeName(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch == '=')
            {
                _parse = &XmlReaderImpl::beforeAttributeValue;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void beforeAttributeValue(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if( isQoute(ch) )
            {
                _parse = &XmlReaderImpl::onAttributeValue;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onAttributeValue(int c)
        {
            Char ch = notEof(c);

            if( isQoute(ch) )
            {
                _startElem.addAttribute(_attr);
                _parse = &XmlReaderImpl::beforeAttribute;
                return;
            }

            if (ch == '&')
            {
                _token.clear();
                _parse = &XmlReaderImpl::onAttributeEntityReference;
                return;
            }

            _attr.value() += c;
        }

        void onAttributeEntityReference(int c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                _token += ch;
                return;
            }
            
            if(ch == ';')
            {
                resolveEntity(_token);
                _attr.value() += _token;
                _token.clear();

                _parse = &XmlReaderImpl::onAttributeValue;
                return;
            }
            
            throw XmlError("XML syntax error", _line);
        };

        void onEmptyElement(int c)
        {
            Char ch = notEof(c);

            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch == '>')
            {
                _endElem.name() = _startElem.name();
                _current = &(_endElem);
                _depth--;

                if(depth() == 0)
                {
                    _parse = &XmlReaderImpl::onEpilog;
                    return;
                }

                _parse = &XmlReaderImpl::afterTag;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onEndElement(int c)
        {
            Char ch = notEof(c);
            
            if( isAlpha(ch) )
            {
                _endElem.name() += c;
                _parse = &XmlReaderImpl::onEndElementName;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onEndElementName(int c)
        {
            Char ch = notEof(c);
            
            if( Pt::isspace(ch) )
            {
                _parse = &XmlReaderImpl::afterEndElementName;
                return;
            }

            if( isAlpha(ch) )
            {
                _endElem.name() += c;
                return;
            }

            if(ch == ':')
            {
                _endElem.name() += c;
                return;
            }

            if(ch == '>')
            {
                _chars.clear();
                _current = &(_endElem);
                _depth--;

                if(depth() == 0)
                {
                    _parse = &XmlReaderImpl::onEpilog;
                    return;
                }

                _parse = &XmlReaderImpl::afterTag;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }
    
        void afterEndElementName(int c)
        {
            Char ch = notEof(c);
            
            if( Pt::isspace(ch) )
            {
                return;
            }

            if(ch == '>')
            {
                _chars.clear();
                _current = &(_endElem);
                _depth--;

                if(depth() == 0)
                {
                    _parse = &XmlReaderImpl::onEpilog;
                    return;
                }

                _parse = &XmlReaderImpl::afterTag;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

        void onCharacters(int c)
        {
            Char ch = notEof(c);

            if(ch == '<')
            {
                _parse = &XmlReaderImpl::onTag;
                return;
            }

            if(ch == '>')
            {
                throw XmlError("XML syntax error", _line);
            }

            if(ch == '&')
            {
                _token.clear();
                _parse = &XmlReaderImpl::onEntityReference;
                return;
            }

            appendContent(c);
        }

        void onEntityReference(int c)
        {
            Char ch = notEof(c);

            if( isAlpha(ch) || ch == '#')
            {
                _token += ch;
                return;
            }

            if(ch == ';')
            {
                resolveEntity(_token);

                _chars.content() += _token;
                _token.clear();

                _parse = &XmlReaderImpl::onCharacters;
                return;
            }

            throw XmlError("invalid entity format", _line);
        };

        void beforeCData(int c)
        {
            Char ch = notEof(c);

            switch( ch.value() )
            {
                case '[':
                case 'C':
                case 'D':
                case 'A':
                case 'T':
                    _token += ch;
                    break;

                default:
                    throw XmlError("XML syntax error", _line);
            }
            
            if( _token.length() < 7 )
                return;

            if( _token == L"[CDATA[" )
            {
                _token.clear();
                _parse = &XmlReaderImpl::onCData;
                return;
            }

            throw XmlError("XML syntax error", _line);
        };

        void onCData(int c)
        {
            Char ch = notEof(c);

            if(ch == '>')
            {
                const String& content = _chars.content();
                unsigned len = content.length();

                if( len > 2 && content[len-2] == ']' && content[len-2] == ']')
                {
                    _chars.content().resize(len-2);

                    _parse = &XmlReaderImpl::afterTag;
                    return;
                }

                appendContent(c);
                return;
            }

            appendContent(c);
        }

        // not neccessary, allow EOF only when depth == 0 in other states

        // join onProlog and onEpilog -> EOF only allowed when root element was found
        void onEpilog(int c)
        {
            if( c == std::char_traits<Char>::eof() )
            {
                _current = &_endDoc;
                return;
            }

            Char ch(c);
            if( Pt::isspace(ch) )
            {
                return;
            }

            if (ch == '<' )
            {
                _parse = &XmlReaderImpl::onTag;
                return;
            }

            throw XmlError("XML syntax error", _line);
        };

        void onProlog(int c)
        {
            if( c == std::char_traits<Char>::eof() )
            {
                _current = &_endDoc;
                return;
            }

            Char ch(c);
            if( Pt::isspace(ch) )
            {
                return;
            }

            if( ch == '<')
            {
                _parse = &XmlReaderImpl::onTag;
                return;
            }

            throw XmlError("XML syntax error", _line);
        }

    private:
        Char notEof(int c) const
        {
            if( c == std::char_traits<Char>::eof() )
            {
                throw XmlError("XML syntax error", _line);
            }

            return Char(c);
        }

        bool isQoute(Char ch) const
        {
            return ch == '\'' || ch =='"';
        }

        // TODO: rename to isXmlName()
        bool isAlpha(Char ch)
        {
            return ch == '.' || ch == '_' || ch == '-' || Pt::isalnum(ch) != 0;
        }

        void resolveEntity(String& str)
        {
            if( ! _resolver.resolveEntity( str ) )
                throw XmlError("invalid entity reference", line());
        }

        void appendContent(Pt::Char c)
        {
            String& content = _chars.content();
            if (content.capacity() <= content.size() + 20)
            {
                if (content.capacity() < 16)
                    content.reserve(16);
                else
                    content.reserve(content.capacity() + content.capacity() / 2);
            }
            content += c;
        }

    public:
        XmlReaderImpl(std::basic_istream<Char>& is, int flags)
        : _textBuffer( is.rdbuf() )
        , _buffer(0)
        , _flags(flags)
        , _standalone(true)
        , _depth(0)
        , _line(1)
        , _parse(0)
        , _current(0)
        {
            _parse = &XmlReaderImpl::onDocumentBegin;
        }

        XmlReaderImpl(std::istream& is, int flags)
        : _textBuffer(0)
        , _buffer(0)
        , _flags(flags)
        , _standalone(true)
        , _depth(0)
        , _line(1)
        , _parse(0)
        , _current(0)
        {
            _parse = &XmlReaderImpl::onDocumentBegin;

            _buffer = new TextBuffer( &is, new Pt::Utf8Codec() );
            _textBuffer = _buffer;
        }

        ~XmlReaderImpl()
        {
            delete _buffer;
        }

        void attach(std::basic_istream<Char>& is, int flags)
        {
            delete _buffer;
            _buffer = 0;
            _textBuffer = is.rdbuf();

            _parse = &XmlReaderImpl::onDocumentBegin;

            _flags = flags;
            _docType.clear();
            _version.clear();
            _encoding.clear();
            _standalone = true;
            _depth = 0;
            _line = 1;
            _current = 0;
        }

        void attach(std::istream& is, int flags)
        {
            delete _buffer;
            _buffer = new TextBuffer( &is, new Pt::Utf8Codec() );
            _textBuffer = _buffer;

            _parse = &XmlReaderImpl::onDocumentBegin;

            _flags = flags;
            _docType.clear();
            _version.clear();
            _encoding.clear();
            _standalone = true;
            _depth = 0;
            _line = 1;
            _current = 0;
        }

        const Pt::String& version() const
        { return _version; }

        const Pt::String& encoding() const
        { return _encoding; }

        bool isStandalone() const
        { return _standalone; }

        EntityResolver& entityResolver()
        { return _resolver; }

        size_t depth() const
        {
            return _depth;
        }

        std::size_t line() const
        {
            return _line;
        }

        const Node& get()
        {
            if( ! _current )
            {
                this->next();
            }

            return *_current;
        }

        const Node& next()
        {
            std::char_traits<char>::int_type eof = std::char_traits<char>::eof();

            _current = 0;
            int c = 0;
            do
            {
                c = _textBuffer->sbumpc();
                (this->*_parse)(c);

                if(c == '\n')
                {
                    ++_line;
                }
            }
            while ( !_current && c != eof);

            return *_current;
        }

        bool advance()
        {
            std::char_traits<char>::int_type eof = std::char_traits<char>::eof();

            _current = 0;
            int c = 0;
            while( ! _current && _textBuffer->in_avail() > 0 )
            {
                c = _textBuffer->sbumpc();
                (this->*_parse)(c);

                if(c == '\n')
                {
                    ++_line;
                }
            }

            return _current != 0;
        }

    private:
        std::basic_streambuf<Char>* _textBuffer;
        std::basic_streambuf<Char>* _buffer;
        int _flags;
        EntityResolver _resolver;

        Pt::String _version;
        Pt::String _encoding;
        bool _standalone;
        size_t _depth;
        std::size_t _line;

        typedef void (XmlReaderImpl::*ParseFunc)(int);
        ParseFunc _parse;
        Node* _current;
        String _token;

        // TODO: some sort of union?
        DocTypeDeclaration _docType;
        ProcessingInstruction _procInstr;
        StartElement _startElem;
        EndElement _endElem;
        Characters _chars;
        Attribute _attr;
        EndDocument _endDoc;
};


XmlReader::XmlReader(std::istream& is, int flags)
: _impl(0)
{
    _impl = new XmlReaderImpl(is, flags);
}


XmlReader::XmlReader(std::basic_istream<Char>& is, int flags)
: _impl(0)
{
    _impl = new XmlReaderImpl(is, flags);
}


XmlReader::~XmlReader()
{
    delete _impl;
}


void XmlReader::attach(std::basic_istream<Char>& is, int flags)
{
    _impl->attach(is, flags);
}


void XmlReader::attach(std::istream& is, int flags)
{
    _impl->attach(is, flags);
}


const Pt::String& XmlReader::version() const
{
    return _impl->version();
}


const Pt::String& XmlReader::encoding() const
{
    return _impl->encoding();
}


bool XmlReader::isStandalone() const
{
    return _impl->isStandalone();
}


EntityResolver& XmlReader::entityResolver()
{
    return _impl->entityResolver();
}


const EntityResolver& XmlReader::entityResolver() const
{
    return _impl->entityResolver();
}


size_t XmlReader::depth() const
{
    return _impl->depth();
}


std::size_t XmlReader::line() const
{
    return _impl->line();
}


const Node& XmlReader::get()
{
    return _impl->get();
}


const Node& XmlReader::next()
{
    return _impl->next();
}


bool XmlReader::advance()
{
    return _impl->advance();
}

} // namespace Xml

} // namespace Pt
