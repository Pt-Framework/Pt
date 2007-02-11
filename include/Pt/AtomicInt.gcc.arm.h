/***************************************************************************
 *   Copyright (C) 2006 by PTV AG                                          *
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
#ifndef PT_ATOMICINT_GCC_ARM_H
#define PT_ATOMICINT_GCC_ARM_H

#include <csignal>


namespace Pt {

    typedef std::sig_atomic_t atomic_t;

    class AtomicInt
    {
        public:
            inline AtomicInt(atomic_t value = 0)
            : _value(value)
            {}

            inline atomic_t value() const
            { return _value; }

            inline void operator+=(atomic_t n)
            {
                volatile register atomic_t tmp;
                volatile register atomic_t result;

                asm volatile (
                    "@ atomic_add_return      \n\t"
                    "1:    ldrex  %0, [%2]    \n\t"
                    "      add    %0, %0, %3  \n\t"
                    "      strex  %1, %0, [%2]\n\t"
                    "      teq    %1, #0      \n\t"
                    "      bne    1b              "
                    : "=&r" (result), "=&r" (tmp)
                    : "r" (&_value), "Ir" (n)
                    : "cc"
                );
            }

            inline void operator-=(atomic_t n)
            {
                volatile register atomic_t tmp;
                volatile register atomic_t result;

                asm volatile (
                    "@ atomic_sub_return     \n\t"
                    "1:    ldrex %0, [%2]    \n\t"
                    "      sub   %0, %0, %3  \n\t"
                    "      strex %1, %0, [%2]\n\t"
                    "      teq   %1, #0      \n\t"
                    "      bne   1b              "
                    : "=&r" (result), "=&r" (tmp)
                    : "r" (&_value), "Ir" (n)
                    : "cc"
                );
            }

            inline void operator=(atomic_t n)
            {
                volatile register atomic_t tmp;

                asm volatile (
                    "@ atomic_set            \n\t"
                    "1:    ldrex %0, [%1]    \n\t"
                    "      strex %0, %2, [%1]\n\t"
                    "      teq   %0, #0      \n\t"
                    "      bne   1b              "
                    : "=&r" (tmp)
                    : "r" (&_value), "r" (n)
                    : "cc"
                );
            }

            inline bool compareExchange(atomic_t oldval, atomic_t newval)
            {
                volatile register atomic_t tmp;
                volatile register atomic_t result;

                asm volatile (
                        "0:    ldr   %1,[%2]   \n\t"
                        "      cmp   %1,%4     \n\t"
                        "      movne %0,%1     \n\t"
                        "      bne   1f        \n\t"
                        "      swp   %0,%3,[%2]\n\t"
                        "      cmp   %1,%0     \n\t"
                        "      swpne %1,%0,[%2]\n\t"
                        "      bne   0b        \n\t"
                        "1:                        "
                    : "=&r" (result), "=&r" (tmp)
                    : "r" (&_value), "r" (newval), "r" (oldval)
                    : "cc", "memory"
                );

                return result;
            }

        private:
            volatile atomic_t _value;
    };

} // namespace Pt

#endif
