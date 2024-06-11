/*
   Copyright (C) 2015-2023 by Dr. Marc Boris Duerner
  
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   As a special exception, you may use this file as part of a free
   software library without restriction. Specifically, if other files
   instantiate templates or use macros or inline functions from this
   file, or you compile this file and link it with other files to
   produce an executable, this file does not by itself cause the
   resulting executable to be covered by the GNU General Public
   License. This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU Library
   General Public License.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
   MA 02110-1301 USA
*/

#include <Pt/Json/JsonFormatter.h>
#include <Pt/Json/JsonReader.h>
#include <Pt/Json/JsonWriter.h>
#include <Pt/Json/Node.h>
#include <Pt/Json/Member.h>
#include <Pt/Json/Integer.h>
#include <Pt/Json/Boolean.h>
#include <Pt/Json/Float.h>
#include <Pt/Json/String.h>
#include <Pt/Json/Null.h>
#include <Pt/SerializationError.h>
#include <Pt/Composer.h>
#include <cassert>

namespace Pt {

namespace Json {

JsonFormatter::JsonFormatter()
: _reader(0)
, _writer(0)
, _composer(0)
{
    _parse = &JsonFormatter::OnBegin;
}


JsonFormatter::JsonFormatter(JsonReader& reader)
: _reader(&reader)
, _writer(0)
, _composer(0)
{
    _parse = &JsonFormatter::OnBegin;
}


JsonFormatter::JsonFormatter(JsonWriter& writer)
: _reader(0)
, _writer(&writer)
, _composer(0)
{
    _parse = &JsonFormatter::OnBegin;
}


JsonFormatter::~JsonFormatter()
{
}


void JsonFormatter::attach(JsonReader& reader)
{
    _reader = &reader;
}


void JsonFormatter::attach(JsonWriter& writer)
{
    _writer = &writer;
}


void JsonFormatter::detach()
{
    _reader = 0;
    _writer = 0;
}


void JsonFormatter::onAddString(const char* name, const char* type,
                                const Pt::Char* value, const char* id)
{
    if(_writer)
        _writer->writeString(value);
}


void JsonFormatter::onAddBool(const char* name, bool value, const char* id)
{
    if(_writer)
        _writer->writeBool(value);
}


void JsonFormatter::onAddChar(const char* name, const Pt::Char& value, const char* id)
{
    if(_writer)
        _writer->writeString(&value, 1);
}


void JsonFormatter::onAddInt8(const char* name, Pt::int8_t value, const char* id)
{
    this->onAddInt64(name, value, id);
}


void JsonFormatter::onAddInt16(const char* name, Pt::int16_t value, const char* id)
{
    this->onAddInt64(name, value, id);
}


void JsonFormatter::onAddInt32(const char* name, Pt::int32_t value, const char* id)
{
    this->onAddInt64(name, value, id);
}


void JsonFormatter::onAddInt64(const char* name, Pt::int64_t value, const char* id)
{
    if(_writer)
        _writer->writeInt(value);
}


void JsonFormatter::onAddUInt8(const char* name, Pt::uint8_t value, const char* id)
{
    this->onAddUInt64(name, value, id);
}


void JsonFormatter::onAddUInt16(const char* name, Pt::uint16_t value, const char* id)
{
    this->onAddUInt64(name, value, id);
}


void JsonFormatter::onAddUInt32(const char* name, Pt::uint32_t value, const char* id)
{
    this->onAddUInt64(name, value, id);
}


void JsonFormatter::onAddUInt64(const char* name, Pt::uint64_t value, const char* id)
{
    if(_writer)
        _writer->writeUInt(value);
}


void JsonFormatter::onAddFloat(const char* name, float value, const char* id)
{
    this->onAddDouble(name, value, id);
}


void JsonFormatter::onAddDouble(const char* name, double value, const char* id)
{
    this->onAddLongDouble(name, value, id);
}


void JsonFormatter::onAddLongDouble(const char* name, long double value, const char* id)
{
    if(_writer)
        _writer->writeFloat(value);
}


void JsonFormatter::onAddBinary(const char* name, const char* type,
                                const char* data, std::size_t length, const char* id)
{
    throw SerializationError("binary data not supported");
}


void JsonFormatter::onAddReference(const char* name, const char* value)
{
    throw SerializationError("references not supported");
}


void JsonFormatter::onBeginSequence(const char* name, const char*, const char*)
{
    if(_writer)
        _writer->writeArray();
}


void JsonFormatter::onBeginElement()
{
}


void JsonFormatter::onFinishElement()
{
}


void JsonFormatter::onFinishSequence()
{
    if(_writer)
        _writer->writeArrayEnd();
}


void JsonFormatter::onBeginStruct(const char* name, const char* type, const char* id)
{
    if(_writer)
        _writer->writeObject();
}


void JsonFormatter::onBeginMember(const char* name)
{
    if(_writer)
        _writer->writeMember(name);
}


void JsonFormatter::onFinishMember()
{
}


void JsonFormatter::onFinishStruct()
{
    if(_writer)
        _writer->writeObjectEnd();
}


void JsonFormatter::onBeginParse(Composer& comp)
{
    _composer = &comp;
    _parse = &JsonFormatter::OnBegin;
    _parseStack.push(_parse);
}


bool JsonFormatter::onParseSome()
{
    assert(_composer);

    while(_composer != 0)
    {
        const Node* node = _reader->advance();
        if( ! node )
            break;

        if( node->type() == Node::EndDocument )
            throw SerializationError("incomplete type");

        (this->*_parse)(*node);
    }

    return _composer == 0;
}


void JsonFormatter::onParse()
{
    assert(_composer);

    InputIterator it = _reader->current();

    for( ; it != _reader->end(); ++it)
    {
        (this->*_parse)(*it);

        if(_composer == 0)
            break;
    }
}


void JsonFormatter::OnBegin(const Node& node)
{
    switch( node.type() )
    {
        case Node::StartArray:
        {
            _parse = &JsonFormatter::onArray;
            _parseStack.push(_parse);
            break;
        }

        case Node::StartObject:
        {
            _parse = &JsonFormatter::onObject;
            _parseStack.push(_parse);
            break;
        }
        
        case Node::String:
        {
            const String& s = toString(node);
            _composer->setString( s.value() );
            _composer = _composer->finish();
            break;
        }

        case Node::Float:
        {           
            const Float& f = toFloat(node);
            _composer->setFloat( f.value() );
            _composer = _composer->finish();
            break;
        }

        case Node::Integer:
        {
            const Integer& i = toInteger(node);
            _composer->setInt( i.value() );
            _composer = _composer->finish();
            break;
        }

        case Node::Boolean:
        {
            const Boolean& b = toBoolean(node);
            _composer->setBool( b.value() );
            _composer = _composer->finish();
            break;
        }

        case Node::Null:
        {
            //const Null& n = toNull(node);
            //_composer->setVoid();
            
            _composer = _composer->finish();
            break;
        }

        default:
            break;
    }
}


void JsonFormatter::onArray(const Node& node)
{
    switch( node.type() )
    {
        case Node::StartArray:
        {
            _composer = _composer->beginElement();

            _parse = &JsonFormatter::onArray;
            _parseStack.push(_parse);
            break;
        }

        case Node::EndArray:
        {
            _composer = _composer->finish();

            _parseStack.pop();
            _parse = _parseStack.top();
            break;
        }

        case Node::StartObject:
        {
            _composer = _composer->beginElement();

            _parse = &JsonFormatter::onObject;
            _parseStack.push(_parse);
            break;
        }

        case Node::String:
        {
            _composer = _composer->beginElement();

            const String& s = toString(node);
            _composer->setString( s.value() );
            _composer = _composer->finish();
            break;
        }

        case Node::Float:
        {
            _composer = _composer->beginElement();

            const Float& f = toFloat(node);
            _composer->setFloat( f.value() );
            _composer = _composer->finish();
            break;
        }

        case Node::Integer:
        {
            _composer = _composer->beginElement();

            const Integer& i = toInteger(node);
            _composer->setInt( i.value() );
            _composer = _composer->finish();
            break;
        }

        case Node::Boolean:
        {
            _composer = _composer->beginElement();

            const Boolean& b = toBoolean(node);
            _composer->setBool( b.value() );
            _composer = _composer->finish();
            break;
        }

        case Node::Null:
        {
            _composer = _composer->beginElement();

            //const Null& n = toNull(node);
            //_composer->setVoid();
            
            _composer = _composer->finish();
            break;
        }

        default:
            break;
    }
}


void JsonFormatter::onObject(const Node& node)
{
    switch( node.type() )
    {
        case Node::StartArray:
        {
            _parse = &JsonFormatter::onArray;
            _parseStack.push(_parse);
            break;
        }

        case Node::StartObject:
        {          
            _parse = &JsonFormatter::onObject;
            _parseStack.push(_parse);
            break;
        }

        case Node::EndObject:
        {
            _composer = _composer->finish();

            _parseStack.pop();
            _parse = _parseStack.top();
            break;
        }

        case Node::Member:
        {
            const Member& m = toMember(node);
            _composer = _composer->beginMember( m.name().narrow() );
            break;
        }

        case Node::String:
        {
            const String& s = toString(node);
            _composer->setString( s.value() );
            _composer = _composer->finish();
            break;
        }

        case Node::Float:
        {           
            const Float& f = toFloat(node);
            _composer->setFloat( f.value() );
            _composer = _composer->finish();
            break;
        }

        case Node::Integer:
        {
            const Integer& i = toInteger(node);
            _composer->setInt( i.value() );
            _composer = _composer->finish();
            break;
        }

        case Node::Boolean:
        {
            const Boolean& b = toBoolean(node);
            _composer->setBool( b.value() );
            _composer = _composer->finish();
            break;
        }

        case Node::Null:
        {
            //const Null& n = toNull(node);
            //_composer->setVoid();
            
            _composer = _composer->finish();
            break;
        }

        default:
            break;
    }
}

} // namespace Json

} // namespace Pt
