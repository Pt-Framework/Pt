/*
 * Copyright (C) 2009-2013 by Marc Boris Duerner
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
#ifndef Pt_XmlRpc_ClientImpl_h
#define Pt_XmlRpc_ClientImpl_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Fault.h>
#include <Pt/XmlRpc/Formatter.h>
#include <Pt/XmlRpc/Scanner.h>
#include <Pt/Xml/InputSource.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Composer.h>
#include <Pt/Decomposer.h>
#include <Pt/Utf8Codec.h>
#include <Pt/TextStream.h>
#include <string>
#include <sstream>
#include <cstddef>

namespace Pt {

namespace XmlRpc {

class IRemoteProcedure;

class ClientImpl
{
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

    public:
        ClientImpl();

        ~ClientImpl();
        
        void beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

        void endCall();

        void formatRequest(std::ostream& os);

        void beginReply(std::istream& is);

        bool advanceReply();

        void execute();

        void readReply(std::istream& is);

        void cancel();

        const IRemoteProcedure* activeProcedure() const;

    private:
        void advance(const Xml::Node& node);
        
    private:
        State _state;
        Pt::Utf8Codec _utf8;
        TextOStream _ts;
        Xml::BinaryInputSource _bin;
        Xml::XmlReader _reader;
        Formatter _formatter;
        Scanner _scanner;
        IDecomposer** _argv;
        unsigned _argc;
        IRemoteProcedure* _method;
        Fault _fault;
        Composer<Fault> _fh;
};

}

}

#endif
