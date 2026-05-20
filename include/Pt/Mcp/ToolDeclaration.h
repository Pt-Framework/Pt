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

#ifndef PT_MCP_TOOLDECLARATION_H
#define PT_MCP_TOOLDECLARATION_H

#include <Pt/Mcp/Api.h>
#include <Pt/Mcp/Type.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <vector>

namespace Pt {

class Decomposer;

namespace Mcp {

/** @brief Formats a tool result as MCP content.

    ContentType determines how a Decomposer result is serialized
    into an MCP content block (text, image, etc.).
*/
class PT_MCP_API ContentType
{
  public:
    virtual ~ContentType();

    /** @brief Formats the decomposed result into an MCP content block.

        Writes a JSON content object (e.g. {"type":"text","text":"..."})
        to the output stream.
    */
    virtual void format(Decomposer* result, std::ostream& os) const = 0;
};


/** @brief Formats tool results as MCP text content.

    Serializes the result using TextFormatter (compact YAML-like)
    and embeds it in a text content block.
*/
class PT_MCP_API TextContent : public ContentType
{
  public:
    TextContent();

    void format(Decomposer* result, std::ostream& os) const override;
};

PT_MCP_API const TextContent& textContent();


/** @brief MCP tool descriptor.

    Describes a tool with its parameters and content type. Provides
    name-to-position mapping for parameter lookup during dispatch.
*/
class PT_MCP_API Tool : private NonCopyable
{
  public:
    Tool(const std::string& name, const std::string& description);

    ~Tool();

    Tool& addParam(const std::string& name, const Type& type,
                   const std::string& description = "");

    Tool& setOptional(const std::string& paramName);

    Tool& setContent(const ContentType& content);

    const std::string& name() const
    { return _name; }

    const std::string& description() const
    { return _description; }

    const std::vector<Property>& params() const
    { return _params; }

    std::size_t paramCount() const
    { return _params.size(); }

    int getParamIndex(const std::string& name) const;

    const ContentType& content() const;

  private:
    std::string _name;
    std::string _description;
    std::vector<Property> _params;
    const ContentType* _content;
};


/** @brief MCP server and tool registry.

    Describes the MCP server capabilities and registered tools.
    Generates protocol responses for initialize and tools/list.
*/
class PT_MCP_API ToolDeclaration : private NonCopyable
{
  public:
    ToolDeclaration(const std::string& serverName,
                    const std::string& serverVersion);

    ~ToolDeclaration();

    Tool& addTool(const std::string& name, const std::string& description);

    const Tool* getTool(const std::string& name) const;

    const std::string& serverName() const
    { return _serverName; }

    const std::string& serverVersion() const
    { return _serverVersion; }

    /** @brief Writes the tools/list result JSON to the stream.
    */
    void toToolsList(std::ostream& os) const;

    /** @brief Writes the initialize result JSON to the stream.
    */
    void toInitializeResult(std::ostream& os) const;

  private:
    std::string _serverName;
    std::string _serverVersion;
    std::vector<Tool*> _tools;
};

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_TOOLDECLARATION_H
