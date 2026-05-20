/*
 * Copyright (C) 2020-2026 by Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <Pt/JsonRpc/Formatter.h>
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

namespace JsonRpc {

Formatter::Formatter()
: _writer(0)
, _composer(0)
{
    _parse = &Formatter::onBeginNode;
}


Formatter::Formatter(Json::JsonWriter& writer)
: _writer(&writer)
, _composer(0)
{
    _parse = &Formatter::onBeginNode;
}


Formatter::~Formatter()
{
}


void Formatter::attach(Json::JsonWriter& writer)
{
    _writer = &writer;
}


void Formatter::detach()
{
    _writer = 0;
}


// --- Write side: Decomposer drives these via Pt::Formatter interface ---

void Formatter::onAddString(const char* /*name*/, const char* /*type*/,
                            const Pt::Char* value, const char* /*id*/)
{
    if(_writer)
        _writer->writeString(value);
}


void Formatter::onAddBool(const char* /*name*/, bool value, const char* /*id*/)
{
    if(_writer)
        _writer->writeBool(value);
}


void Formatter::onAddChar(const char* /*name*/, const Pt::Char& value, const char* /*id*/)
{
    if(_writer)
        _writer->writeString(&value, 1);
}


void Formatter::onAddInt8(const char* name, Pt::int8_t value, const char* id)
{
    onAddInt64(name, value, id);
}


void Formatter::onAddInt16(const char* name, Pt::int16_t value, const char* id)
{
    onAddInt64(name, value, id);
}


void Formatter::onAddInt32(const char* name, Pt::int32_t value, const char* id)
{
    onAddInt64(name, value, id);
}


void Formatter::onAddInt64(const char* /*name*/, Pt::int64_t value, const char* /*id*/)
{
    if(_writer)
        _writer->writeInt(value);
}


void Formatter::onAddUInt8(const char* name, Pt::uint8_t value, const char* id)
{
    onAddUInt64(name, value, id);
}


void Formatter::onAddUInt16(const char* name, Pt::uint16_t value, const char* id)
{
    onAddUInt64(name, value, id);
}


void Formatter::onAddUInt32(const char* name, Pt::uint32_t value, const char* id)
{
    onAddUInt64(name, value, id);
}


void Formatter::onAddUInt64(const char* /*name*/, Pt::uint64_t value, const char* /*id*/)
{
    if(_writer)
        _writer->writeUInt(value);
}


void Formatter::onAddFloat(const char* name, float value, const char* id)
{
    onAddLongDouble(name, static_cast<long double>(value), id);
}


void Formatter::onAddDouble(const char* name, double value, const char* id)
{
    onAddLongDouble(name, static_cast<long double>(value), id);
}


void Formatter::onAddLongDouble(const char* /*name*/, long double value, const char* /*id*/)
{
    if(_writer)
        _writer->writeFloat(value);
}


void Formatter::onAddBinary(const char* /*name*/, const char* /*type*/,
                            const char* /*value*/, std::size_t /*length*/, const char* /*id*/)
{
    throw SerializationError("binary data not supported");
}


void Formatter::onAddReference(const char* /*name*/, const char* /*id*/)
{
    throw SerializationError("references not supported");
}


void Formatter::onBeginSequence(const char* /*name*/, const char* /*type*/, const char* /*id*/)
{
    if(_writer)
        _writer->writeArray();
}


void Formatter::onBeginElement()
{
}


void Formatter::onFinishElement()
{
}


void Formatter::onFinishSequence()
{
    if(_writer)
        _writer->writeArrayEnd();
}


void Formatter::onBeginStruct(const char* /*name*/, const char* /*type*/, const char* /*id*/)
{
    if(_writer)
        _writer->writeObject();
}


void Formatter::onBeginMember(const char* name)
{
    if(_writer)
        _writer->writeMember(name);
}


void Formatter::onFinishMember()
{
}


void Formatter::onFinishStruct()
{
    if(_writer)
        _writer->writeObjectEnd();
}


// --- Read side: beginParse / parseSome / parse ---

void Formatter::onBeginParse(Composer& composer)
{
    _composer = &composer;
    _parse = &Formatter::onBeginNode;

    while( ! _parseStack.empty() )
        _parseStack.pop();

    _parseStack.push(_parse);
}


bool Formatter::onParseSome()
{
    // Not used — JsonRpc uses advance() per node instead
    return _composer == 0;
}


void Formatter::onParse()
{
    // Not used — JsonRpc uses advance() per node instead
}


// --- Public advance method: process one JSON node ---

bool Formatter::advance(const Json::Node& node)
{
    (this->*_parse)(node);
    return _composer == 0;
}


// --- State machine: OnBegin (top-level or nested value) ---

