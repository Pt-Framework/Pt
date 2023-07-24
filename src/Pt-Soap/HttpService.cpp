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

#include <Pt/Soap/HttpService.h>
#include <Pt/Soap/HttpResponder.h>
#include <Pt/Soap/ServiceDefinition.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/Logger.h>
#include <cassert>

namespace Pt {

namespace Soap {

class WsdlResponder : public Pt::Http::Responder
{
    public:
        WsdlResponder(HttpService& httpService, const ServiceDeclaration& serviceDecl)
        : Pt::Http::Responder(httpService)
        , _serviceDecl( serviceDecl )
        {}

        ~WsdlResponder()
        {}

    protected:
        // inheritdoc
        Status onBeginRequest(Http::Request& request, Http::Reply& reply, 
                              System::EventLoop& loop)
        {
          return Continue;
        }

        // inheritdoc
        Status onReadRequest(Http::Request& request, Http::Reply& reply, 
                             System::EventLoop& loop)
        { return Continue; }

        // inheritdoc
        Status onBeginReply(const Http::Request& request, Http::Reply& reply,                                   System::EventLoop& loop)
        {
            _serviceDecl.toWsdl( reply.body() );
            return Done;
        }

        // inheritdoc
        Status onWriteReply(const Http::Request& request, Http::Reply& reply,
                            System::EventLoop& loop)
        { return Done; }

    private:
        const ServiceDeclaration& _serviceDecl;
};


HttpService::HttpService(ServiceDefinition& serviceDef)
: _serviceDecl( &serviceDef.declaration() )
, _serviceDef( &serviceDef )
{ 
}


HttpService::HttpService(const ServiceDeclaration& decl, Remoting::ServiceDefinition& def)
: _serviceDecl(&decl)
, _serviceDef(&def)
{
}


HttpService::~HttpService()
{
}


Http::Responder* HttpService::onGetResponder(const Http::Request& req)
{
    //if (req.isHeaderValue("Content-Type", "text/xml"))
    //    return new XmlRpcResponder(*this);

    //if (req.isHeaderValue("Content-Type", "text/xml; charset=UTF-8")) //! ### Temporary fix ###
    //    return new XmlRpcResponder(*this);

    if(req.qparams() == "wsdl")
    {
        return new WsdlResponder(*this, *_serviceDecl);
    }

    return new HttpResponder(*this, *_serviceDecl, *_serviceDef);
}


void HttpService::onReleaseResponder(Http::Responder* resp)
{
    delete resp;
}

} // namespace Soap

} // namespace Pt
