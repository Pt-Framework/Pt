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

/** @brief Adapts a lambda or function object as a callable.

    The callable is stored by value and must therefore be copyable. For a
    non-generic lambda, the factory derives the %Callable interface from
    operator(). Generic lambdas require an explicitly supplied signature.

    @ingroup Pt-Signals
*/
template <typename L, typename R, typename... A>
class Lambda : public Callable<R, A...>
{
    public:
        /** @brief Constructs from a lambda or function object.
        */
        template <typename T>
        explicit Lambda(T&& lambda)
        : _lambda(std::forward<T>(lambda))
        { }

        // inherit doc
        R call(A... args) const
        {
            return _lambda(std::forward<A>(args)...);
        }

        // inherit doc
        void invoke(A... args) const
        {
            _lambda(std::forward<A>(args)...);
        }

        // inherit doc
        Lambda* clone() const
        {
            return new Lambda(*this);
        }

    private:
        mutable L _lambda;
};

/** @brief Adapts a %Lambda for use as a slot.

    Lambda slots cannot be compared because captured lambda values have no
    general equality operation. Retain and close the returned %Connection to
    disconnect a slot, or bind it to a %Connectable context.

    @ingroup Pt-Signals
*/
template <typename L, typename R, typename... A>
class LambdaSlot : public BasicSlot<R, A...>
{
    public:
        typedef Lambda<L, R, A...> LambdaT;

    public:
        /** @brief Constructs from a lambda and optional lifetime context.
        */
        template <typename T>
        LambdaSlot(T&& lambda, Connectable* context = 0)
        : _lambda(std::forward<T>(lambda))
        , _context(context)
        { }

        // inherit doc
        virtual const Callback* callable() const
        {
            return &_lambda;
        }

        // inherit doc
        Slot* clone() const
        {
            return new LambdaSlot(*this);
        }

        // inherit doc
        virtual void onConnect(const Connection& connection)
        {
            if(_context && connection.sender() != _context)
            {
                _context->onConnectionOpen(connection);
            }
        }

        // inherit doc
        virtual void onDisconnect(const Connection& connection)
        {
            if(_context && connection.sender() != _context)
            {
                _context->onConnectionClose(connection);
            }
        }

        // inherit doc
        virtual bool equals(const Slot&) const
        {
            return false;
        }

    private:
        LambdaT _lambda;
        Connectable* _context;
};

/** @internal Maps a lambda to slot type.
*/
template <typename L,
          class C = typename TypeTraits<L>::Value,
          typename M = decltype( &C::operator() )>
struct LambdaSlotTraits;

/** @internal Maps a lambda to slot type.
*/
template <typename L, class C, typename R, typename... As>
struct LambdaSlotTraits<L, C, R (C::*)(As...)>
{
    typedef LambdaSlot<C, R, As...> Slot;
};

/** @internal Maps a lambda to slot type.
*/
template <typename L, class C, typename R, typename... As>
struct LambdaSlotTraits<L, C, R (C::*)(As...) const>
{
    typedef LambdaSlot<C, R, As...> Slot;
};

/** @brief Returns a deduced slot object for the given lambda.

    @related LambdaSlot
    @related Slot
*/
template <typename L,
          typename SlotT = typename LambdaSlotTraits<L>::Slot>
SlotT slot(L&& lambda)
{
    return SlotT( std::forward<L>(lambda) );
}

/** @brief Returns a deduced slot object for the given lambda.

    @related LambdaSlot
    @related Slot
*/
template <typename L,
          typename SlotT = typename LambdaSlotTraits<L>::Slot>
SlotT slot(Connectable& context, L&& lambda)
{
    return SlotT( std::forward<L>(lambda), &context );
}

/** @brief Returns a slot object for the given lambda.

    @related LambdaSlot
    @related Slot
*/
template <typename R, typename... A, typename T,
          typename L = typename TypeTraits<T>::Value>
LambdaSlot<L, R, A...> slot(T&& lambda)
{
    return LambdaSlot<L, R, A...>( std::forward<T>(lambda) );
}

/** @brief Returns a slot object for the given lambda.

    @related LambdaSlot
    @related Slot
*/
template <typename R, typename... A, typename T,
          typename L = typename TypeTraits<T>::Value>
LambdaSlot<L, R, A...> slot(Connectable& context, T&& lambda)
{
    typedef typename TypeTraits<L>::Value LambdaT;
    return LambdaSlot<L, R, A...>( std::forward<T>(lambda), &context );
}

} // namespace Pt

#endif // PT_LAMBDA_H
