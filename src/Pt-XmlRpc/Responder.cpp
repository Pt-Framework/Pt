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
#include "Pt/XmlRpc/Fault.h"
#include "Pt/Xml/ParseError.h"
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
    _fault.clear();
    _state = OnBegin;
    _ts.attach( is );
    _args = 0;
}


std::size_t HttpXmlRpcResponder::readBody(std::istream& is)
{
    std::size_t n = 0;

   try
   {
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
    }
    catch(const Xml::ParseError& error)
    {
        _fault.setRc(1);
        _fault.setText( error.what() );
        throw;
    }
    catch(const SerializationError& error)
    {
        _fault.setRc(2);
        _fault.setText( error.what() );
        throw;
    }
    catch(const ConversionError& error)
    {
        _fault.setRc(3);
        _fault.setText( error.what() );
        throw;
    }

    return n;
}


void HttpXmlRpcResponder::replyError(std::ostream& os, Pt::Net::HttpRequest& request, Pt::Net::HttpReply& reply)
{
    reply.setHeader("Content-Type", "text/xml");

    os << "<?xml version=\"1.0\"?>\n"
          "<methodResponse>\n"
          "<fault>\n"
          "<value>\n"
          "<struct>\n"
          "<member>\n"
          "<name>faultCode</name>\n"
          "<value><int>" << _fault.rc() << "</int></value>\n"
          "</member>\n"
          "<member>\n"
          "<name>faultString</name>\n"
          "<value><string>" << _fault.what() << "</string></value>\n"
          "</member>\n"
          "</struct>\n"
          "</value>\n"
          "</fault>\n"
          "</methodResponse>\n";
}


void HttpXmlRpcResponder::reply(std::ostream& os, Pt::Net::HttpRequest& request, Pt::Net::HttpReply& reply)
{
    if( ! _proc )
        throw std::runtime_error("invalid XML-RPC, no method found");

    if( _args )
    {
        ++_args;
        if( * _args )
            throw std::runtime_error("invalid XML-RPC, missing arguments");
    }

    reply.setHeader("Content-Type", "text/xml");

    os << "<?xml version=\"1.0\"?>\n"
          "<methodResponse>\n";

    try
    {
        ITypeHandler* rh = _proc->endCall();

        os << "<params>\n";

        _formatter.begin( os );
        rh->decompose(_formatter);
        os << "</param>\n"

              "</params>\n"
              "</methodResponse>\n";
    }
    catch(const Fault& fault)
    {
        os << "<fault>\n"
              "<value>\n"
              "<struct>\n"
              "<member>\n"
              "<name>faultCode</name>\n"
              "<value><int>" << fault.rc() << "</int></value>\n"
              "</member>\n"
              "<member>\n"
              "<name>faultString</name>\n"
              "<value><string>" << fault.what() << "</string></value>\n"
              "</member>\n"
              "</struct>\n"
              "</value>\n"
              "</fault>\n"
              "</methodResponse>\n";
    }
}


void HttpXmlRpcResponder::advance(const Pt::Xml::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        { //std::cerr << "OnBegin" << std::endl;
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name() != "methodCall" )
                    throw Xml::ParseError( "invalid XML-RPC methodCall", _reader.line() );

                _state = OnMethodCallBegin;
            }

            break;
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
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                if( ee.name() != "methodName" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnMethodNameEnd;
            }
            break;
        }

        case OnMethodNameEnd:
        { //std::cerr << "OnMethodNameEnd" << std::endl;
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name() != "params" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnParams;
            }
            break;
        }

        case OnParams:
        { //std::cerr << "OnParams" << std::endl;
            if(node.type() == Xml::Node::EndElement) // </params>
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                if( ee.name() != "params" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnParamsEnd;
                break;
            }

            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name() != L"param" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

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
                    //std::cerr << "-> next argument" << std::endl;
                    ++_args;
                    if( ! *_args)
                        std::runtime_error("too many arguments");
                }

                _deserializer.begin(**_args);
                _state = OnParam;
                break;
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
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                if( ee.name() != "methodCall" )
                    throw std::runtime_error("invalid XML-RPC methodCall");

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
}

}
