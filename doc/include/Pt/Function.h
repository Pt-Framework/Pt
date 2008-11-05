/***************************************************************************
 *   Copyright (C) 2005 by Dr. Marc Boris Duerner                          *
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
#ifndef Pt_Function_h
#define Pt_Function_h

#include <Pt/Callable.h>
#include <Pt/Connectable.h>
#include <Pt/Slot.h>

namespace Pt {

/** @brief Wraps free functions into a generic callable for use with the signals/slots framework
    @ingroup sigslot

    The %Function class wraps free functions in the form of a Callable,
    for use with the signals/slots framework.
*/
template < typename R, typename ARGUMENTS>
class Function : public Callable<R, ARGUMENTS>
{
    public:
        //! @brief The function signature wrapped by this class
        typedef R (*FuncT)(ARGUMENTS);

        //! @brief Construct from function pointer
        Function(FuncT func);

        //! @brief Copy Constructor
        Function(const Function& f);

        // docs inherited
        R operator()(ARGUMENTS args) const;

        // docs inherited
        Function<R, ARGUMENTS>* clone() const;

        //! @brief Returns true if both use the same function pointer
        bool operator==(const Function& rhs) const;

    private:
        //! @internal
        FuncT _funcPtr;
};

} // !namespace Pt


#endif
