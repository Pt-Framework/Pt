/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
 *   Copyright (C) 2006 by Marc Boris Duerner                              *
 *   Copyright (C) 2006 by Aloysius Indrayanto                             *
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
#ifndef PT_ATOMICITY_WINDOWS_H
#define PT_ATOMICITY_WINDOWS_H

#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>

namespace Pt {

typedef LONG atomic_t;


inline atomic_t atomicIncrement(volatile atomic_t& value)
{
    return InterlockedIncrement( const_cast<atomic_t*>(&value) );
}


inline atomic_t atomicDecrement(volatile atomic_t& value)
{
    return InterlockedDecrement( const_cast<atomic_t*>(&value) );
}


inline atomic_t atomicExchangeAdd(volatile atomic_t& value, atomic_t n)
{
    return InterlockedExchangeAdd(const_cast<atomic_t*>(&value), n);
}


inline atomic_t atomicExchange(volatile atomic_t& value, atomic_t new_val)
{
    return InterlockedExchange(const_cast<atomic_t*>(&value), new_val);
}


inline void* atomicExchange(void* volatile& ptr, void* new_val)
{
    return InterlockedExchangePointer( const_cast<void**>(&ptr), new_val );
}


inline atomic_t atomicCompareExchange(volatile atomic_t& value, atomic_t ex, atomic_t cmp)
{
    return InterlockedCompareExchange(const_cast<atomic_t*>(&value), ex, cmp);
}


inline void* atomicCompareExchange(void* volatile& ptr, void* ex, void* cmp)
{
    return InterlockedCompareExchangePointer(&ptr, ex, cmp);
}

} // namespace Pt

#endif
