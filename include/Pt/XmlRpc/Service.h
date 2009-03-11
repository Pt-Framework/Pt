/*
 * Copyright (C) 2009 by Dr. Marc Boris Duerner
 * Copyright (C) 2009 by Tommi Meakitalo
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
#ifndef Pt_XmlRpc_Service_h
#define Pt_XmlRpc_Service_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Formatter.h>
#include <Pt/XmlRpc/TypeHandler.h>
#include <Pt/XmlRpc/Args.h>
#include <Pt/Void.h>
#include <Pt/Method.h>
#include <string>
#include <map>

namespace Pt {

namespace XmlRpc {

class ServiceProcedure
{
    public:
        ServiceProcedure()
        {}

        virtual ~ServiceProcedure()
        {}

        virtual Args* createArgs() const = 0;

        virtual void exec(std::ostream& ret, const Args& args) = 0;
};


template < typename R,
           class C,
           typename A1 = Pt::Void,
           typename A2 = Pt::Void,
           typename A3 = Pt::Void,
           typename A4 = Pt::Void,
           typename A5 = Pt::Void,
           typename A6 = Pt::Void,
           typename A7 = Pt::Void,
           typename A8 = Pt::Void >
class BasicServiceProcedure : public Method<R, C, A1, A2, A3, A4, A5, A6, A7, A8>
                            , public ServiceProcedure
{
    public:
        typedef R (C::*MemFuncT)(A1, A2, A3, A4, A5, A6, A7, A8);

    public:
        BasicServiceProcedure(C& object, MemFuncT ptr)
        : Method<R, C, A1, A2, A3, A4, A5, A6, A7, A8>(object, ptr)
        , ServiceProcedure()
        {}
};


template < typename R,
           class C,
           typename A1,
           typename A2>
class BasicServiceProcedure<R, C, A1, A2,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void,
                            Pt::Void> : public Method<R, C, A1, A2>
                                      , public ServiceProcedure
{
    public:
        typedef R (C::*MemFuncT)(A1, A2);
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<R>::Value RV;

    public:
        BasicServiceProcedure(C& object, MemFuncT ptr)
        : Method<R, C, A1, A2>(object, ptr)
        , ServiceProcedure()
        {}

        Args* createArgs() const
        {
            return new BasicArgs<V1, V2>();
        }

        void exec(std::ostream& ret, const Args& a)
        {
            const BasicArgs<V1, V2>& args = static_cast<const BasicArgs<V1, V2>& >(a);
            R result = Pt::Method<R, C, A1, A2>::call( args.first(), args.second() );

            TypeHandler<R> builder;
            builder.begin(result);

            ResponseFormatter resp(ret);
            builder.decompose(resp);
            resp.finish();
        }
};


class PT_XMLRPC_API Service //: public ::Responder
{
    public:
        Service();

        virtual ~Service();

        ServiceProcedure* procedure(const std::string& name);

        template <typename R, class C, typename A1, typename A2>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, C, A1, A2>(obj, method);
            this->registerProcedure(name, proc);
        }

    protected:
        void registerProcedure(const std::string& name, ServiceProcedure* proc);

    private:
        typedef std::map<std::string, ServiceProcedure*> ProcedureMap;
        ProcedureMap _procedures;
};


/*

class HttpSocket : public TcpSocket
{
    public:
        HttpSocket(Selector s, HttpServer server)
        {
            _selector.add(*client);
            selector.add(timer);
            connect(client.inputReady, *client, &HttpSocket::onInput);
        }

        void onInput(TcpSocket& server)
        {
            if( readHeader )
            {
                url = server.read();
                caller.set(url);
            }
            else
            {
                size_t n = caller.exec(*this);
                _timer->start(timeout);
                ...

                close();
                delete this;
            }
        }

        void onTimeout()
        {
             //timeut error
             close();
             delete this;
        }

    private:
        Timer _timer;
};


class HttpServer : public TcpServer
{
    public:
        HttpServer(Selector& selector)
        : _selector(selector)
        {
            _selector.add(*this);
            connect(connectionPending, *this, &HttpServer::onConnect)
        }

        void addResponder(const std::string& url, Responder& resp)
        { _responder.insert(url, &resp); }

        void onConnect(TcpServer& server)
        {
            HttpSocket* client = new HttpSocket(_selector, *this);

        }

    private:
        std::map<std::string, Resopnder*> _responder;
        Selector* _selector;
};


int multiply(int a, int b);


int main()
{
    Application app;

    Service service();
    service.registerMethod( multiply );

    HttpServer server( app.loop() );
    server.addResponder("Calc", service);

    app.run();
}

*/

}

}

#endif
