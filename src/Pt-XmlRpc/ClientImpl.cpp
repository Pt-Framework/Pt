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

#include "ClientImpl.h"
#include "Pt/XmlRpc/Client.h"
#include "Pt/XmlRpc/RemoteProcedure.h"
#include <Pt/Xml/XmlWriter.h>
#include "Pt/Xml/XmlError.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/System/Selectable.h"
#include "Pt/System/Logger.h"
#include <cassert>

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


ClientImpl::ClientImpl()
: _state(OnBegin)
, _utf8(1)
, _ts( &_utf8 )
, _bin()
, _reader()
, _formatter(_ts)
, _argv(0)
, _argc(0)
, _method(0)
{
}


ClientImpl::~ClientImpl()
{
    _ts.reset();
}


void ClientImpl::beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    _reader.reset(_bin);
    _scanner.begin(r);

    _argv = argv;
    _argc = argc;
}


void ClientImpl::endCall()
{
}


void ClientImpl::cancel()
{
    _method = 0;
    _argc = 0;
    _argv = 0;
}


const IRemoteProcedure* ClientImpl::activeProcedure() const
{
    return _method;
}


void ClientImpl::formatRequest(std::ostream& os)
{
    const String& name = _method->name();

    _ts.attach(os);
    
    _ts.write( XMLRPC_XMLDECL, sizeof(XMLRPC_XMLDECL)/sizeof(Char) );
    
    _ts.write( XMLRPC_METHODCALL, sizeof(XMLRPC_METHODCALL)/sizeof(Char) );
    
    _ts.write( XMLRPC_METHODNAME, sizeof(XMLRPC_METHODNAME)/sizeof(Char) );
    Xml::xmlEncode(_ts, name.c_str(), name.size() );
    _ts.write(XMLRPC_METHODNAME_END, sizeof(XMLRPC_METHODNAME_END)/sizeof(Char) );
    
    _ts.write( XMLRPC_PARAMS, sizeof(XMLRPC_PARAMS)/sizeof(Char) );

    for(unsigned n = 0; n < _argc; ++n)
    {
        _ts.write( XMLRPC_PARAM, sizeof(XMLRPC_PARAM)/sizeof(Char) );
        _argv[n]->format(_formatter);
        _ts.write(XMLRPC_PARAM_END, sizeof(XMLRPC_PARAM_END)/sizeof(Char) );
    }

    _ts.write(XMLRPC_PARAMS_END, sizeof(XMLRPC_PARAMS_END)/sizeof(Char) );
    _ts.write(XMLRPC_METHODCALL_END, sizeof(XMLRPC_METHODCALL_END)/sizeof(Char) );
    
    _ts.flush();
}


bool ClientImpl::beginReply(std::istream& is)
{
    _bin.reset(is);
    return true;
}


bool ClientImpl::advanceReply()
{
    try
    {
        for(;;) 
        {
            const Pt::Xml::Node* node = _reader.advance(); // XmlError
            if( ! node)
                break;
            
            advance(*node); // SerializationError, ConversionError
        }

        // TODO: return true if more is available, false if finished or
        //       an error occured. Do not call finishReply() here but 
        //       let caller do it.
         
        return true;
    }
    catch(const Xml::XmlError& error)
    {
        _method->setFault(Fault::invalidXmlRpc, error.what());
        finishReply();
    }
    catch(const SerializationError& error)
    {
        _method->setFault(Fault::invalidMethodParameters, error.what());
        finishReply();
    }
    catch(const ConversionError& error)
    {
        _method->setFault(Fault::invalidMethodParameters, error.what());
        finishReply();
    }

    // signal caller to not continue reading on error
    return false;
}


void ClientImpl::finishReply()
{
    log_debug("onReplyFinished; method=" << static_cast<void*>(_method));

    if(_method)
    {
        IRemoteProcedure* method = _method;
        _method = 0;
        method->onFinished();
    }
}


void ClientImpl::readReply(std::istream& is)
{   
    _bin.reset(is);

    while( _reader.get().type() !=  Pt::Xml::Node::EndDocument )
    {
        const Pt::Xml::Node& node = _reader.get();
        advance(node);
        _reader.next();
    }

    // let Xml::ParseError SerializationError, ConversionError propagate

    if (_method->failed() )
    {
        _method = 0;
        _state = OnBegin;
        throw _fault;
    }

    _method = 0;
    _state = OnBegin;

    // _method contains a valid return value now
}


void ClientImpl::advance(const Pt::Xml::Node& node)
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
                    _scanner.begin(_fh);
                    _state = OnFaultBegin;
                    break;
                }

                throw SerializationError("invalid XML-RPC methodCall");
            }
            break;
        }

        case OnFaultBegin:
        {
            bool finished = _scanner.advance(node); // start with <value>
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

                _method->setFault(_fault.rc(), _fault.text());

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
            bool finished = _scanner.advance(node); // start with <value>
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
}

} // namespace XmlRpc

} // namespace Pt
