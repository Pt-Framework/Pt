/*
 * Copyright (C) 2009-2013 by Dr. Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Pt_XmlRpc_Fault_h
#define Pt_XmlRpc_Fault_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/Utf8Codec.h>
#include <Pt/SerializationInfo.h>
#include <stdexcept>
#include <string>

namespace Pt {

namespace XmlRpc {

/** @brief XML-RPC fault exception.
*/
class PT_XMLRPC_API Fault : public std::exception
{
    friend void operator >>=(const Pt::SerializationInfo&, Fault&);

    public:
        /** @brief XML-RPC fault error codes.
        */
        enum ErrorCodes 
        {
            ParseError                  = -32700,
            UnsupportedEncoding         = -32701,
            InvalidCharacterForEncoding = -32702,
            InvalidXmlRpc               = -32600,
            MethodNotFound              = -32601,
            InvalidMethodParameters     = -32602,
            InternalXmlRpcError         = -32603,
            ApplicationError            = -32500,
            SystemError                 = -32400,
            TransportError              = -32300
        };

        /** @brief Construct with message and error code.
        */
        Fault(const std::string& msg, int rc);

        /** @brief Construct with message and error code.
        */
        Fault(const char* msg, int rc);

        /** @brief Constructor.
        */
        Fault()
        : _rc(0)
        { }

        /** @brief Destructor.
        */
        ~Fault() throw()
        { }

        /** @brief Clears all content.
        */
        void clear()
        {
            _rc = 0;
            _msg.clear();
        }

        /** @brief Returns the error code.
        */
        int rc() const
        { return _rc; }

        /** @brief Sets the error code.
        */
        void setRc(int rc)
        { _rc = rc; }

        /** @brief Sets the error message.
        */
        void setText(const std::string& msg)
        { _msg = msg; }

        /** @brief Resturns the error message.
        */
        const std::string& text() const
        { return _msg; }

        // inheritdoc
        const char* what() const throw()
        { return _msg.c_str(); }

    private:
        std::string _msg;
        int _rc;
};


inline void operator >>=(const Pt::SerializationInfo& si, Fault& fault)
{
    si.getMember("faultCode") >>= fault._rc;
    si.getMember("faultString").getString( fault._msg );
}


inline void operator <<=(Pt::SerializationInfo& si, const Fault& fault)
{
    si.addMember("faultCode") <<= static_cast<Pt::int32_t>( fault.rc() );
    si.addMember("faultString").setString( fault.text() );
}

} // namespace XmlRpc

} // namespace Pt

#endif
