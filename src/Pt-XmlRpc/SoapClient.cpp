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

#include "Pt/XmlRpc/SoapClient.h"
#include "Pt/XmlRpc/RemoteProcedure.h"
#include <Pt/XmlRpc/SoapServiceDefinition.h>
#include "Pt/Xml/XmlWriter.h"
#include "Pt/Xml/XmlError.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/System/Logger.h"
#include <cassert>

log_define("Pt.XmlRpc.Client")

namespace Pt {

namespace XmlRpc {

static const Pt::Char XMLRPC_XMLDECL[] = { '<', '?', 'x', 'm', 'l', ' ', 
    'v', 'e', 'r', 's', 'i', 'o', 'n', '=', '"', '1', '.', '0' , '"', ' ', 
    'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g', '=', '"', 'U', 'T', 'F', '-', '8', '"', 
    '?', '>' };

static const Pt::Char SOAP_BODY_BEGIN[]  = { '<', 's', 'o', 'a', 'p', ':', 'E', 'n', 'v', 'e', 'l', 'o', 'p', 'e', ' ',
                                               'x', 'm', 'l', 'n', 's', ':', 's', 'o', 'a', 'p', '=',
                                               '"', 'h', 't', 't', 'p', ':', '/', '/', 's', 'c', 'h', 'e', 'm', 'a', 's', '.', 
                                               'x', 'm', 'l', 's', 'o', 'a', 'p', '.', 'o', 'r', 'g', '/', 's', 'o', 'a', 'p', 
                                               '/', 'e', 'n', 'v', 'e', 'l', 'o', 'p', 'e', '/', '"', '>',
                                              '<', 's', 'o', 'a', 'p', ':', 'B', 'o', 'd', 'y', '>' };

static const Pt::Char SOAP_BODY_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'B', 'o', 'd', 'y', '>',
                                            '<', '/', 's', 'o', 'a', 'p', ':', 'E', 'n', 'v', 'e', 'l', 'o', 'p', 'e', '>' }; 

static const Pt::Char SOAP_FAULT[]  = { '<', 's', 'o', 'a', 'p', ':', 'F', 'a', 'u', 'l', 't',  '>' };
static const Pt::Char SOAP_FAULT_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'F', 'a', 'u', 'l', 't',  '>' };

static const Pt::Char SOAP_CODE[]  = { '<', 's', 'o', 'a', 'p', ':', 'C', 'o', 'd', 'e', '>' };
static const Pt::Char SOAP_CODE_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'C', 'o', 'd', 'e', '>' };

static const Pt::Char SOAP_CODE_RECEIVER[]  = { 's', 'o', 'a', 'p', ':', 'R', 'e', 'c', 'e', 'i', 'v', 'e', 'r' };

static const Pt::Char SOAP_VALUE[]  = { '<', 's', 'o', 'a', 'p', ':', 'V', 'a', 'l', 'u', 'e',  '>' };
static const Pt::Char SOAP_VALUE_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'V', 'a', 'l', 'u', 'e',  '>' };

static const Pt::Char SOAP_REASON[]  = { '<', 's', 'o', 'a', 'p', ':', 'R', 'e', 'a', 's', 'o', 'n', '>' };
static const Pt::Char SOAP_REASON_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'R', 'e', 'a', 's', 'o', 'n', '>' };


SoapClient::SoapClient(SoapServiceDefinition& service)
: _method(0)
, _utf8(1)
, _ts( &_utf8 )
, _argv(0)
, _argc(0)
, _arg(0)
, _argn(0)
, _state(OnBegin)
, _serviceDef(&service)
, _port(0)
, _fmt(_ts)
, _error(false)
, _isFault(false)
{
}


SoapClient::~SoapClient()
{
    _ts.detach();
}


void SoapClient::beginCall(Composer& r, RemoteCall& method, Decomposer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    _reader.reset(_bin);
    _fmt.beginParse(r);

    _argv = argv;
    _argc = argc;
    _arg = 0;
    _argn = 0;

    _error = false;
    _isFault = false;


    this->onInvoke();
}


void SoapClient::endCall()
{
    if( _error )
    {
        _error = false;
        onError();
    }

    if( _isFault )
    {
        _isFault = false;
        throw _fault; 
    }
}


void SoapClient::call(Composer& r, RemoteCall& method, Decomposer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    _reader.reset(_bin);
    _fmt.beginParse(r);

    _argv = argv;
    _argc = argc;
    _arg = 0;
    _argn = 0;

    _error = false;
    _isFault = false;

    this->onCall();
}


void SoapClient::cancel()
{
    _ts.detach();
    _ts.discard();

    _method = 0;
    _argc = 0;
    _argv = 0;
    _arg = 0;
    _argn = 0;

    _error = false;
    _isFault = false;


    this->onCancel();
}


const RemoteCall* SoapClient::activeProcedure() const
{
    return _method;
}


bool SoapClient::isFailed() const
{
    return _error || _isFault;
}


