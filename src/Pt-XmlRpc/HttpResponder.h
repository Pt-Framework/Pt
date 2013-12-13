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

#ifndef Pt_XmlRpc_HttpResponder_h
#define Pt_XmlRpc_HttpResponder_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Responder.h>
#include <Pt/Http/Responder.h>

namespace Pt {

namespace XmlRpc {

class HttpService;

class PT_XMLRPC_API HttpResponder : public Http::Responder
                                  , public Responder
{
    public:
        HttpResponder(HttpService& httpService, ServiceDefinition& rpcService);

        ~HttpResponder();

    protected:
        // inheritdoc
        void onBeginRequest(Http::Request& request, Pt::Http::Reply& reply, System::EventLoop& loop);

        // inheritdoc
        void onReadRequest(Http::Request& request, Pt::Http::Reply& reply, System::EventLoop& loop);

        // inheritdoc
        void onBeginReply(const Http::Request& request, Http::Reply& reply, System::EventLoop& loop);

        // inheritdoc
        void onWriteReply(const Http::Request& request, Http::Reply& reply, System::EventLoop& loop);

    protected:
        // inheritdoc
        virtual void onResult();

        // inheritdoc
        virtual void onCancel();

        // inheritdoc
        virtual void onError();

    private:
         Http::Reply* _reply;
};

} // namespace XmlRpc

} // namespace Pt

#endif // Pt_XmlRpc_HttpResponder_h
