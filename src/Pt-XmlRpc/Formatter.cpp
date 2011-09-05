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
#include <Pt/Convert.h>
#include <Pt/SerializationError.h>
#include <limits>
#include <cassert>
#include <cmath>

namespace  {

static const Pt::Char XMLRPC_VALUE[]   = { 'v', 'a', 'l', 'u', 'e', '\0' };
static const Pt::Char XMLRPC_INT[]     = { 'i', 'n', 't', '\0' };
static const Pt::Char XMLRPC_DOUBLE[]  = { 'd', 'o', 'u', 'b', 'l', 'e', '\0' };
static const Pt::Char XMLRPC_STRING[]  = { 's', 't', 'r', 'i', 'n', 'g', '\0' };
static const Pt::Char XMLRPC_BOOLEAN[] = { 'b', 'o', 'o', 'l', 'e', 'a', 'n', '\0' };
static const Pt::Char XMLRPC_STRUCT[]  = { 's', 't', 'r', 'u', 'c', 't', '\0' };
static const Pt::Char XMLRPC_MEMBER[]  = { 'm', 'e', 'm', 'b', 'e', 'r', '\0' };
static const Pt::Char XMLRPC_NAME[]    = { 'n', 'a', 'm', 'e', '\0' };
static const Pt::Char XMLRPC_ARRAY[]   = { 'a', 'r', 'r', 'a', 'y', '\0' };
static const Pt::Char XMLRPC_DATA[]    = { 'd', 'a', 't', 'a', '\0' };
static const Pt::Char XMLRPC_FALSE[]   = { '0', '\0' };
static const Pt::Char XMLRPC_TRUE[]    = { '1', '\0' };

template<typename T>
class array_appender : public std::iterator<std::output_iterator_tag, T>
{
    public:
		array_appender()
		: _ptr(0)
		, _end(0)
		{ }

		array_appender(T* ptr, size_t length)
		: _ptr(ptr)
		, _end(ptr + length)
		{ }
		
		array_appender<T>& operator=(const T& val)
		{
		    if(_ptr != _end)
				*_ptr = val;

		    return *this;
		}

		bool operator==(const array_appender<T>& it) const
		{
		    return _ptr == it._ptr;
		}

		array_appender<T>& operator*()
		{
			return *this;
		}

		array_appender<T>& operator++()
		{
			if(_ptr != _end)
				++_ptr;

			return *this;
		}

		array_appender<T> operator++(int)
		{
		    array_appender<T> tmp = *this;

			if(_ptr != _end)
				++_ptr;

			return tmp;
		}

	private:
		T* _ptr;
		T* _end;
};

}

