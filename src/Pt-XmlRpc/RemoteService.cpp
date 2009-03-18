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
#include "Pt/XmlRpc/RemoteService.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/System/Selector.h"
#include "Pt/Utf8Codec.h"

namespace Pt {

namespace XmlRpc {

RemoteService::RemoteService(System::SelectorBase& selector, const std::string& server,
                             unsigned short port, const std::string& url)
: _state(OnBegin)
, _url(url)
, _client(server, port)
, _request(url)
, _ts( new Utf8Codec )
, _reader(_ts)
, _method(0)
{
    _client.setSelector(selector);
    connect(_client.headerReceived, *this, &RemoteService::onReplyHeader);
    connect(_client.bodyAvailable, *this, &RemoteService::onReplyBody);
}


RemoteService::RemoteService(const std::string& server, unsigned short port, const std::string& url)
: _state(OnBegin)
, _url(url)
, _client(server, port)
, _request(url)
, _ts( new Utf8Codec )
, _reader(_ts)
, _method(0)
{
    connect(_client.headerReceived, *this, &RemoteService::onReplyHeader);
    connect(_client.bodyAvailable, *this, &RemoteService::onReplyBody);
}


RemoteService::~RemoteService()
{
}


void RemoteService::beginCall(ITypeHandler& r, IRemoteMethod& method, ITypeHandler& a1, ITypeHandler& a2)
{
    _method = &method;
    _state = OnBegin;

    this->prepareRequest(method.name(), a1, a2);
    _client.beginExecute(_request);
    _deserializer.begin(r);
}


void RemoteService::call(ITypeHandler& r, IRemoteMethod& method, ITypeHandler& a1, ITypeHandler& a2)
{
    _state = OnBegin;

    this->prepareRequest(method.name(), a1, a2);
    _client.execute(_request);

    _ts.attach( _client.in() );
    while( _state != OnMethodResponseEnd )
    {
        const Pt::Xml::Node& node = _reader.next();
        this->advance(node);
    }

    _state = OnBegin;
}


void RemoteService::onReplyHeader(Net::HttpReply& reply)
{
    _ts.attach( _client.in() );
}


std::size_t RemoteService::onReplyBody(Net::HttpClient& client)
{
    std::size_t n = _ts.buffer().import();

    while( _reader.advance() )
    {
        const Pt::Xml::Node& node = _reader.get();
        this->advance(node);

        if(_state == OnMethodResponseEnd)
            _method->onFinished();
    }

    return n;
}


void RemoteService::prepareRequest(const std::string& name, ITypeHandler& a1, ITypeHandler& a2)
{
    _request.clear();
    _request.url(_url);
    _request.setHeader("Content-Type", "text/xml");

    _request.body() << "<?xml version=\"1.0\"?>\n";
    _request.body() << "<methodCall>\n";
    _request.body() << "<methodName>" << name << "</methodName>\n";
    _request.body() << "<params>\n";

    _formatter.begin( _request.body() );
    a1.decompose(_formatter);
    _request.body() << "</param>\n";

    _formatter.begin( _request.body() );
    a2.decompose(_formatter);
    _request.body() << "</param>\n";

    _request.body() << "</params>\n";
    _request.body() << "</methodCall>\n";
}


void RemoteService::advance(const Pt::Xml::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        { //std::cerr << "RemoteService:: OnBegin" << std::endl;
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name() != "methodResponse" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnMethodResponseBegin;
            }

            break;
        }

        case OnMethodResponseBegin:
        { //std::cerr << "RemoteService:: OnMethodResponseBegin" << std::endl;
            if(node.type() == Xml::Node::StartElement) // <params>
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name() != "params" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnParamsBegin;
            }
            break;
        }

        case OnParamsBegin:
        { //std::cerr << "RemoteService:: OnParams" << std::endl;
            if(node.type() == Xml::Node::StartElement) // <param>
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name() != "param" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnParam;
            }

            break;
        }

        case OnParam:
        { //std::cerr << "RemoteService:: OnParam" << std::endl;
            bool finished = _deserializer.advance(node); // start with <value>
            if(finished)
            {
                // </param>
                _state = OnParamEnd;
            }

            break;
        }

        case OnParamEnd:
        { //std::cerr << "RemoteService:: OnParamsEnd" << std::endl;
            if(node.type() == Xml::Node::EndElement) // </params>
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                if( ee.name() != "params" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnParamsEnd;
            }
            break;
        }

        case OnParamsEnd:
        { //std::cerr << "RemoteService:: OnParamsEnd" << std::endl;
            if(node.type() == Xml::Node::EndElement) // </methodResponse>
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                if( ee.name() != "methodResponse" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnMethodResponseEnd;
            }
            break;
        }

        case OnMethodResponseEnd:
        { //std::cerr << "RemoteService:: OnMethodResponseEnd" << std::endl;
            _state = OnEnd;
            break;
        }

        case OnEnd:
        { //std::cerr << "RemoteService:: OnMethodResponseEnd" << std::endl;
            _state = OnEnd;
            break;
        }
    }
}

}

}
