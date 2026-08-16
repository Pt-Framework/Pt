/*
  Copyright (C) 2005 by Marc Boris Duerner

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

#ifndef Pt_Callable_h
#define Pt_Callable_h

#include <Pt/Api.h>
#include <Pt/Invokable.h>

namespace Pt {

/** @brief An interface for all callable entities.

    The %Callable interface extends the %Invokable interface to handle
    return values. The variadic template argument list determines the
    callable signature.

    @ingroup sigslot
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
