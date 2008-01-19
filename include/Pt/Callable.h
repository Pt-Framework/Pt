/***************************************************************************
 *   Copyright (C) 2005-2008 by Marc Boris Duerner                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef Pt_Callable_h
#define Pt_Callable_h

#include <Pt/Api.h>
#include <Pt/Invokable.h>

#if PT_DOXYGEN_DOCS

namespace Pt {

/** @brief An interface for all callable entities

    The %Callable interface extends the %Invokable interface to handle
    return values. There are partial specializations of this class
    template for up to ten arguments.
*/
template <typename R, ARGUMENTS>
class Callable : public Invokable<ARGUMENTS>
{
    public:
        /** @brief Returns a copy of this instance

            A copy of the instance is created with new is returned. Ownership
            is transfered to the caller, who has to delete it.
        */
        virtual Callable* clone() const = 0;

        /** @brief Call the callable entity.

            Since this class template is partially specialized, the passed
            arguments \a ARGUMENTS must match the template parameters.
        */
        virtual R operator()(ARGUMENTS) const = 0;

        /** @brief Same as operator().
        */
        R call(ARGUMENTS) const;

        /** @brief Invoke the callable entity.

            Inherited from Invokable. Ignores the return value of the %Callable.
            Since this class template is partially specialized, the passed
            arguments \a ARGUMENTS must match the template parameters.
        */
        void invoke(ARGUMENTS) const;
};

}

#endif

namespace Pt {

#include <Pt/Callable.tpp>

} // namespace Pt

#endif
