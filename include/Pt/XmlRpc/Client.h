/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
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
#include <Pt/XmlRpc/Formatter.h>
#include <Pt/XmlRpc/ResponseHandler.h>
#include <Pt/Net/HttpClient.h>
#include <string>
#include <cstddef>

namespace Pt {

namespace XmlRpc {

template <typename R,
          typename A1,
          typename A2 >
class PT_XMLRPC_API RemoteMethod
{
    public:
        RemoteMethod(std::ostream& os, const std::string& name)
        : _name(name)
        , _os(&os)
        {}

        virtual ~RemoteMethod()
        {}

        void begin(const A1& a1, const A2& a2)
        {
            _formatter.begin(*_os, _name);
            _formatter.beginParam();
            _a1handler.begin(a1);
            _a1handler.decompose(_formatter);
            _formatter.finishParam();

            _formatter.beginParam();
            _a2handler.begin(a2);
            _a2handler.decompose(_formatter);
            _formatter.finishParam();

            _formatter.finish();
        }

        R result()
        { }

    private:
        RequestFormatter _formatter;
        std::string _name;
        std::ostream* _os;
        TypeHandler<R> _rhandler;
        TypeHandler<A1> _a1handler;
        TypeHandler<A1> _a2handler;
};


class PT_XMLRPC_API Client
{
    public:
        Client();

        virtual ~Client();
};


class PT_XMLRPC_API RemoteService
{
    public:
        RemoteService(const std::string& addr, unsigned short port, const std::string& url);

        virtual ~RemoteService();

    private:
        TextIStream _ts;
        Xml::XmlReader _reader;
        Net::HttpClient _client;
};

/*

class RemoteService
{
    public:
        virtual std::ostream& out() = 0;
        virtual std::istream& in() = 0;
};

int main()
{
    Client client("Calc");

    RemoteMethod<int, int, int> multiply(client, "multiply");

    HttpRemoteService service("tpfd1", 8080, "/XmlRpc");
    RawSocketRemoteService service2("tpfp1", 3456);

    RemoteMethod<int, int, int> multiply(service, "multiply");

    multiply.begin(2, 3);
    // ... wait
    int r = multiply.result();










    RemoteMethod<int, int, int> multiplyTpfd1(tpfd1, "multiply");
    RemoteMethod<int, int, int> multiplyTpfd2(tpfd2, "multiply");

    int j1 = multiplyTpfd1(2, 3);
    int j2 = multiplyTpfd2(2, 3);

    multiplyTpfd1.begin(2, 3);
    multiplyTpfd2.begin(2, 3);

    int j1 = multiplyTpfd1.result();
    int j2 = multiplyTpfd2.result();



    int j = multiply(2, 3);

    multiply.begin(2, 3);
    // ... wait
    int r = multiply.result();
}
*/

}

}

#endif
