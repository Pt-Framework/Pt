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

#ifndef PT_MCP_IMAGEFORMATTER_H
#define PT_MCP_IMAGEFORMATTER_H

#include <Pt/Mcp/ContentType.h>
#include <Pt/TextStream.h>
#include <Pt/Base64Codec.h>
#include <Pt/Types.h>
#include <ostream>
#include <string>

namespace Pt {

namespace Mcp {

/** @brief Streams decomposed binary data as base64 to an output stream.

    Ignores every decomposed element except binary values, which are
    base64-encoded incrementally as they arrive (no buffering of the
    whole result in memory).
*/
class ImageFormatter : public Pt::Formatter, public ContentFormatter
{
  public:
    explicit ImageFormatter(const std::string& mimeType);

    Pt::Formatter& beginContent(std::ostream& os) override;

    void finishContent(std::ostream& os) override;

  protected:
    void onAddBinary(const char* name, const char* type,
                     const char* value, std::size_t length, const char* id) override;

    void onAddString(const char*, const char*, const Pt::Char*, const char*) override
    {}

    void onAddBool(const char*, bool, const char*) override
    {}

    void onAddChar(const char*, const Pt::Char&, const char*) override
    {}

    void onAddInt8(const char*, Pt::int8_t, const char*) override
    {}

    void onAddInt16(const char*, Pt::int16_t, const char*) override
    {}

    void onAddInt32(const char*, Pt::int32_t, const char*) override
    {}

    void onAddInt64(const char*, Pt::int64_t, const char*) override
    {}

    void onAddUInt8(const char*, Pt::uint8_t, const char*) override
    {}

    void onAddUInt16(const char*, Pt::uint16_t, const char*) override
    {}

    void onAddUInt32(const char*, Pt::uint32_t, const char*) override
    {}

    void onAddUInt64(const char*, Pt::uint64_t, const char*) override
    {}

    void onAddFloat(const char*, float, const char*) override
    {}

    void onAddDouble(const char*, double, const char*) override
    {}

    void onAddLongDouble(const char*, long double, const char*) override
    {}

    void onAddReference(const char*, const char*) override
    {}

    void onBeginStruct(const char*, const char*, const char*) override
    {}

    void onBeginMember(const char*) override
    {}

    void onFinishMember() override
    {}

    void onFinishStruct() override
    {}

    void onBeginSequence(const char*, const char*, const char*) override
    {}

    void onBeginElement() override
    {}

    void onFinishElement() override
    {}

    void onFinishSequence() override
    {}

    void onBeginParse(Pt::Composer& /*composer*/) override
    {}

    bool onParseSome() override
    { return true; }

    void onParse() override
    {}

  private:
    Pt::BasicTextOStream<char, char> _b64;
    std::string _mimeType;
};

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_IMAGEFORMATTER_H
