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
#include <Pt/JsonRpc/Fault.h>
#include <Pt/Json/Boolean.h>
#include <Pt/Json/Float.h>
#include <Pt/Json/Node.h>
#include <Pt/Json/Member.h>
#include <Pt/Json/Integer.h>
#include <Pt/Json/String.h>
#include <Pt/Decomposer.h>
#include <cassert>

namespace Pt {

namespace Mcp {

namespace {

void writeJsonNode(Json::JsonWriter& writer, const Json::Node& node)
{
    switch(node.type())
    {
        case Json::Node::StartObject:
            writer.writeObject();
            break;

        case Json::Node::EndObject:
            writer.writeObjectEnd();
            break;

        case Json::Node::StartArray:
            writer.writeArray();
            break;

        case Json::Node::EndArray:
            writer.writeArrayEnd();
            break;

        case Json::Node::Member:
        {
            const Json::Member& member = static_cast<const Json::Member&>(node);
            writer.writeMember(member.name());
            break;
        }

        case Json::Node::String:
        {
            const Json::String& value = static_cast<const Json::String&>(node);
            writer.writeString(value.value());
            break;
        }

        case Json::Node::Integer:
        {
            const Json::Integer& value = static_cast<const Json::Integer&>(node);
            writer.writeInt(value.value());
            break;
        }

        case Json::Node::Float:
        {
            const Json::Float& value = static_cast<const Json::Float&>(node);
            writer.writeFloat(value.value());
            break;
        }

        case Json::Node::Boolean:
        {
            const Json::Boolean& value = static_cast<const Json::Boolean&>(node);
            writer.writeBool(value.value());
            break;
        }

        case Json::Node::Null:
            writer.writeNull();
            break;

        default:
            break;
    }
}

}

Responder::Responder(Remoting::ServiceDefinition& serviceDef,
                           const ToolDeclaration& decl)
: Remoting::Responder(serviceDef)
, _decl(&decl)
, _tool(0)
, _formatter()
, _args(0)
, _state(OnBegin)
, _id(0)
, _method()
, _toolName()
, _currentParamName()
, _requestedVersion()
, _isFault(false)
, _hasId(false)
, _faultCode(0)
, _faultMessage()
, _bufferedArgumentsDepth(0)
, _bufferedArgumentsJson()
, _bufferedArgumentsStream()
, _utf8(1)
, _bufferedArgumentsText(&_utf8)
, _bufferedArgumentsWriter()
, _tis(&_utf8)
, _reader(_tis)
, _contentFormatter(0)
, _resultFormatter(0)
, _result(0)
, _resultOs(0)
, _skipDepth(0)
{
}


Responder::~Responder()
{
    if(_contentFormatter)
        _tool->content().releaseFormatter(_contentFormatter);
}


void Responder::onCancel()
{
    _state = OnBegin;
    _args = 0;

    if(_contentFormatter)
    {
        _tool->content().releaseFormatter(_contentFormatter);
        _contentFormatter = 0;
        _resultFormatter = 0;
    }

    _tool = 0;
    _isFault = false;
    _hasId = false;
    _faultCode = 0;
    _faultMessage.clear();
    _method.clear();
    _toolName.clear();
    _currentParamName.clear();
    _requestedVersion.clear();
    _bufferedArgumentsDepth = 0;
    _bufferedArgumentsJson.clear();
    _bufferedArgumentsStream.str(std::string());
    _bufferedArgumentsStream.clear();
    _bufferedArgumentsText.reset();
    _bufferedArgumentsWriter.reset();
    _result = 0;
    _resultOs = 0;
    _skipDepth = 0;
}


void Responder::onReady()
{
}


void Responder::reset()
{
    cancel();
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
    if(_isFault)
        return true;

    _tis.textBuffer().import();

    try
    {
        for(;;)
        {
            const Json::Node* node = _reader.advance();
            if( ! node)
                return false;

            if( advance(*node) )
                return true;
        }
    }
    catch(const JsonRpc::Fault& fault)
    {
        setFault(fault.code(), fault.what());
        return true;
    }
    catch(const std::exception& e)
    {
        setFault(JsonRpc::Fault::InternalError, e.what());
        return true;
    }
}


void Responder::finishMessage()
{
    if(isFailed())
        onFault();
    else
        onResult();
}


void Responder::beginResult(std::ostream& os)
{
    _resultOs = &os;

    assert(_hasId);

    if(_method == "tools/call")
    {
        try
        {
            _result = call();

            const ContentType& content = _tool->content();

            os << "{\"jsonrpc\":\"2.0\",\"id\":" << _id
               << ",\"result\":{\"content\":[";// do this in ContentFormatter::beginContent

            if(_contentFormatter)
            {
                content.releaseFormatter(_contentFormatter);
                _contentFormatter = 0;
                _resultFormatter = 0;
            }

            _contentFormatter = content.getFormatter();

            _resultFormatter = &_contentFormatter->beginContent(os);
            _result->beginFormat(*_resultFormatter);
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
               << ",\"error\":{\"code\":" << JsonRpc::Fault::InternalError << ",\"message\":\""
               << e.what()
               << "\"}}";
        }
    }
    else if(_method == "initialize")
    {
        std::string version = ToolDeclaration::preferredVersion(_requestedVersion);
        os << "{\"jsonrpc\":\"2.0\",\"id\":" << _id << ",\"result\":";
        _decl->toInitializeResult(os, version.c_str());
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
           << ",\"error\":{\"code\":" << JsonRpc::Fault::MethodNotFound << ",\"message\":\"Method not found\"}}";
        _result = 0;
    }
}


bool Responder::advanceResult()
{
    if( ! _result)
        return true;

    for(unsigned n = 0; _result && n < 10; ++n)
    {
        _result = _result->advanceFormat(*_resultFormatter);
    }

    return _result == 0;
}


void Responder::finishResult()
{
    if(_resultOs && _method == "tools/call" && ! _isFault)
    {
        _contentFormatter->finishContent(*_resultOs);
        _tool->content().releaseFormatter(_contentFormatter);
        _contentFormatter = 0;
        _resultFormatter = 0;
        *_resultOs << "],\"isError\":false}}"; // do this in ContentFormatter::finishContent
    }
    else if(_contentFormatter)
    {
        _tool->content().releaseFormatter(_contentFormatter);
        _contentFormatter = 0;
        _resultFormatter = 0;
    }

    _result = 0;
    _resultOs = 0;
}


void Responder::formatResult(std::ostream& os)
{
    beginResult(os);
    while( ! advanceResult() )
        ;
    finishResult();
}


void Responder::formatFault(std::ostream& os)
{
    os << "{\"jsonrpc\":\"2.0\",\"id\":";
    if(_hasId)
        os << _id;
    else
        os << "null";
    os << ",\"error\":{\"code\":" << _faultCode
       << ",\"message\":\"" << _faultMessage << "\"}}";
}


bool Responder::isFailed() const
{
    return _isFault;
}


void Responder::setFault(int code, const std::string& msg)
{
    _isFault = true;
    _faultCode = code;
    _faultMessage = msg;
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
                else if(_method == "initialize")
                {
                    _state = OnInitParamName;
                }
                else
                {
                    // skip params for other methods
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
                _currentParamName.clear();
                _state = OnEnvelope;
                break;
            }

            if(node.type() == Json::Node::Member)
            {
                const Json::Member& m = static_cast<const Json::Member&>(node);
                std::string name = m.name().narrow();

                if(name == "name")
                {
                    _currentParamName = name;
                    _state = OnParamNameValue;
                }
                else if(name == "arguments")
                {
                    _currentParamName = name;
                    _state = OnParamArguments;
                }
                else
                {
                    _currentParamName = name;
                    _skipDepth = 0;
                    _state = OnSkipParamValue;
                }
            }
            break;
        }

