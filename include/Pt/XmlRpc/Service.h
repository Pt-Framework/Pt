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
#include <Pt/XmlRpc/TypeHandler.h>
#include <Pt/Net/HttpServer.h>
#include <Pt/Void.h>
#include <Pt/Method.h>
#include <string>
#include <map>

namespace Pt {

namespace XmlRpc {

class Formatter;

class ServiceProcedure
{
    public:
        ServiceProcedure()
        {}

        virtual ~ServiceProcedure()
        {}

        virtual ITypeHandler** beginCall() = 0;

        virtual ITypeHandler* endCall() = 0;
};


template < typename R,
           class C,
           typename A1,
           typename A2>
class BasicServiceProcedure : public Method<R, C, A1, A2>
                            , public ServiceProcedure
{
    public:
        typedef R (C::*MemFuncT)(A1, A2);

    public:
        BasicServiceProcedure(C& object, MemFuncT ptr)
        : Method<R, C, A1, A2>(object, ptr)
        , ServiceProcedure()
        {
            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = 0;
        }

        ITypeHandler** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            return _args;
        }

        ITypeHandler* endCall()
        {
            _rv = Method<R, C, A1, A2>::call(_v1, _v2);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<R>::Value RV;

        RV _rv;
        V1 _v1;
        V2 _v2;
        ITypeHandler* _args[3];
        TypeHandler<V1> _a1;
        TypeHandler<V2> _a2;
        TypeHandler<RV> _r;
};


class PT_XMLRPC_API Service : public Net::HttpService
{
    public:
        Service();

        virtual ~Service();

        // TODO cache service procedures and clone on demand
        ServiceProcedure* procedure(const std::string& name);

        template <typename R, class C, typename A1, typename A2>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R, C, A1, A2>(obj, method);
            this->registerProcedure(name, proc);
        }

        virtual Net::HttpResponder* createResponder(const Net::HttpRequest&);

        virtual void releaseResponder(Net::HttpResponder* resp);

    protected:
        void registerProcedure(const std::string& name, ServiceProcedure* proc);

    private:
        typedef std::map<std::string, ServiceProcedure*> ProcedureMap;
        ProcedureMap _procedures;
};

}

}

#endif