void Formatter::onBeginNode(const Json::Node& node)
{
    switch( node.type() )
    {
        case Json::Node::StartArray:
        {
            _parse = &Formatter::onArrayNode;
            _parseStack.push(_parse);
            break;
        }

        case Json::Node::StartObject:
        {
            _parse = &Formatter::onObjectNode;
            _parseStack.push(_parse);
            break;
        }

        case Json::Node::String:
        {
            const Json::String& s = static_cast<const Json::String&>(node);
            _composer->setString( s.value() );
            _composer = _composer->finish();
            break;
        }

        case Json::Node::Float:
        {
            const Json::Float& f = static_cast<const Json::Float&>(node);
            _composer->setFloat( f.value() );
            _composer = _composer->finish();
            break;
        }

        case Json::Node::Integer:
        {
            const Json::Integer& i = static_cast<const Json::Integer&>(node);
            _composer->setInt( i.value() );
            _composer = _composer->finish();
            break;
        }

        case Json::Node::Boolean:
        {
            const Json::Boolean& b = static_cast<const Json::Boolean&>(node);
            _composer->setBool( b.value() );
            _composer = _composer->finish();
            break;
        }

        case Json::Node::Null:
        {
            _composer = _composer->finish();
            break;
        }

        default:
            break;
    }
}


// --- State machine: onArray (inside a JSON array) ---

void Formatter::onArrayNode(const Json::Node& node)
{
    switch( node.type() )
    {
        case Json::Node::StartArray:
        {
            _composer = _composer->beginElement();

            _parse = &Formatter::onArrayNode;
            _parseStack.push(_parse);
            break;
        }

        case Json::Node::EndArray:
        {
            _composer = _composer->finish();

            _parseStack.pop();
            _parse = _parseStack.top();
            break;
        }

        case Json::Node::StartObject:
        {
            _composer = _composer->beginElement();

            _parse = &Formatter::onObjectNode;
            _parseStack.push(_parse);
            break;
        }

        case Json::Node::String:
        {
            _composer = _composer->beginElement();

            const Json::String& s = static_cast<const Json::String&>(node);
            _composer->setString( s.value() );
            _composer = _composer->finish();
            break;
        }

        case Json::Node::Float:
        {
            _composer = _composer->beginElement();

            const Json::Float& f = static_cast<const Json::Float&>(node);
            _composer->setFloat( f.value() );
            _composer = _composer->finish();
            break;
        }

        case Json::Node::Integer:
        {
            _composer = _composer->beginElement();

            const Json::Integer& i = static_cast<const Json::Integer&>(node);
            _composer->setInt( i.value() );
            _composer = _composer->finish();
            break;
        }

        case Json::Node::Boolean:
        {
            _composer = _composer->beginElement();

            const Json::Boolean& b = static_cast<const Json::Boolean&>(node);
            _composer->setBool( b.value() );
            _composer = _composer->finish();
            break;
        }

        case Json::Node::Null:
        {
            _composer = _composer->beginElement();
            _composer = _composer->finish();
            break;
        }

        default:
            break;
    }
}


// --- State machine: onObject (inside a JSON object) ---

void Formatter::onObjectNode(const Json::Node& node)
{
    switch( node.type() )
    {
        case Json::Node::StartArray:
        {
            _parse = &Formatter::onArrayNode;
            _parseStack.push(_parse);
            break;
        }

        case Json::Node::StartObject:
        {
            _parse = &Formatter::onObjectNode;
            _parseStack.push(_parse);
            break;
        }

        case Json::Node::EndObject:
        {
            _composer = _composer->finish();

            _parseStack.pop();
            _parse = _parseStack.top();
            break;
        }

        case Json::Node::Member:
        {
            const Json::Member& m = static_cast<const Json::Member&>(node);
            _composer = _composer->beginMember( m.name().narrow() );
            break;
        }

        case Json::Node::String:
        {
            const Json::String& s = static_cast<const Json::String&>(node);
            _composer->setString( s.value() );
            _composer = _composer->finish();
            break;
        }

        case Json::Node::Float:
        {
            const Json::Float& f = static_cast<const Json::Float&>(node);
            _composer->setFloat( f.value() );
            _composer = _composer->finish();
            break;
        }

        case Json::Node::Integer:
        {
            const Json::Integer& i = static_cast<const Json::Integer&>(node);
            _composer->setInt( i.value() );
            _composer = _composer->finish();
            break;
        }

        case Json::Node::Boolean:
        {
            const Json::Boolean& b = static_cast<const Json::Boolean&>(node);
            _composer->setBool( b.value() );
            _composer = _composer->finish();
            break;
        }

        case Json::Node::Null:
        {
            _composer = _composer->finish();
            break;
        }

        default:
            break;
    }
}

} // namespace JsonRpc

} // namespace Pt
