/* Copyright (C) 2020 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#ifndef Pt_JsonRpc_Fault_h
#define Pt_JsonRpc_Fault_h

#include <Pt/JsonRpc/Api.h>
#include <Pt/Remoting/Fault.h>
#include <Pt/SerializationInfo.h>
#include <string>

namespace Pt {

namespace JsonRpc {

/** @brief JSON-RPC fault exception.
*/
class PT_JSONRPC_API Fault : public Remoting::Fault
{
    friend void operator >>=(const Pt::SerializationInfo&, Fault&);

    public:
        /** @brief JSON-RPC fault error codes.
        */
        enum ErrorCodes 
        {
            ParseError                  = -32700, //!< Parse error
            InvalidRequest              = -32600, //!< Invalid JSON-RPC
            MethodNotFound              = -32601, //!< Method not found in service
            InvalidParameters           = -32602, //!< Invalid method parameters
            InternalError               = -32603, //!< Internal error
        };

        /** @brief Construct with message and error code.
        */
        Fault(const std::string& msg, int ec);

        /** @brief Construct with message and error code.
        */
        Fault(const char* msg, int rc);

        /** @brief Destructor.
        */
        ~Fault() throw()
        { }

        /** @brief Returns the error code.
        */
        int code() const
        { return _code; }

    private:
        int _code;
};

//! @internal
inline void operator >>=(const Pt::SerializationInfo& si, Fault& fault)
{
    int code = 0;
    std::string msg;

    si.getMember("code") >>= code;
    si.getMember("message").getString(msg );
    
    fault = Fault(msg, code);
}

//! @internal
inline void operator <<=(Pt::SerializationInfo& si, const Fault& fault)
{
    si.addMember("code") <<= static_cast<Pt::int32_t>( fault.code() );
    si.addMember("message").setString( fault.what() );
}

} // namespace

} // namespace

#endif
