/***************************************************************************
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
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
#ifndef PT_ATOMICINT_GCC_PPC_H
#define PT_ATOMICINT_GCC_PPC_H

#include <csignal>


namespace Pt {

typedef std::sig_atomic_t atomic_t;


inline atomic_t atomicIncrement(volatile atomic_t& val)
{
    volatile register atomic_t tmp;
    asm volatile ("\nL_ii_loop:\n\t"
                    "lwarx  %0, 0, %2\n\t"
                    "addi   %0, %0, 1\n\t"
                    "stwcx. %0, 0, %2\n\t"
                    "bne-   L_ii_loop"
                    : "=r" (tmp) : "0" (tmp), "r" (&val));
    return tmp;
}


inline atomic_t atomicDecrement(volatile atomic_t& val)
{
    volatile register atomic_t tmp;
    asm volatile ("\nL_id_loop:\n\t"
                    "lwarx  %0, 0, %2\n\t"
                    "addi   %0, %0, -1\n\t"
                    "stwcx. %0, 0, %2\n\t"
                    "bne-   L_id_loop"
                    : "=r" (tmp) : "0" (tmp), "r" (&val));
    return tmp;
}


inline atomic_t atomicExchangeAdd(volatile atomic_t& val, atomic_t add)
{
    volatile register atomic_t tmp;
    asm volatile ("\nL_iea_loop:\n\t"
                  "lwarx  %0, 0, %2\n\t"
                  "add    %1, %3, %4\n\t"
                  "stwcx. %1, 0, %2\n\t"
                  "bne    L_iea_loop"
                  : "=r" (tmp), "=r" (add) : "r" (&val), "0" (tmp), "1" (add));
    return tmp;
}


inline atomic_t atomicCompareExchange(volatile atomic_t& val, atomic_t exch, atomic_t comp)
{
    atomic_t tmp = 0;
    asm volatile ("\nL_ice_loop:\n\t"
                  "lwarx   %0, 0, %1\n\t"
                  "cmpw    %2, %3\n\t" 
                  "bne-    L_ice_diff\n\t"
                  "stwcx.  %4, 0, %1\n\t"
                  "bne-    L_ice_loop\n"
                  "L_ice_diff:"
                  : "=r" (tmp) : "r" (&val), "0" (tmp) , "r" (comp), "r" (exch));
    return tmp;
}


inline void* atomicCompareExchange(volatile void*& ptr, void* exch, void* comp)
{
    return (void*) atomicCompareExchange( (volatile atomic_t)ptr, (atomic_t)(exch), (atomic_t)(comp) );
}


inline atomic_t atomicExchange(volatile atomic_t& val, atomic_t exch)
{
  atomic_t tmp;
  asm volatile ("\nL_ie_loop:\n\t"
                "lwarx  %0, 0, %1\n\t"
                "stwcx. %2, 0, %1\n\t"
                "bne    L_ie_loop"
                : "=r" (tmp) : "r" (&val), "r" (exch));
  return tmp;
}


inline void* atomicExchange(volatile void*& ptr, void* exch)
{
    return (void*) atomicExchange( (volatile atomic_t)ptr, (atomic_t)(axch) );
}


} // namespace Pt

#endif
