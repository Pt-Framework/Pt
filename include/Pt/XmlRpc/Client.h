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
#include <Pt/Xml/InputSource.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Composer.h>
#include <Pt/Decomposer.h>
#include <Pt/Utf8Codec.h>
#include <Pt/TextStream.h>
#include <Pt/NonCopyable.h>
#include <Pt/SerializationContext.h>
#include <string>

namespace Pt {

namespace XmlRpc {

class RemoteCall;

class PT_XMLRPC_API Client : private NonCopyable
{
    public:
        Client();

        virtual ~Client();

        SerializationContext& context();

        void beginCall(IComposer& r, RemoteCall& call, IDecomposer** argv, unsigned argc);

        void endCall();

        void call(IComposer& r, RemoteCall& call, IDecomposer** argv, unsigned argc);

        void cancel();

        const RemoteCall* activeProcedure() const;

        bool isFailed() const;

    protected:       
        virtual void onInvoke() = 0;

        virtual void onCall() = 0;

        virtual void onCancel() = 0;

        virtual void onError() = 0;

    protected:
        void beginMessage(std::ostream& os);
        
        bool advanceMessage();
        
        void finishMessage();
        
        void beginResult(std::istream& is);

        bool parseResult();

        void finishResult();

        void processResult(std::istream& is);

        void setFault(int rc, const char* msg);

        void setError(bool f = true);

    private:
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

        SerializationContext _ctx;
        RemoteCall* _method;
        
        Pt::Utf8Codec _utf8;
        TextOStream _ts;
        IDecomposer** _argv;
        unsigned _argc;
        IDecomposer* _arg;
        unsigned _argn;

        Xml::BinaryInputSource _bin;
        Xml::XmlReader _reader;
        State _state;
        
        Formatter _formatter;
        Fault _fault;
        Composer<Fault> _fh;
        bool _error;
        bool _isFault;
        void* _r1;
        void* _r2;
};

} // namespace XmlRpc

} // namespace Pt

#endif
