/*
 * Copyright (C) 2026 by Dr. Marc Boris Duerner
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

#ifndef PT_LAMBDA_H
#define PT_LAMBDA_H

#include <Pt/Callable.h>
#include <Pt/Slot.h>
#include <Pt/Connectable.h>
#include <Pt/TypeTraits.h>
#include <utility>

namespace Pt {

/** @internal Lambda callable.
*/
template <typename L, typename R, typename... A>
class Lambda : public Callable<R, A...>
{
    public:
        template <typename T>
        explicit Lambda(T&& lambda)
        : _lambda(std::forward<T>(lambda))
        { }

        R operator()(A... args) const
        {
            return _lambda(std::forward<A>(args)...);
        }

        Lambda* clone() const
        {
            return new Lambda(*this);
        }

    private:
        mutable L _lambda;
};

/** @internal Lambda slot.
*/
template <typename L, typename R, typename... A>
class LambdaSlot : public BasicSlot<R, A...>
{
    public:
        typedef Lambda<L, R, A...> LambdaT;

    public:
        template <typename T>
        LambdaSlot(T&& lambda, Connectable* context = 0)
        : _lambda(std::forward<T>(lambda))
        , _context(context)
        { }

        virtual const void* callable() const
        {
            return &_lambda;
        }

        Slot* clone() const
        {
            return new LambdaSlot(*this);
        }

        virtual void onConnect(const Connection& connection)
        {
            if(_context && connection.sender() != _context)
            {
                _context->onConnectionOpen(connection);
            }
        }

        virtual void onDisconnect(const Connection& connection)
        {
            if(_context && connection.sender() != _context)
            {
                _context->onConnectionClose(connection);
            }
        }

        virtual bool equals(const Slot&) const
        {
            return false;
        }

    private:
        LambdaT _lambda;
        Connectable* _context;
};

/** @internal Maps to slot type.
*/
template <typename L,
          class C = typename TypeTraits<L>::Value,
          typename M = decltype( &C::operator() )>
struct LambdaSlotTraits;


template <typename L, class C, typename R, typename... As>
struct LambdaSlotTraits<L, C, R (C::*)(As...)>
{
    typedef LambdaSlot<C, R, As...> Slot;
};


template <typename L, class C, typename R, typename... As>
struct LambdaSlotTraits<L, C, R (C::*)(As...) const>
{
    typedef LambdaSlot<C, R, As...> Slot;
};

/** @internal Creates a slot from a lambda.
*/
template <typename L,
          typename SlotT = typename LambdaSlotTraits<L>::Slot>
SlotT slot(L&& lambda)
{
    return SlotT( std::forward<L>(lambda) );
}

/** @internal Creates a slot from a lambda and context object.
*/
template <typename L,
          typename SlotT = typename LambdaSlotTraits<L>::Slot>
SlotT slot(Connectable& context, L&& lambda)
{
    return SlotT( std::forward<L>(lambda), &context );
}

/** @internal Creates a slot from a lambda.
*/
template <typename R, typename... A, typename L>
LambdaSlot<typename TypeTraits<L>::Value, R, A...> slot(L&& lambda)
{
    typedef typename TypeTraits<L>::Value LambdaT;
    return LambdaSlot<LambdaT, R, A...>( std::forward<L>(lambda) );
}

/** @internal Creates a slot from a lambda and context object.
*/
template <typename R, typename... A, typename L>
LambdaSlot<typename TypeTraits<L>::Value, R, A...> slot(Connectable& context, L&& lambda)
{
    typedef typename TypeTraits<L>::Value LambdaT;
    return LambdaSlot<LambdaT, R, A...>( std::forward<L>(lambda), &context );
}

} // namespace Pt

#endif // PT_LAMBDA_H