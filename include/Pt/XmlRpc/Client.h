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
#ifndef Pt_XmlRpc_Client_h
#define Pt_XmlRpc_Client_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Serializer.h>
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

class PT_XMLRPC_API RemoteService : public Pt::Connectable
{
    enum State
    {
        OnBegin,
        OnMethodResponseBegin,
        OnParams,
        OnParamsEnd,
        OnMethodResponseEnd
    };

    public:
        RemoteService(System::SelectorBase& selector, const std::string& addr, unsigned short port, const std::string& url);

        virtual ~RemoteService();

        void beginCall(ITypeHandler& r, const std::string& name, ITypeHandler& a1, ITypeHandler& a2)
        {
            _request.body() << "<?xml version=\"1.0\"?>\n";
            _request.body() << "<methodCall>\n";
            _request.body() << "<methodName>" << name << "</methodName>\n";
            _request.body() << "<params>\n";

            _request.body() << "<param>\n";
            a1.decompose(_serializer);
            _request.body() << "</param>\n";

            _request.body() << "<param>\n";
            a2.decompose(_serializer);
            _request.body() << "</param>\n";

            _request.body() << "</params>\n";
            _request.body() << "</methodCall>\n";

            _client.beginExecute(_request);
            _deserializer.begin(r);
        }

    protected:
        std::size_t onReplyBody(Net::HttpClient& client);

    private:
        State _state;
        std::string _url;
        Net::HttpClient _client;
        Net::HttpRequest _request;
        TextIStream _ts;
        Xml::XmlReader _reader;
        Serializer _serializer;
        Deserializer _deserializer;
        ITypeHandler* _rhandler;
};


template <typename R,
          typename A1,
          typename A2 >
class PT_XMLRPC_API RemoteMethod
{
    public:
        RemoteMethod(RemoteService& service, const std::string& name)
        : _name(name)
        , _service(&service)
        { }

        ~RemoteMethod()
        {}

        void begin(const A1& a1, const A2& a2)
        {
            _a1handler.begin(a1);
            _a2handler.begin(a2);
            _rhandler.begin(_result);
            _service->beginCall(_rhandler, _name, _a1handler, _a2handler);
        }

        const R& result()
        { return _result; }

    private:
        std::string _name;
        RemoteService* _service;
        R _result;
        TypeHandler<R> _rhandler;
        TypeHandler<A1> _a1handler;
        TypeHandler<A1> _a2handler;
};

}

}

#endif
