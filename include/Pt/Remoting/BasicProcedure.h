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

#ifndef PT_REMOTING_BASICPROCEDURE_H
#define PT_REMOTING_BASICPROCEDURE_H

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


/** @brief Generic service procedure.
*/
template <typename R, typename... As>
class BasicProcedure : public ServiceProcedure
{
    public:
        BasicProcedure(const Callable<R, As...>& cb, Responder& resp)
        : ServiceProcedure(resp)
        , _cb(cb.clone())
        , _r(&resp.context())
        , _argv(&resp.context())
        {
            _argv.fill(0, _args);
        }

        ~BasicProcedure()
        {
            delete _cb;
        }

    protected:
        Composer** onBeginArgs()
        {
            _argv.begin();
            return _args;
        }

        virtual void onBeginCall(System::EventLoop&)
        {
            this->setReady();
        }

        Decomposer* onEndCall()
        {
            _rv = callWith(_argv);
            _r.begin(_rv, "");
            return &_r;
        }

    protected:
        template <typename... Vs>
        R callWith(Arguments<>& /*args*/, Vs&... vs)
        {
            return _cb->call(vs...);
        }


        template <typename T, typename... Ts, typename... Vs>
        R callWith(Arguments<T, Ts...>& args, Vs&... vs)
        {
            return callWith(args.tail(), vs..., args.head());
        }

    private:
        template <typename T>
        using ValueType = typename TypeTraits<T>::Value;

        Callable<R, As...>* _cb;

        ValueType<R> _rv;
        BasicDecomposer< ValueType<R> > _r;

        Arguments<ValueType<As>...> _argv;
        Composer* _args[sizeof...(As) + 1];
};


template <typename R, typename... As>
class BasicProcedureDef : public ServiceProcedureDef
{
    public:
        BasicProcedureDef(const Callable<R, As...>& cb)
        : _cb(0)
        {
            _cb = cb.clone();
        }

        ~BasicProcedureDef()
        {
            delete _cb;
        }

    protected:
        virtual ServiceProcedure* onCreateProcedure(Responder& resp) const
        {
            return new BasicProcedure<R, As...>(*_cb, resp);
        }

    private:
        Callable<R, As...>* _cb;
};

} // namespace Remoting

} // namespace Pt

#endif // PT_REMOTING_BASICPROCEDURE_H
