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

#include <Pt/Soap/Formatter.h>
#include <Pt/Soap/ServiceDeclaration.h>
#include <Pt/Xml/XmlWriter.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/SerializationError.h>
#include <Pt/Base64Codec.h>
#include <Pt/Convert.h>
#include <iterator>
#include <cassert>
#include <cstddef>

#define PT_LOG_DEFINE(e)
#define PT_LOG_DEBUG(e)
PT_LOG_DEFINE("Pt.Soap.Formatter")

namespace  {

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

} // namespace

namespace Pt {

namespace Soap {

Formatter::Formatter(std::basic_ostream<Char>& os)
: _state(OnBegin)
, _reader(0)
, _composer(0)
, _os(&os)
{ 
}


Formatter::~Formatter()
{
}


void Formatter::setParameter(const Parameter& p)
{
    _paramStack.clear();
    _paramStack.push_back(&p);
}


void Formatter::attach(Xml::XmlReader& reader)
{ 
    _reader = &reader; 
}


void Formatter::attach(std::basic_ostream<Char>& os)
{ 
    _os = &os; 
}


void Formatter::onAddString(const char* name, const char* type,
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


void Formatter::onAddBool(const char* name, bool value, 
                              const char* id)
{
    Char val = value ? Char('1') : Char('0');
    formatValue(*_os, _paramStack.back()->name(), &val, 1 );
}


void Formatter::onAddChar(const char* name, const Pt::Char& value,
                              const char* id)
{
    Pt::Char buf[2] = { value, 0 };
    onAddString(name, "", buf, id);
}


void Formatter::onAddInt8(const char* name, Pt::int8_t value, const char* id)
{
	this->onAddInt64(name, value, id);
}


void Formatter::onAddInt16(const char* name, Pt::int16_t value, const char* id)
{
	this->onAddInt64(name, value, id);
}    


void Formatter::onAddInt32(const char* name, Pt::int32_t value, const char* id)
{
	this->onAddInt64(name, value, id);
}


void Formatter::onAddInt64(const char* name, Pt::int64_t value, const char* id)
{    
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];
        
    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatInt(it, value);

    formatValue(*_os, _paramStack.back()->name(), _buf, it.getPointer() - _buf );
}


void Formatter::onAddUInt8(const char* name, Pt::uint8_t value, const char* id)
{
	this->onAddUInt64(name, value, id);
}


void Formatter::onAddUInt16(const char* name, Pt::uint16_t value, const char* id)
{
	this->onAddUInt64(name, value, id);
}    


void Formatter::onAddUInt32(const char* name, Pt::uint32_t value, const char* id)
{
	this->onAddUInt64(name, value, id);
}


void Formatter::onAddUInt64(const char* name, Pt::uint64_t value, const char* id)
{    
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];

    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatInt(it, value);

    formatValue(*_os, _paramStack.back()->name(), _buf, it.getPointer() - _buf );
}


void Formatter::onAddFloat(const char* name, float value,const char* id)
{
    this->onAddLongDouble(name, value, id);
}


void Formatter::onAddDouble(const char* name, double value, const char* id)
{
    this->onAddLongDouble(name, value, id);
}


void Formatter::onAddLongDouble(const char* name, long double value,const char* id)
{
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];

    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatFloat(it, value);

    formatValue(*_os, _paramStack.back()->name(), _buf, it.getPointer() - _buf );
}


void Formatter::onAddBinary(const char* name, const char* type,
                            const char* data, std::size_t length, const char* id)
{
  _data.resize( (2*length) + 4 );
            
  Pt::MBState state;
  const char* nextFrom = 0;
  char* nextTo = 0;
  Pt::Base64Codec::result r;

  Pt::Base64Codec b64;
  r = b64.out(state, 
              data, data+length, nextFrom, 
              &_data[0], &_data[0] + _data.size(), nextTo);
            
  if(r == Pt::Base64Codec::error)
    throw SerializationError("base64 decoding");

  b64.unshift(state, nextTo, &_data[0] + _data.size(), nextTo);

  Pt::String value(&_data[0], nextTo - &_data[0]);
  formatValue(*_os, _paramStack.back()->name(), value.c_str(), value.size() );
}


