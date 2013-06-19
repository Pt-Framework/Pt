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
#include <Pt/XmlRpc/Service.h>
#include <Pt/Xml/InputSource.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Http/Responder.h>
#include <Pt/SerializationContext.h>
#include <Pt/Serializer.h>
#include <Pt/TextStream.h>

namespace Pt {

namespace XmlRpc {

class Service;
class ServiceProcedure;

class PT_XMLRPC_API XmlRpcResponderBase : public Responder
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
        XmlRpcResponderBase(Service& service);

        ~XmlRpcResponderBase();

    protected:
        void beginRequest(std::istream& is);

        void advanceRequest();

        void finishRequest(System::EventLoop& loop);

        void formatReply(std::ostream& os);

        void formatError(std::ostream& os, int rc, const char* msg);

    private:
        void advance(const Pt::Xml::Node& node);

    private:
       State _state;
       Utf8Codec _utf8;
       TextOStream _ts;
       Xml::BinaryInputSource _bin;
       Xml::XmlReader _reader;
       Scanner _scanner;
       Formatter _formatter;
       Service* _service;
       ServiceProcedure* _proc;
       IComposer** _args;
       IDecomposer* _result;
};


class PT_XMLRPC_API XmlRpcResponder : public Http::Responder
                                    , public XmlRpcResponderBase
{
    public:
        XmlRpcResponder(Service& service);

        ~XmlRpcResponder();

    protected:
        void onBeginRequest(Http::Request& request, Http::Reply& reply, System::EventLoop& loop);

        void onReadRequest(Http::Request& request, Http::Reply& reply, System::EventLoop& loop);

        void onBeginReply(Http::Request& request, Http::Reply& reply, System::EventLoop& loop);

        void onWriteReply(Http::Request& request, Http::Reply& reply, System::EventLoop& loop);

        void replyError(Http::Reply& reply, int rc, const char* msg);

    protected:
        // inheritdoc
        virtual void onEndReply();

    private:
         Http::Reply* _reply;
};

} // namespace XmlRpc

} // namespace Pt

#endif
