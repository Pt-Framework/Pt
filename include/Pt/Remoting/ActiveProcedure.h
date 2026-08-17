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

#ifndef PT_REMOTING_ACTIVEPROCEDURE_H
#define PT_REMOTING_ACTIVEPROCEDURE_H

#include <Pt/Remoting/Api.h>
#include <Pt/Remoting/Arguments.h>
#include <Pt/Remoting/ServiceProcedure.h>
#include <Pt/System/EventLoop.h>
#include <Pt/Decomposer.h>
#include <Pt/Composer.h>
#include <Pt/TypeTraits.h>
#include <cstddef>

namespace Pt {

namespace Remoting {

class Responder;

/** @brief Asynchronous service procedure.

    Instances derived from this class are returned by the asynchronous service
    procedures registered in a Service. The template parameters imply the
    signature of the procedure. The first template parameter is the return
    value type, followed by the argument types, if any. For example, a
    %ActiveProcedure<double, int, int> is a service procedure which returns
    a double and takes two ints as arguments. A %ActiveProcedure<std::string>
    is a service procedure without arguments, only returning a std::string.

    The two methods onInvoke() and onResult() need to be implemented. When a
    RPC request was received and dispatched to an asynchronous procedure,
    onInvoke() is called with the deserialized arguments and an EventLoop to
    execute further asynchronous operations such as I/O. It is the task of
    the implementer to advance progress of the procedure until a result is
    ready. By calling setReady(), the Responder will be notified of the
    completion and call onResult() to get the result and serialize it. Both
    methods may throw exceptions of type Fault to indicate failure.
*/
template <typename R, typename... As>
class ActiveProcedure : public ServiceProcedure
{
    public:
        /** @brief Constructs with Responder.
        */
        ActiveProcedure(Responder& resp)
        : ServiceProcedure(resp)
        , _r(&resp.context())
        , _argv(&resp.context())
        {
            _argv.fill(0, _args);
        }

        /** @brief Destructor.
        */
        virtual ~ActiveProcedure()
        {}

        /** @brief Access argument value by index.
        */
        template <std::size_t I>
        auto& arg()
        {
            return get<I>(_argv);
        }

        /** @brief Access argument value by index.
        */
        template <std::size_t I>
        const auto& arg() const
        {
            return get<I>(_argv);
        }

    protected:
        // inherit docs
        Composer** onBeginArgs()
        {
            _argv.begin();
            return _args;
        }

        // inherit docs
        virtual void onBeginCall(System::EventLoop& loop)
        {
            invokeWith(loop, _argv);
        }

        // inherit docs
        Decomposer* onEndCall()
        {
            const R& r = onResult();
            _r.begin(r, "");
            return &_r;
        }

    protected:
        /** @brief Start an asynchronous procedure.
        */
        virtual void onInvoke(System::EventLoop& loop, const As&... args) = 0;

        /** @brief Return result of the asynchronous procedure.
        */
        virtual const R& onResult() = 0;

    protected:
        template <typename... Vs>
        void invokeWith(System::EventLoop& loop, Arguments<>&, Vs&... vs)
        {
            onInvoke(loop, vs...);
        }

        template <typename T, typename... Ts, typename... Vs>
        void invokeWith(System::EventLoop& loop, Arguments<T, Ts...>& args, Vs&... vs)
        {
            invokeWith(loop, args.tail(), vs..., args.head());
        }

    private:
        template <typename T>
        using ValueType = typename TypeTraits<T>::Value;

        BasicDecomposer< ValueType<R> > _r;
        Arguments<ValueType<As>...> _argv;
        Composer* _args[sizeof...(As) + 1];
};


/** @brief Definition of an asynchronous service procedure.

    @ingroup Pt-Remoting-ActiveProcedure
*/
template <typename CallT>
class ActiveProcedureDef : public ServiceProcedureDef
{
    public:
        /** @brief Constructs with a factory callback.
        */
        ActiveProcedureDef(const Callable<CallT*, Responder&>& cb)
        : _cb(0)
        {
            _cb = cb.clone();
        }

        /** @brief Destructor.
        */
        ~ActiveProcedureDef()
        {
            delete _cb;
        }

    protected:
        virtual ServiceProcedure* onCreateProcedure(Responder& resp) const
        {
            return _cb->call(resp);
        }

    private:
        const Callable<CallT*, Responder&>* _cb;
};

} // namespace Remoting

} // namespace Pt

#endif // PT_REMOTING_ACTIVEPROCEDURE_H
