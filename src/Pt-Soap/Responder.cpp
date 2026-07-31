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

#include <Pt/Soap/Responder.h>
#include <Pt/Soap/Fault.h>
#include <Pt/Xml/XmlError.h>
#include <Pt/Xml/StartElement.h>
#include <Pt/Xml/Characters.h>
#include <Pt/Xml/EndElement.h>
#include <Pt/System/Logger.h>
#include <cassert>

namespace {

static const Pt::Char XMLRPC_XMLDECL[] = { '<', '?', 'x', 'm', 'l', ' ',
    'v', 'e', 'r', 's', 'i', 'o', 'n', '=', '"', '1', '.', '0' , '"', ' ',
    'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g', '=', '"', 'U', 'T', 'F', '-', '8', '"',
    '?', '>' };

static const Pt::Char SOAP_REPLY_BEGIN_1_1[]  = { '<', 's', 'o', 'a', 'p', ':', 'E', 'n', 'v', 'e', 'l', 'o', 'p', 'e', ' ',
                                               'x', 'm', 'l', 'n', 's', ':', 's', 'o', 'a', 'p', '=',
                                               '"', 'h', 't', 't', 'p', ':', '/', '/', 's', 'c', 'h', 'e', 'm', 'a', 's', '.',
                                               'x', 'm', 'l', 's', 'o', 'a', 'p', '.', 'o', 'r', 'g', '/', 's', 'o', 'a', 'p',
                                               '/', 'e', 'n', 'v', 'e', 'l', 'o', 'p', 'e', '/', '"', '>',
                                              '<', 's', 'o', 'a', 'p', ':', 'B', 'o', 'd', 'y', '>' };

static const Pt::Char SOAP_REPLY_BEGIN[]  = { '<', 's', 'o', 'a', 'p', ':', 'E', 'n', 'v', 'e', 'l', 'o', 'p', 'e', ' ',
                                               'x', 'm', 'l', 'n', 's', ':', 's', 'o', 'a', 'p', '=',
                                               '"', 'h', 't', 't', 'p', ':', '/', '/',
                                               'w', 'w', 'w', '.', 'w', '3', '.', 'o', 'r', 'g',
                                               '/', '2', '0', '0', '3', '/', '0', '5', '/', 's', 'o', 'a', 'p',
                                               '-', 'e', 'n', 'v', 'e', 'l', 'o', 'p', 'e', '"', '>',
                                              '<', 's', 'o', 'a', 'p', ':', 'B', 'o', 'd', 'y', '>' };

static const Pt::Char SOAP_REPLY_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'B', 'o', 'd', 'y', '>',
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

static const Pt::Char SOAP_TEXT[]  = { '<', 's', 'o', 'a', 'p', ':', 'T', 'e', 'x', 't', ' ', 'x', 'm', 'l', ':', 'l', 'a', 'n', 'g', '=', '"', 'e', 'n', '"', '>' };
static const Pt::Char SOAP_TEXT_END[]  = { '<', '/', 's', 'o', 'a', 'p', ':', 'T', 'e', 'x', 't', '>' };

} // namespace

