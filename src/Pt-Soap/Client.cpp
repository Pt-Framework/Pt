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

#include <Pt/Soap/Client.h>
#include <Pt/Soap/ServiceDeclaration.h>
#include <Pt/Remoting/RemoteProcedure.h>
#include <Pt/Xml/XmlWriter.h>
#include <Pt/Xml/XmlError.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/System/Logger.h>
#include <cassert>

PT_LOG_DEFINE("Pt.Soap.Client")

namespace Pt {

namespace Soap {

static const Pt::Char XMLRPC_XMLDECL[] = { '<', '?', 'x', 'm', 'l', ' ', 
    'v', 'e', 'r', 's', 'i', 'o', 'n', '=', '"', '1', '.', '0' , '"', ' ', 
    'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g', '=', '"', 'U', 'T', 'F', '-', '8', '"', 
    '?', '>' };

static const Pt::Char SOAP_BODY_BEGIN_1_1[]  = { '<', 's', 'o', 'a', 'p', ':', 'E', 'n', 'v', 'e', 'l', 'o', 'p', 'e', ' ',
                                               'x', 'm', 'l', 'n', 's', ':', 's', 'o', 'a', 'p', '=',
                                               '"', 'h', 't', 't', 'p', ':', '/', '/', 's', 'c', 'h', 'e', 'm', 'a', 's', '.', 
                                               'x', 'm', 'l', 's', 'o', 'a', 'p', '.', 'o', 'r', 'g', '/', 's', 'o', 'a', 'p', 
                                               '/', 'e', 'n', 'v', 'e', 'l', 'o', 'p', 'e', '/', '"', '>',
                                              '<', 's', 'o', 'a', 'p', ':', 'B', 'o', 'd', 'y', '>' };

static const Pt::Char SOAP_BODY_BEGIN[]  = { '<', 's', 'o', 'a', 'p', ':', 'E', 'n', 'v', 'e', 'l', 'o', 'p', 'e', ' ',
                                               'x', 'm', 'l', 'n', 's', ':', 's', 'o', 'a', 'p', '=',
                                               '"', 'h', 't', 't', 'p', ':', '/', '/', 
                                               'w', 'w', 'w', '.', 'w', '3', '.', 'o', 'r', 'g', 
                                               '/', '2', '0', '0', '3', '/', '0', '5', '/', 's', 'o', 'a', 'p', 
                                               '-', 'e', 'n', 'v', 'e', 'l', 'o', 'p', 'e', '"', '>',
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


Client::Client(ServiceDeclaration& service)
: _r(0)
, _argv(0)
, _argc(0)
, _arg(0)
, _argn(0)
, _utf8(1)
, _ts( &_utf8 )
, _state(OnBegin)
, _serviceDef(&service)
, _op(0)
, _fmt(_ts)
, _fault("", 0)
, _isFault(false)
{
}


Client::~Client()
{
    _ts.detach();
}


void Client::reset(ServiceDeclaration& service)
{
    cancel();

    _serviceDef = &service;
}


bool Client::isFailed() const
{
    return _isFault;
}


void Client::onBeginCall(Composer& r, Remoting::RemoteCall& method, Decomposer** argv, unsigned argc)
{
    _r = &r;
    _argv = argv;
    _argc = argc;
    _arg = 0;
    _argn = 0;
    
    _state = OnBegin;
    _reader.reset(_bin);

    _isFault = false;

    this->onBeginInvoke();
}


void Client::onEndCall()
{
    // only check for Fault execptions
    if( _isFault )
    {
        _isFault = false;
        throw _fault; 
    } 

    // might also throw e.g. IOError
    this->onEndInvoke();

    if(_state != OnEnvelopeEnd)
    {
        throw Fault("invalid SOAP reply", Fault::InvalidXmlRpc);
    }
}


void Client::onCall(Composer& r, Remoting::RemoteCall& method, Decomposer** argv, unsigned argc)
{
    _r = &r;
    _argv = argv;
    _argc = argc;
    _arg = 0;
    _argn = 0;
    
    _state = OnBegin;
    _reader.reset(_bin);

    _isFault = false;

    this->onInvoke();

    // this is not neccessary if we directly throw from processResult
    if( _isFault )
    {
        _isFault = false;
        throw _fault; 
    }

    if(_state != OnEnvelopeEnd)
    {
        throw Fault("invalid SOAP reply", Fault::InvalidXmlRpc);
    }
}


void Client::onCancel()
{
    _r = 0;
    _argv = 0;
    _argc = 0;
    _arg = 0;
    _argn = 0;
    
    _ts.detach();
    _ts.discard();

    _isFault = false;
}


void Client::beginMessage(std::ostream& os)
{
    const Remoting::RemoteCall* method = activeProcedure();
    if( ! method )
        return;

    const String& name = method->name();

    _ts.clear();
    _ts.discard();
    _ts.attach(os);
    
    _ts.write( XMLRPC_XMLDECL, sizeof(XMLRPC_XMLDECL)/sizeof(Char) );

    //-->
    _ts.write(SOAP_BODY_BEGIN, sizeof(SOAP_BODY_BEGIN)/sizeof(Char));

    Pt::String targetNamespace = _serviceDef->targetNamespace().c_str();
    _op = _serviceDef->getOperation( name );

    if( ! _op )
      throw SerializationError("no such operation");

    _ts << '<' << name << Pt::String(" xmlns=\"") << targetNamespace << '"' << '>';
    //<--
}


bool Client::advanceMessage()
{
    unsigned n = 10;

    while(_argn < _argc && n > 0)
    {
        if( ! _arg )
        {
            _arg = _argv[_argn];

            //-->
            const Parameter* param = _op->getInput( _argn );
            
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
            //nextArg();
            ++_argn;
        }
    }
    
    return _argn >= _argc;
}


void Client::finishMessage()
{
    const Remoting::RemoteCall* method = activeProcedure();

    //-->
    Pt::String targetNamespace = _serviceDef->targetNamespace().c_str();
    const String& name = method->name();
    _ts << '<' << '/' << name << '>';
    _ts.write(SOAP_BODY_END, sizeof(SOAP_BODY_END)/sizeof(Char) ); 
    //<--  

    _ts.flush();
}


void Client::beginResult(std::istream& is)
{
    _bin.reset(is);

    assert(_r);
    _fmt.beginParse(*_r);
}


bool Client::parseResult()
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


void Client::setFault(int rc, const char* msg)
{
    _fault = Fault(msg, rc);
    _isFault = true;
}


void Client::processResult(std::istream& is)
{
    _bin.reset(is);

    assert(_r);
    _fmt.beginParse(*_r);

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
}


bool Client::advance(const Pt::Xml::Node& node)
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
            const Xml::StartElement* se = Xml::toStartElement(&node);
            if(se)
            {
                if( se->name().name() == L"Fault" )
                    _state = OnFault;
                else
                    _state = OnMethod;
            }
            
            break;
        }