        case OnParamNameValue:
        {
            if(node.type() == Json::Node::String)
            {
                const Json::String& s = static_cast<const Json::String&>(node);
                setToolName(s.value().narrow());
            }

            _currentParamName.clear();
            _state = OnParamName;
            break;
        }

        case OnParamArguments:
        {
            if(node.type() == Json::Node::StartObject)
            {
                if(_tool)
                {
                    _state = OnArgMember;
                }
                else
                {
                    _bufferedArgumentsStream.str(std::string());
                    _bufferedArgumentsStream.clear();
                    _bufferedArgumentsText.reset(_bufferedArgumentsStream);
                    _bufferedArgumentsWriter.reset(_bufferedArgumentsText);
                    _bufferedArgumentsDepth = 0;
                    writeBufferedArgumentNode(node);
                    _state = OnCaptureArguments;
                }
            }
            break;
        }

        case OnArgMember:
        {
            if(node.type() == Json::Node::EndObject)
            {
                // End of arguments object
                _currentParamName.clear();
                _state = OnParamName;
                break;
            }

            if(node.type() == Json::Node::Member)
            {
                const Json::Member& m = static_cast<const Json::Member&>(node);
                beginArgument(m.name().narrow());
                _state = OnArgData;
            }
            break;
        }

        case OnArgData:
        {
            bool done = _formatter.advance(node);
            if(done)
                _state = OnArgMember;
            break;
        }

        case OnCaptureArguments:
        {
            writeBufferedArgumentNode(node);
            break;
        }

