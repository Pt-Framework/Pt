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

#include "Pt/XmlRpc/Client.h"
#include "Pt/XmlRpc/RemoteProcedure.h"
#include "Pt/Xml/XmlWriter.h"
#include "Pt/Xml/XmlError.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/System/Logger.h"

log_define("Pt.XmlRpc.Client")

namespace Pt {

namespace XmlRpc {

static const Pt::Char XMLRPC_XMLDECL[] = { '<', '?', 'x', 'm', 'l', ' ', 
    'v', 'e', 'r', 's', 'i', 'o', 'n', '=', '"', '1', '.', '0' , '"', ' ', 
    'e', 'n', 'c', 'o', 'd', 'i', 'n', 'g', '=', '"', 'U', 'T', 'F', '-', '8', '"', 
    '?', '>' };

//static const Pt::Char XMLRPC_XMLVERSION[]  = { '1', '.', '0', '\0' };
//static const Pt::Char XMLRPC_XMLENCODING[]  = { 'U', 'T', 'F', '-', '8',  '\0' };
static const Pt::Char XMLRPC_METHODRESPONSE[]  = { '<', 'm', 'e', 't', 'h', 'o', 'd', 'R', 'e', 's', 'p', 'o', 'n', 's', 'e', '>' };
static const Pt::Char XMLRPC_METHODCALL[]  = { '<', 'm', 'e', 't', 'h', 'o', 'd', 'C', 'a', 'l', 'l', '>' };
static const Pt::Char XMLRPC_METHODNAME[]  = { '<', 'm', 'e', 't', 'h', 'o', 'd', 'N', 'a', 'm', 'e', '>' };
static const Pt::Char XMLRPC_PARAMS[]  = { '<', 'p', 'a', 'r', 'a', 'm', 's', '>' };
static const Pt::Char XMLRPC_PARAM[]  = { '<', 'p', 'a', 'r', 'a', 'm', '>' };
static const Pt::Char XMLRPC_FAULT[]  = { '<', 'f', 'a', 'u', 'l', 't', '>' };

static const Pt::Char XMLRPC_METHODRESPONSE_END[]  = { '<', '/', 'm', 'e', 't', 'h', 'o', 'd', 'R', 'e', 's', 'p', 'o', 'n', 's', 'e', '>' };
static const Pt::Char XMLRPC_METHODCALL_END[]  = { '<', '/', 'm', 'e', 't', 'h', 'o', 'd', 'C', 'a', 'l', 'l', '>' };
static const Pt::Char XMLRPC_METHODNAME_END[]  = { '<', '/', 'm', 'e', 't', 'h', 'o', 'd', 'N', 'a', 'm', 'e', '>' };
static const Pt::Char XMLRPC_PARAMS_END[]  = { '<', '/', 'p', 'a', 'r', 'a', 'm', 's', '>' };
static const Pt::Char XMLRPC_PARAM_END[]  = { '<', '/', 'p', 'a', 'r', 'a', 'm', '>' };
static const Pt::Char XMLRPC_FAULT_END[]  = { '<', '/', 'f', 'a', 'u', 'l', 't', '>' };


Client::Client()
: _method(0)
, _utf8(1)
, _ts( &_utf8 )
, _argv(0)
, _argc(0)
, _arg(0)
, _argn(0)
, _state(OnBegin)
, _formatter(_ts)
, _error(false)
, _isFault(false)
{
}


Client::~Client()
{
    _ts.detach();
}


SerializationContext& Client::context()
{
    return _ctx;
}


void Client::beginCall(Composer& r, RemoteCall& method, Decomposer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    _reader.reset(_bin);
    _formatter.beginParse(r);

    _argv = argv;
    _argc = argc;
    _arg = 0;
    _argn = 0;

    _error = false;
    _isFault = false;


    this->onInvoke();
}


void Client::endCall()
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


void Client::call(Composer& r, RemoteCall& method, Decomposer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    _reader.reset(_bin);
    _formatter.beginParse(r);

    _argv = argv;
    _argc = argc;
    _arg = 0;
    _argn = 0;

    _error = false;
    _isFault = false;

    this->onCall();
}


void Client::cancel()
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


const RemoteCall* Client::activeProcedure() const
{
    return _method;
}


bool Client::isFailed() const
{
    return _error || _isFault;
}


void Client::beginMessage(std::ostream& os)
{
    if( ! _method )
        return;

    const String& name = _method->name();

    _ts.clear();
    _ts.discard();
    _ts.attach(os);
    
    _ts.write( XMLRPC_XMLDECL, sizeof(XMLRPC_XMLDECL)/sizeof(Char) );
    
    _ts.write( XMLRPC_METHODCALL, sizeof(XMLRPC_METHODCALL)/sizeof(Char) );
    
    _ts.write( XMLRPC_METHODNAME, sizeof(XMLRPC_METHODNAME)/sizeof(Char) );
    Xml::xmlEncode(_ts, name.c_str(), name.size() );
    _ts.write(XMLRPC_METHODNAME_END, sizeof(XMLRPC_METHODNAME_END)/sizeof(Char) );
    
    _ts.write( XMLRPC_PARAMS, sizeof(XMLRPC_PARAMS)/sizeof(Char) );
}


bool Client::advanceMessage()
{
    unsigned n = 10;

    while(_argn < _argc && n > 0)
    {
        if( ! _arg)
        {
            _ts.write( XMLRPC_PARAM, sizeof(XMLRPC_PARAM)/sizeof(Char) );

            _arg = _argv[_argn];
            _arg->beginFormat(_formatter);
        }
        
        while( _arg && n > 0)
        {
            _arg = _arg->advanceFormat(_formatter);
            --n;
        }
        
        if( ! _arg )
        {
            _ts.write(XMLRPC_PARAM_END, sizeof(XMLRPC_PARAM_END)/sizeof(Char) );
            ++_argn;
        }

    }
    
    return _argn >= _argc;
}


void Client::finishMessage()
{
    _ts.write(XMLRPC_PARAMS_END, sizeof(XMLRPC_PARAMS_END)/sizeof(Char) );
    _ts.write(XMLRPC_METHODCALL_END, sizeof(XMLRPC_METHODCALL_END)/sizeof(Char) );
    
    _ts.flush();
}


void Client::beginResult(std::istream& is)
{
    _bin.reset(is);
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
    _fault.setRc(rc);
    _fault.setText(msg);
    _isFault = true;
}


void Client::setError(bool f)
{
    _error = f;
}


void Client::finishResult()
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


void Client::processResult(std::istream& is)
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


bool Client::advance(const Pt::Xml::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        {
            if(node.type() == Xml::Node::StartElement)
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name().name() != L"methodResponse" )
                    throw SerializationError("invalid XML-RPC methodCall");

                _state = OnMethodResponseBegin;
            }

