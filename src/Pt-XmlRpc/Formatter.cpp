/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 * Copyright (C) 2009 by Tommi Meakitalo
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
#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Formatter.h>
#include <Pt/SerializationInfo.h>
#include <Pt/Convert.h>

namespace Pt {

namespace XmlRpc {

Formatter::Formatter(Xml::XmlWriter& writer)
: _writer(&writer)
{
}


Formatter::~Formatter()
{
}


void Formatter::attach(Xml::XmlWriter& writer)
{
    _writer = &writer;
}


void Formatter::addValue(const std::string& name, const std::string& type,
                         const Pt::String& value, const std::string& id)
{
    _writer->writeStartElement( Pt::String::widen("value") );
    _writer->writeElement( Pt::String::widen(type), value );
    _writer->writeEndElement();
}


void Formatter::addBool(const std::string& name, bool value, 
                        const std::string& id)
{
	convert(_value, value);
	this->addValue(name, "boolean", _value, id);
}


void Formatter::addInt(const std::string& name, long long value, 
                       const std::string& id)
{
	convert(_value, value);
	this->addValue(name, "int", _value, id);
}


void Formatter::addUInt(const std::string& name, unsigned long long value, 
                        const std::string& id)
{
	convert(_value, value);
	this->addValue(name, "int", _value, id);
}


void Formatter::addFloat(const std::string& name, double value, 
                         const std::string& id)
{
	convert(_value, value);
	this->addValue(name, "double", _value, id);
}


void Formatter::addBytes(const std::string& name, const std::string& type,
                         const char* data, size_t length, const std::string& id)
{
    _writer->writeStartElement( Pt::String::widen("value") );
    std::string value(data, length);
    _writer->writeElement( Pt::String::widen(type), Pt::String::widen(value) );
    _writer->writeEndElement();
}


void Formatter::addReference(const std::string& name, const std::string& value)
{
    throw SerializationError("references not supported");
}


void Formatter::beginArray(const std::string&, const std::string& type,
                           const std::string&)
{
    _writer->writeStartElement( Pt::String::widen("value") );
    _writer->writeStartElement( Pt::String::widen("array") );
    _writer->writeStartElement( Pt::String::widen("data") );
}


void Formatter::beginElement(const std::string& type, const std::string& id)
{
}


void Formatter::finishElement()
{
}


void Formatter::finishArray()
{
    _writer->writeEndElement();
    _writer->writeEndElement();
    _writer->writeEndElement();
}


void Formatter::beginObject(const std::string& name, const std::string& type,
                            const std::string& id)
{
    _writer->writeStartElement( Pt::String::widen("value") );
    _writer->writeStartElement( Pt::String::widen("struct") );
}


void Formatter::beginMember(const std::string& name, const std::string& type, 
                            const std::string& id)
{
    _writer->writeStartElement( Pt::String::widen("member") );
    _writer->writeElement( Pt::String::widen("name"), Pt::String::widen(name) );
}


void Formatter::finishMember()
{
    _writer->writeEndElement();
}


void Formatter::finishObject()
{
    _writer->writeEndElement();
    _writer->writeEndElement();
}


void Formatter::finish()
{
    _writer->writeEndElement();
}

}

}
