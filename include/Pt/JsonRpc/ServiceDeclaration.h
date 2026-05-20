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

#ifndef PT_JSONRPC_SERVICEDECLARATION_H
#define PT_JSONRPC_SERVICEDECLARATION_H

#include <Pt/JsonRpc/Api.h>
#include <Pt/JsonRpc/ProcedureDeclaration.h>
#include <string>
#include <map>

namespace Pt {

namespace JsonRpc {

/** @brief Declares named parameter mappings for JSON-RPC procedures.

    Associates a ProcedureDeclaration (name-to-position mapping) with
    each procedure name. Used together with a Remoting::ServiceDefinition
    which provides the runtime procedure registration.
*/
class PT_JSONRPC_API ServiceDeclaration
{
  public:
    /** @brief Default constructor.
    */
    ServiceDeclaration()
    {}

    /** @brief Associates a ProcedureDeclaration with a procedure name.
    */
    void addProcedure(const std::string& procedure, const ProcedureDeclaration& decl)
    { _procedures[procedure] = decl; }

    /** @brief Returns the ProcedureDeclaration for a procedure, or nullptr.
    */
    const ProcedureDeclaration* getProcedure(const std::string& procedure) const
    {
        std::map<std::string, ProcedureDeclaration>::const_iterator it = _procedures.find(procedure);
        if(it == _procedures.end())
            return 0;

        return &it->second;
    }

  private:
    std::map<std::string, ProcedureDeclaration> _procedures;
};

} // namespace JsonRpc

} // namespace Pt

#endif // PT_JSONRPC_SERVICEDECLARATION_H