void Formatter::onAddReference(const char* name, const char*value)
{
    throw SerializationError("references not supported");
}


void Formatter::onBeginSequence(const char* name, const char* type, const char*)
{
    _str.assign( _paramStack.back()->name().c_str() );

    *_os << '<';
    Xml::xmlEncode(*_os, _str );
    *_os << '>';
}


void Formatter::onBeginElement()
{
    const Parameter* param = _paramStack.back()->type()->getParameter(0);
    if( ! param )
        throw SerializationError("invalid sequence type");

    _paramStack.push_back(param);
}


void Formatter::onFinishElement()
{   
    _paramStack.pop_back();

    if( _paramStack.empty() )
        throw SerializationError("invalid sequence type");
}


void Formatter::onFinishSequence()
{
    _str.assign( _paramStack.back()->name().c_str() );

    *_os << '<' << '/';
    Xml::xmlEncode(*_os, _str );
    *_os << '>';
}


void Formatter::onBeginStruct(const char* name, const char* type,
                                  const char* id)
{
    _str.assign( _paramStack.back()->name().c_str() );

    *_os << '<';
    Xml::xmlEncode(*_os, _str );
    *_os << '>';
}


void Formatter::onBeginMember(const char* name)
{
    const Parameter* param = _paramStack.back()->type()->getParameter(name);
    if( ! param )
        throw SerializationError("invalid sequence type");

    _paramStack.push_back(param);
}


void Formatter::onFinishMember()
{
    _paramStack.pop_back();

    if( _paramStack.empty() )
      throw SerializationError("invalid struct type");
}


void Formatter::onFinishStruct()
{
    _str.assign( _paramStack.back()->name().c_str() );

    *_os << '<' << '/';
    Xml::xmlEncode(*_os, _str );
    *_os << '>';
}


void Formatter::onBeginDict(const char* name, const char* type,
                            const char* id)
{
    _str.assign( _paramStack.back()->name().c_str() );

    *_os << '<';
    Xml::xmlEncode(*_os, _str );
    *_os << '>';
}


void Formatter::onBeginDictElement()
{
    const Parameter* param = _paramStack.back()->type()->getParameter(0);
    if( ! param )
        throw SerializationError("invalid dict type");

    _paramStack.push_back(param);

    _str.assign( param->name().c_str() );

    *_os << '<';
    Xml::xmlEncode(*_os, _str );
    *_os << '>';
}


void Formatter::onBeginDictKey()
{
    const Parameter* param = _paramStack.back()->type()->getParameter(0);
    if( ! param )
        throw SerializationError("invalid dict type");

    _paramStack.push_back(param);
}


void Formatter::onFinishDictKey()
{
    _paramStack.pop_back();

    if( _paramStack.empty() )
      throw SerializationError("invalid dict type");
}


void Formatter::onBeginDictValue()
{
    const Parameter* param = _paramStack.back()->type()->getParameter(1);
    if( ! param )
        throw SerializationError("invalid dict type");

    _paramStack.push_back(param);
}


void Formatter::onFinishDictValue()
{
    _paramStack.pop_back();

    if( _paramStack.empty() )
      throw SerializationError("invalid dict type");
}


void Formatter::onFinishDictElement()
{
    _str.assign( _paramStack.back()->name().c_str() );

    *_os << '<' << '/';
    Xml::xmlEncode(*_os, _str );
    *_os << '>';

    _paramStack.pop_back();

    if( _paramStack.empty() )
        throw SerializationError("invalid dict type");
}


void Formatter::onFinishDict()
{
    _str.assign( _paramStack.back()->name().c_str() );

    *_os << '<' << '/';
    Xml::xmlEncode(*_os, _str );
    *_os << '>';
}


