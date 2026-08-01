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

#ifndef PT_MCP_TEXTFORMATTER_H
#define PT_MCP_TEXTFORMATTER_H

#include <Pt/Mcp/ContentType.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>

namespace Pt {

namespace Mcp {

/** @brief Base class for text-based MCP content formatters.

    Owns the UTF-8 text stream attached to the output stream owned by
    ContentFormatter. Concrete subclasses inherit Pt::Formatter and
    use textOutput() to write their specific text style.
*/
class TextFormatter : public ContentFormatter
{
  public:
    TextFormatter();

    ~TextFormatter();

    /** @brief Escapes a string for embedding in JSON text.
    */
    static void escape(std::ostream& os, const char* text);

  protected:
    void onBeginContent() override;

    void onFinishContent() override;

    /** @brief Returns the UTF-8 text stream used to write content.
    */
    Pt::TextOStream& textOutput()
    { return _tos; }

  private:
    Pt::Utf8Codec _utf8;
    Pt::TextOStream _tos;
};

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_TEXTFORMATTER_H
