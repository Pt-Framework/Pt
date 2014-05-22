/*
 * Copyright (C) 2014 by Dr. Marc Boris Duerner
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

#include <Pt/XmlRpc/SoapHttpService.h>
#include <Pt/XmlRpc/Fault.h>
#include <Pt/XmlRpc/ServiceDefinition.h>
#include <Pt/Xml/XmlError.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/Http/Request.h>
#include <Pt/Http/Reply.h>
#include <Pt/System/Logger.h>
#include <Pt/Utf8Codec.h>
#include <Pt/Convert.h>
#include <cassert>

namespace Pt {

namespace XmlRpc {

static const Pt::Char XMLRPC_XMLDECL[] = { '<', '?', 'x', 'm', 'l', ' ', 
    'v', 'e', 'r', 's', 'i', 'o', 'n', '=', '"', '1', '.', '0' , '"', ' ', 
    'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g', '=', '"', 'U', 'T', 'F', '-', '8', '"', 
    '?', '>' };

static const Pt::Char XMLRPC_REPLY_BEGIN[]  = { '<', 'm', 'e', 't', 'h', 'o', 'd', 'R', 'e', 's', 'p', 'o', 'n', 's', 'e', '>',
                                                '<', 'p', 'a', 'r', 'a', 'm', 's', '>',
                                                '<', 'p', 'a', 'r', 'a', 'm', '>' };

static const Pt::Char XMLRPC_REPLY_END[]  = { '<', '/', 'p', 'a', 'r', 'a', 'm', '>',
                                              '<', '/', 'p', 'a', 'r', 'a', 'm', 's', '>',
                                              '<', '/', 'm', 'e', 't', 'h', 'o', 'd', 'R', 'e', 's', 'p', 'o', 'n', 's', 'e', '>', };

static const Pt::Char XMLRPC_METHODRESPONSE[]  = { '<', 'm', 'e', 't', 'h', 'o', 'd', 'R', 'e', 's', 'p', 'o', 'n', 's', 'e', '>' };
static const Pt::Char XMLRPC_METHODCALL[]  = { '<', 'm', 'e', 't', 'h', 'o', 'd', 'C', 'a', 'l', 'l', '>' };
static const Pt::Char XMLRPC_PARAMS[]  = { '<', 'p', 'a', 'r', 'a', 'm', 's', '>' };
static const Pt::Char XMLRPC_PARAM[]  = { '<', 'p', 'a', 'r', 'a', 'm', '>' };
static const Pt::Char XMLRPC_FAULT[]  = { '<', 'f', 'a', 'u', 'l', 't', '>' };
static const Pt::Char XMLRPC_FAULTCODE[]  = { 'f', 'a', 'u', 'l', 't', 'C', 'o', 'd', 'e' };
static const Pt::Char XMLRPC_FAULTSTRING[]  = { 'f', 'a', 'u', 'l', 't', 'S', 't', 'r', 'i', 'n', 'g' };
static const Pt::Char XMLRPC_STRUCT[]  = { '<', 's', 't', 'r', 'u', 'c', 't', '>' };
static const Pt::Char XMLRPC_MEMBER[]  = { '<', 'm', 'e', 'm', 'b', 'e', 'r', '>' };
static const Pt::Char XMLRPC_NAME[]    = { '<', 'n', 'a', 'm', 'e', '>' };
static const Pt::Char XMLRPC_VALUE[]   = { '<', 'v', 'a', 'l', 'u', 'e', '>' };
static const Pt::Char XMLRPC_INT[]     = { '<', 'i', 'n', 't', '>' };
static const Pt::Char XMLRPC_STRING[]  = { '<', 's', 't', 'r', 'i', 'n', 'g', '>' };

static const Pt::Char XMLRPC_METHODRESPONSE_END[]  = { '<', '/', 'm', 'e', 't', 'h', 'o', 'd', 'R', 'e', 's', 'p', 'o', 'n', 's', 'e', '>' };
static const Pt::Char XMLRPC_METHODCALL_END[]  = { '<', '/', 'm', 'e', 't', 'h', 'o', 'd', 'C', 'a', 'l', 'l', '>' };
static const Pt::Char XMLRPC_PARAMS_END[]  = { '<', '/', 'p', 'a', 'r', 'a', 'm', 's', '>' };
static const Pt::Char XMLRPC_PARAM_END[]  = { '<', '/', 'p', 'a', 'r', 'a', 'm', '>' };
static const Pt::Char XMLRPC_FAULT_END[]  = { '<', '/', 'f', 'a', 'u', 'l', 't', '>' };
static const Pt::Char XMLRPC_STRUCT_END[]  = { '<', '/', 's', 't', 'r', 'u', 'c', 't', '>' };
static const Pt::Char XMLRPC_MEMBER_END[]  = { '<', '/', 'm', 'e', 'm', 'b', 'e', 'r', '>' };
static const Pt::Char XMLRPC_NAME_END[]    = { '<', '/', 'n', 'a', 'm', 'e', '>' };
static const Pt::Char XMLRPC_VALUE_END[]   = { '<', '/', 'v', 'a', 'l', 'u', 'e', '>' };
static const Pt::Char XMLRPC_INT_END[]     = { '<', '/', 'i', 'n', 't', '>' };
static const Pt::Char XMLRPC_STRING_END[]  = { '<', '/', 's', 't', 'r', 'i', 'n', 'g', '>' };


static const Pt::Char SOAP_REPLY_BEGIN[]  = { '<', 's', 'o', 'a', 'p', ':', 'E', 'n', 'v', 'e', 'l', 'o', 'p', 'e', ' ',
                                               'x', 'm', 'l', 'n', 's', ':', 's', 'o', 'a', 'p', '=',
                                               '"', 'h', 't', 't', 'p', ':', '/', '/', 's', 'c', 'h', 'e', 'm', 'a', 's', '.', 
                                               'x', 'm', 'l', 's', 'o', 'a', 'p', '.', 'o', 'r', 'g', '/', 's', 'o', 'a', 'p', 
                                               '/', 'e', 'n', 'v', 'e', 'l', 'o', 'p', 'e', '"', '>',
                                              '<', 's', 'o', 'a', 'p', ':', 'B', 'o', 'd', 'y', '>' };

static const Pt::Char SOAP_REPLY_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'B', 'o', 'd', 'y', '>',
                                            '<', '/', 's', 'o', 'a', 'p', ':', 'E', 'n', 'v', 'e', 'l', 'o', 'p', 'e', '>' }; 

SoapResponder::SoapResponder(SoapServiceDefinition& service)
: _serviceDef(&service)
, _proc(0)
, _procDef(0)
, _reader(_bin)
, _args(0)
, _state(OnBegin)
, _utf8(1)
, _ts(&_utf8)
, _result(0)
, _formatter(_ts)
, _isFault(false)
{
}


SoapResponder::~SoapResponder()
{
    _ts.detach();

    if(_proc)
        _serviceDef->releaseProcedure(_proc);
}


SerializationContext& SoapResponder::context()
{ 
    return _context; 
}


void SoapResponder::cancel()
{
    this->onCancel();

    _ts.detach();
    _ts.discard();

    if(_proc)
        _serviceDef->releaseProcedure(_proc);

    _state = OnBegin;

    _proc = 0;
    _args = 0;
    _result = 0;
    _isFault = false;
}


void SoapResponder::beginMessage(std::istream& is)
{
    _state = OnBegin;
    _bin.reset(is);

    if(_proc)
        _serviceDef->releaseProcedure(_proc);
    
    _proc = 0;
    _args = 0;
    _result = 0;
    _isFault = false;
}


bool SoapResponder::parseMessage()
{
    try
    {
        if(_isFault)
            return true;
        
        for(;;)
        {
            const Xml::Node* node = _reader.advance();
            if( ! node )
            {
                break;
            }
            
            bool done = this->advance(*node);
            if(done)
            {
                return true;
            }
        }

        return false;
    }
    catch(const Xml::XmlError& error)
    {
        _fault = Fault(error.what(), 1);
        _isFault = true;
    }
    catch(const SerializationError& error)
    {
        _fault = Fault(error.what(), 2);
        _isFault = true;
    }
    catch(const ConversionError& error)
    {
        _fault = Fault(error.what(), 3);
        _isFault = true;
    }
    catch(const Fault& fault)
    {
        _fault = fault;
        _isFault = true;
    }

    return true;
}


void SoapResponder::finishMessage(System::EventLoop& loop)
{
    if( _isFault )
    {
        onError();
        // onResult();
        return;
    }

    try
    {
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

        _proc->beginCall(loop); // throws Fault
    }
    catch(const Fault& fault)
    {
        _fault = fault;
        _isFault = true;

        onError();
        // onResult();
    }
}


void SoapResponder::endCall()
{ 
    try
    {
        if( ! _isFault )
        {
            assert(_proc);
            _result = _proc->endCall(); // throws Fault
        }
    }
    catch(const Fault& fault)
    {
        _fault = fault;
        _isFault = true;
        onError();
        // onResult();
        return;
    }

    this->onResult(); 
}


void SoapResponder::beginResult(std::ostream& os)
{
    if( _isFault )
    {
        _result = 0;
        formatError(os, _fault.rc(), _fault.what());
        return;
    }

    _ts.clear();
    _ts.discard();
    _ts.attach(os);
    //_ts.set(os);
    // _ts.attach(os);

    _ts.write( XMLRPC_XMLDECL, sizeof(XMLRPC_XMLDECL)/sizeof(Char) );

    assert(_result);
    _ts.write(SOAP_REPLY_BEGIN, sizeof(SOAP_REPLY_BEGIN)/sizeof(Char));

    const Parameter* param = _procDef->getOutput();
    assert(param);

    _formatter.setParameter(*param);
    _result->beginFormat(_formatter);
}


bool SoapResponder::advanceResult()
{
    for(unsigned n = 0; _result && n < 10; ++n)
    {
        _result = _result->advanceFormat(_formatter);
    }

    return _result == 0;
}


void SoapResponder::finishResult()
{
    if( ! _isFault )
    {
        _ts.write(SOAP_REPLY_END, sizeof(SOAP_REPLY_END)/sizeof(Char));
        _ts.flush();
    }
}


void SoapResponder::setFault(int rc, const char* msg)
{
    _fault.setRc(rc);
    _fault.setText(msg);
    _isFault = true;
}


void SoapResponder::formatError(std::ostream& os, int rc, const char* msg)
{
    // text stream might still have bytes in text buffer
    _ts.flush();

    _ts.clear();
    _ts.discard();
    _ts.attach(os);
    //_ts.set(os);
    // _ts.attach(os);
    
    _ts.write( XMLRPC_XMLDECL, sizeof(XMLRPC_XMLDECL)/sizeof(Char) );

    _ts.write( XMLRPC_METHODRESPONSE, sizeof(XMLRPC_METHODRESPONSE)/sizeof(Char) );
    _ts.write( XMLRPC_FAULT, sizeof(XMLRPC_FAULT)/sizeof(Char) );
    _ts.write( XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char) );
    _ts.write( XMLRPC_STRUCT, sizeof(XMLRPC_STRUCT)/sizeof(Char) );
    
    _ts.write( XMLRPC_MEMBER, sizeof(XMLRPC_MEMBER)/sizeof(Char) );
    _ts.write(XMLRPC_NAME, sizeof(XMLRPC_NAME)/sizeof(Char));
    _ts.write(XMLRPC_FAULTCODE, sizeof(XMLRPC_FAULTCODE)/sizeof(Char));
    _ts.write(XMLRPC_NAME_END, sizeof(XMLRPC_NAME_END)/sizeof(Char));
    _ts.write( XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char) );
    _ts.write( XMLRPC_INT, sizeof(XMLRPC_INT)/sizeof(Char) );
    _ts << rc;
    _ts.write(XMLRPC_INT_END, sizeof(XMLRPC_INT_END)/sizeof(Char));
    _ts.write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
    _ts.write(XMLRPC_MEMBER_END, sizeof(XMLRPC_MEMBER_END)/sizeof(Char));

    _ts.write( XMLRPC_MEMBER, sizeof(XMLRPC_MEMBER)/sizeof(Char) );
    _ts.write(XMLRPC_NAME, sizeof(XMLRPC_NAME)/sizeof(Char));
    _ts.write(XMLRPC_FAULTSTRING, sizeof(XMLRPC_FAULTSTRING)/sizeof(Char) );
    _ts.write(XMLRPC_NAME_END, sizeof(XMLRPC_NAME_END)/sizeof(Char) );
    _ts.write( XMLRPC_VALUE, sizeof(XMLRPC_VALUE)/sizeof(Char) );
    _ts.write( XMLRPC_STRING, sizeof(XMLRPC_STRING)/sizeof(Char) );

    for(const char* str = msg; *str != '\0'; ++str)
        _ts << Char(*str);

    _ts.write(XMLRPC_STRING_END, sizeof(XMLRPC_STRING_END)/sizeof(Char));
    _ts.write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
    _ts.write(XMLRPC_MEMBER_END, sizeof(XMLRPC_MEMBER_END)/sizeof(Char));

    _ts.write(XMLRPC_STRUCT_END, sizeof(XMLRPC_STRUCT_END)/sizeof(Char));
    _ts.write(XMLRPC_VALUE_END, sizeof(XMLRPC_VALUE_END)/sizeof(Char));
    _ts.write(XMLRPC_FAULT_END, sizeof(XMLRPC_FAULT_END)/sizeof(Char));
    _ts.write(XMLRPC_METHODRESPONSE_END, sizeof(XMLRPC_METHODRESPONSE_END)/sizeof(Char));
    _ts.flush();
}


bool SoapResponder::advance(const Pt::Xml::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        { //std::cerr << "OnBegin" << std::endl;
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name().name() != L"Envelope" )
                    throw SerializationError("invalid SOAP Envelope");

                _state = OnEnvelope;
            }

            break;
        }

        case OnEnvelope:
        {
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name().name() == L"Body" )
                    _state = OnBody;
            }
            
            break;
        }

        case OnBody:
        {
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                // TODO: probably not neccessary to release here...
                if(_proc)
                    _serviceDef->releaseProcedure(_proc);

                _procDef = _serviceDef->getPort( se.name().local().narrow() );
                if( ! _procDef )
                    throw Fault("no such procedure", Pt::XmlRpc::Fault::MethodNotFound);

                _proc = _serviceDef->getProcedure( se.name().local().narrow(), *this );
                if( ! _proc )
                    throw Fault("no such procedure", Pt::XmlRpc::Fault::MethodNotFound);

                std::clog << "-> Found Procedure: " << se.name().local().narrow() << std::endl;

                _state = OnMethod;
            }
            break;
        }

        case OnMethod:
        {
            if(node.type() == Xml::Node::EndElement) // end of method tag
            {
                _state = OnMethodEnd;
                break;
            }

            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                if( ! _args )
                {
                    _args = _proc->beginArgs();
                    if( ! *_args)
                        throw SerializationError("too many arguments");
                }
                else
                {
                    ++_args;
                    if( ! *_args)
                        throw SerializationError("too many arguments");
                }

                const Type* paramType = _procDef->getInput( se.name().local().narrow() );
                if( ! paramType )
                    throw Fault("no such parameter", Pt::XmlRpc::Fault::MethodNotFound);

                _formatter.setParameter(*paramType);
                _formatter.beginParse(**_args);
                
                _state = OnParam;
                break;
            }

            break;
        }

        case OnParam:
        {
            bool finished = _formatter.advance(node);
            if(finished)
            {
                _state = OnMethod;
            }

            break;
        }

        case OnMethodEnd:
        {
            if(node.type() == Xml::Node::EndElement) // </Body>
            {
                assert( Xml::toEndElement(node).name().local() == L"Body" );
                _state = OnBodyEnd;
            }
            
            break;
        }

        case OnBodyEnd:
        {
            if(node.type() == Xml::Node::EndElement) // </Envelope>
            {
                assert( Xml::toEndElement(node).name().local() == L"Envelope" );
                _state = OnEnvelopeEnd;
            }
            
            break;
        }

        case OnEnvelopeEnd:
        {
            break;
        }
    }

    return _state == OnEnvelopeEnd;
}



SoapHttpResponder::SoapHttpResponder(SoapHttpService& httpService, SoapServiceDefinition& rpcService)
: Http::Responder(httpService)
, XmlRpc::SoapResponder(rpcService)
, _reply(0)
{
}


SoapHttpResponder::~SoapHttpResponder()
{
}


// pass only ReplyHeader and body stream
void SoapHttpResponder::onBeginRequest(Http::Request& request, Pt::Http::Reply& reply, System::EventLoop& loop)
{
    _reply = 0;
    
    beginMessage( request.body() );
}


// pass only ReplyHeader and body stream
void SoapHttpResponder::onReadRequest(Http::Request& request, Pt::Http::Reply& reply, System::EventLoop& loop)
{
    parseMessage();
}


void SoapHttpResponder::onBeginReply(const Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    _reply = &reply;
    finishMessage(loop);
}


void SoapHttpResponder::onWriteReply(const Http::Request& request, Http::Reply& reply, System::EventLoop& loop)
{
    while( ! advanceResult() )
    {
        if(reply.buffer().size() > 8192)
        {
            reply.beginSend(false);
            return;
        }
    }

    finishResult();
    reply.beginSend(true);
}


void SoapHttpResponder::onResult()
{
    assert(_reply);

    if( _reply )
    {
        _reply->header().set("Content-Type", "text/xml");

        beginResult(_reply->body() );

        while( ! advanceResult() )
        {
            if(_reply->buffer().size() > 8192)
            {
                _reply->beginSend(false);
                return;
            }
        }

        finishResult();
        _reply->beginSend(true);
    }
}


void SoapHttpResponder::onCancel()
{
    // not really possible, since only the HTTP server uses this class
}


void SoapHttpResponder::onError()
{
    if(_reply)
    {
        _reply->header().set("Connection", "close");
    }

    onResult();
}




SoapHttpService::SoapHttpService(SoapServiceDefinition& rpcService)
: _rpcService(&rpcService)
{ 
}


SoapHttpService::~SoapHttpService()
{
}


Http::Responder* SoapHttpService::onGetResponder(const Http::Request& req)
{
    //if (req.isHeaderValue("Content-Type", "text/xml"))
    //    return new XmlRpcResponder(*this);

    //if (req.isHeaderValue("Content-Type", "text/xml; charset=UTF-8")) //! ### Temporary fix ###
    //    return new XmlRpcResponder(*this);

    return new SoapHttpResponder(*this, *_rpcService);
}


void SoapHttpService::onReleaseResponder(Http::Responder* resp)
{
    delete resp;
}

} // namespace XmlRpc

} // namespace Pt
