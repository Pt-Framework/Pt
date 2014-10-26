/*
 * Copyright (C) 2005-2013 by Dr. Marc Boris Duerner
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

#include "SettingsWriter.h"
#include <Pt/Convert.h>

namespace {

// TODO: use a formatter
Pt::String toStr(const Pt::SerializationInfo& si)
{
    Pt::String s;

    switch( si.type() )
    {
        case Pt::SerializationInfo::Str:
            si.getString(s);
            break;
        
        case Pt::SerializationInfo::Boolean:
            bool b;
            si.getBool(b);
            s = b ? "true" : "false" ;
            break;
    
        case Pt::SerializationInfo::Char:
        {
            Pt::Char c;
            si.getChar(c);
            s += c;
            break;
        }
    
        case Pt::SerializationInfo::Int8:
        case Pt::SerializationInfo::Int16:
        case Pt::SerializationInfo::Int32:
        case Pt::SerializationInfo::Int64:
            Pt::int64_t i;
            si.getInt64(i);
            Pt::formatInt(std::back_inserter(s), i);
            break;
    
        case Pt::SerializationInfo::UInt8:
        case Pt::SerializationInfo::UInt16:
        case Pt::SerializationInfo::UInt32:
        case Pt::SerializationInfo::UInt64:
            Pt::uint64_t u;
            si.getUInt64(u);
            Pt::formatInt(std::back_inserter(s), u);
            break;
    
        case Pt::SerializationInfo::Float:
        case Pt::SerializationInfo::Double:
        case Pt::SerializationInfo::LongDouble:
            long double d;
            si.getLongDouble(d);
            Pt::formatFloat(std::back_inserter(s), d);
            break;
        
        default:
            throw std::logic_error("conversion to string failed");
    }

    return s;
}

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

void formatName(std::basic_ostream<Pt::Char>& os, const char* name)
{
    if(*name)
    {
        while(*name)
            os << Pt::Char(*name++);

        os << Pt::Char(' ') << Pt::Char('=') << Pt::Char(' ') ;
    }
}

void formatIndent(std::basic_ostream<Pt::Char>& os, std::size_t level)
{
    std::size_t indent = level * 2;
    while(indent--)
        os << Pt::Char(' ');
}

} // namespace

namespace Pt {

SettingsFormatter::SettingsFormatter(std::basic_ostream<Char>& os)
: _os(&os)
, _state(0)
{ 
}


SettingsFormatter::~SettingsFormatter()
{
}


void SettingsFormatter::attach(std::basic_ostream<Char>& os)
{ 
    _os = &os; 
}


void SettingsFormatter::onAddString(const char* name, const char* type,
                                    const Pt::Char* value, const char* id)
{
    formatName(*_os, name);

    *_os << Char('"') << value << Char('"');
    
    if( _stack.empty() )
        *_os<< std::endl << std::endl;
}


void SettingsFormatter::onAddBool(const char* name, bool value, 
                          const char* id)
{
    //_os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));

    //_os->write(XMLRPC_BOOLEAN, sizeof(XMLRPC_BOOLEAN)/sizeof(Char));
    //*_os << (value ? Char('1') : Char('0'));
    //_os->write(XMLRPC_BOOLEAN_END, sizeof(XMLRPC_BOOLEAN_END)/sizeof(Char));

    //_os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void SettingsFormatter::onAddChar(const char* name, const Pt::Char& value,
                          const char* id)
{
    //_os->write(XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char));
    //_os->write(XMLRPC_STRING, sizeof(XMLRPC_STRING)/sizeof(Char));
    //Xml::xmlEncode(*_os, &value, 1);
    //_os->write(XMLRPC_STRING_END, sizeof(XMLRPC_STRING_END)/sizeof(Char));
    //_os->write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
}


void SettingsFormatter::onAddInt8(const char* name, Pt::int8_t value, const char* id)
{
    this->onAddInt64(name, value, id);
}


void SettingsFormatter::onAddInt16(const char* name, Pt::int16_t value, const char* id)
{
    this->onAddInt64(name, value, id);
}    


void SettingsFormatter::onAddInt32(const char* name, Pt::int32_t value, const char* id)
{
    this->onAddInt64(name, value, id);
}


void SettingsFormatter::onAddInt64(const char* name, Pt::int64_t value, const char* id)
{    
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];
        
    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatInt(it, value);

    formatName(*_os, name);
    
    _os->write(_buf, it.getPointer() - _buf);
    
    if( _stack.empty() )
        *_os<< std::endl << std::endl;
}


void SettingsFormatter::onAddUInt8(const char* name, Pt::uint8_t value, const char* id)
{
    this->onAddUInt64(name, value, id);
}


void SettingsFormatter::onAddUInt16(const char* name, Pt::uint16_t value, const char* id)
{
    this->onAddUInt64(name, value, id);
}    


void SettingsFormatter::onAddUInt32(const char* name, Pt::uint32_t value, const char* id)
{
    this->onAddUInt64(name, value, id);
}


void SettingsFormatter::onAddUInt64(const char* name, Pt::uint64_t value, const char* id)
{    
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];
        
    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatInt(it, value);

    formatName(*_os, name);
    
    _os->write(_buf, it.getPointer() - _buf);

    if( _stack.empty() )
        *_os<< std::endl << std::endl;
}


void SettingsFormatter::onAddFloat(const char* name, float value,const char* id)
{
    this->onAddDouble(name, value, id);
}


void SettingsFormatter::onAddDouble(const char* name, double value, const char* id)
{
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];
        
    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatFloat(it, value);

    formatName(*_os, name);
    
    _os->write(_buf, it.getPointer() - _buf);

    if( _stack.empty() )
        *_os<< std::endl << std::endl;
}


void SettingsFormatter::onAddLongDouble(const char* name, long double value,const char* id)
{
    this->onAddDouble(name, static_cast<double>(value), id);
}


void SettingsFormatter::onAddBinary(const char* name, const char* type,
                                    const char* data, std::size_t length, const char* id)
{
    throw SerializationError("binary data not supported");
}


void SettingsFormatter::onAddReference(const char* name, const char*value)
{
    throw SerializationError("references not supported");
}


void SettingsFormatter::onBeginSequence(const char* name, const char*,
                                        const char*)
{
    // endl and indent nested sequence 
    if(_state == '[' || _state == ']')
    {
       *_os << std::endl;
       formatIndent( *_os, _stack.size() );
    }

    formatName(*_os, name);
    *_os << Char('[');
    
    _stack.push_back(0);
    _state = '[';
}


void SettingsFormatter::onBeginElement()
{
    if( _stack.empty() )
        return;
    
    // add comma unless first element
    if( _stack.back() != 0 )
        *_os << Char(',') << Char(' ');

    // increase element count
    ++_stack.back();
}


void SettingsFormatter::onFinishElement()
{
}


void SettingsFormatter::onFinishSequence()
{
    _stack.pop_back();

    // no endl if sequence of scalars
    if(_state == ']')
    {
        *_os << std::endl;
        formatIndent( *_os, _stack.size() );
    }

    *_os << Char(']');

    // extra endl when root entry is finished
    if( _stack.empty() )
        *_os  << std::endl << std::endl;

    _state = _stack.empty() ? 0 : ']';
}


void SettingsFormatter::onBeginStruct(const char* name, const char* type,
                                      const char* id)
{
    // endl and indent nested sequence
    if(_state == '[' || _state == ']')
    {
       *_os << std::endl;
       formatIndent( *_os, _stack.size() );
    }

    formatName(*_os, name);
    *_os << Char('{');
    
    _stack.push_back(0);
    _state = 0;
}


void SettingsFormatter::onBeginMember(const char*)
{
    if( _stack.empty() )
        return;

    // add comma unless first member
    if( _stack.back() != 0 )
        *_os << Char(',') << Char(' ');

    // always endl after member
    *_os << std::endl;
    formatIndent( *_os, _stack.size() );
    _state = 0;

    // increase element count
    ++_stack.back();
}


void SettingsFormatter::onFinishMember()
{}


void SettingsFormatter::onFinishStruct()
{
    _stack.pop_back();

    // always endl after last member
    *_os << std::endl;
    formatIndent( *_os, _stack.size() );
    
    *_os << Char('}');

    // extra endl when root entry is finished
    if( _stack.empty() )
        *_os  << std::endl << std::endl; 

    _state = _stack.empty() ? 0 : ']';
}


//
// SettingsWriter
//

void SettingsWriter::write(const SerializationInfo& si)
{    
    SettingsFormatter formatter(*_os);

    SerializationInfo::ConstIterator it;
    for(it = si.begin(); it != si.end(); ++it)
    {
        it->format(formatter);
    }

    //Pt::String value;
    //SerializationInfo::ConstIterator it;
    //for(it = si.begin(); it != si.end(); ++it)
    //{
    //    if( it->isScalar() )
    //    {
    //        value = toStr(*it);
    //        this->writeEntry( it->name(), value, *it );
    //        *_os << std::endl;
    //    }
    //    else if( it->isStruct() || it->isSequence() )
    //    {
    //        // Array types may have no instance-names
    //        if( it->findMember("") )
    //        {
    //            *_os << Pt::String::widen( it->name() ) << Char(' ') << Char('=') << Char(' ');
    //            *_os << Char('{') << Char(' ');
    //            this->writeParent( *it, "");
    //            *_os << Char(' ');
    //            if(it->begin() != it->end() && ! it->begin()->isScalar() )
    //                *_os << std::endl;
    //            *_os << Char('}') << std::endl << std::endl;
    //            continue;
    //        }

    //        //this->writeSection( subdata->name() );
    //        this->writeParent( *it, it->name() );
    //    }
    //}
}


void SettingsWriter::writeParent(const SerializationInfo& sd, const std::string& prefix)
{
    Pt::String value;
    bool separate = false;

    SerializationInfo::ConstIterator it;
    for(it = sd.begin(); it != sd.end(); ++it)
    {
        if( it->isScalar() )
        {
            std::string name = it->name();

            // only comma separate array members (which have no name)
            if( separate && name.empty() )
                *_os << Char(',') << Char(' ');

             value = toStr(*it);
             if( ! prefix.empty() )
                *_os << Pt::String::widen( prefix ) << '.';

            this->writeEntry( name, value, *it );

            if(! name.empty() )
                *_os << std::endl;
        }
        else if( it->isStruct() || it->isSequence() )
        {
            if( separate )
                *_os << Char(',');
            
            *_os << std::endl << String("  ");

            if( it->name()[0] != '\0' )
                *_os << Pt::String::widen( prefix ) << Char('.') << Pt::String::widen( it->name() ) << Char(' ') << Char('=') << Char(' ');

            if( ! it->isSequence() )
                *_os << Pt::String::widen( it->typeName() );
                
            *_os << Char('{') << Char(' ');
            this->writeChild(*it);
            *_os << Char(' ') << Char('}');
        }

        separate = true;
    }
}


void SettingsWriter::writeChild(const SerializationInfo& sd)
{
    Pt::String value;
    bool separate = false;

    SerializationInfo::ConstIterator it;
    for(it = sd.begin(); it != sd.end(); ++it)
    {
        if(separate)
            *_os << Char(',') << Char(' ');

        if( it->isScalar() )
        {
            value = toStr(*it);
            this->writeEntry( it->name(), value, *it );
        }
        else if( it->isStruct() || it->isSequence() )
        {
            if( it->name()[0] != '\0' && ! sd.isSequence() )
                *_os << Pt::String::widen( it->name() ) << Char(' ') << Char('=') << Char(' ');

            *_os << Pt::String::widen( it->typeName() ) << Char('{') << Char(' ') ;
            this->writeChild(*it);
            *_os << Char(' ') << Char('}');
        }

        separate = true;
    }
}


void writeEscapedValue(std::basic_ostream<Pt::Char>& os, const Pt::String& value)
{
    for(size_t n = 0; n < value.size(); ++n)
    {
        switch( value[n].value() )
        {
            case '\\':
                os << Pt::Char('\\');

            default:
                os << value[n];
        }
    }
}


void SettingsWriter::writeEntry(const std::string& name, const Pt::String& value, const SerializationInfo& si)
{
    std::string type = si.typeName();

    if( type.empty() )
    {
        if( name.empty() == false)
            *_os << Pt::String::widen(name) << Char('=');

        if(si.type() == SerializationInfo::Str)
            *_os  << Char('\"');
        writeEscapedValue(*_os, value);

        if(si.type() == SerializationInfo::Str)
            *_os << Char('\"');

        return;
    }

    if( name.empty() == false)
        *_os << Pt::String::widen(name) << Char(' ') << Char('=') << Char(' ');

    *_os << Pt::String::widen(type) << Char('(') << Char('\"');
    writeEscapedValue(*_os, value);
    *_os << Char('\"') << Char(')');
}


void SettingsWriter::writeSection(const Pt::String& prefix)
{
    *_os << Char('[') << prefix << Char(']') << std::endl;
}

} // namespace Pt
