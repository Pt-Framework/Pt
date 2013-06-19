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
#include <Pt/Http/Service.h>
#include <Pt/System/EventLoop.h>
#include <Pt/System/Mutex.h>
#include <Pt/Decomposer.h>
#include <Pt/Composer.h>
#include <Pt/Void.h>
#include <Pt/TypeTraits.h>
#include <string>
#include <map>

namespace Pt {

namespace XmlRpc {

class Responder
{
    public:
        Responder()
        {}

        virtual ~Responder()
        {}

        SerializationContext& sctx()
        { return _context; }

        void endReply()
        { this->onEndReply(); }

    protected:
        virtual void onEndReply() = 0;

    private:
        SerializationContext _context;
};


class ServiceProcedure
{
    public:
        virtual ~ServiceProcedure()
        {}

        void setReady()
        { _responder->endReply(); }

        virtual IComposer** beginArgs() = 0;

        virtual void beginCall(System::EventLoop& loop) = 0;

        virtual IDecomposer* endCall() = 0;      

    protected:
        ServiceProcedure(Responder& r)
        : _responder( &r )
        {}

    private:
        Responder* _responder;
};


class ServiceProcedureDef
{
    public:
        virtual ~ServiceProcedureDef()
        {}

        ServiceProcedure* createProcedure(Responder& ctx) const
        { return this->onCreateProcedure(ctx); }

    protected:
        ServiceProcedureDef()
        {}

        virtual ServiceProcedure* onCreateProcedure(Responder& ctx) const = 0;
};

} // namespace XmlRpc

} // namespace Pt

#include <Pt/XmlRpc/Service.tpp>

namespace Pt {

namespace XmlRpc {

class PT_XMLRPC_API Service : public Http::Service
{
    public:
        Service()
        { }

        virtual ~Service();

        template <typename R>
        void registerFunction(const std::string& name, R (*fn)())
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1>
        void registerFunction(const std::string& name, R (*fn)(A1))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2>
        void registerFunction(const std::string& name, R (*fn)(A1, A2))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        void registerFunction(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R>
        void registerCallable(const std::string& name, const Callable<R>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1>
        void registerCallable(const std::string& name, const Callable<R, A1>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2>
        void registerCallable(const std::string& name, const Callable<R, A1, A2>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        void registerCallable(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, class C>
        void registerMethod(const std::string& name, C& obj, R (C::*method)() )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        void registerMethod(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename CallT, class C>
        void registerAsyncMethod(const std::string& name, C& obj, CallT* (C::*method)(Responder&) )
        {
            ServiceProcedureDef* proc = new AsyncProcedureDef<CallT>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

    protected:
        virtual Http::Responder* onGetResponder(const Http::Request&);

        virtual void onReleaseResponder(Http::Responder* resp);

    public:
        ServiceProcedure* getProcedure(const std::string& name, Responder& resp);

        void releaseProcedure(ServiceProcedure* proc);

        void registerProcedure(const std::string& name, ServiceProcedureDef* proc);

    private:
        typedef std::map<std::string, ServiceProcedureDef*> ProcedureMap;
        ProcedureMap _procedures;
        System::Mutex _mtx;
};

} // namespace XmlRpc

} // namespace Pt

#endif
