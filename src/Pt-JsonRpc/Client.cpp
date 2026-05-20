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

#include <Pt/JsonRpc/Client.h>
#include <Pt/JsonRpc/ProcedureDeclaration.h>
#include <Pt/Remoting/RemoteProcedure.h>
#include <Pt/Json/Node.h>
#include <Pt/Json/Member.h>
#include <Pt/Json/Integer.h>
#include <Pt/Json/String.h>
#include <Pt/System/Logger.h>
#include <cassert>

PT_LOG_DEFINE("Pt.JsonRpc.Client")

namespace Pt {

namespace JsonRpc {

Client::Client()
: _procedure(0)
, _r(0)
, _argv(0)
, _argc(0)
, _arg(0)
, _argn(0)
, _utf8(1)
, _tos(&_utf8)
, _writer(_tos)
, _formatter(_writer)
, _tis(&_utf8)
, _reader(_tis)
, _state(OnBegin)
, _fault("", 0)
, _faultCode(0)
, _isFault(false)
, _id(0)
, _nextId(1)
, _r1()
, _r2()
{
}


Client::~Client()
{
    _tos.detach();
}


bool Client::isFailed() const
{
    return _isFault;
}


void Client::setProcedure(const ProcedureDeclaration* decl)
{
    _procedure = decl;
}


void Client::onBeginCall(Composer& r, Remoting::RemoteCall& method, Decomposer** argv, unsigned argc)
{
    _r = &r;
    _argv = argv;
    _argc = argc;
    _arg = 0;
    _argn = 0;

    _state = OnBegin;
    _isFault = false;
    _id = _nextId++;

    this->onBeginInvoke();
}


void Client::onEndCall()
{
    if( _isFault )
    {
        _isFault = false;
        throw _fault;
    }

    this->onEndInvoke();

    if(_state != OnEnd)
    {
        throw Fault("invalid JSON-RPC response", Fault::InvalidRequest);
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
    _isFault = false;
    _id = _nextId++;

    this->onInvoke();

    if( _isFault )
    {
        _isFault = false;
        throw _fault;
    }

    if(_state != OnEnd)
    {
        throw Fault("invalid JSON-RPC response", Fault::InvalidRequest);
    }
}


void Client::onCancel()
{
    _r = 0;
    _argc = 0;
    _argv = 0;
    _arg = 0;
    _argn = 0;

    _tos.detach();
    _tos.discard();

    _isFault = false;
}


void Client::beginMessage(std::ostream& os)
{
    const Remoting::RemoteCall* method = activeProcedure();
    if( ! method )
        return;

    _tos.flush();
    _tos.clear();
    _tos.discard();
    _tos.attach(os);

    // {"jsonrpc":"2.0","method":"<name>","params":
    _writer.writeObject();
    _writer.writeMember("jsonrpc");
    _writer.writeString(L"2.0");

    _writer.writeMember("method");
    const String& name = method->name();
    _writer.writeString( name.c_str() );

    _writer.writeMember("params");

    if(_procedure && _procedure->hasNames())
    {
        _writer.writeObject();
    }
    else
    {
        _writer.writeArray();
    }
}


bool Client::advanceMessage()
{
    unsigned n = 10;

    while(_argn < _argc && n > 0)
    {
        if( ! _arg )
        {
            if(_procedure && _procedure->hasNames())
            {
                const std::string& paramName = _procedure->getParamName(_argn);
                if( ! paramName.empty() )
                    _writer.writeMember( paramName.c_str() );
            }

            _arg = _argv[_argn];
            _arg->beginFormat(_formatter);
        }

        while(_arg && n > 0)
        {
            _arg = _arg->advanceFormat(_formatter);
            --n;
        }

        if( ! _arg )
        {
            ++_argn;
        }
    }

    return _argn >= _argc;
}


void Client::finishMessage()
{
    if(_procedure && _procedure->hasNames())
    {
        _writer.writeObjectEnd(); // end params object
    }
    else
    {
        _writer.writeArrayEnd(); // end params array
    }

    // ,"id":<id>}
    _writer.writeMember("id");
    _writer.writeInt(_id);
    _writer.writeObjectEnd(); // end request object

    _tos.flush();
}


void Client::beginResult(std::istream& is)
{
    _tis.reset(is);
    _tis.clear();

    _state = OnBegin;
    _faultCode = 0;
    _faultMessage.clear();
}


bool Client::parseResult()
{
    _tis.textBuffer().import();

    try
    {
        for(;;)
        {
            const Json::Node* node = _reader.advance();
            if( ! node )
                break;

            bool done = advance(*node);
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

    return true;
}


void Client::processResult(std::istream& is)
{
    _tis.reset(is);
    _tis.clear();
    _tis.textBuffer().import();

    _state = OnBegin;
    _faultCode = 0;
    _faultMessage.clear();

    try
    {
        for(;;)
        {
            const Json::Node* node = _reader.advance();
            if( ! node )
                break;

            bool done = advance(*node);
            if(done)
                break;
        }
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
}


void Client::setFault(int rc, const char* msg)
{
    _fault = Fault(msg, rc);
    _isFault = true;
}


// JSON-RPC 2.0 response:
// Success: {"jsonrpc":"2.0","result":<value>,"id":1}
// Error:   {"jsonrpc":"2.0","error":{"code":-32601,"message":"..."},"id":1}

bool Client::advance(const Json::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        {
            if(node.type() == Json::Node::StartObject)
            {
                _state = OnResponseObject;
            }
            else
            {
                throw Fault("expected JSON object", Fault::InvalidRequest);
            }
            break;
        }

        case OnResponseObject:
        {
            if(node.type() == Json::Node::Member)
            {
                const Json::Member& m = static_cast<const Json::Member&>(node);
                std::string name = m.name().narrow();

                if(name == "result")
                {
                    assert(_r);
                    _formatter.beginParse(*_r);
                    _state = OnResult;
                }
                else if(name == "error")
                {
                    _state = OnError;
                }
                else if(name == "id")
                {
                    _state = OnId;
                }
                // skip "jsonrpc" and unknown members
            }
            else if(node.type() == Json::Node::EndObject)
            {
                if(_faultCode != 0)
                {
                    setFault(_faultCode, _faultMessage.c_str());
                }
                _state = OnEnd;
                return true;
            }
            // Scalar values of skipped members
            break;
        }

        case OnResult:
        {
            bool done = _formatter.advance(node);
            if(done)
            {
                _state = OnResponseObject;
            }
            break;
        }

        case OnError:
        {
            if(node.type() == Json::Node::StartObject)
            {
                _state = OnErrorObject;
            }
            else
            {
                // null error — treat as no error
                _state = OnResponseObject;
            }
            break;
        }

        case OnErrorObject:
        {
            if(node.type() == Json::Node::Member)
            {
                const Json::Member& m = static_cast<const Json::Member&>(node);
                std::string name = m.name().narrow();

                if(name == "code")
                {
                    _state = OnErrorCode;
                }
                else if(name == "message")
                {
                    _state = OnErrorMessage;
                }
                // skip unknown members like "data"
            }
            else if(node.type() == Json::Node::EndObject)
            {
                _state = OnResponseObject;
            }
            // Scalar values of skipped members
            break;
        }

        case OnErrorCode:
        {
            if(node.type() == Json::Node::Integer)
            {
                const Json::Integer& i = static_cast<const Json::Integer&>(node);
                _faultCode = static_cast<int>(i.value());
            }
            _state = OnErrorObject;
            break;
        }

        case OnErrorMessage:
        {
            if(node.type() == Json::Node::String)
            {
                const Json::String& s = static_cast<const Json::String&>(node);
                _faultMessage = s.value().narrow();
            }
            _state = OnErrorObject;
            break;
        }

        case OnId:
        {
            if(node.type() == Json::Node::Integer)
            {
                const Json::Integer& i = static_cast<const Json::Integer&>(node);
                _id = i.value();
            }
            _state = OnResponseObject;
            break;
        }

        case OnEnd:
        {
            break;
        }
    }

    return _state == OnEnd;
}

} // namespace JsonRpc

} // namespace Pt
