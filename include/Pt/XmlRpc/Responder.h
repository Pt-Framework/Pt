/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 * Copyright (C) 2009 by Tommi Meakitalo
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
#ifndef Pt_XmlRpc_Responder_h
#define Pt_XmlRpc_Responder_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Fault.h>
#include <Pt/XmlRpc/Scanner.h>
#include <Pt/XmlRpc/Formatter.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/XmlWriter.h>
#include <Pt/Http/Server.h>
#include <Pt/Serializer.h>
#include <Pt/TextStream.h>

namespace Pt {

namespace XmlRpc {

class Service;
class ServiceProcedure;

class PT_XMLRPC_API XmlRpcResponder : public Http::Responder
{
    enum State
    {
        OnBegin,
        OnMethodCallBegin,
        OnMethodNameBegin,
        OnMethodName,
        OnMethodNameEnd,
        OnParams,
        OnParam,
        OnParamsEnd,
        OnMethodCallEnd
    };

    public:
        XmlRpcResponder(Service& service);

        ~XmlRpcResponder();

        void beginRequest(std::istream& in, Http::Request& request);

        std::size_t readBody(std::istream& is);

        void replyError(std::ostream& os, Http::Request& request,
                        Http::Reply& reply, const std::exception& ex);

        void reply(std::ostream& os, Http::Request& request, Http::Reply& reply);

    protected:
        void advance(const Pt::Xml::Node& node);

    private:
       State _state;
       TextIStream _ts;
       Xml::XmlReader _reader;
       Xml::XmlWriter _writer;
       Scanner _scanner;
       Formatter _formatter;
       Service* _service;
       SerializationContext _context;
       ServiceProcedure* _proc;
       IDeserializer** _args;
       ISerializer* _result;
       Fault _fault;
};

}

}

#endif
