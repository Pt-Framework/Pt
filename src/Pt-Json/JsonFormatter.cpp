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
#include <Pt/SerializationError.h>
#include <Pt/Convert.h>

namespace {

static const Pt::Char PT_SETTINGS_TRUE[] = { 't', 'r', 'u', 'e' };
static const Pt::Char PT_SETTINGS_FALSE[] = { 'f', 'a', 'l', 's', 'e' };

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
        if (_ptr != _end)
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
    {
        return _ptr;
    }

    array_appender<T>& operator++()
    {
        if (_ptr != _end)
            ++_ptr;

        return *this;
    }

    array_appender<T> operator++(int)
    {
        array_appender<T> tmp = *this;

        if (_ptr != _end)
            ++_ptr;

        return tmp;
    }

private:
    T* _ptr;
    T* _end;
};

void formatName(std::basic_ostream<Pt::Char>& os, const char* name)
{
    if (*name)
    {
        os << "\"";

        while (*name)
            os << Pt::Char(*name++);

        os << "\"";

        os << Pt::Char(':') << Pt::Char(' ');
    }
}

void formatIndent(std::basic_ostream<Pt::Char>& os, std::size_t level)
{
    std::size_t indent = level * 2;
    while (indent--)
        os << Pt::Char(' ');
}

} // namespace

namespace Pt {

namespace Json {

JsonFormatter::JsonFormatter(std::basic_ostream<Char>& os)
: _os(&os)
, _state(0)
{
}


JsonFormatter::~JsonFormatter()
{
}


void JsonFormatter::attach(std::basic_ostream<Char>& os)
{
    _os = &os;
}

void JsonFormatter::onAddString(const char* name, const char* type,
    const Pt::Char* value, const char* id)
{
    formatName(*_os, name);

    *_os << Char('"');

    for (const Pt::Char* ch = value; *ch != 0; ++ch)
    {
        if (*ch == '"' || *ch == '\\')
            *_os << Pt::Char('\\');

        *_os << *ch;
    }

    *_os << Char('"');


    if (_stack.empty())
        *_os << Pt::Char(',')<<std::endl;
}


void JsonFormatter::onAddBool(const char* name, bool value,
    const char* id)
{
    formatName(*_os, name);

    if (value)
        _os->write(PT_SETTINGS_TRUE, sizeof(PT_SETTINGS_TRUE) / sizeof(Char));
    else
        _os->write(PT_SETTINGS_FALSE, sizeof(PT_SETTINGS_FALSE) / sizeof(Char));
    

    if (_stack.empty())
        *_os << Pt::Char(',') <<std::endl;
}


void JsonFormatter::onAddChar(const char* name, const Pt::Char& value,
    const char* id)
{
    formatName(*_os, name);

    *_os << Char('"') << value << Char('"');
    

    if (_stack.empty())
        *_os << Pt::Char(',') << std::endl;
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
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];

    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatInt(it, value);

    formatName(*_os, name);

    _os->write(_buf, it.getPointer() - _buf);


    if (_stack.empty())
        *_os << Pt::Char(',') << std::endl;
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
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];

    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatInt(it, value);

    formatName(*_os, name);

    _os->write(_buf, it.getPointer() - _buf);
    

    if (_stack.empty())
        *_os << Pt::Char(',') << std::endl;
}


void JsonFormatter::onAddFloat(const char* name, float value, const char* id)
{
    this->onAddDouble(name, value, id);
}


void JsonFormatter::onAddDouble(const char* name, double value, const char* id)
{
    const unsigned _bufsize = 64;
    Pt::Char _buf[_bufsize];

    array_appender<Pt::Char> it(_buf, _bufsize);
    it = formatFloat(it, value);

    formatName(*_os, name);

    _os->write(_buf, it.getPointer() - _buf);
    
    if (_stack.empty())
        *_os << Pt::Char(',') << std::endl;
}


void JsonFormatter::onAddLongDouble(const char* name, long double value, const char* id)
{
    this->onAddDouble(name, static_cast<double>(value), id);
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


void JsonFormatter::onBeginSequence(const char* name, const char*,
    const char*)
{
    // endl and indent nested sequence 
    if (_state == '[' || _state == ']')
    {
        *_os << std::endl;
        formatIndent(*_os, _stack.size());
    }

    formatName(*_os, name);
    *_os << Char('[');

    _stack.push_back(0);
    _state = '[';
}


void JsonFormatter::onBeginElement()
{
    if (_stack.empty())
        return;

    // add comma unless first element
    if (_stack.back() != 0)
        *_os << Char(',') << Char(' ');

    // increase element count
    ++_stack.back();
}


void JsonFormatter::onFinishElement()
{
}


void JsonFormatter::onFinishSequence()
{
    _stack.pop_back();

    // no endl if sequence of scalars
    if (_state == ']')
    {
        *_os << std::endl;
        formatIndent(*_os, _stack.size());
    }

    *_os << Char(']');

    // extra endl when root entry is finished
    if (_stack.empty())
        *_os << std::endl;

    _state = _stack.empty() ? 0 : ']';
}


void JsonFormatter::onBeginStruct(const char* name, const char* type,
    const char* id)
{
    // endl and indent nested sequence
    if (_state == '[' || _state == ']')
    {
        *_os << std::endl;
        formatIndent(*_os, _stack.size());
    }

    formatName(*_os, name);
    *_os << Char('{');

    _stack.push_back(0);
    _state = 0;
}


void JsonFormatter::onBeginMember(const char*)
{
    if (_stack.empty())
        return;

    // add comma unless first member
    if (_stack.back() != 0)
        *_os << Char(',') << Char(' ');

    // always endl after member
    *_os << std::endl;
    formatIndent(*_os, _stack.size());
    _state = 0;

    // increase element count
    ++_stack.back();
}


void JsonFormatter::onFinishMember()
{
}


void JsonFormatter::onFinishStruct()
{
    _stack.pop_back();

    // always endl after last member
    *_os << std::endl;
    formatIndent(*_os, _stack.size());

    *_os << Char('}');


    // extra endl when root entry is finished
    if (_stack.empty())
        *_os << Pt::Char(',') << std::endl;

    _state = _stack.empty() ? 0 : ']';
}


void JsonFormatter::onBeginParse(Composer&)
{

}


void JsonFormatter::onParse()
{

}

} // namespace Json

} // namespace Pt
