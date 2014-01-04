/*
 * Copyright (C) 2009-2013 by Dr. Marc Boris Duerner
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

#include <Pt/XmlRpc/Responder.h>
#include <Pt/XmlRpc/Fault.h>
#include <Pt/XmlRpc/ServiceDefinition.h>
#include <Pt/Xml/XmlError.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/System/Logger.h>
#include <Pt/Utf8Codec.h>
#include <Pt/Convert.h>
#include <cassert>

log_define("Pt.XmlRpc.Responder")

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


Responder::Responder(ServiceDefinition& service)
: _serviceDef(&service)
, _proc(0)
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


Responder::~Responder()
{
    _ts.detach();

    if(_proc)
        _serviceDef->releaseProcedure(_proc);
}


SerializationContext& Responder::context()
{ 
    return _context; 
}


void Responder::cancel()
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


void Responder::beginMessage(std::istream& is)
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


bool Responder::parseMessage()
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


void Responder::finishMessage(System::EventLoop& loop)
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


void Responder::endCall()
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


void Responder::beginResult(std::ostream& os)
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
    _ts.write(XMLRPC_REPLY_BEGIN, sizeof(XMLRPC_REPLY_BEGIN)/sizeof(Char));

    _result->beginFormat(_formatter);
}


bool Responder::advanceResult()
{
    //while(_result)
    //{
    //    _result = _result->advanceFormat(_formatter);
    //}
    //
    //return true;

    for(unsigned n = 0; _result && n < 10; ++n)
    {
        _result = _result->advanceFormat(_formatter);
    }

    return _result == 0;
}


void Responder::finishResult()
{
    if( ! _isFault )
    {
        _ts.write(XMLRPC_REPLY_END, sizeof(XMLRPC_REPLY_END)/sizeof(Char));
        _ts.flush();
    }
}


void Responder::setFault(int rc, const char* msg)
{
    _fault.setRc(rc);
    _fault.setText(msg);
    _isFault = true;
}


void Responder::formatError(std::ostream& os, int rc, const char* msg)
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


bool Responder::advance(const Pt::Xml::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        { //std::cerr << "OnBegin" << std::endl;
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name().name() != L"methodCall" )
                    throw SerializationError("invalid XML-RPC methodCall");

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

                // TODO: probably not neccessary to release here...
                if(_proc)
                    _serviceDef->releaseProcedure(_proc);

                _proc = _serviceDef->getProcedure( chars.content().narrow(), *this );
                if( ! _proc )
                    throw Fault("no such procedure", Pt::XmlRpc::Fault::MethodNotFound);

                //std::cerr << "-> Found Procedure: " << chars.content().narrow() << std::endl;

                _state = OnMethodName;
            }
            break;
        }

        case OnMethodName:
        { //std::cerr << "OnMethodName" << std::endl;
            if(node.type() == Xml::Node::EndElement)
            {
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                //if( ee.name() != L"methodName" )
                //    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnMethodNameEnd;
            }
            break;
        }

        case OnMethodNameEnd:
        { //std::cerr << "OnMethodNameEnd" << std::endl;
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name().name() != L"params" )
                    throw SerializationError("invalid XML-RPC methodCall");

                _state = OnParams;
            }
            break;
        }

        case OnParams:
        { //std::cerr << "OnParams" << std::endl;
            if(node.type() == Xml::Node::EndElement) // </params>
            {
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                //if( ee.name() != L"params" )
                //    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnParamsEnd;
                break;
            }

            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name().name() != L"param" )
                    throw SerializationError("invalid XML-RPC methodCall");

                //std::cerr << "-> Found param" << std::endl;
                if( ! _args )
                {
                    //std::cerr << "-> begin call" << std::endl;
                    _args = _proc->beginArgs();
                    if( ! *_args)
                        throw SerializationError("too many arguments");
                }
                else
                {
                    //std::cerr << "-> next argument" << std::endl;
                    ++_args;
                    if( ! *_args)
                        throw SerializationError("too many arguments");
                }

                _formatter.beginParse(**_args);
                _state = OnParam;
                break;
            }

            break;
        }

        case OnParam:
        { //std::cerr << "S: OnParam" << std::endl;
            bool finished = _formatter.advance(node);
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
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                //if( ee.name() != L"methodCall" )
                //    throw std::runtime_error("invalid XML-RPC methodCall");

                _state = OnMethodCallEnd;
            }
            
            break;
        }

        case OnMethodCallEnd:
        {
            if(node.type() == Xml::Node::EndDocument)
            {
                _state = OnMethodCallEnd;
            }
            
            break;
        }
    }

    return _state == OnMethodCallEnd;
}

} // namespace XmlRpc

} // namespace Pt
