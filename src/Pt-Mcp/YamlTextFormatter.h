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

#ifndef PT_MCP_YAMLTEXTFORMATTER_H
#define PT_MCP_YAMLTEXTFORMATTER_H

#include "TextFormatter.h"
#include <Pt/Formatter.h>

namespace Pt {

namespace Mcp {

/** @brief Formats decomposed values as compact YAML-like text.

    Used by TextContent to serialize tool results for MCP responses.
    Scalars are written directly, structs as key: value pairs, and
    arrays as dash-prefixed lists with 2-space indentation.
*/
class YamlTextFormatter : public Pt::Formatter, public TextFormatter
{
  public:
    YamlTextFormatter();

    ~YamlTextFormatter();

  protected:
    Pt::Formatter& onBeginFormat() override;

    /** @brief Writes two spaces per current indentation level.
    */
    void writeIndent(int depth);

    /** @brief Writes a JSON-escaped string value.
    */
    void writeText(const Pt::Char* s, std::size_t n);

    /** @brief Writes a signed integer.
    */
    void writeInt(Pt::int64_t value);

    /** @brief Writes an unsigned integer.
    */
    void writeUInt(Pt::uint64_t value);

    /** @brief Writes a floating point value.
    */
    void writeDouble(double value);

protected:
    void onAddString(const char* name, const char* type,
                     const Pt::Char* value, const char* id) override;

    void onAddBinary(const char* name, const char* type,
                     const char* value, std::size_t length,
                     const char* id) override;

    void onAddBool(const char* name, bool value,
                   const char* id) override;

    void onAddChar(const char* name, const Pt::Char& value,
                   const char* id) override;

    void onAddInt8(const char* name, Pt::int8_t value,
                   const char* id) override;

    void onAddInt16(const char* name, Pt::int16_t value,
                    const char* id) override;

    void onAddInt32(const char* name, Pt::int32_t value,
                    const char* id) override;

    void onAddInt64(const char* name, Pt::int64_t value,
                    const char* id) override;

    void onAddUInt8(const char* name, Pt::uint8_t value,
                    const char* id) override;

    void onAddUInt16(const char* name, Pt::uint16_t value,
                     const char* id) override;

    void onAddUInt32(const char* name, Pt::uint32_t value,
                     const char* id) override;

    void onAddUInt64(const char* name, Pt::uint64_t value,
                     const char* id) override;

    void onAddFloat(const char* name, float value,
                    const char* id) override;

    void onAddDouble(const char* name, double value,
                     const char* id) override;

    void onAddLongDouble(const char* name, long double value,
                         const char* id) override;

    void onAddReference(const char* name, const char* refId) override;

    void onBeginStruct(const char* name, const char* type,
                       const char* id) override;

    void onBeginMember(const char* name) override;

    void onFinishMember() override;

    void onFinishStruct() override;

    void onBeginSequence(const char* name, const char* type,
                         const char* id) override;

    void onBeginElement() override;

    void onFinishElement() override;

    void onFinishSequence() override;

    void onBeginParse(Composer& /*composer*/) override
    {}

    bool onParseSome() override
    { return true; }

    void onParse() override
    {}

  private:
    int  _depth;
    bool _afterKey;
    bool _firstMember;
    bool _firstElement;
};

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_YAMLTEXTFORMATTER_H