namespace Pt {

namespace XmlRpc {

void Formatter::addString(const char* name, const char* type,
                          const Pt::String& value, const char* id)
{
    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_STRING, value.c_str());
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::addString8(const char* name, const char* value, const char* id)
{
	Pt::String str = Pt::String::widen(value);
    
    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_STRING, str.c_str());
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::addBool(const char* name, bool value, 
                        const char* id)
{
    _writer->writeStartTag(XMLRPC_VALUE);

    if(value)
        _writer->writeElement(XMLRPC_BOOLEAN, XMLRPC_TRUE);
    else
        _writer->writeElement(XMLRPC_BOOLEAN, XMLRPC_FALSE);

    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::addChar(const char* name, const Pt::Char& value,
                        const char* id)
{
    Pt::Char str[2] = { value, '\0' };

    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_STRING , str);
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::addChar8(const char* name, char value,
                         const char* id)
{
    Pt::Char str[2] = { value, '\0' };

    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_STRING , str);
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::addInt8(const char* name, Pt::int8_t value, const char* id)
{
	this->addInt64(name, value, id);
}


void Formatter::addInt16(const char* name, Pt::int16_t value, const char* id)
{
	this->addInt64(name, value, id);
}    


void Formatter::addInt32(const char* name, Pt::int32_t value, const char* id)
{
	this->addInt64(name, value, id);
}


void Formatter::addInt64(const char* name, Pt::int64_t value, const char* id)
{
    const size_t bufsize = (sizeof(value) * 4) + 1 ;
    Pt::Char buf[bufsize];
    
    array_appender<Pt::Char> it(buf, bufsize);
    array_appender<Pt::Char> end;
    it = putInt(it, value);
    if(it == end)
		throw std::logic_error("invalid buffer size");

    *it = '\0';

    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_INT, buf);
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::addUInt8(const char* name, Pt::uint8_t value, const char* id)
{
	this->addUInt64(name, value, id);
}


void Formatter::addUInt16(const char* name, Pt::uint16_t value, const char* id)
{
	this->addUInt64(name, value, id);
}    


void Formatter::addUInt32(const char* name, Pt::uint32_t value, const char* id)
{
	this->addUInt64(name, value, id);
}


void Formatter::addUInt64(const char* name, Pt::uint64_t value, const char* id)
{
    const size_t bufsize = (sizeof(value) * 4) + 1 ;
    Pt::Char buf[bufsize];
    
    array_appender<Pt::Char> it(buf, bufsize);
    array_appender<Pt::Char> end;
    it = putInt(it, value);
    if(it == end)
		throw std::logic_error("invalid buffer size");

    *it = '\0';

    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_INT, buf);
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::addFloat(const char* name, float value,const char* id)
{
    // spec supports only double precision floats
    this->addDouble(name, value, id);
}


void Formatter::addDouble(const char* name, double value, const char* id)
{
    const size_t bufsize = 64;
    Pt::Char buf[bufsize];
    
    array_appender<Pt::Char> it(buf, bufsize);
    array_appender<Pt::Char> end;
    it = putFloat(it, value);
    if(it == end)
    {
		// TODO: use dynamic buffer now or allow to write to writer directly
		throw std::logic_error("float too large");
	}

    *it = '\0';

    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeElement(XMLRPC_DOUBLE, buf);
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::addLongDouble(const char* name, long double value,const char* id)
{
    // spec supports only double precision floats
    this->addDouble(name, static_cast<double>(value), id);
}


void Formatter::addBytes(const char* name, const char* type,
                         const char* data, size_t length, const char* id)
{
    // TODO: this should be base64 encoded

    _writer->writeStartTag(XMLRPC_VALUE);
    std::string value(data, length);
    _writer->writeElement( Pt::String::widen(type), Pt::String::widen(value) );
    _writer->writeStartTag(XMLRPC_VALUE);
}


void Formatter::addReference(const char* name, const char*value)
{
    throw SerializationError("references not supported");
}


void Formatter::beginArray(const char*, const char*,
                           const char*)
{
    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeStartTag(XMLRPC_ARRAY);
    _writer->writeStartTag(XMLRPC_DATA);
}


void Formatter::beginElement(const char* type, const char* id)
{
}


void Formatter::finishElement()
{
}


void Formatter::finishArray()
{
    _writer->writeEndTag(XMLRPC_DATA);
    _writer->writeEndTag(XMLRPC_ARRAY);
    _writer->writeEndTag(XMLRPC_VALUE);
}


void Formatter::beginObject(const char* name, const char* type,
                            const char* id)
{
    _writer->writeStartTag(XMLRPC_VALUE);
    _writer->writeStartTag(XMLRPC_STRUCT);
}


void Formatter::beginMember(const char* name, const char*, const char*)
{
    _writer->writeStartTag(XMLRPC_MEMBER);
    _writer->writeElement(XMLRPC_NAME, Pt::String::widen(name) );
}


void Formatter::finishMember()
{
    _writer->writeEndTag(XMLRPC_MEMBER);
}


void Formatter::finishObject()
{
    _writer->writeEndTag(XMLRPC_STRUCT);
    _writer->writeEndTag(XMLRPC_VALUE);
}

}

}
