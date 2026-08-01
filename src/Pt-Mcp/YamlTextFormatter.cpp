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

#include "YamlTextFormatter.h"
#include <Pt/Convert.h>
#include <limits>

namespace Pt {

namespace Mcp {

YamlTextFormatter::YamlTextFormatter()
: _depth(0)
, _afterKey(false)
, _firstMember(true)
, _firstElement(true)
{
}


YamlTextFormatter::~YamlTextFormatter()
{
}


Pt::Formatter& YamlTextFormatter::onBeginFormat()
{
    return *this;
}


void YamlTextFormatter::writeIndent(int depth)
{
    Pt::TextOStream& tos = textOutput();
    for(int i = 0; i < depth; ++i)
    {
        tos << Pt::Char(' ') << Pt::Char(' ');
    }
}


void YamlTextFormatter::writeText(const Pt::Char* s, std::size_t n)
{
    Pt::TextOStream& tos = textOutput();
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
            tos.write(begin, p - begin);

        if(p == end)
            break;

        Pt::uint32_t ch = p->value();
        switch(ch)
        {
            case '"':  tos << Pt::Char('\\') << Pt::Char('"'); break;
            case '\\': tos << Pt::Char('\\') << Pt::Char('\\'); break;
            case '\n': tos << Pt::Char('\\') << Pt::Char('n'); break;
            case '\r': tos << Pt::Char('\\') << Pt::Char('r'); break;
            case '\t': tos << Pt::Char('\\') << Pt::Char('t'); break;
            default:
            {
                Pt::Char buf[6];
                buf[0] = Pt::Char('\\');
                buf[1] = Pt::Char('u');
                buf[2] = Pt::Char("0123456789abcdef"[(ch >> 12) & 0xF]);
                buf[3] = Pt::Char("0123456789abcdef"[(ch >> 8) & 0xF]);
                buf[4] = Pt::Char("0123456789abcdef"[(ch >> 4) & 0xF]);
                buf[5] = Pt::Char("0123456789abcdef"[ch & 0xF]);
                tos.write(buf, 6);
                break;
            }
        }

        begin = p + 1;
    }
}


void YamlTextFormatter::writeInt(Pt::int64_t value)
{
    Pt::TextOStream& tos = textOutput();
    Pt::Char buf[32];
    Pt::Char* p = formatInt(buf, 32, value, Pt::DecimalFormat<Pt::Char>());
    tos.write(p, buf + 32 - p);
}


void YamlTextFormatter::writeUInt(Pt::uint64_t value)
{
    Pt::TextOStream& tos = textOutput();
    Pt::Char buf[32];
    Pt::Char* p = formatInt(buf, 32, value, Pt::DecimalFormat<Pt::Char>());
    tos.write(p, buf + 32 - p);
}


void YamlTextFormatter::writeDouble(double value)
{
    Pt::TextOStream& tos = textOutput();
    const int precision = std::numeric_limits<long double>::digits10;
    Pt::FloatFormat<Pt::Char> fmt;
    Pt::String str;
    formatFloat(std::back_inserter(str), static_cast<long double>(value),
                fmt, precision);
    tos.write(str.c_str(), str.size());
}


void YamlTextFormatter::onAddString(const char* /*name*/, const char* /*type*/,
                                    const Pt::Char* value, const char* /*id*/)
{
    const Pt::Char* end = value;
    while(end && *end != Pt::Char(0))
        ++end;

    if(end > value)
        writeText(value, end - value);
}


void YamlTextFormatter::onAddBinary(const char* /*name*/, const char* /*type*/,
                                    const char* /*value*/, std::size_t /*length*/,
                                    const char* /*id*/)
{
    static const Pt::Char binary[] = { '[', 'b', 'i', 'n', 'a', 'r', 'y', ']' };
    textOutput().write(binary, 8);
}


void YamlTextFormatter::onAddBool(const char* /*name*/, bool value,
                                  const char* /*id*/)
{
    Pt::TextOStream& tos = textOutput();
    static const Pt::Char TRUE_STR[] = { 't', 'r', 'u', 'e' };
    static const Pt::Char FALSE_STR[] = { 'f', 'a', 'l', 's', 'e' };

    if(value)
        tos.write(TRUE_STR, 4);
    else
        tos.write(FALSE_STR, 5);
}


void YamlTextFormatter::onAddChar(const char* /*name*/, const Pt::Char& value,
                                  const char* /*id*/)
{
    writeText(&value, 1);
}


void YamlTextFormatter::onAddInt8(const char* /*name*/, Pt::int8_t value,
                                  const char* /*id*/)
{
    writeInt(value);
}


void YamlTextFormatter::onAddInt16(const char* /*name*/, Pt::int16_t value,
                                   const char* /*id*/)
{
    writeInt(value);
}


void YamlTextFormatter::onAddInt32(const char* /*name*/, Pt::int32_t value,
                                   const char* /*id*/)
{
    writeInt(value);
}


void YamlTextFormatter::onAddInt64(const char* /*name*/, Pt::int64_t value,
                                   const char* /*id*/)
{
    writeInt(value);
}


void YamlTextFormatter::onAddUInt8(const char* /*name*/, Pt::uint8_t value,
                                   const char* /*id*/)
{
    writeUInt(value);
}


void YamlTextFormatter::onAddUInt16(const char* /*name*/, Pt::uint16_t value,
                                    const char* /*id*/)
{
    writeUInt(value);
}


void YamlTextFormatter::onAddUInt32(const char* /*name*/, Pt::uint32_t value,
                                    const char* /*id*/)
{
    writeUInt(value);
}


void YamlTextFormatter::onAddUInt64(const char* /*name*/, Pt::uint64_t value,
                                    const char* /*id*/)
{
    writeUInt(value);
}


void YamlTextFormatter::onAddFloat(const char* /*name*/, float value,
                                   const char* /*id*/)
{
    writeDouble(static_cast<double>(value));
}


void YamlTextFormatter::onAddDouble(const char* /*name*/, double value,
                                    const char* /*id*/)
{
    writeDouble(value);
}


void YamlTextFormatter::onAddLongDouble(const char* /*name*/, long double value,
                                        const char* /*id*/)
{
    writeDouble(static_cast<double>(value));
}


void YamlTextFormatter::onAddReference(const char* /*name*/, const char* /*refId*/)
{
}


void YamlTextFormatter::onBeginStruct(const char* /*name*/, const char* /*type*/,
                                      const char* /*id*/)
{
    if(_afterKey)
    {
        textOutput() << Pt::Char('\\') << Pt::Char('n');
        _afterKey = false;
    }

    ++_depth;
    _firstMember = true;
}


void YamlTextFormatter::onBeginMember(const char* name)
{
    Pt::TextOStream& tos = textOutput();

    if( ! _firstMember)
        tos << Pt::Char('\\') << Pt::Char('n');

    writeIndent(_depth);

    while(*name != '\0')
    {
        tos << Pt::Char(*name);
        ++name;
    }

    tos << Pt::Char(':') << Pt::Char(' ');
    _afterKey = true;
    _firstMember = false;
}


void YamlTextFormatter::onFinishMember()
{
    _afterKey = false;
}


void YamlTextFormatter::onFinishStruct()
{
    --_depth;
}


void YamlTextFormatter::onBeginSequence(const char* /*name*/, const char* /*type*/,
                                        const char* /*id*/)
{
    if(_afterKey)
    {
        textOutput() << Pt::Char('\\') << Pt::Char('n');
        _afterKey = false;
    }

    ++_depth;
    _firstElement = true;
}


void YamlTextFormatter::onBeginElement()
{
    Pt::TextOStream& tos = textOutput();

    if( ! _firstElement)
        tos << Pt::Char('\\') << Pt::Char('n');

    writeIndent(_depth);
    tos << Pt::Char('-') << Pt::Char(' ');
    _afterKey = true;
    _firstElement = false;
}


void YamlTextFormatter::onFinishElement()
{
    _afterKey = false;
}


void YamlTextFormatter::onFinishSequence()
{
    --_depth;
}

} // namespace Mcp

} // namespace Pt
