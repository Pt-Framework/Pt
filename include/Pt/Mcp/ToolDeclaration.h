/* Copyright (C) 2020-2026 by Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef PT_MCP_TOOLDECLARATION_H
#define PT_MCP_TOOLDECLARATION_H

#include <Pt/Mcp/Api.h>
#include <Pt/Mcp/Type.h>
#include <Pt/Mcp/ContentType.h>
#include <Pt/NonCopyable.h>
#include <string>
#include <vector>

namespace Pt {

namespace Mcp {

/** @brief One MCP tool in a server declaration.

    %Tool is one callable in the catalog %ToolDeclaration owns. The
    name is the MCP tool name and must match a procedure on the
    %Pt::Remoting::ServiceDefinition. The description is what
    tools/list shows. %addParam() appends a parameter whose name is
    the JSON member the client sends. Parameters are required until
    %setOptional() is called with that name; a name that is not
    present is ignored.

    %setContent() selects the %ContentType for the result. The default
    is %textContent(). The content type and every parameter %Type must
    outlive this tool. %getParamIndex() maps a parameter name to its
    position, or -1 when the name is unknown. The tool is not
    copyable. Application code does not construct a %Tool; it comes
    from %ToolDeclaration::addTool().

    @ingroup Pt-Mcp-Tools
*/
class PT_MCP_API Tool : private NonCopyable
{
  public:
    /** @brief Creates a tool named @a name with @a description.
    */
    Tool(const std::string& name, const std::string& description);

    /** @brief Destroys the tool.
    */
    ~Tool();

    /** @brief Adds a required parameter named @a name of @a type.

        @a description is the schema description of the parameter.
    */
    Tool& addParam(const std::string& name, const Type& type,
                   const std::string& description = "");

    /** @brief Marks the parameter named @a paramName as optional.
    */
    Tool& setOptional(const std::string& paramName);

    /** @brief Sets the content type used to format the tool result.
    */
    Tool& setContent(const ContentType& content);

    /** @brief Returns the tool name.
    */
    const std::string& name() const
    { return _name; }

    /** @brief Returns the tool description.
    */
    const std::string& description() const
    { return _description; }

    /** @brief Returns the parameters of this tool.
    */
    const std::vector<Property>& params() const
    { return _params; }

    /** @brief Returns the number of parameters.
    */
    std::size_t paramCount() const
    { return _params.size(); }

    /** @brief Returns the index of the parameter named @a name, or -1.
    */
    int getParamIndex(const std::string& name) const;

    /** @brief Returns the content type of the tool result.
    */
    const ContentType& content() const;

  private:
    std::string _name;
    std::string _description;
    std::vector<Property> _params;
    const ContentType* _content;
};


/** @brief MCP server name, version and tool catalog.

    %ToolDeclaration is the server catalog the tools group described.
    The constructor stores the server name and version that initialize
    returns. %addTool() creates a %Tool, owns it, and returns it so
    %addParam() and %setContent() can be chained. %getTool() finds a
    tool by name, or returns a null pointer.

    %toToolsList() writes the tools/list result JSON from the declared
    tools and their schemas. %toInitializeResult() writes the
    initialize result JSON, echoing @a protocolVersion when it is not
    null and the latest supported version otherwise. Transports call
    both. Application code rarely writes those payloads itself.

    %preferredVersion() returns @a requested when it is a supported
    protocol version, otherwise the latest supported version.
    %isSupportedVersion() tests a version string. The supported
    versions are 2025-11-25 and 2025-03-26.

    The declaration owns every %Tool it creates. It does not own the
    %Type or %ContentType objects those tools refer to. The
    declaration is not copyable.

    @ingroup Pt-Mcp-Tools
*/
class PT_MCP_API ToolDeclaration : private NonCopyable
{
  public:
    /** @brief Creates a declaration for a server named @a serverName.

        @a serverVersion is the version string initialize returns.
    */
    ToolDeclaration(const std::string& serverName,
                    const std::string& serverVersion);

    /** @brief Destroys the declaration and its tools.
    */
    ~ToolDeclaration();

    /** @brief Creates a tool named @a name and returns it.

        The returned tool is owned by this declaration.
    */
    Tool& addTool(const std::string& name, const std::string& description);

    /** @brief Returns the tool named @a name, or a null pointer.
    */
    const Tool* getTool(const std::string& name) const;

    /** @brief Returns the server name.
    */
    const std::string& serverName() const
    { return _serverName; }

    /** @brief Returns the server version string.
    */
    const std::string& serverVersion() const
    { return _serverVersion; }

    /** @brief Returns the protocol version to use for @a requested.

        If @a requested is supported it is returned verbatim.
        Otherwise the latest supported version is returned.
    */
    static std::string preferredVersion(const std::string& requested);

    /** @brief Returns true if @a version is a supported protocol version.
    */
    static bool isSupportedVersion(const std::string& version);

    /** @brief Writes the tools/list result JSON to @a os.
    */
    void toToolsList(std::ostream& os) const;

    /** @brief Writes the initialize result JSON to @a os.

        @a protocolVersion is the negotiated version string to echo.
        If null, the latest supported version is used.
    */
    void toInitializeResult(std::ostream& os,
                            const char* protocolVersion = 0) const;

  private:
    std::string _serverName;
    std::string _serverVersion;
    std::vector<Tool*> _tools;
};

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_TOOLDECLARATION_H
