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

#ifndef Pt_XmlRpc_Client_h
#define Pt_XmlRpc_Client_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Fault.h>
#include <Pt/XmlRpc/Formatter.h>
#include <Pt/Remoting/Client.h>
#include <Pt/Xml/InputSource.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Composer.h>
#include <Pt/Decomposer.h>
#include <Pt/Utf8Codec.h>
#include <Pt/TextStream.h>
#include <Pt/NonCopyable.h>
#include <Pt/Types.h>
#include <string>

namespace Pt {

namespace XmlRpc {

/** @brief A client for remote procedure calls.
*/
class PT_XMLRPC_API Client : public Pt::Remoting::Client
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

    protected:
        virtual void onBeginCall(Composer& r, Pt::Remoting::RemoteCall& method, Decomposer** argv, unsigned argc);

        virtual void onEndCall();

        virtual void onCall(Composer& r, Pt::Remoting::RemoteCall& method, Decomposer** argv, unsigned argc);

        virtual void onCancel();

        /** @brief An asynchronous remote procedure is invoked.

            Derived Clients implement this method to format and send a
            message to the service.
        */
        virtual void onBeginInvoke() = 0;

        virtual void onEndInvoke() = 0;

        /** @brief A synchronous remote procedure is called.

            Derived Clients implement this method to format and send a
            message to the service and receive and parse the result.
        */
        virtual void onInvoke() = 0;

    protected:
        /** @brief Formats the XML-RPC message.

            This method is used by derived Clients in onInvoke() and onCall()
            to begin formatting a XML-RPC message to a std::ostream.
        */
        void beginMessage(std::ostream& os);
        
        /** @brief Formats the XML-RPC message.

            This method is used by derived Clients in onInvoke() and onCall()
            to format a XML-RPC message. Each call generates a chunk of the
            message and returns true if the message is complete.
        */
        bool advanceMessage();
        
        /** @brief Formats the XML-RPC message.

            This method is used by derived Clients in onInvoke() and onCall()
            to format the end of a XML-RPC message. It is called after 
            advanceMessage() returns true.
        */
        void finishMessage();
        
        /** @brief Parses the XML-RPC result.

            This method is used by derived Clients to begin parsing a XML-RPC
            result from a std::istream.
        */
        void beginResult(std::istream& is);

        /** @brief Parses the XML-RPC result.

            This method is used by derived Clients to parse a XML-RPC result
            Each call consumes the available data from the std::istream set
            with beginResult() and returns true if the result is complete.
        */
        bool parseResult();

        /** @brief Parses the XML-RPC result.

            This method is used by derived Clients after the XML-RPC result 
            has been parsed by parseResult(). The current RemoteProcedure will
            receive completion notification to process the result.
        */
        //void finishResult();

        /** @brief Parses the XML-RPC result.

            This method is used by derived Clients in in onCall() to parse a
            XML-RPC result from a std::istream.
        */
        void processResult(std::istream& is);

        /** @brief Fails the current procedure.

            This method is used by derived Clients before calling finishResult()
            so that the RemoteProcedure throws a Fault when the result is 
            processed.
        */
        void setFault(int rc, const char* msg);

    private:
        //! @internal
        bool advance(const Xml::Node& node);

    private:
        enum State
        {
            OnBegin,
            OnMethodResponseBegin,
            OnFaultBegin,
            OnFaultEnd,
            OnFaultResponseEnd,
            OnParamsBegin,
            OnParam,
            OnParamEnd,
            OnParamsEnd,
            OnMethodResponseEnd
        };

        Composer* _r;
        Decomposer** _argv;
        unsigned _argc;
        Decomposer* _arg;
        unsigned _argn;

        Pt::Utf8Codec _utf8;
        TextOStream _ts;
        
        Xml::BinaryInputSource _bin;
        Xml::XmlReader _reader;
        State _state;
        
        Formatter _formatter;
        Fault _fault;
        BasicComposer<Fault> _fh;
        bool _isFault;
        Pt::varint_t _r1;
        Pt::varint_t _r2;
};

} // namespace XmlRpc

} // namespace Pt

#endif // Pt_XmlRpc_Client_h