void Formatter::onBeginParse(Composer& composer)
{
    _str.clear();
    _state = OnBegin;
    _composer = &composer;
}


bool Formatter::onParseSome()
{ 
    return false; 
}


void Formatter::onParse()
{
}


bool Formatter::advance(const Pt::Xml::Node& node)
{
    if(node.type() == Xml::Node::StartElement)
    {
        const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

        const Type::TypeId typeId = _paramStack.back()->type()->typeId();
                
        if(typeId == Type::Struct)
        {
            _composer = _composer->beginMember( se.name().local().narrow() );
        }
        else if(typeId == Type::Array)
        {
            _composer = _composer->beginElement();
        }
        else if(typeId == Type::Dict)
        {
            _composer = _composer->beginDictElement();
        }
        else if(typeId == Type::DictElement)
        {
            if( _state != OnDictElement )
              _composer = _composer->beginDictKey();
            else
              _composer = _composer->beginDictValue();
        }

        const Parameter* child = _paramStack.back()->type()->getParameter( se.name().local().narrow() );
        if( ! child)
            throw SerializationError("invalid compound type");
        
        _paramStack.push_back(child);
        _state = OnStartElement;
    }
    else if(node.type() == Xml::Node::Characters)
    {
        const Xml::Characters& c = static_cast<const Xml::Characters&>(node);

        const Type::TypeId typeId = _paramStack.back()->type()->typeId();
        if(typeId == Type::Bool)
        {
            const Pt::String& strval = c.content();
            bool value = false;
                
            if( strval == "1" || strval == "true")
                value = true;
            else if(strval == "0" || strval == "false")
                value = false;
            else
                throw SerializationError("invalid boolean parameter");

            _composer->setBool(value);
        }
        else if(typeId == Type::Int)
        {
            Pt::int64_t number = 0;
            bool ok = false;
            parseInt( c.content().begin(), c.content().end(), number, ok);

            if( ! ok )
                throw SerializationError("invalid integer parameter");

            _composer->setInt(number);
        }
        else if(typeId == Type::Float)
        {
            double number = 0.0;
            bool ok = false;
            parseFloat( c.content().begin(), c.content().end(), number, ok);

            if( ! ok )
                throw SerializationError("invalid float parameter");

            _composer->setFloat(number);
        }
        else if(typeId == Type::String)
        {
            if( ! c.isChunk() )
                _composer->setString( c.content() );
            else
                _str += c.content(); 
        }
        else if(typeId == Type::Base64)
        {
            std::string from = c.content().narrow();
            _data.resize( from.size() );
            
            Pt::MBState state;
            const char* nextFrom = 0;
            char* nextTo = 0;
            Pt::Base64Codec::result r;

            Pt::Base64Codec b64;
            r = b64.in(state, 
                       from.c_str(), from.c_str() + from.size(), nextFrom, 
                       &_data[0], &_data[0] + _data.size(), nextTo);
            
            if(r != Pt::Base64Codec::ok)
              throw SerializationError("invalid base64 decoding");

            _composer->setBinary( &_data[0], nextTo - &_data[0] );
        }

        _state = OnCharacters;
    }
    else if(node.type() == Xml::Node::EndElement)
    {
        const Type::TypeId typeId = _paramStack.back()->type()->typeId();

        // handle empty elements
        if(_state == OnStartElement || _state == OnBegin)
        {
            if(typeId == Type::String)
                _composer->setString( Pt::String() );
            else if(typeId == Type::Base64)
                _composer->setBinary("", 0);
        }

        if( ! _str.empty() )
        {
            _composer->setString(_str);
            _str.clear();
        }

        _composer = _composer->finish();

        _paramStack.pop_back();

        _state = OnEndElement;

        // handle dict element value in next start element
        if( ! _paramStack.empty() )
          if( _paramStack.back()->type()->typeId() == Type::DictElement )
            _state = OnDictElement;
    }

    return _paramStack.size() == 0;
}

} // namespace Soap

} // namespace Pt
