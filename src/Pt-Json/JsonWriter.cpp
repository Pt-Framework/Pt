/*
   Copyright (C) 2015-2024 by Dr. Marc Boris Duerner
  
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

#include <Pt/Json/JsonWriter.h>
#include <Pt/Convert.h>

namespace {

static const Pt::Char JSON_TRUE[] = { 't', 'r', 'u', 'e' };
static const Pt::Char JSON_FALSE[] = { 'f', 'a', 'l', 's', 'e' };
static const Pt::Char JSON_NULL[] = { 'n', 'u', 'l', 'l' };

void jsonEncode(std::basic_ostream<Pt::Char>& os, const Pt::Char* str, std::size_t n)
{
    const Pt::Char* it = str;
    const Pt::Char* begin = str;
    const Pt::Char* end = begin + n;

    const Pt::Char qoute('"');
    const Pt::Char escape('\\');

    for( ; it != end; ++it)
    {
        if(*it == qoute || *it == escape)
        {
            // all characters until the qoute
            if(it != begin)
                os.write(begin, it - begin);

            // the escaped character
            os << Pt::Char('\\') << *it;

            // escaping continues at next character (or end) 
            begin = it + 1;
        }
    }

    if(it != begin)
        os.write(begin, it - begin);
}


void jsonEncode(std::basic_ostream<Pt::Char>& os, const Pt::Char* str)
{
    const Pt::Char* it = str;
    const Pt::Char* begin = str;

    const Pt::Char qoute('"');

    for( ; *it != '\0'; ++it)
    {
        if(*it == qoute)
        {
            // all characters until the qoute
            if(it != begin)
                os.write(begin, it - begin);

            // the escaped character
            os << Pt::Char('\\') << *it;

            // escaping continues at next character (or end) 
            begin = it + 1;
        }
    }

    if(it != begin)
        os.write(begin, it - begin);
}

} // namespace

namespace Pt {

namespace Json {

class JsonWriterImpl
{
    enum State
    {
        OnBegin,
        OnScalar,
        OnMember,
        OnCompound,
        OnCompoundEnd
    };

    public:
        JsonWriterImpl()
        : _tos(0)
        , _formatting(true)
        , _indent("  ")
        , _state(OnBegin)
        , _depth(0)
        { }

        JsonWriterImpl(std::basic_ostream<Char>& tos)
        : _tos(&tos)
        , _formatting(true)
        , _indent("  ")
        , _state(OnBegin)
        , _depth(0)
        { }

        bool isFormatting() const
        { 
            return _formatting; 
        }

        void setFormatting(bool value)
        { 
            _formatting = value; 
        }

        const Pt::String& indent() const
        { 
            return _indent; 
        }

        void setIndent(const Pt::String& indent)
        { 
            _indent = indent; 
        }

        void reset()
        {
            _tos = 0;
            _depth = 0;
            _state = OnBegin;
        }

        void reset(std::basic_ostream<Char>& tos)
        {
            reset();
            _tos = &tos;
        }

        std::basic_ostream<Char>* output()
        { 
            return _tos; 
        }

        void writeObject()
        {
            if( ! _tos )
                return;

            if(_state == OnCompoundEnd || _state == OnScalar)
                *_tos << Pt::Char(',');

            if(_state == OnCompound || _state == OnCompoundEnd)
              formatIndent(*_tos, _depth);

            *_tos << Pt::Char('{') << Pt::Char(' ');

            _depth++;
            _state = OnCompound;
        }

        void writeObjectEnd()
        {
            if( ! _tos || _depth == 0 )
                return;

            formatIndent(*_tos, _depth - 1);

            *_tos << Pt::Char('}');

            _depth--;
            _state = OnCompoundEnd;
        }

        void writeArray()
        {
            if( ! _tos )
                return;

            if(_state == OnCompoundEnd || _state == OnScalar)
                *_tos << Pt::Char(',');

            if(_state == OnCompound || _state == OnCompoundEnd)
              formatIndent(*_tos, _depth);

            *_tos << Pt::Char('[') << Pt::Char(' ');

            _depth++;
            _state = OnCompound;
        }

        void writeArrayEnd()
        {
            if( ! _tos || _depth == 0 )
                return;
            
            if( _state != OnScalar )
                formatIndent(*_tos, _depth - 1);
            else
                *_tos << Pt::Char(' ');
            
            *_tos << Pt::Char(']');

            _depth--;
            _state = OnCompoundEnd;
        }

        void writeMember(const Pt::Char* name, std::size_t nameSize)
        {
            if( ! _tos )
                return;

            if(_state == OnCompoundEnd || _state == OnScalar)
                *_tos << Pt::Char(',');

            formatIndent(*_tos, _depth);

            *_tos << Pt::Char('"');
            _tos->write(name, nameSize);
            *_tos << Pt::Char('"') << Pt::Char(' ') << Pt::Char(':') << Pt::Char(' ');

            _state = OnMember;
        }

        void writeMember(const char* name)
        {
            if( ! _tos )
                return;

            if(_state == OnCompoundEnd || _state == OnScalar)
                *_tos << Pt::Char(',');

            formatIndent(*_tos, _depth);

            *_tos << Pt::Char('"');

            while(*name != '\0')
                *_tos << Pt::Char(*name++);

            *_tos << Pt::Char('"') << Pt::Char(' ') << Pt::Char(':') << Pt::Char(' ');

            _state = OnMember;
        }

        void writeString(const Pt::Char* value, std::size_t valueSize)
        {
            if( ! _tos)
                return;

            if(_state == OnCompoundEnd || _state == OnScalar)
                *_tos << Pt::Char(',') << Pt::Char(' ');

            *_tos << Pt::Char('"');
            jsonEncode(*_tos, value, valueSize);
            *_tos << Pt::Char('"');

            _state = OnScalar;
        }

        void writeString(const Pt::Char* value)
        {
            if( ! _tos )
                return;

            if(_state == OnCompoundEnd || _state == OnScalar)
                *_tos << Pt::Char(',') << Pt::Char(' ');

            *_tos << Pt::Char('"');
            jsonEncode(*_tos, value);
            *_tos << Pt::Char('"');

            _state = OnScalar;
        }

        void writeUInt(Pt::uint64_t value)
        {
            if( ! _tos )
                return;

            if(_state == OnCompoundEnd || _state == OnScalar)
                *_tos << Pt::Char(',') << Pt::Char(' ');

            _value.clear();
            formatInt( std::back_inserter(_value), value );

            *_tos << _value;

            _state = OnScalar;
        }

        void writeInt(Pt::int64_t value)
        {
            if( ! _tos )
                return;

            if(_state == OnCompoundEnd || _state == OnScalar)
                *_tos << Pt::Char(',') << Pt::Char(' ');

            _value.clear();
            formatInt( std::back_inserter(_value), value );

            *_tos << _value;

            _state = OnScalar;
        }

        void writeFloat(long double value)
        {
            if( ! _tos )
                return;

            if(_state == OnCompoundEnd || _state == OnScalar)
                *_tos << Pt::Char(',') << Pt::Char(' ');

            _value.clear();
            formatFloat( std::back_inserter(_value), value );

            *_tos << _value;

            _state = OnScalar;
        }

        void writeBool(bool value)
        {
            if( ! _tos )
                return;

            if(_state == OnCompoundEnd || _state == OnScalar)
                *_tos << Pt::Char(',') << Pt::Char(' ');

            const Pt::Char* keyword = value ? JSON_TRUE : JSON_FALSE;

            const size_t size = value ? (sizeof(JSON_TRUE) / sizeof(Pt::Char)) : (sizeof(JSON_FALSE) / sizeof(Pt::Char));

            _tos->write(keyword, size);

            _state = OnScalar;
        }

        void writeNull()
        {
            if( ! _tos )
                return;

            if(_state == OnCompoundEnd || _state == OnScalar)
                *_tos << Pt::Char(',') << Pt::Char(' ');

            _tos->write(JSON_NULL, sizeof(JSON_TRUE)/sizeof(Pt::Char));

            _state = OnScalar;
        }

    private:
        void formatIndent(std::basic_ostream<Char>& tos, std::size_t width)
        {
            if( ! _formatting )
                return;

            tos.put( Pt::Char('\n') );

            for(size_t n = 0; n < width; ++n)
            {
                tos << _indent;
            }
        }

    private:
        std::basic_ostream<Char>* _tos;
        bool                      _formatting;
        Pt::String                _indent;
        State                     _state;
        std::size_t               _depth;
        Pt::String                _value;
};


JsonWriter::JsonWriter()
: _impl(0)
{
    _impl = new JsonWriterImpl();
}


JsonWriter::JsonWriter(std::basic_ostream<Char>& os)
: _impl(0)
{
    _impl = new JsonWriterImpl(os);
}


JsonWriter::~JsonWriter()
{
    delete _impl;
}


bool JsonWriter::isFormatting() const
{
    return _impl->isFormatting();
}


void JsonWriter::setFormatting(bool value)
{
    _impl->setFormatting(value);
}


const Pt::String& JsonWriter::indent() const
{
    return _impl->indent();
}


void JsonWriter::setIndent(const Pt::String& indent)
{
    _impl->setIndent(indent);
}


void JsonWriter::reset()
{
    _impl->reset();
}


void JsonWriter::reset(std::basic_ostream<Char>& os)
{
    _impl->reset(os);
}


std::basic_ostream<Char>* JsonWriter::output()
{
    return _impl->output();
}


void JsonWriter::writeObject()
{
    _impl->writeObject();
}


void JsonWriter::writeObjectEnd()
{
    _impl->writeObjectEnd();
}


void JsonWriter::writeArray()
{
    _impl->writeArray();
}


void JsonWriter::writeArrayEnd()
{
    _impl->writeArrayEnd();
}


void JsonWriter::writeMember(const Pt::Char* name, std::size_t nameSize)
{
    _impl->writeMember(name, nameSize);
}


void JsonWriter::writeMember(const char* name)
{
    _impl->writeMember(name);
}


void JsonWriter::writeString(const Pt::Char* value, std::size_t valueSize)
{
    _impl->writeString(value, valueSize);
}


void JsonWriter::writeString(const Pt::Char* value)
{
    _impl->writeString(value);
}


void JsonWriter::writeInt(Pt::int64_t value)
{
    _impl->writeUInt(value);
}


void JsonWriter::writeUInt(Pt::uint64_t value)
{
    _impl->writeInt(value);
}


void JsonWriter::writeFloat(long double value)
{
    _impl->writeFloat(value);
}


void JsonWriter::writeBool(bool value)
{
    _impl->writeBool(value);
}


void JsonWriter::writeNull()
{
    _impl->writeNull();
}

} // namespace

} // namespace
