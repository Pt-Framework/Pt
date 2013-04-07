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
#include <Pt/Xml/XmlWriter.h>
#include <Pt/Convert.h>
#include <Pt/SerializationError.h>
#include <limits>
#include <cassert>
#include <cmath>
#include <cstddef>

namespace  {

static const Pt::Char XMLRPC_VALUE[]   = { '<', 'v', 'a', 'l', 'u', 'e', '>' };
static const Pt::Char XMLRPC_INT[]     = { '<', 'i', 'n', 't', '>' };
static const Pt::Char XMLRPC_DOUBLE[]  = { '<', 'd', 'o', 'u', 'b', 'l', 'e', '>' };
static const Pt::Char XMLRPC_STRING[]  = { '<', 's', 't', 'r', 'i', 'n', 'g', '>' };
static const Pt::Char XMLRPC_BOOLEAN[] = { '<', 'b', 'o', 'o', 'l', 'e', 'a', 'n', '>' };
static const Pt::Char XMLRPC_STRUCT[]  = { '<', 's', 't', 'r', 'u', 'c', 't', '>' };
static const Pt::Char XMLRPC_MEMBER[]  = { '<', 'm', 'e', 'm', 'b', 'e', 'r', '>' };
static const Pt::Char XMLRPC_NAME[]    = { '<', 'n', 'a', 'm', 'e', '>' };
static const Pt::Char XMLRPC_ARRAY[]   = { '<', 'a', 'r', 'r', 'a', 'y', '>' };
static const Pt::Char XMLRPC_DATA[]    = { '<', 'd', 'a', 't', 'a', '>' };

static const Pt::Char XMLRPC_VALUE_END[]   = { '<', '/', 'v', 'a', 'l', 'u', 'e', '>' };
static const Pt::Char XMLRPC_INT_END[]     = { '<', '/', 'i', 'n', 't', '>' };
static const Pt::Char XMLRPC_DOUBLE_END[]  = { '<', '/', 'd', 'o', 'u', 'b', 'l', 'e', '>' };
static const Pt::Char XMLRPC_STRING_END[]  = { '<', '/', 's', 't', 'r', 'i', 'n', 'g', '>' };
static const Pt::Char XMLRPC_BOOLEAN_END[] = { '<', '/', 'b', 'o', 'o', 'l', 'e', 'a', 'n', '>' };
static const Pt::Char XMLRPC_STRUCT_END[]  = { '<', '/', 's', 't', 'r', 'u', 'c', 't', '>' };
static const Pt::Char XMLRPC_MEMBER_END[]  = { '<', '/', 'm', 'e', 'm', 'b', 'e', 'r', '>' };
static const Pt::Char XMLRPC_NAME_END[]    = { '<', '/', 'n', 'a', 'm', 'e', '>' };
static const Pt::Char XMLRPC_ARRAY_END[]   = { '<', '/', 'a', 'r', 'r', 'a', 'y', '>' };
static const Pt::Char XMLRPC_DATA_END[]    = { '<', '/', 'd', 'a', 't', 'a', '>' };


template<typename T>
class array_appender : public std::iterator<std::output_iterator_tag, T>
{
    public:
		array_appender()
		: _ptr(0)
		, _end(0)
		{ }

		array_appender(T* ptr, std::size_t length)
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

