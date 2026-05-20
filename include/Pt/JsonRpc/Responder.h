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

#ifndef PT_JSONRPC_RESPONDER_H
#define PT_JSONRPC_RESPONDER_H

#include <Pt/JsonRpc/Api.h>
#include <Pt/JsonRpc/Fault.h>
#include <Pt/JsonRpc/Formatter.h>
#include <Pt/Remoting/Responder.h>
#include <Pt/Json/JsonReader.h>
#include <Pt/Json/JsonWriter.h>
#include <Pt/TextStream.h>
#include <Pt/NonCopyable.h>
#include <Pt/Utf8Codec.h>
#include <Pt/Types.h>

namespace Pt {

namespace JsonRpc {

class ProcedureDeclaration;
class ServiceDeclaration;

/** @brief Dispatches JSON-RPC requests to a service procedure.
*/
class PT_JSONRPC_API Responder : public Remoting::Responder
{
  public:
    /** @brief Construct with ServiceDeclaration and ServiceDefinition.
    */
    Responder(const ServiceDeclaration& decl,
              Remoting::ServiceDefinition& def);

    /** @brief Destructor.
    */
    virtual ~Responder();

    /** @brief Indicates if the procedure has failed.
    */
    bool isFailed() const;

  protected:
    // inheritdoc
    virtual void onReady();

    // inheritdoc
    virtual void onCancel();

    /** @brief The service procedure has failed.

        Derived responders implement this method to format and send the
        JSON-RPC error response. Use beginFault() to format the response.
    */
    virtual void onFault(const Fault& fault) = 0;

    /** @brief The service procedure has finished.

        Derived responders implement this method to format and send the
        JSON-RPC result. Use beginResult(), advanceResult() and
        finishResult() to format the response.
    */
    virtual void onResult() = 0;

  protected:
    /** @brief Begin parsing a JSON-RPC request from a stream.
    */
    void beginMessage(std::istream& is);

    /** @brief Parse available data from the input stream.

        Each call consumes available data. Returns true if parsing is
        complete (success or error).
    */
    bool parseMessage();

    /** @brief Execute the service procedure after parsing completes.
    */
    void finishMessage(System::EventLoop& loop);

    /** @brief Execute the service procedure synchronously.
    */
    void finishMessage();

    /** @brief Begin formatting a JSON-RPC success response.
    */
    void beginResult(std::ostream& os);

    /** @brief Begin formatting a JSON-RPC error response.
    */
    void beginFault(std::ostream& os, const Fault& fault);

    /** @brief Continue formatting the result.

        @return true if the result is fully formatted.
    */
    bool advanceResult();

    /** @brief Finish formatting the result response.
    */
    void finishResult();

    /** @brief Mark the procedure as failed.
    */
    void setFault(int rc, const char* msg);

  private:
    //! @internal
    bool advance(const Json::Node& node);

  private:
    enum State
    {
        OnBegin,
        OnRequestObject,
        OnMethod,
        OnParams,
        OnParam,
        OnNamedParams,
        OnNamedParam,
        OnParamsEnd,
        OnId,
        OnEnd
    };

    const ServiceDeclaration*    _decl;
    const ProcedureDeclaration*  _procedure;

    Utf8Codec        _utf8;
    TextIStream      _tis;
    Json::JsonReader _reader;
    Composer**       _args;
    State            _state;
    long long        _id;
    std::string      _methodName;

    TextOStream      _tos;
    Json::JsonWriter _writer;
    Formatter        _formatter;
    Decomposer*      _result;

    Fault _fault;
    bool  _isFault;
    Pt::varint_t _r1;
    Pt::varint_t _r2;
};

} // namespace JsonRpc

} // namespace Pt

#endif // PT_JSONRPC_RESPONDER_H
