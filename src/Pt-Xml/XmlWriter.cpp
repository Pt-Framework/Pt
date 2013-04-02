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
#include <Pt/Xml/XmlWriter.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Utf8Codec.h>
#include <vector>

namespace {

static const Pt::Char XML_QUOT[] = { '&', 'q', 'u', 'o', 't', ';' };
static const Pt::Char XML_AMP[]  = { '&', 'a', 'm', 'p', ';'};
static const Pt::Char XML_APOS[] = { '&', 'a', 'p', 'o', 's', ';' };
static const Pt::Char XML_LT[]   = { '&', 'l', 't', ';' };
static const Pt::Char XML_GT[]   = { '&', 'g', 't', ';' };

void xmlEncode(std::basic_ostream<Pt::Char>& os, const Pt::Char* str)
{
    const Pt::Char* it = str;
    const Pt::Char* begin = str;

    for( ; *it != '\0'; ++it)
    {
        switch( it->value() )
        {
            case 0x0022:
                if(it != begin)
                    os.write(begin, it - begin);
                
                begin = it + 1;
                os.write(XML_QUOT, sizeof(XML_QUOT)/sizeof(Pt::Char));
                break;

            case 0x0026:
                if(it != begin)
                    os.write(begin, it - begin);
                
                begin = it + 1;
                os.write(XML_AMP, sizeof(XML_AMP)/sizeof(Pt::Char));
                break;

            case 0x0027:
                if(it != begin)
                    os.write(begin, it - begin);
                
                begin = it + 1;
                os.write(XML_APOS, sizeof(XML_APOS)/sizeof(Pt::Char));
                break;

            case 0x003C:
                if(it != begin)
                    os.write(begin, it - begin);
                
                begin = it + 1;
                os.write(XML_LT, sizeof(XML_LT)/sizeof(Pt::Char));
                break;

            case 0x003E:
                if(it != begin)
                    os.write(begin, it - begin);

                begin = it + 1;
                os.write(XML_GT, sizeof(XML_GT)/sizeof(Pt::Char));
                break;

            default:
                break;
        }
    }

    if(it != begin)
        os.write(begin, it - begin);
}

}


namespace Pt {

namespace Xml {

class XmlWriterImpl
{
    public:
        enum FormatFlags {
          UseXmlDeclaration = 1,
          UseIndent = 2,
          UseEndl = 4
        };

    public:
        XmlWriterImpl()
        : _tos(0)
        , _flags(UseXmlDeclaration | UseIndent | UseEndl)
        {
        }

        XmlWriterImpl(std::basic_ostream<Char>& tos)
        : _tos(0)
        , _flags(UseXmlDeclaration | UseIndent | UseEndl)
        {
            begin(tos);
        }

        void begin(std::basic_ostream<Char>& tos)
        {
            _elements.clear();
            _tos = &tos;
        }

        void writeStartDocument(const Pt::Char* version, const Pt::Char* encoding)
        {
            if( ! _tos)
                return;

            static const Pt::Char declbegin[] = { '<', '?', 'x', 'm', 'l', ' ', 'v', 
                'e', 'r', 's', 'i', 'o', 'n', '=', '"' };

            static const Pt::Char declmid[] = { '"', ' ', 
            'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g', '=', '"' };

            static const Pt::Char declend[] = { '"', '?', '>' };

            _tos->write(declbegin, sizeof(declbegin)/sizeof(Pt::Char));
            *_tos << version;

            _tos->write(declmid, sizeof(declmid)/sizeof(Pt::Char));
            *_tos << encoding;

            _tos->write(declend, sizeof(declend)/sizeof(Pt::Char));

            if( useEndl() )
                this->endl();
        }

        void writeStartTag(const Pt::Char* name)
        {
            if( ! _tos)
                return;

            *_tos << Pt::Char('<') << name;
            *_tos << Pt::Char('>');

            if( useEndl() )
                this->endl();
        }

        void writeEndTag(const Pt::Char* name)
        {
            if( ! _tos)
                return;

            *_tos << Pt::Char('<') << Pt::Char('/') << name << Pt::Char('>');

            if( useEndl() )
                this->endl();
        }

        void writeStartElement(const Pt::String& prefix, const Pt::String& localName, const Pt::String& ns)
        {
        }

        void writeStartElement(const Pt::Char* localName, const Attribute* attr, size_t attrCount)
        {
            this->writeStartElement(Pt::String(localName), attr, attrCount);
        }

        void writeStartElement(const Pt::String& localName, const Attribute* attr, size_t attrCount)
        {
            if( ! _tos)
                return;

            if (useIndent())
            {
                for(size_t n = 0; n < _elements.size(); ++n)
                {
                    *_tos << Pt::Char(' ') << Pt::Char(' ');
                }
            }

            *_tos << Pt::Char('<') << localName;

            for(size_t n = 0; n < attrCount; ++n)
            {
                *_tos << Pt::Char(' ') << attr[n].name() << Pt::Char('=') << Pt::Char('"');
                writeCharacters( attr[n].value() );
                *_tos << Pt::Char('"');
            }

            *_tos << Pt::Char('>');

            if (useEndl())
                this->endl();

            _elements.push_back(localName);
        }

