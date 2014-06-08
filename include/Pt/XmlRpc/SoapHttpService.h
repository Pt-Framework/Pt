/*
 * Copyright (C) 2014 by Dr. Marc Boris Duerner
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

#ifndef Pt_XmlRpc_SoapHttpService_h
#define Pt_XmlRpc_SoapHttpService_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/SoapServiceDefinition.h>
#include <Pt/XmlRpc/SoapFormatter.h>
#include <Pt/Http/Service.h>
#include <Pt/Http/Responder.h>
#include <Pt/Types.h>
#include <Pt/XmlRpc/Fault.h>
#include <Pt/Xml/InputSource.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/System/EventLoop.h>
#include <Pt/SerializationContext.h>
#include <Pt/TextStream.h>
#include <Pt/NonCopyable.h>
#include <Pt/Types.h>

namespace Pt {

namespace XmlRpc {

/** @brief Dispatches requests to a service procedure.
*/
class PT_XMLRPC_API SoapResponderBase : public ResponderBase
{
    public:
        /** @brief Construct with Service.
        */
        explicit SoapResponderBase(ServiceDefinition& serviceDef);

        /** @brief Destructor.
        */
        virtual ~SoapResponderBase();

        //! @internal called by SericeProcedure
        virtual void beginResult()
        { this->onReady(); }

        /** @brief Resets to initial state.
        */
        void cancel();

    protected:
        /** @brief Gets the service procedure.
        */
        Pt::Composer** setProcedure(const std::string& name);

        void beginCall(System::EventLoop& loop);

        Pt::Decomposer* endCall();

        Composer* arg() const
        {           
            return *_args; 
        }

        void nextArg()
        { 
            ++_args;
        }

    protected:
        /** @brief Cancels all operations.

            Derived responders implement this method to cancel all operations.
        */
        virtual void onCancel() = 0;

        virtual void onReady() = 0;

    private:
        ServiceDefinition* _serviceDef;
        ServiceProcedure* _proc;
        Composer** _args;
};


/** @brief Dispatches requests to a service procedure.
*/
class PT_XMLRPC_API SoapResponder : public SoapResponderBase
{
    public:
        /** @brief Construct with Service declaration and definition.
        */
        SoapResponder(const SoapServiceDeclaration& decl, ServiceDefinition& def);

        /** @brief Destructor.
        */
        virtual ~SoapResponder();

    protected:
        // inheritdoc
        virtual void onReady();

        // inheritdoc
        virtual void onCancel();

        /** @brief The service procedure has failed.

            Derived responders implement this method to format and send the
            XML-RPC fault result. It is called when the service procedure has
            failed. Use beginResult(), advanceResult() and finishResult()
            to format the XML-RPC result.
        */
        virtual void onFault(const Fault& fault) = 0;

        /** @brief The service procedure has finished.

            Derived responders implement this method to format and send the
            XML-RPC result. It is called when the service procedure has
            finished. Use beginResult(), advanceResult() and finishResult()
            to format the XML-RPC result.
        */
        virtual void onResult() = 0;

    protected:
        /** @brief Parses the XML-RPC message.

            This method is used by derived responders to begin parsing a
            XML-RPC message from a std::istream.
        */
        void beginMessage(std::istream& is);

        /** @brief Parses the XML-RPC message.

            This method is used by derived responders to parse a XML-RPC
            message. Each call consumes the available data from the 
            std::istream set with beginMessage() and returns true if no more
            message data needs to be parsed, either because the message is
            complete or an error occured.
        */
        bool parseMessage();

        /** @brief Parses the XML-RPC message.

            This method is used by derived responders after the XML-RPC message 
            has been parsed by parseMessage(). This will execute the service
            procedure.
        */
        void finishMessage(System::EventLoop& loop);

        /** @brief Formats the XML-RPC result.

            This method is used by derived responders in onResult() and onError()
            to begin formatting a XML-RPC result to a std::ostream.
        */
        void beginResult(std::ostream& os);

        void beginFault(std::ostream& os, const Fault& fault);

        /** @brief Formats the XML-RPC message.

            This method is used by derived responders in onResult() and onError()
            to format a XML-RPC result. Each call generates a chunk of the
            result and returns true if the message is complete.
        */
        bool advanceResult();

        /** @brief Formats the XML-RPC message.

            This method is used by derived responders in onResult() and onError()
            to format the end of a XML-RPC result. It is called after 
            advanceResult() returns true.
        */
        void finishResult();

        /** @brief Fails the service procedure.

            This method is used by derived responders to indicate that the
            service procedure should not be executed, but a fault result 
            be generated instead.
        */
        void setFault(int rc, const char* msg);

        /** @brief Indicates if the procedure has failed.
        */
        bool isFault() const;

    private:
        //! @internal
        bool advance(const Pt::Xml::Node& node);

    private:
        enum State
        {
            OnBegin,
            OnEnvelope,
            OnBody,
            OnMethod,
            OnParam,
            OnMethodEnd,
            OnBodyEnd,
            OnEnvelopeEnd,
        };

        const SoapServiceDeclaration* _serviceDecl;
        const Operation* _op;
        
        Xml::BinaryInputSource _bin;
        Xml::XmlReader _reader;
        Composer* _arg;
        State _state;
        
        Utf8Codec _utf8;
        TextOStream _ts;
        Decomposer* _result;
        
        SoapFormatter _formatter;
        Fault _fault;
        bool _isFault;
        Pt::varint_t _r1;
        Pt::varint_t _r2;
};

class SoapHttpService;

class PT_XMLRPC_API SoapHttpResponder : public Http::Responder
                                      , public SoapResponder
{
    public:
        SoapHttpResponder(SoapHttpService& httpService, const SoapServiceDeclaration& decl, ServiceDefinition& def);

        ~SoapHttpResponder();

    protected:
        // inheritdoc
        void onBeginRequest(Http::Request& request, Http::Reply& reply, System::EventLoop& loop);

        // inheritdoc
        void onReadRequest(Http::Request& request, Http::Reply& reply, System::EventLoop& loop);

        // inheritdoc
        void onBeginReply(const Http::Request& request, Http::Reply& reply, System::EventLoop& loop);

        // inheritdoc
        void onWriteReply(const Http::Request& request, Http::Reply& reply, System::EventLoop& loop);

    protected:
        // inheritdoc
        virtual void onResult();

        // inheritdoc
        virtual void onFault(const Fault& fault);

        // inheritdoc
        virtual void onCancel();

        void advanceSoapReply(Http::Reply& reply);

    private:
         Http::Reply* _reply;
};

class PT_XMLRPC_API SoapHttpService : public Http::Service
{
    public:
        /** @brief Constructor.
        */
        SoapHttpService(SoapServiceDefinition& serviceDef);

        SoapHttpService(const SoapServiceDeclaration& decl, ServiceDefinition& def);

        /** @brief Destructor.
        */
        virtual ~SoapHttpService();

    protected:
        // inheritdoc
        virtual Http::Responder* onGetResponder(const Http::Request&);

        // inheritdoc
        virtual void onReleaseResponder(Http::Responder* resp);

    private:
        const SoapServiceDeclaration* _serviceDecl;
        ServiceDefinition* _serviceDef;
        Pt::varint_t _r1;
        Pt::varint_t _r2;
};

} // namespace XmlRpc

} // namespace Pt

#endif // Pt_XmlRpc_SoapHttpService_h
