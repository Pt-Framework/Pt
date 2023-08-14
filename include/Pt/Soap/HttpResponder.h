/*
 * Copyright (C) 2014 by Dr. Marc Boris Duerner
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

#ifndef Pt_Soap_HttpResponder_h
#define Pt_Soap_HttpResponder_h

#include <Pt/Soap/Api.h>
#include <Pt/Soap/Responder.h>
#include <Pt/Remoting/ServiceDefinition.h>
#include <Pt/Http/Responder.h>
#include <Pt/System/EventLoop.h>

namespace Pt {

namespace Soap {

class Fault;
class HttpService;
class ServiceDeclaration;

class PT_SOAP_API HttpResponder : public Http::Responder
                                , public Responder
{
    public:
        HttpResponder(HttpService& httpService, const ServiceDeclaration& decl, Remoting::ServiceDefinition& def);

        ~HttpResponder();

        Http::Request* request()
        { return _request; }

        Http::Reply* reply()
        { return _reply; }

    protected:
        // inheritdoc
        void onBeginRequest(Http::Request& request, Http::Reply& reply, 
                            System::EventLoop& loop);

        // inheritdoc
        void onReadRequest(Http::Request& request, Http::Reply& reply, 
                           System::EventLoop& loop);

        // inheritdoc
        void onBeginReply(const Http::Request& request, Http::Reply& reply, 
                          System::EventLoop& loop);

        // inheritdoc
        void onWriteReply(const Http::Request& request, Http::Reply& reply,
                          System::EventLoop& loop);

    protected:
        // inheritdoc
        virtual void onResult();

        // inheritdoc
        virtual void onFault(const Fault& fault);

        // inheritdoc
        virtual void onCancel();

        bool advanceReply(Http::Reply& reply);

    private:
        Http::Request* _request;
        Http::Reply* _reply;
};

} // namespace Soap

} // namespace Pt

#endif // Pt_Soap_HttpResponder_h
