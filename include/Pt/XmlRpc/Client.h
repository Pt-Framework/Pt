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
#include <Pt/XmlRpc/Fault.h>
#include <Pt/XmlRpc/Formatter.h>
#include <Pt/XmlRpc/Scanner.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Xml/XmlWriter.h>
#include <Pt/Http/Client.h>
#include <Pt/Deserializer.h>
#include <Pt/SerializationContext.h>
#include <Pt/Serializer.h>
#include <Pt/Connectable.h>
#include <Pt/TextStream.h>
#include <string>
#include <sstream>
#include <cstddef>

namespace Pt {

namespace System {

class SelectorBase;

}

namespace XmlRpc {

class IRemoteProcedure;


class PT_XMLRPC_API Client : public Pt::Connectable
{
    enum State
    {
        OnBegin,
        OnMethodResponseBegin,
        OnFaultBegin,
        OnFaultEnd,
        OnFaultResponseEnd,
        OnParamsBegin,
        OnParam,
        OnParamEnd,
        OnParamsEnd,
        OnMethodResponseEnd
    };

    public:
        Client();

        Client(System::SelectorBase& selector, const std::string& addr,
               unsigned short port, const std::string& url);

        Client(const std::string& addr, unsigned short port, const std::string& url);

        virtual ~Client();

        void connect(const std::string& addr, unsigned short port,
                     const std::string& url)
        {
            _client.connect(addr, port);
            _request.url(url);
        }

        void beginCall(IDeserializer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        void call(IDeserializer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        std::size_t timeout() const  { return _timeout; }

        void timeout(std::size_t t)  { _timeout = t; }

        SerializationContext& context()
        { return _context; }

        std::string url() const
        {
            std::ostringstream s;
            s << "http://"
              << _client.server()
              << ':'
              << _client.port()
              << _request.url();

            return s.str();
        }

    protected:
        void onReplyHeader(Http::Client& client);

        std::size_t onReplyBody(Http::Client& client);

        void onReplyFinished(Http::Client& client);

        void onErrorOccured(Http::Client& client, const std::exception& e);

        void prepareRequest(const std::string& name, IDecomposer** argv, unsigned argc);

        void advance(const Xml::Node& node);

    private:
        State _state;
        Http::Client _client;
        Http::Request _request;
        TextIStream _ts;
        Xml::XmlReader _reader;
        Xml::XmlWriter _writer;
        Formatter _formatter;
        Scanner _scanner;
        IRemoteProcedure* _method;
        SerializationContext _context;
        Fault _fault;
        Deserializer<Fault> _fh;
        std::size_t _timeout;
};

}

}

#endif
