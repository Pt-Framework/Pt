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

#include "Pt/XmlRpc/Client.h"
#include "ClientImpl.h"
#include "Pt/XmlRpc/RemoteProcedure.h"
#include "Pt/Xml/XmlError.h"
#include "Pt/Xml/StartElement.h"
#include "Pt/Xml/Characters.h"
#include "Pt/Xml/EndElement.h"
#include "Pt/System/Selectable.h"
#include "Pt/Utf8Codec.h"

#define log_debug(x)

namespace Pt {

namespace XmlRpc {

static const Pt::Char XMLRPC_METHODRESPONSE[]  = { 'm', 'e', 't', 'h', 'o', 'd', 'R', 'e', 's', 'p', 'o', 'n', 's', 'e', '\0' };
static const Pt::Char XMLRPC_METHODCALL[]  = { 'm', 'e', 't', 'h', 'o', 'd', 'C', 'a', 'l', 'l', '\0' };
static const Pt::Char XMLRPC_METHODNAME[]  = { 'm', 'e', 't', 'h', 'o', 'd', 'N', 'a', 'm', 'e', '\0' };
static const Pt::Char XMLRPC_PARAMS[]  = { 'p', 'a', 'r', 'a', 'm', 's', '\0' };
static const Pt::Char XMLRPC_PARAM[]  = { 'p', 'a', 'r', 'a', 'm', '\0' };
static const Pt::Char XMLRPC_FAULT[]  = { 'f', 'a', 'u', 'l', 't', '\0' };

ClientImpl::ClientImpl()
: _state(OnBegin)
, _ts( new Utf8Codec )
, _reader(_ts)
, _formatter(_writer)
, _method(0)
, _timeout(System::EventLoop::WaitInfinite)
, _errorPending(false)
{
    _writer.useIndent(false);
    _writer.useEndl(false);
}

ClientImpl::~ClientImpl()
{
}


void ClientImpl::beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    prepareRequest(method.name(), argv, argc);

    beginExecute();

    _reader.reset(_ts);
    _scanner.begin(r);
}


void ClientImpl::endCall()
{
    endExecute();
}


void ClientImpl::call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _method = &method;
    _state = OnBegin;

    prepareRequest(method.name(), argv, argc);

    std::istringstream is(execute());
    _ts.attach(is);
    _reader.reset(_ts);
    _scanner.begin(r);

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


const IRemoteProcedure* ClientImpl::activeProcedure() const
{
    return _method;
}

void ClientImpl::cancel()
{
    _method = 0;
}

void ClientImpl::onReadReplyBegin(std::istream& is)
{
    _ts.attach(is);
}

std::size_t ClientImpl::onReadReply()
{
    std::size_t n = 0;

    try
    {
        _errorPending = false;

        while(true)
        {
            std::streamsize m = _ts.buffer().import();
            if( ! m )
                break;

            n += m;

            while( _reader.advance() ) // Xml::ParseError
            {
                const Pt::Xml::Node& node = _reader.get();
                advance(node); // SerializationError, ConversionError
            }
        }
    }
    catch(const Xml::XmlError& error)
    {
        _method->setFault(Fault::invalidXmlRpc, error.what());
        _method->onFinished();
    }
    catch(const SerializationError& error)
    {
        _method->setFault(Fault::invalidMethodParameters, error.what());
        _method->onFinished();
    }
    catch(const ConversionError& error)
    {
        _method->setFault(Fault::invalidMethodParameters, error.what());
        _method->onFinished();
    }
    catch(const std::exception& error)
    {
        _errorPending = true;
        _method->onFinished();
    }

    return n;
}


void ClientImpl::onReplyFinished()
{
    log_debug("onReplyFinished; method=" << static_cast<void*>(_method));

    try
    {
        _errorPending = false;
        endExecute();
    }
    catch (const std::exception& e)
    {
        if (!_method)
            throw;

        _errorPending = true;

        IRemoteProcedure* method = _method;
        _method = 0;
        method->onFinished();
        _errorPending = false;
        return;
    }

    IRemoteProcedure* method = _method;
    _method = 0;
    method->onFinished();
}


void ClientImpl::prepareRequest(const String& name, IDecomposer** argv, unsigned argc)
{
    _writer.begin( prepareRequest() );
    _writer.writeStartTag( XMLRPC_METHODCALL );
    _writer.writeElement( XMLRPC_METHODNAME, name.c_str() );
    _writer.writeStartTag( XMLRPC_PARAMS );

    for(unsigned n = 0; n < argc; ++n)
    {
        _writer.writeStartTag( XMLRPC_PARAM );
        argv[n]->format(_formatter);
        _writer.writeEndTag(XMLRPC_PARAM);
    }

    _writer.writeEndTag(XMLRPC_PARAMS);
    _writer.writeEndTag(XMLRPC_METHODCALL);
    _writer.flush();
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
                if( se.name() != L"methodResponse" )
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
                if( se.name() == L"params")
                {
                    _state = OnParamsBegin;
                    break;
                }

                else if( se.name() == L"fault")
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
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                if( ee.name() != L"methodResponse" )
                    throw SerializationError("invalid XML-RPC methodCall");

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
                if( se.name() != L"param" )
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
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                if( ee.name() != L"params" )
                    throw SerializationError("invalid XML-RPC methodCall");

                _state = OnParamsEnd;
            }
            break;
        }

        case OnParamsEnd:
        {
            if(node.type() == Xml::Node::EndElement) // </methodResponse>
            {
                const Xml::EndElement& ee = static_cast<const Xml::EndElement&>(node);
                if( ee.name() != L"methodResponse" )
                    throw SerializationError("invalid XML-RPC methodCall");

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

}

}
