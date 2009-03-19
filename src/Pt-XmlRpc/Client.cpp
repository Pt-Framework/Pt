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
#include "Pt/XmlRpc/Client.h"
#include "Pt/XmlRpc/RemoteProcedure.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/System/Selector.h"
#include "Pt/Utf8Codec.h"

namespace Pt {

namespace XmlRpc {

Client::Client(System::SelectorBase& selector, const std::string& server,
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
    connect(_client.headerReceived, *this, &Client::onReplyHeader);
    connect(_client.bodyAvailable, *this, &Client::onReplyBody);
    connect(_client.replyFinished, *this, &Client::onReplyFinished);
}


Client::Client(const std::string& server, unsigned short port, const std::string& url)
: _state(OnBegin)
, _url(url)
, _client(server, port)
, _request(url)
, _ts( new Utf8Codec )
, _reader(_ts)
, _method(0)
{
    connect(_client.headerReceived, *this, &Client::onReplyHeader);
    connect(_client.bodyAvailable, *this, &Client::onReplyBody);
}


Client::~Client()
{
}


void Client::beginCall(ITypeHandler& r, IRemoteProcedure& method, ITypeHandler** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    this->prepareRequest(method.name(), argv, argc);
    _client.beginExecute(_request);
    _deserializer.begin(r);
}


void Client::call(ITypeHandler& r, IRemoteProcedure& method, ITypeHandler** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    this->prepareRequest(method.name(), argv, argc);
    Net::HttpReplyHeader header = _client.execute(_request);

    std::string body = _client.readBody();
    std::istringstream is(body);
    _ts.attach(is);
    _deserializer.begin(r);

    while( _reader.get().type() !=  Pt::Xml::Node::EndDocument )
    {
        const Pt::Xml::Node& node = _reader.get();
        this->advance(node);
        _reader.next();
    }

    _ts.detach();
    _state = OnBegin;
}


void Client::onReplyHeader(Net::HttpClient& client)
{
    _ts.attach( client.in() );
}


std::size_t Client::onReplyBody(Net::HttpClient& client)
{
    std::size_t n = 0;

    while(true)
    {
        std::streamsize m = _ts.buffer().import();
        if( ! m)
            break;

        n += m;

        while( _reader.advance() )
        {
            const Pt::Xml::Node& node = _reader.get();
            this->advance(node);
        }
    }

    return n;
}


void Client::onReplyFinished(Net::HttpClient& client)
{
    if(_state == OnMethodResponseEnd)
        _method->onFinished();
}


void Client::prepareRequest(const std::string& name, ITypeHandler** argv, unsigned argc)
{
    _request.clear();
    _request.url(_url);
    _request.setHeader("Content-Type", "text/xml");

    _request.body() << "<?xml version=\"1.0\"?>\n";
    _request.body() << "<methodCall>\n";
    _request.body() << "<methodName>" << name << "</methodName>\n";
    _request.body() << "<params>\n";

    for(unsigned n = 0; n < argc; ++n)
    {
        _formatter.begin( _request.body() );
        argv[n]->decompose(_formatter);
        _request.body() << "</param>\n";
    }

    _request.body() << "</params>\n";
    _request.body() << "</methodCall>\n";
}


void Client::advance(const Pt::Xml::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        { //std::cerr << "Client:: OnBegin" << std::endl;
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
        { //std::cerr << "Client:: OnMethodResponseBegin" << std::endl;
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
        { //std::cerr << "Client:: OnParams" << std::endl;
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
        { //std::cerr << "Client:: OnParam" << std::endl;
            bool finished = _deserializer.advance(node); // start with <value>
            if(finished)
            {
                // </param>
                _state = OnParamEnd;
            }

            break;
        }

        case OnParamEnd:
        { //std::cerr << "Client:: OnParamsEnd" << std::endl;
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
        { //std::cerr << "Client:: OnParamsEnd" << std::endl;
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
        { //std::cerr << "Client:: OnMethodResponseEnd" << std::endl;
            _state = OnMethodResponseEnd;
            break;
        }
    }
}

}

}
