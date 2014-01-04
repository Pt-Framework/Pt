/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
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

#ifndef Pt_XmlRpc_HttpService_h
#define Pt_XmlRpc_HttpService_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/ServiceDefinition.h>
#include <Pt/Http/Service.h>
#include <Pt/Types.h>

namespace Pt {

namespace XmlRpc {

/** @brief HTTP service for XML-RPC.

    The %HttpService makes procedures registered in a XML-RPC Service
    available for HTTP. Use the HttpClient to run a RemoteProcedure accessing
    this service.
*/
class PT_XMLRPC_API HttpService : public Http::Service
{
    public:
        /** @brief Constructs with RPC service.
        */
        HttpService(XmlRpc::ServiceDefinition& rpcService);

        /** @brief Destructor.
        */
        virtual ~HttpService();

    protected:
        // inheritdoc
        virtual Http::Responder* onGetResponder(const Http::Request&);

        // inheritdoc
        virtual void onReleaseResponder(Http::Responder* resp);

    private:
        XmlRpc::ServiceDefinition* _rpcService;
        Pt::varint_t _r1;
        Pt::varint_t _r2;
};

} // namespace XmlRpc

} // namespace Pt

#endif // Pt_XmlRpc_HttpService_h
