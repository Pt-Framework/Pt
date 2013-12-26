/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
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
log_define("Pt.XmlRpc.Formatter")

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


void throwSerializationError(const char* msg = "invalid XML-RPC parameter")
{
    throw Pt::SerializationError(msg);
}

}

namespace Pt {

namespace XmlRpc {

Formatter::Formatter(std::basic_ostream<Char>& os)
: _reader(0)
, _state(OnParam)
, _composer(0)
, _os(&os)
{ 
}


Formatter::~Formatter()
{
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
    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_STRING, sizeof(XMLRPC_STRING)/sizeof(Char));
    Xml::xmlEncode(*_os, value);
    _os->write(XMLRPC_STRING_END, sizeof(XMLRPC_STRING_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::onAddBool(const char* name, bool value, 
                          const char* id)
{
    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));

    _os->write(XMLRPC_BOOLEAN, sizeof(XMLRPC_BOOLEAN)/sizeof(Char));
    *_os << (value ? Char('1') : Char('0'));
    _os->write(XMLRPC_BOOLEAN_END, sizeof(XMLRPC_BOOLEAN_END)/sizeof(Char));

    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::onAddChar(const char* name, const Pt::Char& value,
                          const char* id)
{
    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_STRING, sizeof(XMLRPC_STRING)/sizeof(Char));
    Xml::xmlEncode(*_os, &value, 1);
    _os->write(XMLRPC_STRING_END, sizeof(XMLRPC_STRING_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
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

    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_INT, sizeof(XMLRPC_INT)/sizeof(Char));
    _os->write(_buf, it.getPointer() - _buf);
    _os->write(XMLRPC_INT_END, sizeof(XMLRPC_INT_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
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

    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_INT, sizeof(XMLRPC_INT)/sizeof(Char));
    _os->write( _buf, it.getPointer() - _buf );
    _os->write(XMLRPC_INT_END, sizeof(XMLRPC_INT_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::onAddFloat(const char* name, float value,const char* id)
{
    // spec supports only double precision floats
    this->onAddDouble(name, value, id);
}


void Formatter::onAddDouble(const char* name, double value, const char* id)
{
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];

    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatFloat(it, value);

    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));

    _os->write(XMLRPC_DOUBLE, sizeof(XMLRPC_DOUBLE)/sizeof(Char));
    _os->write(_buf, it.getPointer() - _buf);
    _os->write(XMLRPC_DOUBLE_END, sizeof(XMLRPC_DOUBLE_END)/sizeof(Char));

    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::onAddLongDouble(const char* name, long double value,const char* id)
{
    // spec supports only double precision floats
    this->onAddDouble(name, static_cast<double>(value), id);
}


void Formatter::onAddBinary(const char* name, const char* type,
                            const char* data, std::size_t length, const char* id)
{
    // TODO: this should be base64 encoded

    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    std::string value(data, length);

    throw SerializationError("base64 data not supported");
    //_writer->writeStartTag(Pt::String::widen(type).c_str());
    //Xml::xmlEncode(Pt::String::widen(value).c_str());
    //_writer->writeEndTag(Pt::String::widen(type).c_str());

    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::onAddReference(const char* name, const char*value)
{
    throw SerializationError("references not supported");
}


void Formatter::onBeginSequence(const char*, const char*,
                                const char*)
{
    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_ARRAY, sizeof(XMLRPC_ARRAY)/sizeof(Char));
    _os->write(XMLRPC_DATA, sizeof(XMLRPC_DATA)/sizeof(Char));
}


void Formatter::onBeginElement()
{
}


void Formatter::onFinishElement()
{
}


void Formatter::onFinishSequence()
{
    _os->write(XMLRPC_DATA_END, sizeof(XMLRPC_DATA_END)/sizeof(Char));
    _os->write(XMLRPC_ARRAY_END, sizeof(XMLRPC_ARRAY_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::onBeginStruct(const char* name, const char* type,
                             const char* id)
{
    _os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    _os->write(XMLRPC_STRUCT, sizeof(XMLRPC_STRUCT)/sizeof(Char));
}


void Formatter::onBeginMember(const char* name)
{
    _str.assign(name);

    _os->write(XMLRPC_MEMBER, sizeof(XMLRPC_MEMBER)/sizeof(Char));
    _os->write(XMLRPC_NAME, sizeof(XMLRPC_NAME)/sizeof(Char));
    Xml::xmlEncode(*_os, _str );
    _os->write(XMLRPC_NAME_END, sizeof(XMLRPC_NAME_END)/sizeof(Char));
}


void Formatter::onFinishMember()
{
    _os->write(XMLRPC_MEMBER_END, sizeof(XMLRPC_MEMBER_END)/sizeof(Char));
}


void Formatter::onFinishStruct()
{
    _os->write(XMLRPC_STRUCT_END, sizeof(XMLRPC_STRUCT_END)/sizeof(Char));
    _os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void Formatter::onBeginParse(Composer& composer)
{
    _state = OnParam;
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
    switch(_state)
    {
        case OnParam:
        {
            log_debug("OnParam");
            if(node.type() == Xml::Node::StartElement) // i4, struct, array...
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                if( se.name().name() != L"value" )
                    throwSerializationError();

                _state = OnValueBegin;
            }
            else if(node.type() == Xml::Node::EndElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnValueBegin:
        {
            log_debug("OnValueBegin, node type " << node.type());
            if(node.type() == Xml::Node::StartElement) // i4, struct, array...
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                log_debug("-> found type " << se.name().narrow());
                if( se.name().name() == "struct" )
                {
                    _state = OnStructBegin;
                }
                else if(se.name().name() == "array")
                {
                    _state = OnArrayBegin;
                }
                else if(se.name().name() == "int" || se.name().name() == "i4")
                {
                    _state = OnIntBegin;
                }
                else if(se.name().name() == "boolean")
                {
                    _state = OnBoolBegin;
                }
                else if(se.name().name() == "double")
                {
                    _state = OnDoubleBegin;
                }
                else
                {
                    _state = OnStringBegin;
                }

                _str.clear();
            }
            else if(node.type() == Xml::Node::Characters)
            {
                // maybe <value>...<type>...</type>...</value>  (case 1)
                //    or <value>...</value>                     (case 2)
                const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                _str = chars.content();
                
                //NOTE we could get rid of this is the XmlReader could be set up
                //     to ignore characters between two start tags and only report
                //     "leaf characters".
            }
            else if(node.type() == Xml::Node::EndElement)
            {
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                //if(ee.name() != L"value")
                //    throwSerializationError();

                // is always type string
                _composer->setString( _str );
                _str.clear();

                _state = OnValueEnd;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnValueEnd:
        {
            log_debug("OnValueEnd, node type " << node.type());

            if(node.type() == Xml::Node::EndElement)
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);

                if( ee.name().name() == "member" )
                {
                    log_debug("OnValueEnd member");
                    _composer = _composer->finish();
                    if( ! _composer )
                        throwSerializationError("invalid XML-RPC struct");

                    _state = OnStructBegin;
                }
                else if( ee.name().name() == "data" )
                {
                    log_debug("OnValueEnd data");
                    _composer = _composer->finish();
                    if( ! _composer )
                        throwSerializationError("invalid XML-RPC array");

                    _state = OnDataEnd;
                }
                else if( ee.name().name() == "param" )
                {
                    log_debug("OnValueEnd data other " << ee.name().narrow());
                    if( 0 != _composer->finish() )
                        throwSerializationError();

                    _state = OnValueEnd;
                    return true;
                }
                else if( ee.name().name() == "fault" )
                {
                    log_debug("OnValueEnd data other " << ee.name().narrow());
                    if( 0 != _composer->finish() )
                        throwSerializationError("invalid XML-RPC fault");

                    _state = OnValueEnd;
                    return true;
                }
                else
                {
                    throwSerializationError();
                }
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if(se.name().name() == "value")
                {
                    log_debug("OnValueEnd data value");
                    _composer = _composer->finish();

                    if( ! _composer )
                        throwSerializationError("invalid XML-RPC element");

                    _composer = _composer->beginElement();
                    _state = OnValueBegin;
                }
                else
                {
                    throwSerializationError();
                }
            }

            break;
        }

        case OnStructBegin:
        {
            log_debug("OnStructBegin");
            if(node.type() == Xml::Node::StartElement) // <member>
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                if(se.name().name() != L"member")
                    throwSerializationError();

                _state = OnMemberBegin;
            }
            else if(node.type() == Xml::Node::EndElement) // </struct>
            {
                _state = OnStructEnd;
            }
            break;
        }

        case OnStructEnd:
        {
            log_debug("OnStructEnd");
            if(node.type() == Xml::Node::EndElement) // </value>
            {
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);

                //if(ee.name() != L"value")
                //    throwSerializationError();

                _state = OnValueEnd;
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnMemberBegin:
        {
            log_debug("OnMemberBegin");
            if(node.type() == Xml::Node::StartElement) // name
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                if( se.name().name() != L"name")
                    throwSerializationError();

                _state = OnNameBegin;
            }
            else if(node.type() == Xml::Node::EndElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnNameBegin:
        {
            log_debug("OnNameBegin");
            if(node.type() == Xml::Node::Characters) // member-name
            {
                const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                const std::string& name = chars.content().narrow();

                _composer = _composer->beginMember(name);

                _state = OnName;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnName:
        {
            log_debug("OnName");
            if(node.type() == Xml::Node::EndElement) // </name>
            {
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);

                //if(ee.name() != L"name")
                //    throwSerializationError();

                _state = OnNameEnd;
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnNameEnd:
        {
            log_debug("OnNameEnd");
            if(node.type() == Xml::Node::StartElement) // <value>
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                if( se.name().name() != L"value" )
                    throwSerializationError();

                _state = OnValueBegin;
            }
            else if(node.type() == Xml::Node::EndElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnBoolBegin:
        {
            log_debug("OnBoolBegin ");
            if(node.type() == Xml::Node::Characters)
            {
                const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                log_debug("-> found bool " << chars.content().narrow());

                bool value = false;
                const Pt::String& strval = chars.content();
                Pt::String::const_iterator it = strval.begin();

                // skip leading whitespace
                for( ; it != strval.end(); it++)
                    if( ! Pt::isspace(*it) )
                        break;

                if( it == strval.end() )
                    throwSerializationError();

                if(*it == '0')
                    value = false;
                else if(*it == '1')
                    value = true;
                else if(*it == 'f')
                {
                    if( ++it == strval.end() || *it != 'a')
                        throwSerializationError();
                    if( ++it == strval.end() || *it != 'l')
                        throwSerializationError();
                    if( ++it == strval.end() || *it != 's')
                        throwSerializationError();
                    if( ++it == strval.end() || *it != 'e')
                        throwSerializationError();

                    value = false;
                }
                else if(*it == 't')
                {
                    if( ++it == strval.end() || *it != 'r')
                        throwSerializationError();
                    if( ++it == strval.end() || *it != 'u')
                        throwSerializationError();
                    if( ++it == strval.end() || *it != 'e')
                        throwSerializationError();

                    value = true;
                }

                // allow only trailing whitespace
                for( ++it; it != strval.end(); it++)
                    if( ! Pt::isspace(*it) )
                        throwSerializationError();

                _composer->setBool(value);
                _state = OnScalar;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnIntBegin:
        {
            log_debug("OnIntBegin ");
            if(node.type() == Xml::Node::Characters)
            {
                const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                log_debug("-> found int " << chars.content().narrow());

                Pt::int32_t number = 0;
                bool ok = false;
                parseInt( chars.content().begin(), chars.content().end(), number, ok);

                if( ! ok )
                    throwSerializationError();

                _composer->setInt(number);
                _state = OnScalar;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnDoubleBegin:
        {
            log_debug("OnDoubleBegin ");
            if(node.type() == Xml::Node::Characters)
            {
                const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                log_debug("-> found double " << chars.content().narrow());

                double number = 0.0;
                bool ok = false;
                parseFloat( chars.content().begin(), chars.content().end(), number, ok);

                if( ! ok )
                    throwSerializationError();

                _composer->setFloat(number);
                log_debug("-> parsed double " << number);
                _state = OnScalar;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnStringBegin:
        {
            log_debug("OnStringBegin ");
            if(node.type() == Xml::Node::Characters)
            {
                const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);
                _state = OnScalar;

                log_debug("-> found string " << chars.content().narrow());
                _composer->setString( chars.content() );
            }
            else if(node.type() == Xml::Node::EndElement) // no content, for example empty strings
            {
                log_debug("-> found empty value ");
                _composer->setString( Pt::String() );
                _state = OnScalarEnd;
            }
            else
            {
                throwSerializationError();
            }

            break;
        }

        case OnScalar:
        {
            log_debug("OnScalar");
            if(node.type() == Xml::Node::EndElement) // </int>, boolean ...
            {
                _state = OnScalarEnd;
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnScalarEnd:
        {
            log_debug("OnScalarEnd");
            if(node.type() == Xml::Node::EndElement) // </value>
            {
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);

                //if(ee.name() != L"value")
                //    throwSerializationError();

                _state = OnValueEnd;
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnArrayBegin:
        {
            log_debug("OnArrayBegin");
            if(node.type() == Xml::Node::StartElement) // <data>
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                if( se.name().name() != L"data" )
                    throwSerializationError();

                _state = OnDataBegin;
            }
            else if(node.type() == Xml::Node::EndElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnDataBegin:
        {
            log_debug("OnDataBegin");
            if(node.type() == Xml::Node::StartElement) // value
            {
                _composer = _composer->beginElement();
                _state = OnValueBegin;
            }
            else if(node.type() == Xml::Node::EndElement) // empty array
            {
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                //if(ee.name() != L"data")
                //    throwSerializationError();

                _state = OnDataEnd;
            }

            break;
        }

        case OnDataEnd:
        {
            log_debug("OnDataEnd");
            if(node.type() == Xml::Node::EndElement) // </array>
            {
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);

                //if(ee.name() != L"array")
                //    throwSerializationError();

                _state = OnArrayEnd;
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }

        case OnArrayEnd:
        {
            log_debug("OnArrayEnd");
            if(node.type() == Xml::Node::EndElement) // </value>
            {
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);

                //if(ee.name() != L"value")
                //    throwSerializationError();

                _state = OnValueEnd;
            }
            else if(node.type() == Xml::Node::StartElement)
            {
                throwSerializationError();
            }

            break;
        }
    }

    return false;
}

} // namespace Xml

} // namespace Pt