        case OnMethod:
        {
            if(node.type() == Xml::Node::EndElement) // end of method tag
            {
                // no return parameter
                const Parameter* param = _op->getOutput();
                if(param)
                    throw Fault("invalid output parameter", Fault::MethodNotFound);

                _state = OnMethodEnd;
                break;
            }

            if(node.type() == Xml::Node::StartElement)
            {
                const Parameter* param = _op->getOutput();
                if( ! param )
                    throw Fault("invalid output parameter", Fault::MethodNotFound);

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
                assert( Xml::toEndElement(node).name().local() == _op->outputName() );
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

        case OnFault:
        {
            const Xml::StartElement* se = Xml::toStartElement(&node);
            if(se)
            {
                if( se->name().name() == L"Code" )
                    _state = OnFaultCode;
                else if( se->name().name() == L"Reason" )
                    _state = OnFaultReason;
            }

            const Xml::EndElement* ee = Xml::toEndElement(&node);
            if(ee)
            {
                if( ee->name().name() == L"Fault" )
                    _state = OnFaultEnd;
            }
            
            break;
        }

        case OnFaultCode:
        {
            const Xml::StartElement* se = Xml::toStartElement(&node);
            if(se)
            {
                if( se->name().name() == L"Value" )
                    _state = OnFaultCodeValue;
            }

            const Xml::EndElement* ee = Xml::toEndElement(&node);
            if(ee)
            {
                if( ee->name().name() == L"Code" )
                    _state = OnFault;
            }
            
            break;
        }

        case OnFaultCodeValue:
        {
            const Xml::EndElement* ee = Xml::toEndElement(&node);
            if(ee)
            {
                if( ee->name().name() == L"Value" )
                    _state = OnFaultCode;
            }

            const Xml::Characters* c = Xml::toCharacters(&node);
            if(c)
            {
                _fault = Fault(_fault.what(), 0);
            }
            
            break;
        }

        case OnFaultReason:
        {
            const Xml::StartElement* se = Xml::toStartElement(&node);
            if(se)
            {
                if( se->name().name() == L"Text" )
                    _state = OnFaultReasonText;
            }

            const Xml::EndElement* ee = Xml::toEndElement(&node);
            if(ee)
            {
                if( ee->name().name() == L"Reason" )
                    _state = OnFault;
            }
            
            break;
        }

        case OnFaultReasonText:
        {
            const Xml::EndElement* ee = Xml::toEndElement(&node);
            if(ee)
            {
                if( ee->name().name() == L"Text" )
                    _state = OnFaultReason;
            }

            const Xml::Characters* c = Xml::toCharacters(&node);
            if(c)
            {
                _fault = Fault( c->content().narrow(), _fault.rc() );
            }
            
            break;
        }

        case OnFaultEnd:
        {
            if(node.type() == Xml::Node::EndElement) // </Body>
            {
                assert( Xml::toEndElement(node).name().local() == L"Body" );
                _state = OnBodyEnd;
                _isFault = true;
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

} // namespace Soap

} // namespace Pt
