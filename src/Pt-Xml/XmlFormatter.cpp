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
#include <Pt/Xml/XmlFormatter.h>
#include <Pt/Xml/XmlSerializationContext.h>
#include <Pt/Xml/XmlWriter.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/Composer.h>
#include <Pt/Convert.h>
#include <Pt/String.h>
#include <cassert>

namespace Pt {

namespace Xml {

XmlFormatter::XmlFormatter()
: _writer(0)
, _reader(0)
, _valueType(0)
, _composer(0)
{
    _processNode = &XmlFormatter::OnBegin;
}


XmlFormatter::XmlFormatter(XmlWriter& writer)
: _writer(0)
, _reader(0)
, _valueType(0)
, _composer(0)
{
    this->attach(writer);
    _processNode = &XmlFormatter::OnBegin;
}


XmlFormatter::XmlFormatter(XmlReader& reader)
: _writer(0)
, _reader(0)
, _valueType(0)
, _composer(0)
{
    this->attach(reader);
    _processNode = &XmlFormatter::OnBegin;
}


XmlFormatter::~XmlFormatter()
{
    this->detach();
}


void XmlFormatter::attach(XmlWriter& writer)
{
    _writer = &writer;
}


void XmlFormatter::attach(XmlReader& reader)
{
    _reader = &reader;
}


void XmlFormatter::detach()
{
    _writer = 0;
    _reader = 0;
}


void XmlFormatter::onAddString(const char* name, const char* type,
                               const Pt::Char* value, const char* id)
{
    addValue(name, type, value, id);
}


void XmlFormatter::onAddBinary(const char* name, const char* type,
                               const char* value, std::size_t length, const char* id)
{
    _value.assign(value, length);
    this->addValue(name, type, _value.c_str(), id);
}


void XmlFormatter::onAddBool(const char* name, bool value,
                             const char* id)
{
    _value = value ? "true" : "false";
    this->addValue(name, "bool", _value.c_str(), id);
}


void XmlFormatter::onAddChar(const char* name, const Pt::Char& value,
                             const char* id)
{
    _value.clear();
    _value += value;
    this->addValue(name, "char", _value.c_str(), id);
}


void XmlFormatter::onAddInt8(const char* name, Pt::int8_t value, const char* id)
{
    this->onAddInt64(name, value, id);
}


void XmlFormatter::onAddInt16(const char* name, Pt::int16_t value, const char* id)
{
    this->onAddInt64(name, value, id);
}    


void XmlFormatter::onAddInt32(const char* name, Pt::int32_t value, const char* id)
{
    this->onAddInt64(name, value, id);
}


void XmlFormatter::onAddInt64(const char* name, Pt::int64_t value,
                              const char* id)
{
    _value.clear();
    formatInt( std::back_inserter(_value), value );

    this->addValue(name, "int", _value.c_str(), id);
}


void XmlFormatter::onAddUInt8(const char* name, Pt::uint8_t value, const char* id)
{
    this->onAddUInt64(name, value, id);
}


void XmlFormatter::onAddUInt16(const char* name, Pt::uint16_t value, const char* id)
{
    this->onAddUInt64(name, value, id);
}    


void XmlFormatter::onAddUInt32(const char* name, Pt::uint32_t value, const char* id)
{
    this->onAddUInt64(name, value, id);
}


void XmlFormatter::onAddUInt64(const char* name, Pt::uint64_t value,
                               const char* id)
{
    _value.clear();
    formatInt( std::back_inserter(_value), value );
    
    this->addValue(name, "unsigned", _value.c_str(), id);
}


void XmlFormatter::onAddFloat(const char* name, float value,
                              const char* id)
{
    _value.clear();
    formatFloat( std::back_inserter(_value), value );

    this->addValue(name, "float", _value.c_str(), id);
}


void XmlFormatter::onAddDouble(const char* name, double value,
                              const char* id)
{
    _value.clear();
    formatFloat( std::back_inserter(_value), value );
    
    this->addValue(name, "double", _value.c_str(), id);
}


void XmlFormatter::onAddLongDouble(const char* name, long double value,
                                   const char* id)
{
    _value.clear();
    formatFloat( std::back_inserter(_value), value );
    
    this->addValue(name, "long double", _value.c_str(), id);
}


void XmlFormatter::addValue(const char* name, const char* type,
                            const Pt::Char* value, const char* id)
{
    if( ! _writer )
        return;

    _writer->writeStartElement( String::widen( *name ? name : type ) );

    if(*type)
    {
        _writer->writeAttribute(String("type"), Pt::String(type));
    }

    if(*id)
    {
        _writer->writeAttribute(String("id"), Pt::String(id));
    }

    _writer->writeCharacters(value);
    _writer->writeEndElement();
}


void XmlFormatter::onAddReference(const char* name, const char* id)
{
    if( ! _writer )
        return;

    _writer->writeStartElement( String::widen(name) );
    _writer->writeAttribute(String("ref"), Pt::String(id));
    _writer->writeEndElement();
}


void XmlFormatter::onBeginSequence(const char* name, const char* type,
                                   const char* id)
{
    if( ! _writer )
        return;

    _writer->writeStartElement( String::widen( *name ? name : "array" ) );

    if(*type)
    {
        _writer->writeAttribute(String("type"), Pt::String(type));
    }

    if(*id)
    {
        _writer->writeAttribute(String("id"), Pt::String(id));
    }
}


void XmlFormatter::onBeginElement()
{
}


void XmlFormatter::onFinishElement()
{
}


void XmlFormatter::onFinishSequence()
{
    if( ! _writer )
        return;

    _writer->writeEndElement();
}


void XmlFormatter::onBeginStruct(const char* name, const char* type,
                                 const char* id)
{
    if( ! _writer )
        return;

    _writer->writeStartElement( String::widen( *name ? name : "object" ) );

    if(*type)
    {
        _writer->writeAttribute(String("type"), Pt::String(type));
    }

    if(*id)
    {
        _writer->writeAttribute(String("id"), Pt::String(id));
    }
}


void XmlFormatter::onBeginMember(const char*)
{
}


void XmlFormatter::onFinishMember()
{
}


void XmlFormatter::onFinishStruct()
{
    if( ! _writer )
        return;

    _writer->writeEndElement();
}


void XmlFormatter::onBeginParse(Composer& comp)
{
    _composer = &comp;
    _processNode = &XmlFormatter::OnBegin;
}


void XmlFormatter::onParse()
{
    assert(_composer);

    InputIterator it = _reader->current();
    if(it->type() == Node::EndElement)
        ++it;

    for( ; it != _reader->end(); ++it)
    {
        (this->*_processNode)(*it);

        if(_composer == 0)
            break;
    }
}


bool XmlFormatter::onParseSome()
{
    assert(_composer);

    while(_composer != 0)
    {
        const Pt::Xml::Node* node = _reader->advance();
        if( ! node)
            break;

        if(node->type() == Node::EndDocument)
            throw SerializationError("incomplete type");

        (this->*_processNode)(*node);
    }

    return _composer == 0 ;
}

enum ValueType
{
    Void = 0,
    Int = 1,
    Unsigned = 2,
    String = 3,
    Bool = 4,
    Char = 5,
    Float = 6,
    Double = 7,
    LongDouble = 8
};

int toValueType(const Pt::String& str)
{
    if(str == "int")
        return Int;
    else if(str == "unsigned")
        return Unsigned;
    else if(str == "string")
        return String;
    else if(str == "bool")
        return Bool;
    else if(str == "char")
        return Char;
    else if(str == "float")
        return Float;
    else if(str == "double")
        return Double;
    else if(str == "long double")
        return LongDouble;

    return Void;
}

void XmlFormatter::OnBegin(const Node& node)
{
    switch( node.type() )
    {
        case Node::StartElement:
        {
            const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

            const Pt::String& name = se.name().name();

            AttributeList::ConstIterator nodeId = se.attributes().find( Pt::String("id") );
            if( nodeId != se.attributes().end() )
            {
                _composer->setId( nodeId->value().narrow() );
            }

            AttributeList::ConstIterator type = se.attributes().find( Pt::String("type"));
            if( type == se.attributes().end() )
            {
                _composer->setTypeName(name.narrow());
            }
            else
            {
                _composer->setTypeName(type->value().narrow());
                _valueType = toValueType( type->value() );
            }

            AttributeList::ConstIterator refId = se.attributes().find( Pt::String("ref"));
            if( refId!= se.attributes().end() )
            {
                _composer->setReference( refId->value().narrow() );
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
            _composer->setString(Pt::String() );

            const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
            this->finishXmlMember(ee);
            break;
        }

        default:
            break;
    }
}


void setValue(Pt::Composer& composer, const Pt::String& value, int valueType)
{
    switch(valueType)
    {
        case Bool:
            if(value == L"yes" || value == L"YES" ||
                value == L"on" || value == L"ON" ||
                value == L"true" || value == L"TRUE" )
            {
                composer.setBool(true);
            }
            else if(value == L"no" || value == L"NO" ||
                    value == L"off" || value == L"OFF" ||
                    value == L"false" || value == L"FALSE" )
            {
                composer.setBool(false);
            }
            else
                throw SerializationError("invalid bool");

            break;

        case Int:
        {
            Pt::int32_t i = 0;
            parseInt(value.begin(), value.end(), i);
            composer.setInt(i);
            break;
        }

        case Unsigned:
        {
            Pt::uint32_t u = 0;
            parseInt(value.begin(), value.end(), u);
            composer.setUInt(u);
            break;
        } 
      
        case Float:
        case Double:
        {
            double d = 0;
            parseFloat(value.begin(), value.end(), d);
            composer.setFloat(d);
            break;
        }

        default:
        case String:
            composer.setString(value);
            break;
    }
}


void XmlFormatter::OnValue(const Node& node)
{
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
            setValue(*_composer, _value, _valueType);

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
    const Pt::String& name = se.name().name();
    _composer = _composer->beginMember(name.narrow() );

    AttributeList::ConstIterator nodeId = se.attributes().find(Pt::String("id"));
    if( nodeId != se.attributes().end() )
    {
        _composer->setId( nodeId->value().narrow() );
    }

    AttributeList::ConstIterator type = se.attributes().find(Pt::String("type"));
    if( type != se.attributes().end() )
    {
        _composer->setTypeName(type->value().narrow());
        _valueType = toValueType( type->value() );
    }

    AttributeList::ConstIterator refId = se.attributes().find(Pt::String("ref"));
    if( refId != se.attributes().end() )
    {
        _composer->setReference( refId->value().narrow() );
        _processNode = &XmlFormatter::OnReferenceBegin;
        return;
    }

    _processNode = &XmlFormatter::OnMemberBegin;
}


void XmlFormatter::finishXmlMember(const Xml::EndElement& e)
{
    _composer = _composer->finish();
    _processNode = &XmlFormatter::OnMemberEnd;
}

} // namespace Xml

} // namespace Pt