void SoapClient::beginMessage(std::ostream& os)
{
    if( ! _method )
        return;

    const String& name = _method->name();

    _ts.clear();
    _ts.discard();
    _ts.attach(os);
    
    _ts.write( XMLRPC_XMLDECL, sizeof(XMLRPC_XMLDECL)/sizeof(Char) );

    //-->
    _ts.write(SOAP_BODY_BEGIN, sizeof(SOAP_BODY_BEGIN)/sizeof(Char));

    Pt::String targetNamespace = _serviceDef->targetNamespace().c_str();
    _port = _serviceDef->getPort( name.narrow() );

    _ts << '<' << name << Pt::String(" xmlns=\"") << targetNamespace << '"' << '>';
    //<--
}


bool SoapClient::advanceMessage()
{
    unsigned n = 10;

    while(_argn < _argc && n > 0)
    {
        if( ! _arg)
        {
            _arg = _argv[_argn];

            //-->
            const Parameter* param = _port->getInput(_argn);
            
            assert(param);
            if( ! param)
                throw SerializationError("too many parameters"); // check if catched

            _fmt.setParameter(*param);
            //<--
            _arg->beginFormat(_fmt);
        }
        
        while( _arg && n > 0)
        {
            _arg = _arg->advanceFormat(_fmt);
            --n;
        }
        
        if( ! _arg )
        {
            //--> <--
            ++_argn;
        }

    }
    
    return _argn >= _argc;
}


void SoapClient::finishMessage()
{
    //-->
    Pt::String targetNamespace = _serviceDef->targetNamespace().c_str();
    const String& name = _method->name();
    _ts << '<' << '/' << name << '>';
    _ts.write(SOAP_BODY_END, sizeof(SOAP_BODY_END)/sizeof(Char) ); 
    //<--  

    _ts.flush();
}


void SoapClient::beginResult(std::istream& is)
{
    _bin.reset(is);
}


bool SoapClient::parseResult()
{
    try
    {
        for(;;) 
        {
            const Pt::Xml::Node* node = _reader.advance(); // XmlError
            if( ! node )
            {
                break;
            }
            
            bool done = advance(*node); // SerializationError, ConversionError
            if(done)
            {
                return true;
            }
        }
         
        return false;
    }
    catch(const Xml::XmlError& error)
    {
        setFault(Fault::InvalidXmlRpc, error.what());
    }
    catch(const SerializationError& error)
    {
        setFault(Fault::InvalidMethodParameters, error.what());
    }
    catch(const ConversionError& error)
    {
        setFault(Fault::InvalidMethodParameters, error.what());
    }

    return true;
}


void SoapClient::setFault(int rc, const char* msg)
{
    _fault.setRc(rc);
    _fault.setText(msg);
    _isFault = true;
}


void SoapClient::setError(bool f)
{
    _error = f;
}


void SoapClient::finishResult()
{
    if( _method )
    {
        RemoteCall* method = _method;
        _method = 0;
        method->finish();
    }
    else if(_error)
    {
        _error = false;
        onError();
    }
}


void SoapClient::processResult(std::istream& is)
{
    _bin.reset(is);

    try
    {
        while( _reader.get().type() !=  Pt::Xml::Node::EndDocument )
        {
            const Pt::Xml::Node& node = _reader.get();
            
            bool done = advance(node);
            if(done)
            {
                break;
            }
            
            _reader.next();
        }
    }
    catch(const Xml::XmlError& error)
    {
        setFault(Fault::InvalidXmlRpc, error.what());
    }
    catch(const SerializationError& error)
    {
        setFault(Fault::InvalidMethodParameters, error.what());
    }
    catch(const ConversionError& error)
    {
        setFault(Fault::InvalidMethodParameters, error.what());
    }

    // _method contains a return value or fault now
    _method = 0;
    _state = OnBegin;
}


bool SoapClient::advance(const Pt::Xml::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        {
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
                _state = OnMethod;
            }
            break;
        }

        case OnMethod:
        {
            if(node.type() == Xml::Node::EndElement) // end of method tag
            {
                // no return parameter
                _state = OnMethodEnd;
                break;
            }

            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                const Parameter* param = _port->getOutput();
                if( ! param )
                    throw Fault("undefined output parameter", Pt::XmlRpc::Fault::MethodNotFound);

                _fmt.setParameter(*param);
                _state = OnParam;
                break;
            }

            break;
        }

        case OnParam:
        {
            bool finished = _fmt.advance(node);
            if(finished)
            {
                _state = OnParamEnd;
            }

            break;
        }

        case OnParamEnd:
        {
            if(node.type() == Xml::Node::EndElement) // end of method tag
            {
                assert( Xml::toEndElement(node).name().local().narrow() == _port->outputName() );
                _state = OnMethodEnd;
                break;
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
            int n = 0;
            break;
        }
    }

    return _state == OnEnvelopeEnd;
}

} // namespace XmlRpc

} // namespace Pt
