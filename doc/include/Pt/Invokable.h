/***************************************************************************
 *   Copyright (C) 2005-2007 by Marc Boris Duerner                         *
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
#ifndef Pt_Invokable_h
#define Pt_Invokable_h

#include <Pt/Api.h>
#include <Pt/Void.h>

namespace Pt {

/** @brief Interface for invokable entities

    Invokable is a type which can be "called" via the invoke() member with a
    number of arguments, but does not provide a return value. It serves as
    a base type for other types in the Pt signals/slots framework.

    @ingroup sigslot
*/
template <typename ARGUMENTS>
class Invokable {
    public:
        /** @brief Default Constructor
            Does nothing. Does not throw.
        */
        virtual ~Invokable()
        {}

        /** @brief Invokes the invokable entity with the given arguments

            This class template is partially specialized and the passed
            arguments \a ARGUMENTS must match the template parameters.
        */
        virtual void invoke(ARGUMENTS) const = 0;
};

} // namespace Pt


#endif
