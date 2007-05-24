/***************************************************************************
 *   Copyright (C) 2007 by Marc Boris Drner                               *
 *   Copyright (C) 2007 by Laurentiu-Gheorghe Crisan                       *
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
#ifndef Pt_IProperty_h
#define Pt_IProperty_h

#include <Pt/Api.h>
#include <Pt/Any.h>
#include <Pt/Exception.h>
#include <Pt/Signal.h>


namespace Pt {

/** @brief Property interface
    @ingroup Reflection
*/
class PT_API IProperty
{
    public:
        IProperty()
        {}

        virtual ~IProperty()
        {}

        virtual Pt::Any value()
        { throw std::logic_error("Property is not readable" + PT_SOURCEINFO); }

        // Set value and notify all listeners
        virtual void setValue(const Pt::Any& value)
        { throw std::logic_error("Property is not writable" + PT_SOURCEINFO); }

        Signal<> valueChanged;
};

} // namespace Pt

#endif
