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

#include "Responder.h"
#include "TextFormatter.h"
#include <Pt/JsonRpc/Fault.h>
#include <Pt/Json/Node.h>
#include <Pt/Json/Member.h>
#include <Pt/Json/Integer.h>
#include <Pt/Json/String.h>
#include <Pt/Decomposer.h>
#include <cassert>

namespace Pt {

namespace Mcp {

Responder::Responder(Remoting::ServiceDefinition& serviceDef,
                           const ToolDeclaration& decl)
: Remoting::Responder(serviceDef)
, _decl(&decl)
, _tool(0)
, _formatter()
, _args(0)
, _state(OnBegin)
, _id(0)
, _isFault(false)
, _hasId(false)
, _utf8(1)
, _tis(&_utf8)
, _reader(_tis)
, _outUtf8(1)
, _tos(&_outUtf8)
, _textFmt(0)
, _result(0)
, _resultOs(0)
, _skipDepth(0)
{
}


Responder::~Responder()
{
    delete _textFmt;
}


bool Responder::isFailed() const
{
    return _isFault;
}


void Responder::onCancel()
{
    _state = OnBegin;
    _args = 0;
    _tool = 0;
    _isFault = false;
    _hasId = false;
    _method.clear();
    _toolName.clear();
    _result = 0;
    _resultOs = 0;
    _skipDepth = 0;
    _tos.flush();
    _tos.discard();
    delete _textFmt;
    _textFmt = 0;
}


void Responder::onReady()
{
}


void Responder::reset()
{
    cancel();
}


bool Responder::advance(const Json::Node& node)
{
    switch(_state)
    {
        case OnBegin:
        {
            if(node.type() == Json::Node::StartObject)
                _state = OnEnvelope;
            break;
        }

        case OnEnvelope:
        {
            if(node.type() == Json::Node::Member)
            {
                const Json::Member& m = static_cast<const Json::Member&>(node);
                std::string name = m.name().narrow();

                if(name == "method")
                    _state = OnMethod;
                else if(name == "params")
                    _state = OnParams;
                else if(name == "id")
                    _state = OnId;
                // skip "jsonrpc" and unknown members
            }
            else if(node.type() == Json::Node::EndObject)
            {
                _state = OnEnd;
                return true;
            }
            break;
        }

        case OnMethod:
        {
            if(node.type() == Json::Node::String)
            {
                const Json::String& s = static_cast<const Json::String&>(node);
                _method = s.value().narrow();
            }
            _state = OnEnvelope;
            break;
        }

        case OnParams:
        {
            if(node.type() == Json::Node::StartObject)
            {
                if(_method == "tools/call")
                {
                    _state = OnParamName;
                }
                else
                {
                    // skip params for non-tools/call methods
                    _skipDepth = 1;
                    _state = OnSkipParams;
                }
            }
            break;
        }

        case OnParamName:
        {
            if(node.type() == Json::Node::EndObject)
            {
                // End of params object
                _state = OnEnvelope;
                break;
            }

            if(node.type() == Json::Node::Member)
            {
                const Json::Member& m = static_cast<const Json::Member&>(node);
                std::string name = m.name().narrow();

                if(name == "name")
                {
                    // Next node will be the tool name string
                    // Stay in OnParamName, handle string below
                }
                else if(name == "arguments")
                {
                    _state = OnParamArguments;
                }
                // else skip unknown params members
            }
            else if(node.type() == Json::Node::String)
            {
                // This is the tool name value
                const Json::String& s = static_cast<const Json::String&>(node);
                _toolName = s.value().narrow();

                _tool = _decl->getTool(_toolName);
                if( ! _tool)
                    throw JsonRpc::Fault("tool not found", JsonRpc::Fault::MethodNotFound);

                _args = setProcedure(_toolName);
                if( ! _args)
                    throw JsonRpc::Fault("tool not registered", JsonRpc::Fault::MethodNotFound);
            }
            break;
        }

        case OnParamArguments:
        {
            if(node.type() == Json::Node::StartObject)
            {
                // Begin of arguments object — route members by name
                _state = OnArgValue;
            }
            break;
        }

        case OnArgValue:
        {
            if(node.type() == Json::Node::EndObject)
            {
                // End of arguments object
                _state = OnParamName;
                break;
            }

            if(node.type() == Json::Node::Member)
            {
                const Json::Member& m = static_cast<const Json::Member&>(node);
                std::string argName = m.name().narrow();

                if( ! _tool)
                    throw JsonRpc::Fault("tool not set", JsonRpc::Fault::InvalidRequest);

                int index = _tool->getParamIndex(argName);
                if(index < 0)
                    throw JsonRpc::Fault("unknown argument", JsonRpc::Fault::InvalidParameters);

                if( ! _args || ! _args[index])
                    throw JsonRpc::Fault("invalid argument index", JsonRpc::Fault::InvalidParameters);

                _formatter.beginParse( *_args[index] );
            }
            else
            {
                // Value node — feed to formatter
                bool done = _formatter.advance(node);
                (void)done;
            }
            break;
        }

        case OnId:
        {
            if(node.type() == Json::Node::Integer)
            {
                const Json::Integer& i = static_cast<const Json::Integer&>(node);
                _id = i.value();
                _hasId = true;
            }

            _state = OnEnvelope;
            break;
        }

        case OnSkipParams:
        {
            if(node.type() == Json::Node::StartObject ||
               node.type() == Json::Node::StartArray)
            {
                ++_skipDepth;
            }
            else if(node.type() == Json::Node::EndObject ||
                    node.type() == Json::Node::EndArray)
            {
                --_skipDepth;
                if(_skipDepth == 0)
                    _state = OnEnvelope;
            }
            break;
        }

        case OnEnd:
            break;
    }

    return _state == OnEnd;
}


void Responder::formatResult(std::ostream& os)
{
    beginResult(os);
    while( ! advanceResult() )
        ;
    finishResult();
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
    _tis.textBuffer().import();

    for(;;)
    {
        const Json::Node* node = _reader.advance();
        if( ! node)
            return false;

        if( advance(*node) )
            return true;
    }
}


void Responder::beginResult(std::ostream& os)
{
    assert(_hasId);

    _resultOs = &os;

    if(_method == "tools/call")
    {
        try
        {
            _result = call();

            os << "{\"jsonrpc\":\"2.0\",\"id\":" << _id
               << ",\"result\":{\"content\":[{\"type\":\"text\",\"text\":\"";

            _tos.clear();
            _tos.discard();
            _tos.attach(os);

            delete _textFmt;
            _textFmt = new TextFormatter(_tos);
            _result->beginFormat(*_textFmt);
        }
        catch(const JsonRpc::Fault& f)
        {
            _isFault = true;
            _result = 0;

            os << "{\"jsonrpc\":\"2.0\",\"id\":" << _id
               << ",\"result\":{\"content\":[{\"type\":\"text\",\"text\":\""
               << f.what()
               << "\"}],\"isError\":true}}";
        }
        catch(const std::exception& e)
        {
            _isFault = true;
            _result = 0;

            os << "{\"jsonrpc\":\"2.0\",\"id\":" << _id
               << ",\"error\":{\"code\":-32603,\"message\":\""
               << e.what()
               << "\"}}";
        }
    }
    else if(_method == "initialize")
    {
        os << "{\"jsonrpc\":\"2.0\",\"id\":" << _id << ",\"result\":";
        _decl->toInitializeResult(os);
        os << '}';
        _result = 0;
    }
    else if(_method == "tools/list")
    {
        os << "{\"jsonrpc\":\"2.0\",\"id\":" << _id << ",\"result\":";
        _decl->toToolsList(os);
        os << '}';
        _result = 0;
    }
    else if(_method == "ping")
    {
        os << "{\"jsonrpc\":\"2.0\",\"id\":" << _id << ",\"result\":{}}";
        _result = 0;
    }
    else
    {
        // Unknown method
        os << "{\"jsonrpc\":\"2.0\",\"id\":" << _id
           << ",\"error\":{\"code\":-32601,\"message\":\"Method not found\"}}";
        _result = 0;
    }
}


bool Responder::advanceResult()
{
    if( ! _result)
        return true;

    for(unsigned n = 0; _result && n < 10; ++n)
    {
        _result = _result->advanceFormat(*_textFmt);
    }

    return _result == 0;
}


void Responder::finishResult()
{
    if(_resultOs && _method == "tools/call" && ! _isFault)
    {
        _tos.flush();
        *_resultOs << "\"}],\"isError\":false}}";
    }

    delete _textFmt;
    _textFmt = 0;
    _result = 0;
    _resultOs = 0;
}

} // namespace Mcp

} // namespace Pt