    T* getPointer()
    { return _ptr; }

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
                          const Pt::Char* value, const char* id)
{
    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_STRING, sizeof(XMLRPC_STRING)/sizeof(Char));
    Xml::xmlEncode(*_os, value);
    _os->write(XMLRPC_STRING_END, sizeof(XMLRPC_STRING_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::addString8(const char* name, const char* value, const char* id)
{
    _str.assign(value);
    
    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_STRING, sizeof(XMLRPC_STRING)/sizeof(Char));
    Xml::xmlEncode(*_os, _str);
    _os->write(XMLRPC_STRING_END, sizeof(XMLRPC_STRING_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::addBool(const char* name, bool value, 
                        const char* id)
{
    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));

    _os->write(XMLRPC_BOOLEAN, sizeof(XMLRPC_BOOLEAN)/sizeof(Char));
    *_os << (value ? Char('1') : Char('0'));
    _os->write(XMLRPC_BOOLEAN_END, sizeof(XMLRPC_BOOLEAN_END)/sizeof(Char));

    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::addChar(const char* name, const Pt::Char& value,
                        const char* id)
{
    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_STRING, sizeof(XMLRPC_STRING)/sizeof(Char));
    Xml::xmlEncode(*_os, &value, 1);
    _os->write(XMLRPC_STRING_END, sizeof(XMLRPC_STRING_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::addChar8(const char* name, char ch,
                         const char* id)
{
    Pt::Char value(ch);

    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_STRING, sizeof(XMLRPC_STRING)/sizeof(Char));
    Xml::xmlEncode(*_os, &value, 1);
    _os->write(XMLRPC_STRING_END, sizeof(XMLRPC_STRING_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
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
    array_appender<Pt::Char> it(_buf, _bufsize);
    it = putInt(it, value);

    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_INT, sizeof(XMLRPC_INT)/sizeof(Char));
    _os->write(_buf, it.getPointer() - _buf);
    _os->write(XMLRPC_INT_END, sizeof(XMLRPC_INT_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
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
    array_appender<Pt::Char> it(_buf, _bufsize);
    it = putInt(it, value);

    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_INT, sizeof(XMLRPC_INT)/sizeof(Char));
    _os->write( _buf, it.getPointer() - _buf );
    _os->write(XMLRPC_INT_END, sizeof(XMLRPC_INT_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::addFloat(const char* name, float value,const char* id)
{
    // spec supports only double precision floats
    this->addDouble(name, value, id);
}


void Formatter::addDouble(const char* name, double value, const char* id)
{
    array_appender<Pt::Char> it(_buf, _bufsize);
    it = putFloat(it, value);

    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));

    _os->write(XMLRPC_DOUBLE, sizeof(XMLRPC_DOUBLE)/sizeof(Char));
    _os->write(_buf, it.getPointer() - _buf);
    _os->write(XMLRPC_DOUBLE_END, sizeof(XMLRPC_DOUBLE_END)/sizeof(Char));

    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::addLongDouble(const char* name, long double value,const char* id)
{
    // spec supports only double precision floats
    this->addDouble(name, static_cast<double>(value), id);
}


void Formatter::addBytes(const char* name, const char* type,
                         const char* data, std::size_t length, const char* id)
{
    // TODO: this should be base64 encoded

    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    std::string value(data, length);

    throw std::logic_error("base64 data not supported");
    //_writer->writeStartTag(Pt::String::widen(type).c_str());
    //Xml::xmlEncode(Pt::String::widen(value).c_str());
    //_writer->writeEndTag(Pt::String::widen(type).c_str());

    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::addReference(const char* name, const char*value)
{
    throw SerializationError("references not supported");
}


void Formatter::beginArray(const char*, const char*,
                           const char*)
{
    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_ARRAY, sizeof(XMLRPC_ARRAY)/sizeof(Char));
    _os->write(XMLRPC_DATA, sizeof(XMLRPC_DATA)/sizeof(Char));
}


void Formatter::beginElement(const char* type, const char* id)
{
}


void Formatter::finishElement()
{
}


void Formatter::finishArray()
{
    _os->write(XMLRPC_DATA_END, sizeof(XMLRPC_DATA_END)/sizeof(Char));
    _os->write(XMLRPC_ARRAY_END, sizeof(XMLRPC_ARRAY_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::beginObject(const char* name, const char* type,
                            const char* id)
{
    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_STRUCT, sizeof(XMLRPC_STRUCT)/sizeof(Char));
}


void Formatter::beginMember(const char* name, const char*, const char*)
{
    _str.assign(name);

    _os->write(XMLRPC_MEMBER, sizeof(XMLRPC_MEMBER)/sizeof(Char));
    _os->write(XMLRPC_NAME, sizeof(XMLRPC_NAME)/sizeof(Char));
    Xml::xmlEncode(*_os, _str );
    _os->write(XMLRPC_NAME_END, sizeof(XMLRPC_NAME_END)/sizeof(Char));
}


void Formatter::finishMember()
{
    _os->write(XMLRPC_MEMBER_END, sizeof(XMLRPC_MEMBER_END)/sizeof(Char));
}


void Formatter::finishObject()
{
    _os->write(XMLRPC_STRUCT_END, sizeof(XMLRPC_STRUCT_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}

}

}
