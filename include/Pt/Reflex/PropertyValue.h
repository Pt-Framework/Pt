/***************************************************************************
 *   Copyright (C) 2007 by Marc Boris Dürner                               *
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
#ifndef Pt_Reflex_PropertyValue_h
#define Pt_Reflex_PropertyValue_h

#include <Pt/Api.h>
#include <Pt/Any.h>
#include <Pt/Reflex/Api.h>


namespace Pt {

namespace Reflex {

/** @brief Value type registerable as property
    @ingroup Reflection
*/
template <typename T>
class PropertyValue
{
    public:
        PropertyValue( const T& value = T() )
        : _value(value)
        {}

        const T& get() const
        { 
            const Any::Value* v = _value.value();
            const Any::BasicValue<T>* b = static_cast< const Any::BasicValue<T>* >(v);
            return b->value();
        }

        T& get()
        {
            Any::Value* v = _value.value();
            Any::BasicValue<T>* b = static_cast<Any::BasicValue<T>* >(v);
            return b->value();
        }

        void set( const T& value )
        { _value = value; }

        const Pt::Any& value() const
        { return _value; }

    protected:
        Pt::Any _value;
};

} // namespace Reflex

} // namespace Reflex

#endif
