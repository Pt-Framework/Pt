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
#include "Pt/Xml/XmlError.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/Http/Reply.h"
#include <Pt/Http/Request.h>
#include "Pt/Utf8Codec.h"
#include "Pt/Convert.h"
#include <cassert>

namespace Pt {

namespace XmlRpc {

static const Pt::Char XMLRPC_METHODRESPONSE[]  = { 'm', 'e', 't', 'h', 'o', 'd', 'R', 'e', 's', 'p', 'o', 'n', 's', 'e', '\0' };
static const Pt::Char XMLRPC_METHODCALL[]  = { 'm', 'e', 't', 'h', 'o', 'd', 'C', 'a', 'l', 'l', '\0' };
static const Pt::Char XMLRPC_PARAMS[]  = { 'p', 'a', 'r', 'a', 'm', 's', '\0' };
static const Pt::Char XMLRPC_PARAM[]  = { 'p', 'a', 'r', 'a', 'm', '\0' };
static const Pt::Char XMLRPC_FAULT[]  = { 'f', 'a', 'u', 'l', 't', '\0' };
static const Pt::Char XMLRPC_FAULTCODE[]  = { 'f', 'a', 'u', 'l', 't', 'C', 'o', 'd', 'e', '\0' };
static const Pt::Char XMLRPC_FAULTSTRING[]  = { 'f', 'a', 'u', 'l', 't', 'S', 't', 'r', 'i', 'n', 'g', '\0' };
static const Pt::Char XMLRPC_STRUCT[]  = { 's', 't', 'r', 'u', 'c', 't', '\0' };
static const Pt::Char XMLRPC_MEMBER[]  = { 'm', 'e', 'm', 'b', 'e', 'r', '\0' };
static const Pt::Char XMLRPC_NAME[]    = { 'n', 'a', 'm', 'e', '\0' };
static const Pt::Char XMLRPC_VALUE[]   = { 'v', 'a', 'l', 'u', 'e', '\0' };
static const Pt::Char XMLRPC_INT[]     = { 'i', 'n', 't', '\0' };
static const Pt::Char XMLRPC_STRING[]  = { 's', 't', 'r', 'i', 'n', 'g', '\0' };

XmlRpcResponder::XmlRpcResponder(Service& service)
: Http::Responder(service)
, _state(OnBegin)
, _ts(new Utf8Codec)
, _reader(_ts)
, _formatter(_writer)
, _service(&service)
, _reply(0)
, _proc(0)
, _args(0)
{
    _writer.useIndent(false);
    _writer.useEndl(false);
}


XmlRpcResponder::~XmlRpcResponder()
{
    if(_proc)
        _service->releaseProcedure(_proc);
}


void XmlRpcResponder::onBeginRequest(Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    _state = OnBegin;
    _ts.attach( request.body() );
    _args = 0;
}


void XmlRpcResponder::onReadRequest(Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
   try
   {
        for(;;)
        {
            _ts.buffer().import();
            std::streamsize m = _ts.buffer().in_avail();
            if( ! m)
                break;

            while( _reader.advance() )
            {
                const Xml::Node& node = _reader.get();
                this->advance(node);
            }
        }
    }
    catch(const Xml::XmlError& error)
    {
        replyError(reply, 1, error.what());
    }
    catch(const SerializationError& error)
    {
        replyError(reply, 2, error.what());
    }
    catch(const ConversionError& error)
    {
        replyError(reply, 3, error.what());
    }
}


void XmlRpcResponder::onBeginReply(Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    try
    {
        _reply = &reply;
        _writer.begin( _reply->body() );
        reply.header().set("Content-Type", "text/xml");

        if( ! _proc )
        {
            throw Fault("invalid XML-RPC", 4);
        }

        if( _args )
        {
            ++_args;
            if( * _args )
            {
                throw Fault("invalid XML-RPC, missing arguments", 5);
            }
        }

        _proc->init(*this, loop);
        _proc->beginCall();
    }
    catch (const Fault& fault)
    {
        replyError(reply, fault.rc(), fault.what());
    }
}


void XmlRpcResponder::onWriteReply(Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
}


void XmlRpcResponder::replyError(Http::Reply& reply, int rc, const char* msg)
{
    reply.clear();
    reply.header().set("Content-Type", "text/xml");
    reply.header().set("Connection", "close");

    _writer.begin( reply.body() );
    _writer.writeStartTag( XMLRPC_METHODRESPONSE );
    _writer.writeStartTag( XMLRPC_FAULT );
    _writer.writeStartTag( XMLRPC_VALUE );
    _writer.writeStartTag( XMLRPC_STRUCT );

    _writer.writeStartTag( XMLRPC_MEMBER );
    _writer.writeElement( XMLRPC_NAME, XMLRPC_FAULTCODE );
    _writer.writeStartTag( XMLRPC_VALUE );
    _writer.writeElement( XMLRPC_INT, Pt::convert<Pt::String>(rc) );
    _writer.writeEndTag(XMLRPC_VALUE); // value
    _writer.writeEndTag(XMLRPC_MEMBER); // member

    _writer.writeStartTag( XMLRPC_MEMBER );
    _writer.writeElement( XMLRPC_NAME, XMLRPC_FAULTSTRING );
    _writer.writeStartTag( XMLRPC_VALUE );

    _writer.writeElement( XMLRPC_STRING, Pt::String::widen(msg));

    _writer.writeEndTag(XMLRPC_VALUE); // value
    _writer.writeEndTag(XMLRPC_MEMBER); // member

    _writer.writeEndTag(XMLRPC_STRUCT); // struct
    _writer.writeEndTag(XMLRPC_VALUE); // value
    _writer.writeEndTag(XMLRPC_FAULT); // fault
    _writer.writeEndTag(XMLRPC_METHODRESPONSE); // methodResponse
    _writer.flush();

    reply.beginSend(true);
}


void XmlRpcResponder::endReply()
{
    try
    {
        IDecomposer* rh = _proc->endCall();
        _writer.writeStartTag( XMLRPC_METHODRESPONSE );
        _writer.writeStartTag( XMLRPC_PARAMS );
        _writer.writeStartTag( XMLRPC_PARAM );
        rh->format(_formatter);
        _writer.writeEndTag(XMLRPC_PARAM); // param
        _writer.writeEndTag(XMLRPC_PARAMS); // params
        _writer.writeEndTag(XMLRPC_METHODRESPONSE); // methodResponse
        _writer.flush();

        assert(_reply);
        if( ! _reply)
            throw std::logic_error("XML-RPC responder without reply");

        _reply->beginSend(true);
    }
    catch (const Fault& fault)
    {
        assert(_reply);
        replyError(*_reply, fault.rc(), fault.what());
    }
    catch(const Xml::XmlError& error)
    {
        assert(_reply);
        replyError( *_reply, 1, error.what());
    }
    catch(const SerializationError& error)
    {
        assert(_reply);
        replyError(*_reply, 2, error.what());
    }
    catch(const ConversionError& error)
    {
        assert(_reply);
        replyError(*_reply, 3, error.what());
    }
}


void XmlRpcResponder::advance(const Pt::Xml::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        { //std::cerr << "OnBegin" << std::endl;
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name() != L"methodCall" )
                    throw Xml::XmlError( "invalid XML-RPC methodCall", _reader.line() );

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

                if(_proc)
                    _service->releaseProcedure(_proc);

                _proc = _service->getProcedure( chars.content().narrow(), &_context );
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
                if( ee.name() != L"methodName" )
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
                if( se.name() != L"params" )
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
                if( ee.name() != L"params" )
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
                    _args = _proc->beginArgs();
                    if( ! *_args)
                        throw std::runtime_error("too many arguments");
                }
                else
                {
                    //std::cerr << "-> next argument" << std::endl;
                    ++_args;
                    if( ! *_args)
                        throw std::runtime_error("too many arguments");
                }

                _scanner.begin(**_args);
                _state = OnParam;
                break;
            }

            break;
        }

        case OnParam:
        { //std::cerr << "S: OnParam" << std::endl;
            bool finished = _scanner.advance(node);
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
                if( ee.name() != L"methodCall" )
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
