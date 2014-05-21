/*
 * Copyright (C) 2014 by Dr. Marc Boris Duerner
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
#include <Pt/XmlRpc/SoapFormatter.h>
#include <Pt/XmlRpc/SoapServiceDefinition.h>
#include <Pt/Xml/XmlWriter.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/Convert.h>
#include <Pt/SerializationError.h>
#include <limits>
#include <cassert>
#include <cmath>
#include <cstddef>

#define log_define(e)
#define log_debug(e)
log_define("Pt.XmlRpc.SoapFormatter")

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


void formatValue(std::basic_ostream<Pt::Char>& os, const std::string& name, const Pt::Char* value, std::size_t valueSize)
{
    os << '<';
    for(std::size_t n = 0; n < name.size(); ++n)
        os << name[n];
    os << '>';
    
    os.write(value, valueSize);
    
    os << '<' << '/';
    for(std::size_t n = 0; n < name.size(); ++n)
        os << name[n];
    os << '>';
}

}

namespace Pt {

namespace XmlRpc {

SoapFormatter::SoapFormatter(std::basic_ostream<Char>& os)
: _reader(0)
, _paramType(0)
, _composer(0)
, _os(&os)
{ 
}


SoapFormatter::~SoapFormatter()
{
}


void SoapFormatter::setParameter(const Type& p)
{
    _paramType = &p;
}


void SoapFormatter::setParameter(const Parameter& p)
{
    _paramStack.clear();
    _paramStack.push_back(&p);
}


void SoapFormatter::attach(Xml::XmlReader& reader)
{ 
    _reader = &reader; 
}


void SoapFormatter::attach(std::basic_ostream<Char>& os)
{ 
    _os = &os; 
}


void SoapFormatter::onAddString(const char* name, const char* type,
                                const Pt::Char* value, const char* id)
{
    const std::string& paramName = _paramStack.back()->name();

    *_os << '<';
    for(std::size_t n = 0; n < paramName.size(); ++n)
        *_os << paramName[n];
    *_os << '>';
    
    Xml::xmlEncode(*_os, value);
    
    *_os << '<' << '/';
    for(std::size_t n = 0; n < paramName.size(); ++n)
        *_os << paramName[n];
    *_os << '>';
}


void SoapFormatter::onAddBool(const char* name, bool value, 
                              const char* id)
{
    Char val = value ? Char('1') : Char('0');
    formatValue(*_os, _paramStack.back()->name(), &val, 1 );
}


void SoapFormatter::onAddChar(const char* name, const Pt::Char& value,
                              const char* id)
{
    const std::string& paramName = _paramStack.back()->name();

    *_os << '<';
    for(std::size_t n = 0; n < paramName.size(); ++n)
        *_os << paramName[n];
    *_os << '>';
    
    Xml::xmlEncode(*_os, &value, 1);
    
    *_os << '<' << '/';
    for(std::size_t n = 0; n < paramName.size(); ++n)
        *_os << paramName[n];
    *_os << '>';
}


void SoapFormatter::onAddInt8(const char* name, Pt::int8_t value, const char* id)
{
	this->onAddInt64(name, value, id);
}


void SoapFormatter::onAddInt16(const char* name, Pt::int16_t value, const char* id)
{
	this->onAddInt64(name, value, id);
}    


void SoapFormatter::onAddInt32(const char* name, Pt::int32_t value, const char* id)
{
	this->onAddInt64(name, value, id);
}


void SoapFormatter::onAddInt64(const char* name, Pt::int64_t value, const char* id)
{    
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];
        
    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatInt(it, value);

    formatValue(*_os, _paramStack.back()->name(), _buf, it.getPointer() - _buf );
}


void SoapFormatter::onAddUInt8(const char* name, Pt::uint8_t value, const char* id)
{
	this->onAddUInt64(name, value, id);
}


void SoapFormatter::onAddUInt16(const char* name, Pt::uint16_t value, const char* id)
{
	this->onAddUInt64(name, value, id);
}    


void SoapFormatter::onAddUInt32(const char* name, Pt::uint32_t value, const char* id)
{
	this->onAddUInt64(name, value, id);
}


void SoapFormatter::onAddUInt64(const char* name, Pt::uint64_t value, const char* id)
{    
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];

    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatInt(it, value);

    formatValue(*_os, _paramStack.back()->name(), _buf, it.getPointer() - _buf );
}


void SoapFormatter::onAddFloat(const char* name, float value,const char* id)
{
    this->onAddLongDouble(name, value, id);
}


void SoapFormatter::onAddDouble(const char* name, double value, const char* id)
{
    this->onAddLongDouble(name, value, id);
}


void SoapFormatter::onAddLongDouble(const char* name, long double value,const char* id)
{
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];

    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatFloat(it, value);

    formatValue(*_os, _paramStack.back()->name(), _buf, it.getPointer() - _buf );
}


void SoapFormatter::onAddBinary(const char* name, const char* type,
                            const char* data, std::size_t length, const char* id)
{
    // TODO: this should be base64 encoded
    throw SerializationError("binary type not supported");
}


void SoapFormatter::onAddReference(const char* name, const char*value)
{
    throw SerializationError("references not supported");
}


void SoapFormatter::onBeginSequence(const char* name, const char* type, const char*)
{
    _str.assign( _paramStack.back()->name().c_str() );

    *_os << '<';
    Xml::xmlEncode(*_os, _str );
    *_os << '>';
}


void SoapFormatter::onBeginElement()
{
    const Parameter* param = _paramStack.back()->type()->getParameter(0);
    _paramStack.push_back(param);
}


void SoapFormatter::onFinishElement()
{
    _paramStack.pop_back();
}


void SoapFormatter::onFinishSequence()
{
    _str.assign( _paramStack.back()->name().c_str() );

    *_os << '<' << '/';
    Xml::xmlEncode(*_os, _str );
    *_os << '>';
}


void SoapFormatter::onBeginStruct(const char* name, const char* type,
                                  const char* id)
{
    _str.assign( _paramStack.back()->name().c_str() );

    *_os << '<';
    Xml::xmlEncode(*_os, _str );
    *_os << '>';
}


void SoapFormatter::onBeginMember(const char* name)
{
    const Parameter* param = _paramStack.back()->type()->getParameter(name);
    _paramStack.push_back(param);
}


void SoapFormatter::onFinishMember()
{
    _paramStack.pop_back();
}


void SoapFormatter::onFinishStruct()
{
    _str.assign( _paramStack.back()->name().c_str() );

    *_os << '<' << '/';
    Xml::xmlEncode(*_os, _str );
    *_os << '>';
}


void SoapFormatter::onBeginParse(Composer& composer)
{
    _composer = &composer;
}


bool SoapFormatter::onParseSome()
{ 
    return false; 
}


void SoapFormatter::onParse()
{
}


bool SoapFormatter::advance(const Pt::Xml::Node& node)
{
    _paramType = _paramType->parse(node, _composer);

    if( ! _paramType )
        return true;

    return false;
}

} // namespace Xml

} // namespace Pt
