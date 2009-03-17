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
{
    _client.setSelector(selector);
    connect(_client.bodyReceived, *this, &RemoteService::onReplyBody);
}


RemoteService::~RemoteService()
{
}


void RemoteService::beginCall(ITypeHandler& r, const std::string& name, ITypeHandler& a1, ITypeHandler& a2)
{
    _state = OnBegin;

    _request.body() << "<?xml version=\"1.0\"?>\n";
    _request.body() << "<methodCall>\n";
    _request.body() << "<methodName>" << name << "</methodName>\n";
    _request.body() << "<params>\n";

    _serializer.begin( _request.body() );
    a1.decompose(_serializer);
    _request.body() << "</param>\n";

    _serializer.begin( _request.body() );
    a2.decompose(_serializer);
    _request.body() << "</param>\n";

    _request.body() << "</params>\n";
    _request.body() << "</methodCall>\n";

    _client.beginExecute(_request);
    _deserializer.begin(r);
}


void RemoteService::endCall()
{
    while( _state != OnMethodResponseEnd )
    { }

    _state = OnBegin;
}


std::size_t RemoteService::onReplyBody(Net::HttpClient& client)
{
    _ts.attach( client.in() );
    std::size_t n = _ts.buffer().import();
    if(n == 0)
        return n;

    while( _reader.advance() )
    {
        const Pt::Xml::Node& node = _reader.get();
        switch(_state)
        {
            case OnBegin:
            { //std::cerr << "RemoteService:: OnBegin" << std::endl;
                if(node.type() == Xml::Node::StartElement) // <methodResponse>
                {
                    _state = OnMethodResponseBegin;
                }
                break;
            }

            case OnMethodResponseBegin:
            { //std::cerr << "RemoteService:: OnMethodResponseBegin" << std::endl;
                if(node.type() == Xml::Node::StartElement) // <params>
                {
                    _state = OnParamsBegin;
                }
                break;
            }

            case OnParamsBegin:
            { //std::cerr << "RemoteService:: OnParams" << std::endl;
                if(node.type() == Xml::Node::StartElement) // <param>
                {
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
                    _state = OnParamsEnd;
                }
                break;
            }

            case OnParamsEnd:
            { //std::cerr << "RemoteService:: OnParamsEnd" << std::endl;
                if(node.type() == Xml::Node::EndElement) // </methodResponse>
                {
                    _state = OnMethodResponseEnd;
                }
                break;
            }

            case OnMethodResponseEnd:
            { //std::cerr << "RemoteService:: OnMethodResponseEnd" << std::endl;
                if(node.type() == Xml::Node::EndDocument)
                {
                    _state = OnMethodResponseEnd;
                }
                break;
            }
        }
    }

    return n;
}

}

}
