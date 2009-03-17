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
#include <Pt/XmlRpc/Deserializer.h>
#include <Pt/XmlRpc/Serializer.h>
#include <Pt/XmlRpc/RequestHandler.h>
#include <Pt/Xml/XmlReader.h>
#include <Pt/Net/HttpServer.h>
#include <Pt/TextStream.h>
#include <Pt/SerializationInfo.h>
#include <Pt/Void.h>
#include <Pt/Method.h>
#include <string>
#include <map>
#include <vector>

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

        virtual void run(SerializationInfo& result, SerializationInfo* argv, unsigned argc) = 0;

        virtual void exec(std::ostream& ret, const Args& args) = 0;
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

    public:
        BasicServiceProcedure(C& object, MemFuncT ptr)
        : Method<R, C, A1, A2>(object, ptr)
        , ServiceProcedure()
        {
            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = 0;
        }

        Args* createArgs() const
        {
            return new BasicArgs<V1, V2>();
        }

        ITypeHandler** beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            // return null terminated array of type handler pointers
            return _args;
        }

        ITypeHandler* endCall()
        {
            _rv = Method<R, C, A1, A2>::call(_v1, _v2);
            _r.begin(_rv);
            return &_r;
        }

        void run(SerializationInfo& result, SerializationInfo* argv, unsigned argc)
        {
            if(argc != 2)
                throw std::invalid_argument("invalid number of arguments");

            typedef typename TypeTraits<A1>::Value Arg1;
            typedef typename TypeTraits<A2>::Value Arg2;
            typedef typename TypeTraits<R>::Value RValue;

            Arg1 a1 = Arg1();
            argv[0] >>= a1;

            Arg2 a2 = Arg2();
            argv[1] >>= a2;

            RValue r = Method<R, C, A1, A2>::call(a1, a2);
            result <<= r;
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

        virtual Net::HttpResponder* createResponder();

        virtual void releaseResponder(Net::HttpResponder* resp);

    protected:
        void registerProcedure(const std::string& name, ServiceProcedure* proc);

    private:
        typedef std::map<std::string, ServiceProcedure*> ProcedureMap;
        ProcedureMap _procedures;
};


class PT_XMLRPC_API HttpXmlRpcResponder : public Net::HttpResponder
{
    enum State
    {
        OnBegin,
        OnMethodCallBegin,
        OnMethodNameBegin,
        OnMethodName,
        OnMethodNameEnd,
        OnParams,
        OnParam,
        OnParamsEnd,
        OnMethodCallEnd,
    };

    public:
        HttpXmlRpcResponder(Service& service);

        ~HttpXmlRpcResponder();

        std::size_t advance(std::istream& is);

        void finish(std::ostream& os);

        static void formatResult(const Pt::SerializationInfo& si, Formatter& formatter);
    private:
       State _state;
       Pt::TextIStream _ts;
       Pt::Xml::XmlReader _reader;
       Deserializer _deserializer;
       Serializer _serializer;
       Service* _service;
       ServiceProcedure* _proc;
       ITypeHandler** _args;
       ITypeHandler* _result;
};

}

}

#endif
