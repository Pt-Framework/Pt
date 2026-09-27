/* Copyright (C) 2026 Marc Boris Duerner
   SPDX-License-Identifier: LGPL-2.1-or-later WITH mif-exception
*/

#ifndef Pt_Callable_h
#define Pt_Callable_h

#include <Pt/Api.h>
#include <Pt/Invokable.h>

namespace Pt {

/** @brief An interface for all callable entities.

    The %Callable interface extends the %Invokable interface to handle
    return values. The variadic template argument list determines the
    callable signature.

    @ingroup Pt-Signals
*/
template <typename R, typename... As>
class Callable : public Invokable<As...>
{
    public:
        /** @brief Returns a copy of this instance

            A copy of the instance is created with new is returned. Ownership
            is transfered to the caller, who has to delete it.
        */
        virtual Callable* clone() const = 0;

        /** @brief Calls the callable entity and returns its result.

            This is the primary non-virtual entry point used by Delegate
            and other callers that need the return value. All derived
            classes must implement this.
        */
        virtual R call(As... args) const = 0;

        /** @brief Same as call().
        */
        R operator()(As... args) const
        {
            return this->call(args...);
        }

        // inherit docs
        virtual void invoke(As... args) const = 0;
};

} // namespace Pt

#endif