            break;
        }

        case OnMethodResponseBegin:
        {
            if(node.type() == Xml::Node::StartElement) // <params> or <fault>
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name().name() == "params" )
                {
                    _state = OnParamsBegin;
                    break;
                }

                else if( se.name().name() == "fault" )
                {
                    _fh.begin(_fault);
                    _formatter.beginParse(_fh);
                    _state = OnFaultBegin;
                    break;
                }

                throw SerializationError("invalid XML-RPC methodCall");
            }
            break;
        }

        case OnFaultBegin:
        {
            bool finished = _formatter.advance(node); // start with <value>
            if(finished)
            {
                // </fault>
                _state = OnFaultEnd;
            }

            break;
        }

        case OnFaultEnd:
        {
            if(node.type() == Xml::Node::EndElement) // </methodResponse>
            {
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                //if( ee.name() != L"methodResponse" )
                //    throw SerializationError("invalid XML-RPC methodCall");

                _isFault = true;

                _state = OnFaultResponseEnd;
            }
            break;
        }

        case OnFaultResponseEnd:
        {
            _state = OnFaultResponseEnd;
            break;
        }

        case OnParamsBegin:
        {
            if(node.type() == Xml::Node::StartElement) // <param>
            {
                const Xml::StartElement& se = static_cast<const Xml::StartElement&>(node);
                if( se.name().name() != L"param" )
                    throw SerializationError("invalid XML-RPC methodCall");

                _state = OnParam;
            }

            break;
        }

        case OnParam:
        {
            bool finished = _formatter.advance(node); // start with <value>
            if(finished)
            {
                // </param>
                _state = OnParamEnd;
            }

            break;
        }

        case OnParamEnd:
        {
            if(node.type() == Xml::Node::EndElement) // </params>
            {
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                //if( ee.name() != L"params" )
                //    throw SerializationError("invalid XML-RPC methodCall");

                _state = OnParamsEnd;
            }
            break;
        }

        case OnParamsEnd:
        {
            if(node.type() == Xml::Node::EndElement) // </methodResponse>
            {
                //const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                //if( ee.name() != L"methodResponse" )
                //    throw SerializationError("invalid XML-RPC methodCall");

                _state = OnMethodResponseEnd;
            }
            break;
        }

        case OnMethodResponseEnd:
        {
            _state = OnMethodResponseEnd;
            break;
        }
    }

    return _state == OnMethodResponseEnd;
}

} // namespace XmlRpc

} // namespace Pt
