/*
 * Copyright (C) 2020-2026 by Marc Boris Duerner
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <Pt/JsonRpc/Responder.h>
#include <Pt/JsonRpc/Fault.h>
#include <Pt/JsonRpc/ServiceDeclaration.h>
#include <Pt/JsonRpc/ProcedureDeclaration.h>
#include <Pt/Json/Node.h>
#include <Pt/Json/Member.h>
#include <Pt/Json/Integer.h>
#include <Pt/Json/String.h>
#include <Pt/Remoting/ServiceDefinition.h>
#include <Pt/System/Logger.h>
#include <Pt/Utf8Codec.h>
#include <Pt/Convert.h>
#include <cassert>

PT_LOG_DEFINE("Pt.JsonRpc.Responder")

namespace Pt {

namespace JsonRpc {

Responder::Responder(const ServiceDeclaration& decl,
                     Remoting::ServiceDefinition& def)
: Remoting::Responder(def)
, _decl(&decl)
, _procedure(0)
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
    _writer.setFormatting(false);
}


Responder::~Responder()
{
    _tos.detach();
}


bool Responder::isFailed() const
{
    return _isFault;
}


void Responder::onCancel()
{
    _state = OnBegin;
    _tos.detach();
    _tos.discard();

    _args = 0;
    _result = 0;
    _procedure = 0;
    _isFault = false;
    _methodName.clear();
}


void Responder::beginMessage(std::istream& is)
{
    cancel();
    _tis.reset(is);
    _tis.clear();
    _tis.textBuffer().import();
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
                break;

            bool done = this->advance(*node);
            if(done)
                return true;
        }

        return false;
    }
    catch(const Json::JsonError& error)
    {
        setFault(Fault::ParseError, error.what());
    }
    catch(const SerializationError& error)
    {
        setFault(Fault::ParseError, error.what());
    }
    catch(const ConversionError& error)
    {
        setFault(Fault::ParseError, error.what());
    }
    catch(const Fault& fault)
    {
        setFault(fault.code(), fault.what());
    }
    catch(const Remoting::Fault& fault)
    {
        setFault(Fault::InternalError, fault.what());
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
            throw Fault("expected more arguments", Fault::InvalidParameters);
        }

        beginCall(loop);
    }
    catch(const Fault& fault)
    {
        setFault(fault.code(), fault.what());
        onFault(_fault);
    }
    catch(const Remoting::Fault& fault)
    {
        setFault(Fault::InternalError, fault.what());
        onFault(_fault);
    }
}


void Responder::finishMessage()
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
            throw Fault("expected more arguments", Fault::InvalidParameters);
        }

        _result = call();
        onResult();
    }
    catch(const Fault& fault)
    {
        setFault(fault.code(), fault.what());
        onFault(_fault);
    }
    catch(const Remoting::Fault& fault)
    {
        setFault(Fault::InternalError, fault.what());
        onFault(_fault);
    }
}


void Responder::onReady()
{
    try
    {
        _result = endCall();
        onResult();
    }
    catch(const Fault& fault)
    {
        setFault(fault.code(), fault.what());
        onFault(_fault);
    }
    catch(const Remoting::Fault& fault)
    {
        setFault(Fault::InternalError, fault.what());
        onFault(_fault);
    }
}


void Responder::beginResult(std::ostream& os)
{
    // text stream might still have bytes in text buffer
    _tos.flush();

    _tos.clear();
    _tos.discard();
    _tos.attach(os);

    // {"jsonrpc":"2.0","result":
    _writer.writeObject();
    _writer.writeMember("jsonrpc");
    _writer.writeString(L"2.0");
    _writer.writeMember("result");

    assert(_result);
    _result->beginFormat(_formatter);
}


void Responder::beginFault(std::ostream& os, const Fault& fault)
{
    // text stream might still have bytes in text buffer
    _tos.flush();

    _tos.clear();
    _tos.discard();
    _tos.attach(os);

    // {"jsonrpc":"2.0","error":{"code":<code>,"message":"<msg>"},"id":<id>}
    _writer.writeObject();
    _writer.writeMember("jsonrpc");
    _writer.writeString(L"2.0");

    _writer.writeMember("error");
    _writer.writeObject();
    _writer.writeMember("code");
    _writer.writeInt( fault.code() );
    _writer.writeMember("message");

    const char* msg = fault.what();
    Pt::String wmsg;
    for(const char* p = msg; *p != '\0'; ++p)
        wmsg += Pt::Char(*p);
    _writer.writeString( wmsg.c_str() );

    _writer.writeObjectEnd(); // end error object

    _writer.writeMember("id");
    _writer.writeInt(_id);
    _writer.writeObjectEnd(); // end response object

    _tos.flush();
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
        // ,"id":<id>}
        _writer.writeMember("id");
        _writer.writeInt(_id);
        _writer.writeObjectEnd();
        _tos.flush();
    }
}


void Responder::setFault(int rc, const char* msg)
{
    _fault = Fault(msg, rc);
    _isFault = true;
}


// JSON-RPC 2.0 request:
// {"jsonrpc": "2.0", "method": "name", "params": [...] or {...}, "id": 1}
// Members can appear in any order.

bool Responder::advance(const Json::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        {
            if(node.type() == Json::Node::StartObject)
            {
                _state = OnRequestObject;
            }
            else
            {
                throw Fault("expected JSON object", Fault::InvalidRequest);
            }
            break;
        }

        case OnRequestObject:
        {
            if(node.type() == Json::Node::Member)
            {
                const Json::Member& m = static_cast<const Json::Member&>(node);
                std::string name = m.name().narrow();

                if(name == "method")
                {
                    _state = OnMethod;
                }
                else if(name == "params")
                {
                    _state = OnParams;
                }
                else if(name == "id")
                {
                    _state = OnId;
                }
                // skip "jsonrpc" and unknown members
            }
            else if(node.type() == Json::Node::EndObject)
            {
                _state = OnEnd;
                return true;
            }
            // Scalar values of skipped members
            break;
        }

        case OnMethod:
        {
            if(node.type() == Json::Node::String)
            {
                const Json::String& s = static_cast<const Json::String&>(node);
                _methodName = s.value().narrow();

                _args = setProcedure(_methodName);
                if( ! _args )
                    throw Fault("method not found", Fault::MethodNotFound);

                _procedure = _decl->getProcedure(_methodName);
            }
            else
            {
                throw Fault("method must be a string", Fault::InvalidRequest);
            }

            _state = OnRequestObject;
            break;
        }

        case OnParams:
        {
            if(node.type() == Json::Node::StartArray)
            {
                _state = OnParam;
            }
            else if(node.type() == Json::Node::StartObject)
            {
                if( ! _procedure )
                    throw Fault("named params require a procedure declaration", Fault::InvalidParameters);

                _state = OnNamedParams;
            }
            else
            {
                throw Fault("params must be array or object", Fault::InvalidRequest);
            }
            break;
        }

        case OnParam:
        {
            if(node.type() == Json::Node::EndArray)
            {
                _state = OnRequestObject;
                break;
            }

            if( ! _args || ! *_args )
                throw Fault("too many arguments", Fault::InvalidParameters);

            _formatter.beginParse(**_args);
            bool done = _formatter.advance(node);

            if(done)
                ++_args;

            break;
        }

        case OnNamedParams:
        {
            if(node.type() == Json::Node::EndObject)
            {
                // Advance _args past all entries so finishMessage
                // does not report "expected more arguments"
                while(_args && *_args)
                    ++_args;

                _state = OnRequestObject;
                break;
            }

            if(node.type() == Json::Node::Member)
            {
                const Json::Member& m = static_cast<const Json::Member&>(node);
                std::string paramName = m.name().narrow();

                int index = _procedure->getParamIndex(paramName);
                if(index < 0)
                    throw Fault("unknown parameter name", Fault::InvalidParameters);

                if( ! _args || ! _args[index] )
                    throw Fault("invalid parameter index", Fault::InvalidParameters);

                _formatter.beginParse( *_args[index] );
                _state = OnNamedParam;
            }
            break;
        }

        case OnNamedParam:
        {
            bool done = _formatter.advance(node);
            if(done)
                _state = OnNamedParams;

            break;
        }

        case OnId:
        {
            if(node.type() == Json::Node::Integer)
            {
                const Json::Integer& i = static_cast<const Json::Integer&>(node);
                _id = i.value();
            }

            _state = OnRequestObject;
            break;
        }

        case OnParamsEnd:
        case OnEnd:
        {
            break;
        }
    }

    return _state == OnEnd;
}

} // namespace JsonRpc

} // namespace Pt
