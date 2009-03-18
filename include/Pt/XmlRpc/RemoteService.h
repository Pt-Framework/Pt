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
#ifndef Pt_XmlRpc_RemoteService_h
#define Pt_XmlRpc_RemoteService_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Formatter.h>
#include <Pt/XmlRpc/Deserializer.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Net/HttpClient.h>
#include <Pt/Connectable.h>
#include <Pt/TextStream.h>
#include <string>
#include <cstddef>

namespace Pt {

namespace System {

class SelectorBase;

}

namespace XmlRpc {

class IRemoteProcedure
{
    friend class RemoteService;

    public:
        IRemoteProcedure(const std::string& name)
        :_name(name)
        { }

        virtual ~IRemoteProcedure()
        { }

        const std::string& name() const
        { return _name; }

    protected:
        virtual void onFinished() = 0;

    private:
        std::string _name;
};


class PT_XMLRPC_API RemoteService : public Pt::Connectable
{
    enum State
    {
        OnBegin,
        OnMethodResponseBegin,
        OnParamsBegin,
        OnParam,
        OnParamEnd,
        OnParamsEnd,
        OnMethodResponseEnd,
        OnEnd,
    };

    public:
        RemoteService(System::SelectorBase& selector, const std::string& addr,
                      unsigned short port, const std::string& url);

        RemoteService(const std::string& addr, unsigned short port, const std::string& url);

        virtual ~RemoteService();

        void beginCall(ITypeHandler& r, IRemoteProcedure& method,
                       ITypeHandler& a1, ITypeHandler& a2);

        void call(ITypeHandler& r, IRemoteProcedure& method,
                  ITypeHandler& a1, ITypeHandler& a2);

    protected:
        void onReplyHeader(Net::HttpReply& reply);

        std::size_t onReplyBody(Net::HttpClient& client);

        void prepareRequest(const std::string& name, ITypeHandler& a1, ITypeHandler& a2);

        void advance(const Xml::Node& node);

    private:
        State _state;
        std::string _url;
        Net::HttpClient _client;
        Net::HttpRequest _request;
        TextIStream _ts;
        Xml::XmlReader _reader;
        Formatter _formatter;
        Deserializer _deserializer;
        ITypeHandler* _rhandler;
        IRemoteProcedure* _method;
};

}

}

#endif
