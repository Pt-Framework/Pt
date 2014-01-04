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

#ifndef Pt_XmlRpc_ServiceDefinition_h
#define Pt_XmlRpc_ServiceDefinition_h

#include <Pt/XmlRpc/Api.h>
#include <Pt/XmlRpc/Responder.h>
#include <Pt/XmlRpc/ServiceProcedure.h>
#include <Pt/System/Mutex.h>
#include <Pt/NonCopyable.h>
#include <Pt/Types.h>
#include <string>
#include <map>

namespace Pt {

namespace XmlRpc {

class PT_XMLRPC_API ServiceDefinition : private NonCopyable
{
    public:
        ServiceDefinition();

        virtual ~ServiceDefinition();

        ServiceProcedure* getProcedure(const std::string& name, Responder& resp);

        void releaseProcedure(ServiceProcedure* proc);

        template <typename R>
        void registerProcedure(const std::string& name, R (*fn)())
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1>
        void registerProcedure(const std::string& name, R (*fn)(A1))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2>
        void registerProcedure(const std::string& name, R (*fn)(A1, A2))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3>
        void registerProcedure(const std::string& name, R (*fn)(A1, A2, A3))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4>
        void registerProcedure(const std::string& name, R (*fn)(A1, A2, A3, A4))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerProcedure(const std::string& name, R (*fn)(A1, A2, A3, A4, A5))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        void registerProcedure(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        void registerProcedure(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        void registerProcedure(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        void registerProcedure(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        void registerProcedure(const std::string& name, R (*fn)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(Pt::callable(fn));
            this->registerProcedure(name, proc);
        }

        template <typename R, class C>
        void registerProcedure(const std::string& name, C& obj, R (C::*method)() )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1>
        void registerProcedure(const std::string& name, C& obj, R (C::*method)(A1) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2>
        void registerProcedure(const std::string& name, C& obj, R (C::*method)(A1, A2) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3>
        void registerProcedure(const std::string& name, C& obj, R (C::*method)(A1, A2, A3) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4>
        void registerProcedure(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerProcedure(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        void registerProcedure(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        void registerProcedure(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        void registerProcedure(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        void registerProcedure(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R, class C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        void registerProcedure(const std::string& name, C& obj, R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename R>
        void registerProcedure(const std::string& name, const Callable<R>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1>
        void registerProcedure(const std::string& name, const Callable<R, A1>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2>
        void registerProcedure(const std::string& name, const Callable<R, A1, A2>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3>
        void registerProcedure(const std::string& name, const Callable<R, A1, A2, A3>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4>
        void registerProcedure(const std::string& name, const Callable<R, A1, A2, A3, A4>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
        void registerProcedure(const std::string& name, const Callable<R, A1, A2, A3, A4, A5>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
        void registerProcedure(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
        void registerProcedure(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
        void registerProcedure(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
        void registerProcedure(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9, typename A10>
        void registerProcedure(const std::string& name, const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(cb);
            this->registerProcedure(name, proc);
        }

        template <typename A, class C>
        void registerActiveProcedure(const std::string& name, A* (*fn)(Responder&) )
        {
            ServiceProcedureDef* proc = new ActiveProcedureDef<A>( callable(fn) );
            this->registerProcedure(name, proc);
        }

        template <typename A, class C>
        void registerActiveProcedure(const std::string& name, C& obj, A* (C::*method)(Responder&) )
        {
            ServiceProcedureDef* proc = new ActiveProcedureDef<A>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        template <typename A, class C>
        void registerActiveProcedure(const std::string& name, C& obj, A* (C::*method)(Responder&) const )
        {
            ServiceProcedureDef* proc = new ActiveProcedureDef<A>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

    protected:
        void registerProcedure(const std::string& name, ServiceProcedureDef* proc);

    private:
        typedef std::map<std::string, ServiceProcedureDef*> ProcedureMap;
        ProcedureMap _procedures;
        System::Mutex _mtx;
        Pt::varint_t _r1;
        Pt::varint_t _r2;
};

} // namespace XmlRpc

} // namespace Pt

#endif // Pt_XmlRpc_ServiceDefinition_h
