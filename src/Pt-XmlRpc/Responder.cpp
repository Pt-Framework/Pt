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
#include "Pt/XmlRpc/Responder.h"
#include "Pt/XmlRpc/Service.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Utf8Codec.h"

namespace Pt {

namespace XmlRpc {


HttpXmlRpcResponder::HttpXmlRpcResponder(Service& service)
: Net::HttpResponder(service)
, _state(OnBegin)
, _ts(new Utf8Codec)
, _reader(_ts)
, _service(&service)
, _proc(0)
, _args(0)
{
}


HttpXmlRpcResponder::~HttpXmlRpcResponder()
{
}


void HttpXmlRpcResponder::beginRequest(std::istream& is, Pt::Net::HttpRequest& request)
{
    _ts.attach( is );
}


std::size_t HttpXmlRpcResponder::readBody(std::istream& is)
{
    std::size_t n = _ts.buffer().import();
    if(n == 0)
        return n;

    while( _reader.advance() )
    {
        const Pt::Xml::Node& node = _reader.get();
        switch(_state)
        {
            case OnBegin:
            { //std::cerr << "OnBegin" << std::endl;
                if(node.type() == Xml::Node::StartElement)
                {
                    const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                    if( se.name() == L"methodCall" )
                    {
                        _state = OnMethodCallBegin;
                        break;
                    }
                }
                throw std::runtime_error("invalid XML-RPC methodCall");
            }

            case OnMethodCallBegin:
            { //std::cerr << "OnMethodCallBegin" << std::endl;
                if(node.type() == Xml::Node::StartElement)
                {
                    _state = OnMethodNameBegin;
                }
                break;
            }

            case OnMethodNameBegin:
            { //std::cerr << "OnMethodNameBegin" << std::endl;
                if(node.type() == Xml::Node::Characters)
                {
                    const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);

                    _proc = _service->procedure( chars.content().narrow() );
                    if( ! _proc )
                        throw std::runtime_error("no such procedure");

                    //std::cerr << "-> Found Procedure: " << chars.content().narrow() << std::endl;

                    _state = OnMethodName;
                }
                break;
            }

            case OnMethodName:
            { //std::cerr << "OnMethodName" << std::endl;
                if(node.type() == Xml::Node::EndElement)
                {
                    _state = OnMethodNameEnd;
                }
                break;
            }

            case OnMethodNameEnd:
            { //std::cerr << "OnMethodNameEnd" << std::endl;
                if(node.type() == Xml::Node::StartElement)
                {
                    _state = OnParams;
                }
                break;
            }

            case OnParams:
            { //std::cerr << "OnParams" << std::endl;
                if(node.type() == Xml::Node::EndElement) // </params>
                {
                    _state = OnParamsEnd;
                    break;
                }

                if(node.type() == Xml::Node::StartElement)
                {
                    const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                    if( se.name() == L"param" )
                    {
                        //std::cerr << "-> Found param" << std::endl;
                        if( ! _args )
                        {
                            //std::cerr << "-> begin call" << std::endl;
                            _args = _proc->beginCall();
                            if( ! *_args)
                                std::runtime_error("too many arguments");
                        }
                        else
                        {
                            //std::cerr << "-> next arg" << std::endl;
                            ++_args;
                            if( ! *_args)
                                std::runtime_error("too many arguments");
                        }

                        _deserializer.begin(**_args);
                        _state = OnParam;
                    }
                }

                if(node.type() == Xml::Node::EndElement) // </params>
                {
                    _state = OnParamsEnd;
                }

                break;
            }

            case OnParam:
            { //std::cerr << "S: OnParam" << std::endl;
                bool finished = _deserializer.advance(node);
                if(finished)
                {
                    //std::cerr << "-> param finished" << std::endl; // node is </param>
                    _state = OnParams;
                }

                break;
            }

            case OnParamsEnd:
            { //std::cerr << "OnParamsEnd" << std::endl;
                if(node.type() == Xml::Node::EndElement) // </methodCall>
                {
                    _state = OnMethodCallEnd;
                }
                break;
            }

            case OnMethodCallEnd:
            { //std::cerr << "OnMethodCallEnd" << std::endl;
                if(node.type() == Xml::Node::EndDocument)
                {
                    _state = OnMethodCallEnd;
                }
                break;
            }
        }
    }

    return n;
}


void HttpXmlRpcResponder::reply(std::ostream& os, Pt::Net::HttpRequest& request, Pt::Net::HttpReply& reply)
{
    _state = OnBegin;

    if( ! _proc )
        return;

    reply.setHeader("Content-Type", "text/xml");

    ITypeHandler* rh = _proc->endCall();

    os << "<?xml version=\"1.0\"?>\n";
    os << "<methodResponse>\n";
    os << "<params>\n";

    _serializer.begin( os );
    rh->decompose(_serializer);
    os << "</param>\n";

    os << "</params>\n";
    os << "</methodResponse>\n";
}

}

}
