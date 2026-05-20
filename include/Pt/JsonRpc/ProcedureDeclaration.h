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

#ifndef PT_JSONRPC_PROCEDUREDECLARATION_H
#define PT_JSONRPC_PROCEDUREDECLARATION_H

#include <Pt/JsonRpc/Api.h>
#include <string>
#include <vector>
#include <map>

namespace Pt {

namespace JsonRpc {

/** @brief Maps parameter names to positional indices for named parameters.

    JSON-RPC 2.0 allows parameters to be passed as a JSON object with named
    keys. The ProcedureDeclaration maps each name to its positional index so
    the Responder can dispatch named parameters to the correct Composer.
*/
class PT_JSONRPC_API ProcedureDeclaration
{
  public:
    /** @brief Default constructor.
    */
    ProcedureDeclaration()
    {}

    /** @brief Register a parameter name at a positional index.
    */
    void setParamName(std::size_t index, const std::string& name)
    {
        if(index >= _names.size())
            _names.resize(index + 1);

        _names[index] = name;
        _indexByName[name] = index;
    }

    /** @brief Lookup positional index by parameter name.

        @return The index, or -1 if not found.
    */
    int getParamIndex(const std::string& name) const
    {
        std::map<std::string, std::size_t>::const_iterator it = _indexByName.find(name);
        if(it == _indexByName.end())
            return -1;

        return static_cast<int>(it->second);
    }

    /** @brief Lookup parameter name by positional index.

        @return The name, or an empty string if index is out of range.
    */
    const std::string& getParamName(std::size_t index) const
    {
        static const std::string empty;
        if(index >= _names.size())
            return empty;

        return _names[index];
    }

    /** @brief Returns the number of named parameters.
    */
    std::size_t paramCount() const
    { return _names.size(); }

    /** @brief Returns true if any parameter names are registered.
    */
    bool hasNames() const
    { return ! _names.empty(); }

  private:
    std::vector<std::string> _names;
    std::map<std::string, std::size_t> _indexByName;
};

} // namespace JsonRpc

} // namespace Pt

#endif // PT_JSONRPC_PROCEDUREDECLARATION_H
