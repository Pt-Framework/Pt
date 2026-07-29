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

#include "TextFormatter.h"
#include <Pt/Convert.h>
#include <limits>

namespace Pt {

namespace Mcp {

TextFormatter::TextFormatter()
: _os(0)
, _utf8(1)
, _tos(&_utf8)
, _depth(0)
, _afterKey(false)
, _firstMember(true)
, _firstElement(true)
{
    _os = &_tos;
}


TextFormatter::~TextFormatter()
{
}


Pt::Formatter& TextFormatter::beginContent(std::ostream& os)
{
    os << "{\"type\":\"text\",\"text\":\"";
    _tos.attach(os);
    return *this;
}


void TextFormatter::finishContent(std::ostream& os)
{
    _tos.flush();
    os << "\"}";
}


void TextFormatter::writeIndent()
{
    for(int i = 0; i < _depth; ++i)
    {
        *_os << Pt::Char(' ') << Pt::Char(' ');
    }
}


void TextFormatter::writeEscaped(const Pt::Char* s, std::size_t n)
{
    const Pt::Char* begin = s;
    const Pt::Char* end = s + n;

    while(begin < end)
    {
        const Pt::Char* p = begin;
        while(p < end)
        {
            Pt::uint32_t cp = p->value();
            if(cp == '"' || cp == '\\' || cp < 0x20)
                break;
            ++p;
        }

        if(p > begin)
            _os->write(begin, p - begin);

        if(p == end)
            break;

        Pt::uint32_t ch = p->value();
        switch(ch)
        {
            case '"':  *_os << Pt::Char('\\') << Pt::Char('"'); break;
            case '\\': *_os << Pt::Char('\\') << Pt::Char('\\'); break;
            case '\n': *_os << Pt::Char('\\') << Pt::Char('n'); break;
            case '\r': *_os << Pt::Char('\\') << Pt::Char('r'); break;
            case '\t': *_os << Pt::Char('\\') << Pt::Char('t'); break;
            default:
            {
                Pt::Char buf[6];
                buf[0] = Pt::Char('\\');
                buf[1] = Pt::Char('u');
                buf[2] = Pt::Char("0123456789abcdef"[(ch >> 12) & 0xF]);
                buf[3] = Pt::Char("0123456789abcdef"[(ch >> 8) & 0xF]);
                buf[4] = Pt::Char("0123456789abcdef"[(ch >> 4) & 0xF]);
                buf[5] = Pt::Char("0123456789abcdef"[ch & 0xF]);
                _os->write(buf, 6);
                break;
            }
        }

        begin = p + 1;
    }
}


void TextFormatter::writeInt(Pt::int64_t value)
{
    Pt::Char buf[32];
    Pt::Char* p = formatInt(buf, 32, value, Pt::DecimalFormat<Pt::Char>());
    _os->write(p, buf + 32 - p);
}


void TextFormatter::writeUInt(Pt::uint64_t value)
{
    Pt::Char buf[32];
    Pt::Char* p = formatInt(buf, 32, value, Pt::DecimalFormat<Pt::Char>());
    _os->write(p, buf + 32 - p);
}


void TextFormatter::writeDouble(double value)
{
    const int precision = std::numeric_limits<long double>::digits10;
    Pt::FloatFormat<Pt::Char> fmt;
    Pt::String str;
    formatFloat(std::back_inserter(str), static_cast<long double>(value),
                fmt, precision);
    _os->write(str.c_str(), str.size());
}


void TextFormatter::onAddString(const char* /*name*/, const char* /*type*/,
                                   const Pt::Char* value, const char* /*id*/)
{
    // Find the length and delegate to writeEscaped
    const Pt::Char* end = value;
    while(end && *end != Pt::Char(0))
        ++end;

    if(end > value)
        writeEscaped(value, end - value);
}


void TextFormatter::onAddBinary(const char* /*name*/, const char* /*type*/,
                                   const char* /*value*/, std::size_t /*length*/,
                                   const char* /*id*/)
{
    static const Pt::Char binary[] = { '[', 'b', 'i', 'n', 'a', 'r', 'y', ']' };
    _os->write(binary, 8);
}


void TextFormatter::onAddBool(const char* /*name*/, bool value,
                                 const char* /*id*/)
{
    static const Pt::Char TRUE_STR[] = { 't', 'r', 'u', 'e' };
    static const Pt::Char FALSE_STR[] = { 'f', 'a', 'l', 's', 'e' };

    if(value)
        _os->write(TRUE_STR, 4);
    else
        _os->write(FALSE_STR, 5);
}


void TextFormatter::onAddChar(const char* /*name*/, const Pt::Char& value,
                                 const char* /*id*/)
{
    writeEscaped(&value, 1);
}


void TextFormatter::onAddInt8(const char* /*name*/, Pt::int8_t value,
                                 const char* /*id*/)
{
    writeInt(value);
}


void TextFormatter::onAddInt16(const char* /*name*/, Pt::int16_t value,
                                  const char* /*id*/)
{
    writeInt(value);
}


void TextFormatter::onAddInt32(const char* /*name*/, Pt::int32_t value,
                                  const char* /*id*/)
{
    writeInt(value);
}


void TextFormatter::onAddInt64(const char* /*name*/, Pt::int64_t value,
                                  const char* /*id*/)
{
    writeInt(value);
}


void TextFormatter::onAddUInt8(const char* /*name*/, Pt::uint8_t value,
                                  const char* /*id*/)
{
    writeUInt(value);
}


void TextFormatter::onAddUInt16(const char* /*name*/, Pt::uint16_t value,
                                   const char* /*id*/)
{
    writeUInt(value);
}


void TextFormatter::onAddUInt32(const char* /*name*/, Pt::uint32_t value,
                                   const char* /*id*/)
{
    writeUInt(value);
}


void TextFormatter::onAddUInt64(const char* /*name*/, Pt::uint64_t value,
                                   const char* /*id*/)
{
    writeUInt(value);
}


void TextFormatter::onAddFloat(const char* /*name*/, float value,
                                  const char* /*id*/)
{
    writeDouble(static_cast<double>(value));
}


void TextFormatter::onAddDouble(const char* /*name*/, double value,
                                   const char* /*id*/)
{
    writeDouble(value);
}


void TextFormatter::onAddLongDouble(const char* /*name*/, long double value,
                                       const char* /*id*/)
{
    writeDouble(static_cast<double>(value));
}


void TextFormatter::onAddReference(const char* /*name*/, const char* /*refId*/)
{
}


void TextFormatter::onBeginStruct(const char* /*name*/, const char* /*type*/,
                                     const char* /*id*/)
{
    if(_afterKey)
    {
        *_os << Pt::Char('\\') << Pt::Char('n');
        _afterKey = false;
    }

    ++_depth;
    _firstMember = true;
}


void TextFormatter::onBeginMember(const char* name)
{
    if( ! _firstMember)
        *_os << Pt::Char('\\') << Pt::Char('n');

    writeIndent();

    while(*name != '\0')
    {
        *_os << Pt::Char(*name);
        ++name;
    }

    *_os << Pt::Char(':') << Pt::Char(' ');
    _afterKey = true;
    _firstMember = false;
}


void TextFormatter::onFinishMember()
{
    _afterKey = false;
}


void TextFormatter::onFinishStruct()
{
    --_depth;
}


void TextFormatter::onBeginSequence(const char* /*name*/, const char* /*type*/,
                                       const char* /*id*/)
{
    if(_afterKey)
    {
        *_os << Pt::Char('\\') << Pt::Char('n');
        _afterKey = false;
    }

    ++_depth;
    _firstElement = true;
}


void TextFormatter::onBeginElement()
{
    if( ! _firstElement)
        *_os << Pt::Char('\\') << Pt::Char('n');

    writeIndent();
    *_os << Pt::Char('-') << Pt::Char(' ');
    _afterKey = true;
    _firstElement = false;
}


void TextFormatter::onFinishElement()
{
    _afterKey = false;
}


void TextFormatter::onFinishSequence()
{
    --_depth;
}

} // namespace Mcp

} // namespace Pt