        case OnSkipParamValue:
        {
            if(node.type() == Json::Node::StartObject ||
               node.type() == Json::Node::StartArray)
            {
                ++_skipDepth;
            }
            else if(node.type() == Json::Node::EndObject ||
                    node.type() == Json::Node::EndArray)
            {
                if(_skipDepth > 0)
                {
                    --_skipDepth;
                    if(_skipDepth == 0)
                    {
                        _currentParamName.clear();
                        _state = OnParamName;
                    }
                }
                else
                {
                    _currentParamName.clear();
                    _state = OnParamName;
                }
            }
            else if(_skipDepth == 0)
            {
                _currentParamName.clear();
                _state = OnParamName;
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

        case OnInitParamName:
        {
            if(node.type() == Json::Node::EndObject)
            {
                _state = OnEnvelope;
                break;
            }
            if(node.type() == Json::Node::Member)
            {
                const Json::Member& m = static_cast<const Json::Member&>(node);
                std::string name = m.name().narrow();
                if(name == "protocolVersion")
                    _state = OnInitProtocolVersion;
                else
                {
                    _skipDepth = 0;
                    _state = OnSkipInitValue;
                }
            }
            break;
        }

        case OnInitProtocolVersion:
        {
            if(node.type() == Json::Node::String)
            {
                const Json::String& s = static_cast<const Json::String&>(node);
                _requestedVersion = s.value().narrow();
            }
            _state = OnInitParamName;
            break;
        }

        case OnSkipInitValue:
        {
            if(node.type() == Json::Node::StartObject ||
               node.type() == Json::Node::StartArray)
            {
                ++_skipDepth;
            }
            else if(node.type() == Json::Node::EndObject ||
                    node.type() == Json::Node::EndArray)
            {
                if(_skipDepth > 0)
                {
                    --_skipDepth;
                    if(_skipDepth == 0)
                        _state = OnInitParamName;
                }
                else
                {
                    // Unexpected: treat as end of params
                    _state = OnEnvelope;
                }
            }
            else if(_skipDepth == 0)
            {
                // Scalar value — skip is complete
                _state = OnInitParamName;
            }
            break;
        }

        case OnEnd:
            break;
    }

    return _state == OnEnd;
}


void Responder::setToolName(const std::string& toolName)
{
    _toolName = toolName;

    _tool = _decl->getTool(_toolName);
    if( ! _tool)
        throw JsonRpc::Fault("tool not found", JsonRpc::Fault::MethodNotFound);

    _args = setProcedure(_toolName);
    if( ! _args)
        throw JsonRpc::Fault("tool not registered", JsonRpc::Fault::MethodNotFound);

    if( ! _bufferedArgumentsJson.empty() )
        parseBufferedArguments();
}


void Responder::beginArgument(const std::string& argName)
{
    if( ! _tool)
        throw JsonRpc::Fault("tool not set", JsonRpc::Fault::InvalidRequest);

    int index = _tool->getParamIndex(argName);
    if(index < 0)
        throw JsonRpc::Fault("unknown argument", JsonRpc::Fault::InvalidParameters);

    if( ! _args || ! _args[index])
        throw JsonRpc::Fault("invalid argument index", JsonRpc::Fault::InvalidParameters);

    _formatter.beginParse( *_args[index] );
}


void Responder::parseBufferedArguments()
{
    std::istringstream input(_bufferedArgumentsJson);
    Pt::TextIStream tis(input, new Pt::Utf8Codec);
    tis.textBuffer().import();

    Json::JsonReader reader(tis);
    State previousState = _state;
    _state = OnArgMember;

    for(;;)
    {
        const Json::Node* node = reader.advance();
        if( ! node)
            break;

        if( advance(*node) )
            break;
    }

    _state = previousState;
    _bufferedArgumentsJson.clear();
    _bufferedArgumentsDepth = 0;
}


void Responder::writeBufferedArgumentNode(const Json::Node& node)
{
    writeJsonNode(_bufferedArgumentsWriter, node);

    if(node.type() == Json::Node::StartObject ||
       node.type() == Json::Node::StartArray)
    {
        ++_bufferedArgumentsDepth;
    }
    else if(node.type() == Json::Node::EndObject ||
            node.type() == Json::Node::EndArray)
    {
        --_bufferedArgumentsDepth;
        if(_bufferedArgumentsDepth == 0)
        {
            _bufferedArgumentsText.flush();
            _bufferedArgumentsJson = _bufferedArgumentsStream.str();
            _bufferedArgumentsStream.str(std::string());
            _bufferedArgumentsStream.clear();
            _bufferedArgumentsText.reset();
            _bufferedArgumentsWriter.reset();
            _state = OnParamName;
        }
    }
}

} // namespace Mcp

} // namespace Pt
