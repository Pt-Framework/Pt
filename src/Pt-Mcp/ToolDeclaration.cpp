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

#include <Pt/Mcp/ToolDeclaration.h>
#include <ostream>

namespace Pt {

namespace Mcp {


//
// Tool
//

Tool::Tool(const std::string& name, const std::string& description)
: _name(name)
, _description(description)
, _content( &textContent() )
{
}


Tool::~Tool()
{
}


Tool& Tool::addParam(const std::string& name, const Type& type,
                     const std::string& description)
{
    _params.push_back( Property(name, type, description) );
    return *this;
}


Tool& Tool::setOptional(const std::string& paramName)
{
    for(std::size_t i = 0; i < _params.size(); ++i)
    {
        if(_params[i].name() == paramName)
        {
            _params[i].setOptional();
            break;
        }
    }

    return *this;
}


Tool& Tool::setContent(const ContentType& content)
{
    _content = &content;
    return *this;
}


int Tool::getParamIndex(const std::string& name) const
{
    for(std::size_t i = 0; i < _params.size(); ++i)
    {
        if(_params[i].name() == name)
            return static_cast<int>(i);
    }

    return -1;
}


const ContentType& Tool::content() const
{
    return *_content;
}


//
// ToolDeclaration
//

ToolDeclaration::ToolDeclaration(const std::string& serverName,
                                 const std::string& serverVersion)
: _serverName(serverName)
, _serverVersion(serverVersion)
{
}


ToolDeclaration::~ToolDeclaration()
{
    for(std::size_t i = 0; i < _tools.size(); ++i)
        delete _tools[i];
}


Tool& ToolDeclaration::addTool(const std::string& name,
                               const std::string& description)
{
    Tool* tool = new Tool(name, description);
    _tools.push_back(tool);
    return *tool;
}


const Tool* ToolDeclaration::getTool(const std::string& name) const
{
    for(std::size_t i = 0; i < _tools.size(); ++i)
    {
        if(_tools[i]->name() == name)
            return _tools[i];
    }

    return 0;
}


void ToolDeclaration::toToolsList(std::ostream& os) const
{
    os << "{\"tools\":[";

    for(std::size_t i = 0; i < _tools.size(); ++i)
    {
        if(i > 0)
            os << ',';

        const Tool& tool = *_tools[i];
        os << "{\"name\":\"" << tool.name()
           << "\",\"description\":\"" << tool.description()
           << "\",\"inputSchema\":{\"type\":\"object\",\"properties\":{";

        const std::vector<Property>& params = tool.params();
        for(std::size_t p = 0; p < params.size(); ++p)
        {
            if(p > 0)
                os << ',';

            os << "\"" << params[p].name() << "\":";
            params[p].type().toSchema(os, params[p].description());
        }

        os << "},\"required\":[";

        bool firstRequired = true;
        for(std::size_t p = 0; p < params.size(); ++p)
        {
            if(params[p].isRequired())
            {
                if( ! firstRequired)
                    os << ',';
                os << "\"" << params[p].name() << "\"";
                firstRequired = false;
            }
        }

        os << "]}}";
    }

    os << "]}";
}


namespace
{

static const char* const SupportedVersions[] =
{
    "2025-11-25",
    "2025-03-26",
    0
};

} // anonymous namespace


std::string ToolDeclaration::preferredVersion(const std::string& requested)
{
    for(int i = 0; SupportedVersions[i]; ++i)
    {
        if(requested == SupportedVersions[i])
            return SupportedVersions[i];
    }
    return SupportedVersions[0];
}


bool ToolDeclaration::isSupportedVersion(const std::string& version)
{
    for(int i = 0; SupportedVersions[i]; ++i)
    {
        if(version == SupportedVersions[i])
            return true;
    }
    return false;
}


void ToolDeclaration::toInitializeResult(std::ostream& os,
                                         const char* protocolVersion) const
{
    const char* version = protocolVersion ? protocolVersion : SupportedVersions[0];
    os << "{\"protocolVersion\":\"" << version << "\""
       << ",\"capabilities\":{\"tools\":{}}"
       << ",\"serverInfo\":{\"name\":\"" << _serverName
       << "\",\"version\":\"" << _serverVersion << "\"}}";
}

} // namespace Mcp

} // namespace Pt
