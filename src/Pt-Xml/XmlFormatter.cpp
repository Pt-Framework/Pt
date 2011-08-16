/*
 * Copyright (C) 2008 by Marc Boris Duerner
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
#include "Pt/Xml/XmlFormatter.h"
#include "Pt/Xml/XmlSerializationContext.h"
#include "Pt/Xml/XmlWriter.h"
#include "Pt/Xml/XmlReader.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Xml/Characters.h"
#include <Pt/Composer.h>
#include "Pt/Convert.h"
#include "Pt/String.h"
#include "Pt/SourceInfo.h"
#include <stdexcept>

namespace Pt {

namespace Xml {

XmlFormatter::XmlFormatter()
: _writer(0)
, _wrPtr(0)
, _reader(0)
, _rdPtr(0)
, _composer(0)
{
    _processNode = &XmlFormatter::OnBegin;
}


XmlFormatter::XmlFormatter(std::ostream& os)
: _writer(0)
, _wrPtr(0)
, _reader(0)
, _rdPtr(0)
, _composer(0)
{
    this->attach(os);
    _processNode = &XmlFormatter::OnBegin;
    _writer = _wrPtr.get();
}


XmlFormatter::XmlFormatter(XmlWriter& writer)
: _writer(0)
, _wrPtr(0)
, _reader(0)
, _rdPtr(0)
, _composer(0)
{
    this->attach(writer);
    _processNode = &XmlFormatter::OnBegin;
}


XmlFormatter::XmlFormatter(std::istream& is)
: _writer(0)
, _wrPtr(0)
, _reader(0)
, _rdPtr(0)
, _composer(0)
{
    this->attach(is);
    _processNode = &XmlFormatter::OnBegin;
    _writer = _wrPtr.get();
}


XmlFormatter::XmlFormatter(XmlReader& reader)
: _writer(0)
, _wrPtr(0)
, _reader(0)
, _rdPtr(0)
, _composer(0)
{
    this->attach(reader);
    _processNode = &XmlFormatter::OnBegin;
}


XmlFormatter::~XmlFormatter()
{
    this->detach();
}


void XmlFormatter::attach(std::ostream& os)
{
    if (_writer)
        throw std::logic_error("XmlSerizalizer is already open." + PT_SOURCEINFO);

    _wrPtr.reset(new XmlWriter(os));
    _writer = _wrPtr.get();
}


void XmlFormatter::attach(XmlWriter& writer)
{
    if (_writer)
        throw std::logic_error("XmlSerizalizer is already open." + PT_SOURCEINFO);

    _wrPtr.reset(0);
    _writer = &writer;
}


void XmlFormatter::attach(std::istream& is)
{
    _rdPtr.reset( new XmlReader(is) );
    _reader = _rdPtr.get();
}


void XmlFormatter::attach(XmlReader& reader)
{
    _rdPtr.reset(0);
    _reader = &reader;
}


void XmlFormatter::detach()
{
    if (_writer)
    {
        _wrPtr.reset(0);
        _writer = 0;
    }

    if (_reader)
    {
        _rdPtr.reset(0);
        _reader = 0;
    }
}


void XmlFormatter::flush()
{
    if (_writer)
        _writer->flush();
}


void XmlFormatter::addString(const char* name, const char* type,
                             const Pt::String& value, const char* id)
{
    if( ! _writer )
        return;

    if( *id != '\0' )
    {
        Xml::Attribute attr[2];
        attr[0] = Xml::Attribute( String(L"type"), String::widen( type ) );
        attr[1] = Xml::Attribute( String(L"id"), String::widen( id ) );

        if( *name != '\0' )
            _writer->writeElement( String::widen( name ), value, attr, 2 );
        else
            _writer->writeElement( String::widen( type ), value, attr, 2 );
    }
    else
    {
        Xml::Attribute typeAttr( String(L"type"), String::widen( type ) );

        if( *name != '\0' )
            _writer->writeElement( String::widen( name ), value, &typeAttr, 1 );
        else
            _writer->writeElement( String::widen( type ), value, &typeAttr, 1 );
    }
}


void XmlFormatter::addBytes(const char* name, const char* type,
                            const char* value, size_t length, const char* id)
{
    convert(_value, std::string(value, length));
    this->addString(name, type, _value, id);
}


void XmlFormatter::addBool(const char* name, bool value,
                           const char* id)
{
    convert(_value, value);
    this->addString(name, "bool", _value, id);
}


void XmlFormatter::addChar(const char* name, const Pt::Char& value,
                     const char* id)
{
    _value.clear();
    _value += value;
    this->addString(name, "char", _value, id);
}

void XmlFormatter::addInt(const char* name, long long value,
                          const char* id)
{
    convert(_value, value);
    this->addString(name, "int", _value, id);
}


void XmlFormatter::addUInt(const char* name, unsigned long long value,
                           const char* id)
{
    convert(_value, value);
    this->addString(name, "unsigned", _value, id);
}


void XmlFormatter::addFloat(const char* name, float value,
                            const char* id)
{
    convert(_value, value);
    this->addString(name, "float", _value, id);
}


void XmlFormatter::addDouble(const char* name, double value,
                            const char* id)
{
    convert(_value, value);
    this->addString(name, "double", _value, id);
}


void XmlFormatter::addLongDouble(const char* name, long double value,
                                 const char* id)
{
    convert(_value, value);
    this->addString(name, "long double", _value, id);
}


void XmlFormatter::addReference(const char* name, const char* id)
{
    if( ! _writer )
        return;

    Attribute attr( Pt::String(L"ref"), Pt::String::widen( id ) );
    _writer->writeElement( Pt::String::widen( name ), Pt::String(), &attr, 1 );
}


void XmlFormatter::beginArray(const char* name, const char* type,
                              const char*id)
{
    if( ! _writer )
        return;

    if( *id != '\0' )
    {
        Attribute attr( Pt::String(L"id"), Pt::String::widen( id ) );

        if( *name != '\0' )
            _writer->writeStartElement( Pt::String::widen( name ), &attr, 1 );
        else
            _writer->writeStartElement( String::widen( type ), &attr, 1 );
    }
    else
    {
        if( *name != '\0' )
            _writer->writeStartElement( Pt::String::widen( name ) );
        else
            _writer->writeStartElement( String::widen( type ) );
    }
}


void XmlFormatter::beginElement(const char* type, const char* id)
{
}


void XmlFormatter::finishElement()
{
}


void XmlFormatter::finishArray()
{
    if( ! _writer )
        return;

    _writer->writeEndElement();
}


void XmlFormatter::beginObject(const char* name, const char* type,
                               const char* id)
{
    this->onBeginObject(name, type, id);
}


void XmlFormatter::onBeginObject(const char* name, const char* type,
                                 const char* id)
{
    if( ! _writer )
        return;

    if( *id != '\0' )
    {
        Xml::Attribute attr( String(L"id"), String::widen( id ) );

        if( *name != '\0' )
            _writer->writeStartElement( String::widen( name ), &attr, 1 );
        else
            _writer->writeStartElement( String::widen( type ), &attr, 1 );
    }
    else
    {
        if( *name != '\0' )
            _writer->writeStartElement( String::widen( name ) );
        else
            _writer->writeStartElement( String::widen( type ) );
    }
}


void XmlFormatter::beginMember(const char* name, const char* type,
                               const char* id)
{
}


void XmlFormatter::finishMember()
{
}


void XmlFormatter::finishObject()
{
    this->onFinishObject();
}


void XmlFormatter::onFinishObject()
{
    if( ! _writer )
        return;

    _writer->writeEndElement();
}


void XmlFormatter::parse(IComposer& comp)
{
    //std::cerr << "-> GET"<< std::endl;
    _composer = &comp;

    _processNode = &XmlFormatter::OnBegin;

    XmlReader::Iterator it = _reader->current();
    if(it->type() == Node::EndElement)
        ++it;

    for( ; it != _reader->end(); ++it)
    {
        //std::cerr << "-> GOT NODE: " << it->type() << std::endl;
        (this->*_processNode)(*it);

        if(_composer == 0)
            break;
    }

    //std::cerr << "-> DONE"<< std::endl;
}


bool XmlFormatter::parseSome(IComposer& comp)
{
    _composer = &comp;

    while( _reader->advance() )
    {
        const Pt::Xml::Node& node = _reader->get();
        //std::cerr << "-> GOT NODE: " << node.type() << std::endl;

        if(node.type() == Node::EndDocument)
            throw SerializationError("incomplete type");

        (this->*_processNode)(node);

        if( _composer == 0 )
        {
            //std::cerr << "-> COMPOSER END" << std::endl;
            _processNode = &XmlFormatter::OnBegin;
            return true;
        }
    }

    //std::cerr << "-> COMPOSER MORE" << std::endl;
    return false;
}


void XmlFormatter::OnBegin(const Node& node)
{
    //std::cerr << "-> OnBegin" << std::endl;

    switch( node.type() )
    {
        case Node::StartElement:
        {
            const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

            //std::cerr << "BEGIN MEMBER: " << se.name().narrow() << std::endl;
            _composer->setName( se.name().narrow() );

            String nodeId = se.attribute(L"id");
            if( ! nodeId.empty() )
            {
                //std::cerr << "ID: " << nodeId.narrow() << std::endl;
                _composer->setId( nodeId.narrow() );
            }

            String type = se.attribute(L"type");
            if( ! type.empty() )
            {
                //std::cerr << "TYPE: " << type.narrow() << std::endl;
                _composer->setTypeName(type.narrow());
            }

            String refId = se.attribute(L"ref");
            if( ! refId.empty() )
            {
                //std::cerr << "REF: " << refId.narrow() << std::endl;
                _composer->setReference( refId.narrow() );
                _processNode = &XmlFormatter::OnReferenceBegin;
                break;
            }

            _processNode = &XmlFormatter::OnMemberBegin;
            break;
        }

        case Node::EndElement:
        {
            throw SerializationError("expected start element");
        }

        default:
            break;
    }
}


void XmlFormatter::OnReferenceBegin(const Node& node)
{
    //std::cerr << "-> OnReferenceBegin" << std::endl;

    switch( node.type() )
    {
        case Node::StartElement:
        {
            throw SerializationError("invalid reference");
        }

        case Node::EndElement:
        {
            const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
            this->finishXmlMember(ee);
            break;
        }

        default:
            break;
    }
}


void XmlFormatter::OnMemberBegin(const Node& node)
{
    //std::cerr << "-> OnMemberBegin" << std::endl;

    switch( node.type() )
    {
        case Node::StartElement:
        {
            const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
            this->beginXmlMember(se);
            break;
        }

        case Node::Characters:
        {
            const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
            _value = chars.content();
            _processNode = &XmlFormatter::OnValue;
            break;
        }

        case Node::EndElement:
        {
            //std::cerr << "VALUE: <empty>" << std::endl;
            _composer->setValue(Pt::String() );

            const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
            this->finishXmlMember(ee);
            break;
        }

        default:
            break;
    }
}


void XmlFormatter::OnValue(const Node& node)
{
    //std::cerr << "-> OnValue" << std::endl;

    switch( node.type() )
    {
        case Node::StartElement:
        {
            const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
            this->beginXmlMember(se);
            break;
        }

        case Node::EndElement:
        {
            //std::cerr << "VALUE: " << _value.narrow() << std::endl;
            _composer->setValue(_value);

            const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
            this->finishXmlMember(ee);
            break;
        }

        default:
            break;
    }
}


void XmlFormatter::OnMemberEnd(const Node& node)
{
    //std::cerr << "-> OnMemberEnd" << std::endl;

    switch( node.type() )
    {
        case Node::StartElement:
        {
            const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
            this->beginXmlMember(se);
            break;
        }

        case Node::EndElement:
        {
            const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
            this->finishXmlMember(ee);
            break;
        }

        default:
            break;
    }
}


void XmlFormatter::beginXmlMember(const Xml::StartElement& se)
{
    //std::cerr << "BEGIN MEMBER: " << se.name().narrow() << std::endl;
    _composer = _composer->beginMember(se.name().narrow() );

    String nodeId = se.attribute(L"id");
    if( ! nodeId.empty() )
    {
        //std::cerr << "ID: " << nodeId.narrow() << std::endl;
        _composer->setId( nodeId.narrow() );
    }

    String type = se.attribute(L"type");
    if( ! type.empty() )
    {
        //std::cerr << "BTYPE: " << type.narrow() << std::endl;
        _composer->setTypeName(type.narrow());
    }

    String refId = se.attribute(L"ref");
    if( ! refId.empty() )
    {
        //std::cerr << "REF: " << refId.narrow() << std::endl;
        _composer->setReference( refId.narrow() );
        _processNode = &XmlFormatter::OnReferenceBegin;
        return;
    }

    _processNode = &XmlFormatter::OnMemberBegin;
}


void XmlFormatter::finishXmlMember(const Xml::EndElement& )
{
    //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
    //std::cerr << "END MEMBER: " << ee.name().narrow() << std::endl;
    _composer = _composer->finish();
    _processNode = &XmlFormatter::OnMemberEnd;
}

} // namespace Xml

} // namespace Pt
