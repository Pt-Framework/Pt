/* Copyright (C) 2020 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
  MA 02110-1301 USA
*/

#include <Pt/JsonRpc/Responder.h>
#include <Pt/JsonRpc/Fault.h>
#include <Pt/Remoting/ServiceDefinition.h>
//#include <Pt/Xml/XmlError.h>
//#include <Pt/Xml/StartElement.h>
//#include <Pt/Xml/Characters.h>
//#include <Pt/Xml/EndElement.h>
#include <Pt/System/Logger.h>
#include <Pt/Utf8Codec.h>
#include <Pt/Convert.h>
#include <cassert>

PT_LOG_DEFINE("Pt.JsonRpc.Responder")

namespace Pt {

namespace JsonRpc {

static const Pt::Char JSONRPC_MESSAGE_BEGIN[] = 
{ '{', '"', 'j', 's', 'o', 'n', 'r', 'p', 'c', '"', ':', ' ', '"', '2', '.', '0', '"', ',' };

static const Pt::Char JSONRPC_ERROR_BEGIN[] = 
{ '"', 'e', 'r', 'r', 'o', 'r', '"', ':', ' ', '{', ' ' };

static const Pt::Char JSONRPC_ERROR_CODE[] = 
{ '"', 'c', 'o', 'd', 'e', '"', ':', ' ' };

static const Pt::Char JSONRPC_ERROR_MESSAGE[]  = 
{ ',', ' ', '"', 'm', 'e', 's', 's', 'a', 'g', 'e','"', ':', ' ' };

static const Pt::Char JSONRPC_MESSAGE_ID[] = 
{ '"', 'i', 'd', '"', ':', ' ' };

Responder::Responder(Remoting::ServiceDefinition& def)
: Remoting::Responder(def)

, _utf8(1)
, _tis(&_utf8)
, _reader(_tis)
, _args(0)
, _state(OnBegin)
, _id(0)
, _tos(&_utf8)
, _writer(_tos)
, _formatter(_writer)
, _result(0)
, _fault("", 0)
, _isFault(false)
, _r1()
, _r2()
{
}


Responder::~Responder()
{
    _tos.detach();
}


bool Responder::isFailed() const
{
    return  _isFault;
}


void Responder::onCancel()
{
    _state = OnBegin;
    _tos.detach();
    _tos.discard();

    _args = 0;
    _result = 0;
    _isFault = false;
}


void Responder::beginMessage(std::istream& is)
{
    cancel();

    _tis.reset(is);
}


bool Responder::parseMessage()
{
    try
    {
        if( this->isFailed() )
            return true;
        
        for(;;)
        {
            const Json::Node* node = _reader.advance();
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
    catch(const Json::JsonError& error)
    {
        setFault(Fault::ParseError, error.what() );
    }
    catch(const SerializationError& error)
    {
        setFault(Fault::ParseError, error.what() );
    }
    catch(const ConversionError& error)
    {
        setFault(Fault::ParseError, error.what() );
    }
    catch(const Fault& fault)
    {
        setFault(fault.code(), fault.what() );
    }
    catch(const Remoting::Fault& fault)
    {
        setFault( Fault::InternalError, fault.what() );
    } 

    return true;
}


void Responder::finishMessage(System::EventLoop& loop)
{
    if(  this->isFailed()  )
    {
        onFault(_fault);
        return;
    }

    try
    {
        if( _args && *_args )
        {
            throw Fault("expected more arguments", Fault::InvalidParameters);
        }

        beginCall(loop);
    }
    catch(const Fault& fault)
    {
        setFault(fault.code(), fault.what() );
        onFault(_fault);
    }
    catch(const Remoting::Fault& fault)
    {
        setFault( Fault::InternalError, fault.what() );
        onFault(_fault);
    } 
}


void Responder::onReady()
{    
    try
    {       
        _result = endCall(); // throws Fault
        onResult();
    }
    catch(const Fault& fault)
    {
        setFault( fault.code(), fault.what() );
        onFault(_fault);
    } 
    catch(const Remoting::Fault& fault)
    {
        setFault( Fault::InternalError, fault.what() );
        onFault(_fault);
    } 
}


void Responder::beginResult(std::ostream& os)
{
    _tos.clear();
    _tos.discard();
    _tos.attach(os);

    //_ts.write( XMLRPC_XMLDECL, sizeof(XMLRPC_XMLDECL)/sizeof(Char) );

    //assert(_result);
    //_ts.write(XMLRPC_REPLY_BEGIN, sizeof(XMLRPC_REPLY_BEGIN)/sizeof(Char));

    //_result->beginFormat(_formatter);
}


void Responder::beginFault(std::ostream& os, const Fault& fault)
{
    int ec = fault.code();
    const char* msg = fault.what();

    // text stream might still have bytes in text buffer
    _tos.flush();

    _tos.clear();
    _tos.discard();
    _tos.attach(os);

    // {"jsonrpc": "2.0", "error": {"code": -32700, "message": "Parse error"}, "id": 42}

    _tos.write( JSONRPC_MESSAGE_BEGIN, sizeof(JSONRPC_MESSAGE_BEGIN)/sizeof(Char) );
    _tos.write( JSONRPC_ERROR_BEGIN, sizeof(JSONRPC_ERROR_BEGIN)/sizeof(Char) );
    
    _tos.write( JSONRPC_ERROR_CODE, sizeof(JSONRPC_ERROR_CODE)/sizeof(Char) );
    _tos << ec;
    
    _tos.write( JSONRPC_ERROR_MESSAGE, sizeof(JSONRPC_ERROR_MESSAGE)/sizeof(Char) );
    _tos << Char('"');
    for(const char* str = msg; *str != '\0'; ++str)
        _tos << Char(*str);
    _tos << Char('"');

    _tos.write( JSONRPC_MESSAGE_ID, sizeof(JSONRPC_MESSAGE_ID)/sizeof(Char) );
    _tos << ec;
    _tos << Char('}');

    _tos.flush();
}


bool Responder::advanceResult()
{
    //for(unsigned n = 0; _result && n < 10; ++n)
    //{
    //    _result = _result->advanceFormat(_formatter);
    //}

    return _result == 0;
}


void Responder::finishResult()
{
    if( ! this->isFailed() )
    {
        //_ts.write(XMLRPC_REPLY_END, sizeof(XMLRPC_REPLY_END)/sizeof(Char));
        _tos.flush();
    }
}


void Responder::setFault(int rc, const char* msg)
{
    _fault = Fault(msg, rc);
    _isFault = true;
}

// {"jsonrpc": "2.0", "method": "foobar", "id": "1"}
bool Responder::advance(const Pt::Json::Node& node)
{
    throw Fault("no such method", Fault::MethodNotFound);

    //switch(_state)
    //{
    //    case OnBegin:
    //    { //std::cerr << "OnBegin" << std::endl;
    //        if(node.type() == Xml::Node::StartElement)
    //        {
    //            const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
    //            if( se.name().name() != L"methodCall" )
    //                throw SerializationError("invalid XML-RPC methodCall");

    //            _state = OnMethodCallBegin;
    //        }

    //        break;
    //    }

    //    case OnMethodCallBegin:
    //    { //std::cerr << "OnMethodCallBegin" << std::endl;
    //        if(node.type() == Xml::Node::StartElement)
    //        {
    //            _state = OnMethodNameBegin;
    //        }
    //        break;
    //    }

    //    case OnMethodNameBegin:
    //    { //std::cerr << "OnMethodNameBegin" << std::endl;
    //        if(node.type() == Xml::Node::Characters)
    //        {
    //            const Xml::Characters& chars = static_cast<const Xml::Characters&>(node);

    //            _args = setProcedure( chars.content().narrow() );
    //            if( ! _args )
    //                throw Fault("no such procedure", Pt::XmlRpc::Fault::MethodNotFound);

    //            //std::cerr << "-> Found Procedure: " << chars.content().narrow() << std::endl;

    //            _state = OnMethodName;
    //        }
    //        break;
    //    }

    //    case OnMethodName:
    //    { //std::cerr << "OnMethodName" << std::endl;
    //        if(node.type() == Xml::Node::EndElement)
    //        {
    //            //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
    //            //if( ee.name() != L"methodName" )
    //            //    throw std::runtime_error("invalid XML-RPC methodCall");

    //            _state = OnMethodNameEnd;
    //        }
    //        break;
    //    }

    //    case OnMethodNameEnd:
    //    { //std::cerr << "OnMethodNameEnd" << std::endl;
    //        if(node.type() == Xml::Node::StartElement)
    //        {
    //            const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
    //            if( se.name().name() != L"params" )
    //                throw SerializationError("invalid XML-RPC methodCall");

    //            _state = OnParams;
    //        }
    //        break;
    //    }

    //    case OnParams:
    //    { //std::cerr << "OnParams" << std::endl;
    //        if(node.type() == Xml::Node::EndElement) // </params>
    //        {
    //            //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
    //            //if( ee.name() != L"params" )
    //            //    throw std::runtime_error("invalid XML-RPC methodCall");

    //            _state = OnParamsEnd;
    //            break;
    //        }

    //        if(node.type() == Xml::Node::StartElement)
    //        {
    //            const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
    //            
    //            if( se.name().name() != L"param" )
    //                throw SerializationError("invalid XML-RPC methodCall");

    //            if( ! *_args )
    //                throw SerializationError("too many arguments");

    //            _formatter.beginParse(**_args);
    //            _state = OnParam;
    //            break;
    //        }

    //        break;
    //    }

    //    case OnParam:
    //    { //std::cerr << "S: OnParam" << std::endl;
    //        bool finished = _formatter.advance(node);
    //        if(finished)
    //        {
    //            ++_args;
    //            //std::cerr << "-> param finished" << std::endl; // node is </param>
    //            _state = OnParams;
    //        }

    //        break;
    //    }

    //    case OnParamsEnd:
    //    { //std::cerr << "OnParamsEnd" << std::endl;
    //        if(node.type() == Xml::Node::EndElement) // </methodCall>
    //        {
    //            //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
    //            //if( ee.name() != L"methodCall" )
    //            //    throw std::runtime_error("invalid XML-RPC methodCall");

    //            _state = OnMethodCallEnd;
    //        }
    //        
    //        break;
    //    }

    //    case OnMethodCallEnd:
    //    {
    //        if(node.type() == Xml::Node::EndDocument)
    //        {
    //            _state = OnMethodCallEnd;
    //        }
    //        
    //        break;
    //    }
    //}

    return _state == OnMethodCallEnd;
}

} // namespace XmlRpc

} // namespace Pt
