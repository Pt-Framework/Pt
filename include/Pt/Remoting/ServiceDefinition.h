/*
  Copyright (C) 2009-2026 by Marc Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the:
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301 USA
*/

#ifndef PT_REMOTING_SERVICEDEFINITION_H
#define PT_REMOTING_SERVICEDEFINITION_H

#include <Pt/Remoting/Api.h>
#include <Pt/Remoting/ServiceProcedure.h>
#include <Pt/System/Mutex.h>
#include <Pt/NonCopyable.h>
#include <Pt/Types.h>
#include <string>
#include <map>

namespace Pt {

namespace Remoting {

class Responder;

/** @brief Remote service definition.

    The remote service provides a number of service procedures to its clients.
    Regular C++ functions or methods can be registered as service procedures,
    but in many cases a derived service simply registers member functions. The
    RPC requests are then dispatched according to the name of the registered
    procedures. The arguments and return values must be serializable, for
    example by supporting serialization to a SerializationInfo. The service
    supports synchronous and asynchronous procedures, where the latter ones
    are factories for %ActiveProcedure objects.
*/
class PT_REMOTING_API ServiceDefinition : private NonCopyable
{
    public:
        /** @brief Constructor.
        */
        ServiceDefinition();

        /** @brief Destructor.
        */
        virtual ~ServiceDefinition();

        //! @internal
        ServiceProcedure* getProcedure(const std::string& name, Responder& resp);

        //! @internal
        void releaseProcedure(ServiceProcedure* proc);

        /** @brief Registers a function as a synchronous procedure.

            Registers the function \a func as a service procedure named \a
            name. Functions with up to ten arguments are supported. The
            template parameter ARGS expands to the argument types of the
            funtcions signature.
        */
        template <typename R, typename... As>
        void registerProcedure(const std::string& name, R (*func)(As...))
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, As...>(Pt::callable(func));
            this->registerProcedure(name, proc);
        }

        /** @brief Registers a member function as a synchronous procedure.

            Registers the method \a mth of the object instance \a obj as
            a service procedure named \a name. Methods with up to ten
            arguments are supported. The template parameter ARGS expands to
            the argument types of the methods signature.
        */
        template <typename R, class C, typename... As>
        void registerProcedure(const std::string& name, C& obj, R (C::*mth)(As...) )
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, As...>( callable(obj, mth) );
            this->registerProcedure(name, proc);
        }

        /** @brief Registers a generic callable as a synchronous procedure.

            Registers the callable \a cb as a service procedure named \a
            name. Callables with up to ten arguments are supported. The
            template parameter ARGS expands to the argument types of the
            signature.
        */
        template <typename R, typename... As>
        void registerProcedure(const std::string& name, const Callable<R, As...>& cb)
        {
            ServiceProcedureDef* proc = new BasicProcedureDef<R, As...>(cb);
            this->registerProcedure(name, proc);
        }

        /** @brief Registers a function as an asynchronous procedure.

            Registers the function \a func as an asynchronous service procedure
            named \a name. Asynchronous service procedures return an instance
            of an ActiveProcedure created with default new. The Responder
            needs to be passed to the constructed %ActiveProcedure.
        */
        template <typename A, class C>
        void registerActiveProcedure(const std::string& name, A* (*fn)(Responder&) )
        {
            ServiceProcedureDef* proc = new ActiveProcedureDef<A>( callable(fn) );
            this->registerProcedure(name, proc);
        }

        /** @brief Registers a member function as an asynchronous procedure.

            Registers the method \a mth of the object instance \a obj as an
            asynchronous service procedure named \a name. Asynchronous service
            procedures return an instance of an ActiveProcedure created with
            default new. The Responder needs to be passed to the constructed
            %ActiveProcedure.
        */
        template <typename A, class C>
        void registerActiveProcedure(const std::string& name, C& obj, A* (C::*method)(Responder&) )
        {
            ServiceProcedureDef* proc = new ActiveProcedureDef<A>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

        /** @brief Registers a const member function as an asynchronous procedure.

            Registers the method \a mth of the object instance \a obj as an
            asynchronous service procedure named \a name. Asynchronous service
            procedures return an instance of an ActiveProcedure created with
            default new. The Responder needs to be passed to the constructed
            %ActiveProcedure.
        */
        template <typename A, class C>
        void registerActiveProcedure(const std::string& name, C& obj, A* (C::*method)(Responder&) const )
        {
            ServiceProcedureDef* proc = new ActiveProcedureDef<A>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

    protected:
        void registerProcedure(const std::string& name, ServiceProcedureDef* proc);

        System::Mutex& mutex();

        System::Mutex& mutex() const;

    private:
        typedef std::map<std::string, ServiceProcedureDef*> ProcedureMap;
        ProcedureMap _procedures;
        mutable System::Mutex _mtx;
        Pt::varint_t _r1;
        Pt::varint_t _r2;
};

} // namespace Remoting

} // namespace Pt

#endif // PT_REMOTING_SERVICEDEFINITION_H