namespace Pt {

namespace Soap {

Responder::Responder(const ServiceDeclaration& decl,
                     Remoting::ServiceDefinition& def)
: Remoting::Responder(def)
, _serviceDecl( &decl )
, _op(0)
, _reader(_bin)
, _args(0)
, _state(OnBegin)
, _utf8(1)
, _ts(&_utf8)
, _result(0)
, _formatter(_ts)
, _fault("", 0)
, _isFault(false)
{
}


Responder::~Responder()
{
    _ts.detach();
}


bool Responder::isFailed() const
{
    return  _isFault;
}


void Responder::setFault(int rc, const char* msg)
{
    _fault = Fault(msg, rc);
    _isFault = true;
}


void Responder::onCancel()
{
    _state = OnBegin;
    _ts.detach();
    _ts.discard();

    _args = 0;
    _result = 0;
    _isFault = false;
}


void Responder::beginMessage(std::istream& is)
{
    cancel();

    _bin.reset(is);
}


bool Responder::parseMessage()
{
    try
    {
        if( this->isFailed() )
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
    catch(const Fault& fault)
    {
        setFault(fault.rc(), fault.what() );
    }
    catch(const Xml::XmlError& error)
    {
        setFault(1, error.what() );
    }
    catch(const SerializationError& error)
    {
        setFault(2, error.what() );
    }
    catch(const ConversionError& error)
    {
        setFault(3, error.what() );
    }
    catch(const Remoting::Fault& fault)
    {
        setFault( Fault::InternalXmlRpcError, fault.what() );
    }

    return true;
}


void Responder::finishMessage(System::EventLoop& loop)
{
    if( this->isFailed() )
    {
        onFault(_fault);
        return;
    }

    try
    {
        if( _args && *_args )
        {
            throw Fault("expected more arguments", 5);
        }

        beginCall(loop);
    }
    catch(const Fault& fault)
    {
        setFault(fault.rc(), fault.what() );
        onFault(_fault);
    }
    catch(const Remoting::Fault& fault)
    {
        setFault( Fault::InternalXmlRpcError, fault.what() );
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
        setFault( fault.rc(), fault.what() );
        onFault(_fault);
    }
    catch(const Remoting::Fault& fault)
    {
        setFault( Fault::InternalXmlRpcError, fault.what() );
        onFault(_fault);
    }
}


void Responder::beginResult(std::ostream& os)
{
    _ts.clear();
    _ts.discard();
    _ts.attach(os);

    _ts.write( XMLRPC_XMLDECL, sizeof(XMLRPC_XMLDECL)/sizeof(Char) );

    assert(_result);
    _ts.write(SOAP_REPLY_BEGIN, sizeof(SOAP_REPLY_BEGIN)/sizeof(Char));

    const Pt::String& outName = _op->outputName();
    Pt::String targetNamespace = _serviceDecl->targetNamespace().c_str();
    _ts << '<' << outName << Pt::String(" xmlns=\"") << targetNamespace << '"' << '>';

    const Parameter* param = _op->getOutput();
    assert(param);
    if( ! param)
        throw Remoting::Fault("invalid output parameter");

    _formatter.setParameter(*param);
    _result->beginFormat(_formatter);
}


void Responder::beginFault(std::ostream& os, const Fault& fault)
{
    //int rc = fault.rc();
    const char* msg = fault.what();

    // text stream might still have bytes in text buffer
    _ts.flush();

    _ts.clear();
    _ts.discard();
    _ts.attach(os);

    _ts.write( XMLRPC_XMLDECL, sizeof(XMLRPC_XMLDECL)/sizeof(Char) );
    _ts.write( SOAP_REPLY_BEGIN, sizeof(SOAP_REPLY_BEGIN)/sizeof(Char) );

    _ts.write( SOAP_FAULT, sizeof(SOAP_FAULT)/sizeof(Char) );
    _ts.write( SOAP_CODE, sizeof(SOAP_CODE)/sizeof(Char) );
    _ts.write( SOAP_VALUE, sizeof(SOAP_VALUE)/sizeof(Char) );

    // TODO: insert fault code
    _ts.write( SOAP_CODE_RECEIVER, sizeof(SOAP_CODE_RECEIVER)/sizeof(Char) );

    _ts.write( SOAP_VALUE_END, sizeof(SOAP_VALUE_END)/sizeof(Char) );
    _ts.write( SOAP_CODE_END, sizeof(SOAP_CODE_END)/sizeof(Char) );

    _ts.write( SOAP_REASON, sizeof(SOAP_REASON)/sizeof(Char) );
    _ts.write( SOAP_TEXT, sizeof(SOAP_TEXT)/sizeof(Char) );

     for(const char* str = msg; *str != '\0'; ++str)
        _ts << Char(*str);

    _ts.write( SOAP_TEXT_END, sizeof(SOAP_TEXT_END)/sizeof(Char) );
    _ts.write( SOAP_REASON_END, sizeof(SOAP_REASON_END)/sizeof(Char) );
    _ts.write( SOAP_FAULT_END, sizeof(SOAP_FAULT_END)/sizeof(Char) );
    _ts.write( SOAP_REPLY_END, sizeof(SOAP_REPLY_END)/sizeof(Char) );

    _ts.flush();
}


bool Responder::advanceResult()
{
    for(unsigned n = 0; _result && n < 10; ++n)
    {
        _result = _result->advanceFormat(_formatter);
    }

    return _result == 0;
}


void Responder::finishResult()
{
    if( ! this->isFailed() )
    {
        const Pt::String& outName = _op->outputName();
        _ts << '<' << '/' << outName << '>';
        _ts.write(SOAP_REPLY_END, sizeof(SOAP_REPLY_END)/sizeof(Char));
        _ts.flush();
    }
}


bool Responder::advance(const Pt::Xml::Node& node)
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
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                _op = _serviceDecl->getOperation( se.name().local() );
                if( ! _op )
                    throw Fault("no such procedure", Fault::MethodNotFound);

                _args = setProcedure( se.name().local().narrow() );
                if( ! _args )
                    throw Fault("no such procedure", Fault::MethodNotFound);

                _state = OnMethod;
            }
            break;
        }

        case OnMethod:
        {
            if(node.type() == Xml::Node::EndElement) // end of method tag
            {
                if( *_args )
                    throw SerializationError("too few arguments");

                _state = OnMethodEnd;
                break;
            }

            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);

                if( ! *_args )
                    throw SerializationError("too many arguments");

                const Parameter* param = _op->getInput( se.name().local().narrow() );
                if( ! param )
                    throw Fault("no such parameter", Fault::MethodNotFound);

                _formatter.setParameter(*param);
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
                ++_args;
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

} // namespace Soap

} // namespace Pt
