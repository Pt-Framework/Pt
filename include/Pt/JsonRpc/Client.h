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

#ifndef PT_JSONRPC_CLIENT_H
#define PT_JSONRPC_CLIENT_H

#include <Pt/JsonRpc/Api.h>
#include <Pt/JsonRpc/Fault.h>
#include <Pt/JsonRpc/Formatter.h>
#include <Pt/Remoting/Client.h>
#include <Pt/Json/JsonReader.h>
#include <Pt/Json/JsonWriter.h>
#include <Pt/Composer.h>
#include <Pt/Decomposer.h>
#include <Pt/Utf8Codec.h>
#include <Pt/TextStream.h>
#include <Pt/NonCopyable.h>
#include <Pt/Types.h>
#include <string>

namespace Pt {

namespace JsonRpc {

class ProcedureDeclaration;

/** @brief A client for JSON-RPC 2.0 remote procedure calls.
*/
class PT_JSONRPC_API Client : public Remoting::Client
{
  public:
    /** @brief Constructor.
    */
    Client();

    /** @brief Destructor.
    */
    virtual ~Client();

    /** @brief Indicates if the procedure has failed.
    */
    bool isFailed() const;

    /** @brief Set procedure declaration for named parameters.

        If set, parameters will be sent as a JSON object with named keys.
        If not set, parameters are sent as a JSON array.
    */
    void setProcedure(const ProcedureDeclaration* decl);

  protected:
    virtual void onBeginCall(Composer& r, Remoting::RemoteCall& method, Decomposer** argv, unsigned argc);

    virtual void onEndCall();

    virtual void onCall(Composer& r, Remoting::RemoteCall& method, Decomposer** argv, unsigned argc);

    virtual void onCancel();

    /** @brief Begin an asynchronous invocation.

        Derived Clients implement this to send the formatted request.
    */
    virtual void onBeginInvoke() = 0;

    /** @brief End an asynchronous invocation.
    */
    virtual void onEndInvoke() = 0;

    /** @brief Perform a synchronous invocation.

        Derived Clients implement this to send the request and receive
        the response.
    */
    virtual void onInvoke() = 0;

  protected:
    /** @brief Begin formatting a JSON-RPC request to a stream.
    */
    void beginMessage(std::ostream& os);

    /** @brief Continue formatting the request.

        @return true if all parameters are formatted.
    */
    bool advanceMessage();

    /** @brief Finish formatting the request.
    */
    void finishMessage();

    /** @brief Begin parsing a JSON-RPC response from a stream.
    */
    void beginResult(std::istream& is);

    /** @brief Parse available response data.

        @return true if parsing is complete.
    */
    bool parseResult();

    /** @brief Parse the entire response synchronously.
    */
    void processResult(std::istream& is);

    /** @brief Mark the current procedure as failed.
    */
    void setFault(int rc, const char* msg);

  private:
    //! @internal
    bool advance(const Json::Node& node);

  private:
    enum State
    {
        OnBegin,
        OnResponseObject,
        OnResult,
        OnError,
        OnErrorObject,
        OnErrorCode,
        OnErrorMessage,
        OnId,
        OnEnd
    };

    const ProcedureDeclaration* _procedure;

    Composer*    _r;
    Decomposer** _argv;
    unsigned     _argc;
    Decomposer*  _arg;
    unsigned     _argn;

    Utf8Codec        _utf8;
    TextOStream      _tos;
    Json::JsonWriter _writer;
    Formatter        _formatter;

    TextIStream      _tis;
    Json::JsonReader _reader;
    State            _state;

    Fault  _fault;
    int    _faultCode;
    std::string _faultMessage;
    bool   _isFault;

    long long    _id;
    long long    _nextId;

    Pt::varint_t _r1;
    Pt::varint_t _r2;
};

} // namespace JsonRpc

} // namespace Pt

#endif // PT_JSONRPC_CLIENT_H
