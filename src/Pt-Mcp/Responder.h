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

#ifndef PT_MCP_RESPONDER_H
#define PT_MCP_RESPONDER_H

#include <Pt/Mcp/Api.h>
#include <Pt/Mcp/ToolDeclaration.h>
#include <Pt/JsonRpc/Formatter.h>
#include <Pt/Json/JsonReader.h>
#include <Pt/Remoting/Responder.h>
#include <Pt/Remoting/ServiceDefinition.h>
#include <Pt/TextStream.h>
#include <Pt/Utf8Codec.h>
#include <Pt/Types.h>
#include <string>

namespace Pt {

namespace Json {
    class Node;
}

namespace Mcp {

class TextFormatter;

/** @brief Dispatches MCP requests to service procedures.

    Parses the MCP envelope, determines the method (initialize,
    tools/list, tools/call), and routes arguments to the correct
    service procedure composers by name.
*/
class Responder : public Remoting::Responder
{
  public:
    Responder(Remoting::ServiceDefinition& serviceDef,
              const ToolDeclaration& decl);

    ~Responder();

    /** @brief Feeds one JSON node to the state machine.
        @return true when the request is complete.
    */
    bool advance(const Json::Node& node);

    /** @brief Calls the tool and formats the result synchronously.
    */
    void formatResult(std::ostream& os);

    /** @brief Resets the responder state.
    */
    void reset();

    /** @brief Begin parsing a request from a stream.
    */
    void beginMessage(std::istream& is);

    /** @brief Parse available data from the input stream.
        @return true if parsing is complete.
    */
    bool parseMessage();

    /** @brief Begin formatting the response.
    */
    void beginResult(std::ostream& os);

    /** @brief Continue formatting the result.
        @return true if the result is fully formatted.
    */
    bool advanceResult();

    /** @brief Finish formatting the response.
    */
    void finishResult();

    bool isFailed() const override;

    const std::string& method() const
    { return _method; }

    const std::string& toolName() const
    { return _toolName; }

    Pt::int64_t requestId() const
    { return _id; }

    bool isNotification() const
    { return ! _hasId; }

  protected:
    void onCancel() override;
    void onReady() override;

  private:
    enum State
    {
        OnBegin,
        OnEnvelope,
        OnMethod,
        OnParams,
        OnParamName,
        OnParamArguments,
        OnArgValue,
        OnId,
        OnSkipParams,
        OnEnd
    };

    const ToolDeclaration* _decl;
    const Tool* _tool;
    JsonRpc::Formatter _formatter;
    Pt::Composer** _args;
    State _state;
    Pt::int64_t _id;
    std::string _method;
    std::string _toolName;
    bool _isFault;
    bool _hasId;

    // Incremental parsing
    Pt::Utf8Codec _utf8;
    Pt::TextIStream _tis;
    Json::JsonReader _reader;

    // Incremental result formatting
    Pt::Utf8Codec _outUtf8;
    Pt::TextOStream _tos;
    TextFormatter* _textFmt;
    Decomposer* _result;
    std::ostream* _resultOs;
    int _skipDepth;
};

} // namespace Mcp

} // namespace Pt

#endif // PT_MCP_RESPONDER_H
