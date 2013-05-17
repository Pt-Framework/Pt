/*
 * Copyright (C) 2005-2013 Marc Boris Duerner
 *
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

#include "NamespaceContext.h"
#include <Pt/Xml/XmlWriter.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Utf8Codec.h>
#include <vector>
#include <cassert>

namespace Pt {

namespace Xml {

static const Pt::Char XML_QUOT[] = { '&', 'q', 'u', 'o', 't', ';' };
static const Pt::Char XML_AMP[]  = { '&', 'a', 'm', 'p', ';'};
static const Pt::Char XML_APOS[] = { '&', 'a', 'p', 'o', 's', ';' };
static const Pt::Char XML_LT[]   = { '&', 'l', 't', ';' };
static const Pt::Char XML_GT[]   = { '&', 'g', 't', ';' };


void xmlEncode(std::basic_ostream<Pt::Char>& os, const Pt::Char* str, std::size_t n)
{
    const Pt::Char* it = str;
    const Pt::Char* begin = str;
    const Pt::Char* end = begin + n;
    const Pt::Char* replace = 0;
    std::size_t replSize = 0;

    for( ; it != end; ++it)
    {
        switch( it->value() )
        {
            case 0x0022:
                replace = XML_QUOT;
                replSize = sizeof(XML_QUOT)/sizeof(Pt::Char);
                break;

            case 0x0026:
                replace = XML_AMP;
                replSize = sizeof(XML_AMP)/sizeof(Pt::Char);
                break;

            case 0x0027:
                replace = XML_APOS;
                replSize = sizeof(XML_APOS)/sizeof(Pt::Char);
                break;

            case 0x003C:
                replace = XML_LT;
                replSize = sizeof(XML_LT)/sizeof(Pt::Char);
                break;

            case 0x003E:
                replace = XML_GT;
                replSize = sizeof(XML_GT)/sizeof(Pt::Char);
                break;

            default:
                break;
        }

        if(replace)
        {
            if(it != begin)
                os.write(begin, it - begin);
            
            begin = it + 1;
            os.write(replace, replSize);
            replace = 0;
        }
    }

    if(it != begin)
        os.write(begin, it - begin);
}


void xmlEncode(std::basic_ostream<Pt::Char>& os, const Pt::Char* str)
{
    const Pt::Char* it = str;
    const Pt::Char* begin = str;
    const Pt::Char* replace = 0;
    std::size_t replSize = 0;

    for( ; *it != '\0'; ++it)
    {
        switch( it->value() )
        {
            case 0x0022:
                replace = XML_QUOT;
                replSize = sizeof(XML_QUOT)/sizeof(Pt::Char);
                break;

            case 0x0026:
                replace = XML_AMP;
                replSize = sizeof(XML_AMP)/sizeof(Pt::Char);
                break;

            case 0x0027:
                replace = XML_APOS;
                replSize = sizeof(XML_APOS)/sizeof(Pt::Char);
                break;

            case 0x003C:
                replace = XML_LT;
                replSize = sizeof(XML_LT)/sizeof(Pt::Char);
                break;

            case 0x003E:
                replace = XML_GT;
                replSize = sizeof(XML_GT)/sizeof(Pt::Char);
                break;

            default:
                break;
        }

        if(replace)
        {
            if(it != begin)
                os.write(begin, it - begin);
            
            begin = it + 1;
            os.write(replace, replSize);
            replace = 0;
        }
    }

    if(it != begin)
        os.write(begin, it - begin);
}


class XmlWriterImpl
{
    public:
        enum State
        {
            OnBegin = 0,
            OnStartElement = 1,
            OnStartElementOpen = 2,
            OnCharacters = 3,
            OnTag = 4
        };

    public:
        XmlWriterImpl()
        : _tos(0)
        , _formatting(true)
        , _indent(L"  ")
        , _quote('"')
        , _state(OnBegin)
        , _defaultNamespace(false)
        { }

        XmlWriterImpl(std::basic_ostream<Char>& tos)
        : _tos(&tos)
        , _formatting(true)
        , _indent(L"  ")
        , _quote('"')
        , _state(OnBegin)
        , _defaultNamespace(false)
        { }

        bool isFormatting() const
        { return _formatting; }

        void setFormatting(bool value)
        { _formatting = value; }

        const Pt::String& indent() const
        { return _indent; }

        void setIndent(const Pt::String& indent)
        { _indent = indent; }

        Pt::Char quote() const
        { return _quote; }

        void setQuote(Pt::Char ch)
        { _quote = ch; }

        void reset()
        {
            _tos = 0;
            _state = OnBegin;
            _defaultNamespace = false;
            
            _nsctx.clear();
            _namespaces.clear();
            _elements.clear();
        }

        void reset(std::basic_ostream<Char>& tos)
        {
            reset();
            _tos = &tos;
        }

        std::basic_ostream<Char>* output()
        { return _tos; }

        std::size_t depth() const
        { return _elements.size(); }

        void setDefaultNamespace(const Pt::String& ns)
        {
            _nsctx.pushDefaultNamespace(_elements.size() + 1, ns);
            _defaultNamespace = true;
        }

        void setNamespacePrefix(const String& prefix, const String& ns)
        {
            _nsctx.pushNamespace(_elements.size() + 1, prefix, ns);
            _namespaces.push_back(prefix);
        }

        void writeStartDocument(const Pt::Char* version, std::size_t versionSize,
                                const Pt::Char* encoding, std::size_t encodingSize, bool standalone)
        {
            if( ! _tos)
                return;

            static const Pt::Char declvers[] = { '<', '?', 'x', 'm', 'l', ' ', 'v', 
                'e', 'r', 's', 'i', 'o', 'n', '=', '"' };

            static const Pt::Char declenc[] = { '"', ' ', 
            'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g', '=', '"' };

            static const Pt::Char declstandalone[] = { '"', ' ', 
            's', 't', 'a', 'n', 'd', 'a', 'l', 'o', 'n', 'e', '=', '"', 'y', 'e', 's' };

            static const Pt::Char declend[] = { '"', '?', '>' };

            _tos->write(declvers, sizeof(declvers)/sizeof(Pt::Char));
            _tos->write(version, versionSize);

            _tos->write(declenc, sizeof(declenc)/sizeof(Pt::Char));
            _tos->write(encoding, encodingSize);

            if(standalone)
            {
              _tos->write(declstandalone, sizeof(declstandalone)/sizeof(Pt::Char));
            }

            _tos->write(declend, sizeof(declend)/sizeof(Pt::Char));

            if( _formatting )
                *_tos << Pt::Char('\n');
        }

        void writeEndDocument()
        {
            while( _elements.size() )
            {
                writeEndElement();
            }
        }

        void writeDocType(const Pt::Char* dtd, std::size_t n)
        {
            if( ! _tos)
                return;

            Pt::Char docType[] = {'<', '!', 'D', 'O', 'C', 'T', 'Y', 'P', 'E', ' '};

            _tos->write(docType, sizeof(docType)/sizeof(Pt::Char));
            _tos->write(dtd, n);
            *_tos << Char('>');

            if( _formatting )
                *_tos << Pt::Char('\n');
        }

        void writeStartElement(const Pt::Char* localName, std::size_t localNameSize)
        {
            if( ! _tos)
                return;

            if(_state == OnStartElementOpen)
            {
                *_tos << Pt::Char('>');
            }

            if((_state == OnStartElementOpen) || (_state == OnStartElement) || (_state == OnTag))
            {
                formatIndent(*_tos, _elements.size());
            }

            _state = OnStartElementOpen;

            *_tos << Pt::Char('<');

            _tos->write(localName, localNameSize);

            std::vector<Pt::String>::iterator it;
            for(it = _namespaces.begin(); it != _namespaces.end(); ++it)
            {
                const Namespace* n = _nsctx.findPrefix(*it);
                assert(n);

                *_tos << Pt::String(L" xmlns:") << n->prefix() << Pt::Char('=') 
                      << _quote << n->namespaceUri() << _quote;
            }

            _namespaces.clear();

            if( _defaultNamespace )
            {
                const Namespace& n = _nsctx.getDefaultNamespace();
                *_tos << Pt::String(L" xmlns") << Pt::Char('=') << _quote << n.namespaceUri() << _quote;
                _defaultNamespace = false;
            }

            _elements.push_back( Pt::String(localName, localNameSize) );
        }

        void writeStartElement(const Char* ns, std::size_t nsSize,
                               const Char* localName, std::size_t localNameSize)
        {
            if( ! _tos)
                return;


            if(_state == OnStartElementOpen)
            {
                *_tos << Pt::Char('>');
            }

            if((_state == OnStartElementOpen) || (_state == OnStartElement) || (_state == OnTag))
            {
                formatIndent(*_tos, _elements.size());
            }

            _state = OnStartElementOpen;

            const Namespace* nsdecl = _nsctx.findUri(ns, nsSize);
            if( ! nsdecl)
                throw XmlError("undeclared namespace");

            *_tos << Pt::Char('<');

            if( ! nsdecl->isDefaultNamespace() )
            {
                *_tos << nsdecl->prefix() << Pt::Char(':');
            }

            _tos->write(localName, localNameSize);

            std::vector<Pt::String>::iterator it;
            for(it = _namespaces.begin(); it != _namespaces.end(); ++it)
            {
                const Namespace* n = _nsctx.findPrefix(*it);
                assert(n);

                *_tos << Pt::String(L" xmlns:") << n->prefix() << Pt::Char('=') 
                      << _quote << n->namespaceUri() << _quote;
              }

            _namespaces.clear();

            if( _defaultNamespace )
            {
                const Namespace& n = _nsctx.getDefaultNamespace();
                *_tos << Pt::String(L" xmlns") << Pt::Char('=') << _quote << n.namespaceUri() << _quote;
                _defaultNamespace = false;
            }

            if( ! nsdecl->isDefaultNamespace() )
            {
                _elements.push_back( nsdecl->prefix() );
                _elements.back() += Char(':');
                _elements.back().append(localName, localNameSize);
            }
            else
            {
                _elements.push_back( Pt::String(localName, localNameSize) );
            }
        }

        void writeAttribute(const Char* localName, std::size_t localNameSize,
                            const Char* value, std::size_t valueSize)
        {
            if( ! _tos || ! _state == OnStartElement)
                return;

            *_tos << Pt::Char(' ');

            _tos->write(localName, localNameSize) << Pt::Char('=') << _quote;
            xmlEncode(*_tos, value, valueSize);
            *_tos << _quote;
        }

        void writeAttribute(const Char* ns, std::size_t nsSize,
                            const Char* localName, std::size_t localNameSize,
                            const Char* value, std::size_t valueSize)
        {
            if( ! _tos || ! _state == OnStartElement)
                return;

            const Namespace* nsdecl = _nsctx.findUri(ns, nsSize);
            if( ! nsdecl)
                throw XmlError("undeclared namespace");

            *_tos << Pt::Char(' ');

            if( ! nsdecl->isDefaultNamespace() )
                *_tos << nsdecl->prefix() << Pt::Char(':');

            _tos->write(localName, localNameSize) << Pt::Char('=') << _quote;
            xmlEncode(*_tos, value, valueSize);
            *_tos << _quote;
        }

        void writeEmptyElement(const Pt::Char* localName, std::size_t localNameSize)
        {
            writeStartElement(localName, localNameSize);
            writeEndElement();
        }
        
        void writeEmptyElement(const Char* ns, std::size_t nsSize,
                               const Char* localName, std::size_t localNameSize)
        {
            writeStartElement(ns, nsSize, localName, localNameSize);
            writeEndElement();
        }
        
        void writeEndElement()
        {
            if( ! _tos || _elements.empty() )
                return;

            if(_state == OnStartElementOpen)
            {
                *_tos << Pt::Char('/') << Pt::Char('>');
            }
            
            if(_state == OnTag)
            {
                formatIndent(*_tos, _elements.size() - 1);
            }

            if(_state != OnStartElementOpen)
                *_tos << Pt::Char('<') << Pt::Char('/') << _elements.back() << Pt::Char('>');

            _state = OnTag;

            std::size_t depth = _elements.size();
            _nsctx.popNamespace(depth);

            _elements.pop_back();
        }

        void writeCharacters(const Pt::Char* text, std::size_t n)
        {
            if( ! _tos)
                return;

            if(_state == OnStartElementOpen)
            {
                *_tos << Pt::Char('>');
            }

            _state = OnCharacters;

            xmlEncode(*_tos, text, n);
        }

        void writeCData(const Pt::Char* text, std::size_t n)
        {
            if( ! _tos)
                return;

            if(_state == OnStartElementOpen)
            {
                *_tos << Pt::Char('>');
            }

            _state = OnCharacters;

            Pt::Char cdataBegin[] = {'<', '!', '[', 'C', 'D', 'A', 'T', 'A', '['};
            Pt::Char cdataEnd[] = {']', ']', '>'};
            
            _tos->write(cdataBegin, sizeof(cdataBegin)/sizeof(Char));
            _tos->write(text, n);
            _tos->write(cdataEnd, sizeof(cdataEnd)/sizeof(Char));
        }
        
        void writeEntityReference(const Pt::Char* name, std::size_t n)
        {
            if( ! _tos)
                return;

            if(_state == OnStartElementOpen)
            {
                *_tos << Pt::Char('>');
            }

            _state = OnCharacters;
            
            *_tos << Char('&');
            _tos->write(name, n);
            *_tos << Char(';');
        }

        void writeComment(const Pt::Char* text, std::size_t n)
        {
            if( ! _tos)
                return;

            if(_state == OnStartElementOpen)
            {
                *_tos << Pt::Char('>');
                _state = OnStartElement;
            }

            if((_state == OnStartElement) || (_state == OnTag))
            {
                formatIndent(*_tos, depth());
            }

            *_tos << Pt::Char('<') << Pt::Char('!') << Pt::Char('-') << Pt::Char('-');
            _tos->write(text, n);
            *_tos << Pt::Char('-') << Pt::Char('-') << Pt::Char('>');
        }

        void writeProcessingInstruction(const Pt::Char* target, std::size_t targetSize,
                                        const Pt::Char* data, std::size_t dataSize)
        {
            if( ! _tos)
                return;

            if(_state == OnStartElementOpen)
            {
                *_tos << Pt::Char('>');
                _state = OnStartElement;
            }

            if((_state == OnStartElement) || (_state == OnTag))
            {
                formatIndent(*_tos, depth());
            }

            *_tos << Pt::Char('<') << Pt::Char('?');
            _tos->write(target, targetSize);
            *_tos << Pt::Char(' ');
            _tos->write(data, dataSize);
            *_tos << Pt::Char('?') << Pt::Char('>');
        }

        void writeStartTag(const Pt::Char* name)
        {
            if( ! _tos)
                return;

            *_tos << Pt::Char('<') << name;
            *_tos << Pt::Char('>');
        }

        void writeEndTag(const Pt::Char* name)
        {
            if( ! _tos)
                return;

            *_tos << Pt::Char('<') << Pt::Char('/') << name << Pt::Char('>');
        }

    private:
        void formatIndent(std::basic_ostream<Char>& tos, std::size_t width)
        {
            if( _formatting )
            {
                tos << std::endl;

                for(size_t n = 0; n < width; ++n)
                {
                    tos << _indent;
                }
            }
        }

    private:
        std::basic_ostream<Char>* _tos;
        bool _formatting;
        Pt::String _indent;
        Pt::Char _quote;
        State _state;
        std::vector<Pt::String> _elements;
        NamespaceContext _nsctx;
        std::vector<Pt::String> _namespaces;
        bool _defaultNamespace;
};



XmlWriter::XmlWriter()
: _impl(0)
{
    _impl = new XmlWriterImpl();
}


XmlWriter::XmlWriter(std::basic_ostream<Char>& os)
: _impl(0)
{
    _impl = new XmlWriterImpl(os);
}


XmlWriter::~XmlWriter()
{
    delete _impl;
}


bool XmlWriter::isFormatting() const
{
    return _impl->isFormatting();
}


void XmlWriter::setFormatting(bool value)
{
    _impl->setFormatting(value);
}


const Pt::String& XmlWriter::indent() const
{
    return _impl->indent();
}


void XmlWriter::setIndent(const Pt::String& indent)
{
    _impl->setIndent(indent);
}


Pt::Char XmlWriter::quote() const
{
    return _impl->quote();
}


void XmlWriter::setQuote(Pt::Char ch)
{
    _impl->setQuote(ch);
}


void XmlWriter::reset()
{
    _impl->reset();
}


void XmlWriter::reset(std::basic_ostream<Char>& os)
{
    _impl->reset(os);
}


std::basic_ostream<Char>* XmlWriter::output()
{
    return _impl->output();
}


std::size_t XmlWriter::depth() const
{
    return _impl->depth();
}


void XmlWriter::writeStartDocument(const Pt::Char* version, std::size_t versionSize,
                                   const Pt::Char* encoding, std::size_t encodingSize, bool standalone)
{
    _impl->writeStartDocument(version, versionSize, encoding, encodingSize, standalone);
}


void XmlWriter::writeEndDocument()
{
    _impl->writeEndDocument();
}


void XmlWriter::writeDocType(const Pt::Char* dtd, std::size_t n)
{
    _impl->writeDocType(dtd, n);
}


void XmlWriter::setDefaultNamespace(const Pt::String& ns)
{
    _impl->setDefaultNamespace(ns);
}


void XmlWriter::setNamespacePrefix(const Pt::String& prefix, const Pt::String& ns)
{
    _impl->setNamespacePrefix(prefix, ns);
}


void XmlWriter::writeStartElement(const Pt::Char* localName, std::size_t localNameSize)
{
    _impl->writeStartElement(localName, localNameSize);
}


void XmlWriter::writeStartElement(const Char* ns, std::size_t nsSize,
                                  const Char* localName, std::size_t localNameSize)
{
    _impl->writeStartElement(ns, nsSize, localName, localNameSize);
}


void XmlWriter::writeAttribute(const Char* localName, std::size_t localNameSize,
                               const Char* value, std::size_t valueSize)
{
    _impl->writeAttribute(localName, localNameSize, value, valueSize);
}


void XmlWriter::writeAttribute(const Char* ns, std::size_t nsSize,
                               const Char* localName, std::size_t localNameSize,
                               const Char* value, std::size_t valueSize)
{
    _impl->writeAttribute(ns, nsSize, localName, localNameSize, value, valueSize);
}


void XmlWriter::writeEmptyElement(const Pt::Char* localName, std::size_t localNameSize)
{
    _impl->writeEmptyElement(localName, localNameSize);
}


void XmlWriter::writeEmptyElement(const Char* ns, std::size_t nsSize,
                                  const Char* localName, std::size_t localNameSize)
{
    _impl->writeEmptyElement(ns, nsSize, localName, localNameSize);
}


void XmlWriter::writeEndElement()
{
    _impl->writeEndElement();
}


void XmlWriter::writeCharacters(const Pt::Char* text, std::size_t n)
{
    _impl->writeCharacters(text, n);
}


void XmlWriter::writeCData(const Pt::Char* text, std::size_t n)
{
    _impl->writeCData(text, n);
}


void XmlWriter::writeEntityReference(const Pt::Char* name, std::size_t n)
{
    _impl->writeEntityReference(name, n);
}


void XmlWriter::writeComment(const Pt::Char* text, std::size_t n)
{
    _impl->writeComment(text, n);
}


void XmlWriter::writeProcessingInstruction(const Pt::Char* target, std::size_t targetSize,
                                           const Pt::Char* data, std::size_t dataSize)
{
    _impl->writeProcessingInstruction(target, targetSize, data, dataSize);
}


void XmlWriter::writeStartTag(const Pt::Char* name)
{
    _impl->writeStartTag(name);
}


void XmlWriter::writeEndTag(const Pt::Char* name)
{
    _impl->writeEndTag(name);
}

} // namespace Xml

} // namespace Pt