        void writeEndElement()
        {
            if( ! _tos)
                return;

            if( _elements.empty() )
                return;

            if (useIndent())
            {
                for(size_t n = 1; n < _elements.size(); ++n)
                {
                    *_tos << Pt::Char(' ') << Pt::Char(' ');
                }
            }

            *_tos << Pt::Char('<') << Pt::Char('/') << _elements.back() << Pt::Char('>');

            if (useEndl())
                this->endl();

            _elements.pop_back();
        }

        void writeElement(const Pt::String& localName, const Pt::String& content, const Attribute* attr, size_t attrCount)
        {
            if( ! _tos)
                return;
            
            if( useIndent() )
            {
                for(size_t n = 0; n < _elements.size(); ++n)
                {
                    *_tos << Pt::Char(' ') << Pt::Char(' ');
                }
            }

            *_tos << Pt::Char('<') << localName;

            for(size_t n = 0; n < attrCount; ++n)
            {
                *_tos << Pt::Char(' ') << attr[n].name() << Pt::Char('=') << Pt::Char('"');
                writeCharacters( attr[n].value() );
                *_tos << Pt::Char('"');
            }

            *_tos << Pt::Char('>');

            this->writeCharacters(content);
            *_tos << Pt::Char('<') << Pt::Char('/') << localName << Pt::Char('>');

            if( useEndl() )
                this->endl();
        }

        void writeElement(const Pt::Char* localName, const Pt::Char* content, const Attribute* attr, size_t attrCount)
        {
            if( ! _tos)
                return;

            if( useIndent() )
            {
                for(size_t n = 0; n < _elements.size(); ++n)
                {
                    *_tos << Pt::Char(' ') << Pt::Char(' ');
                }
            }

            *_tos << Pt::Char('<') << localName;

            for(size_t n = 0; n < attrCount; ++n)
            {
                *_tos << Pt::Char(' ') << attr[n].name() << Pt::Char('=') << Pt::Char('"');
                writeCharacters( attr[n].value() );
                *_tos << Pt::Char('"');
            }

            *_tos << Pt::Char('>');

            this->writeCharacters(content);
            *_tos << Pt::Char('<') << Pt::Char('/') << localName << Pt::Char('>');

            if( useEndl() )
                this->endl();
        }

        void writeCharacters(const Pt::Char* text)
        {
            if( ! _tos)
                return;

            xmlEncode(*_tos, text);
        }

        void writeCharacters(const Pt::String& text)
        {
            if( ! _tos)
                return;

            xmlEncode(*_tos, text.c_str());
        }

        void flush()
        {
            if(_tos)
                _tos->flush();
        }

        void endl()
        {
            if(_tos)
                *_tos << Pt::Char('\n');
        }

        void setFormatFlags(int f, bool sw = true)  
        { if (sw) _flags |= f; else _flags &= ~f; }

        bool useXmlDeclaration() const   
        { return UseXmlDeclaration == (_flags & UseXmlDeclaration); }

        void useXmlDeclaration(bool sw)  
        { setFormatFlags(UseXmlDeclaration, sw); }

        bool useIndent() const           
        { return UseIndent == (_flags & UseIndent); }

        void useIndent(bool sw)          
        { setFormatFlags(UseIndent, sw); }

        bool useEndl() const             
        { return UseEndl == (_flags & UseEndl); }

        void useEndl(bool sw)            
        { setFormatFlags(UseEndl, sw); }

    private:
        std::basic_ostream<Char>* _tos;
        std::vector<Pt::String> _elements;
        int _flags;
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


void XmlWriter::begin(std::basic_ostream<Char>& os)
{
    _impl->begin(os);
}


void XmlWriter::writeStartDocument(const Pt::Char* version, const Pt::Char* encoding)
{
    _impl->writeStartDocument(version, encoding);
}


void XmlWriter::writeStartTag(const Pt::Char* name)
{
    _impl->writeStartTag(name);
}


void XmlWriter::writeEndTag(const Pt::Char* name)
{
    _impl->writeEndTag(name);
}


void XmlWriter::writeStartElement(const Pt::String& prefix, const Pt::String& localName, const Pt::String& ns)
{
    _impl->writeStartElement(prefix, localName, ns);
}


void XmlWriter::writeStartElement(const Pt::Char* localName, const Attribute* attr, size_t attrCount)
{
    _impl->writeStartElement(localName, attr,attrCount);
}


void XmlWriter::writeStartElement(const Pt::String& localName, const Attribute* attr, size_t attrCount)
{
    _impl->writeStartElement(localName, attr, attrCount);
}


void XmlWriter::writeEndElement()
{
    _impl->writeEndElement();
}


void XmlWriter::writeElement(const Pt::String& localName, const Pt::String& content, const Attribute* attr, size_t attrCount)
{
    _impl->writeElement(localName, content, attr, attrCount);
}


void XmlWriter::writeElement(const Pt::Char* localName, const Pt::Char* content, const Attribute* attr, size_t attrCount)
{
    _impl->writeElement(localName, content, attr, attrCount);
}


void XmlWriter::writeCharacters(const Pt::Char* text)
{
    _impl->writeCharacters(text);
}


void XmlWriter::writeCharacters(const Pt::String& text)
{
    _impl->writeCharacters(text);
}


void XmlWriter::flush()
{
    _impl->flush();
}


void XmlWriter::endl()
{
    _impl->endl();
}


void XmlWriter::useIndent(bool sw)          
{ 
    _impl->useIndent(sw);
}


void XmlWriter::useEndl(bool sw)            
{ 
    _impl->useEndl(sw);
}

} // namespace Xml

} // namespace Pt
