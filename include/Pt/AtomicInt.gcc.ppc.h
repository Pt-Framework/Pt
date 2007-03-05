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
#ifndef PT_ATOMICINT_GCC_PPC_H
#define PT_ATOMICINT_GCC_PPC_H

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
            { return _value;  }

            inline void operator+=(atomic_t n)
            {
                volatile register atomic_t ret  = 0;
                volatile register atomic_t zero = 0;

                asm volatile (
                    "0:    lwarx  %0, %3, %1\n\t"
                    "      add    %0, %2, %0\n\t"
                    "      stwcx. %0, %3, %1\n\t"
                    "      bne-   0b        \n\t"
                    "      isync                "
                    : "=&r"(ret)
                    :   "r"(&_value), "r"(n), "r"(zero)
                    : "cr0", "memory", "r0"
                );
            }

            inline void operator-=(atomic_t n)
            {
                volatile register atomic_t ret  = 0;
                volatile register atomic_t zero = 0;

                asm volatile (
                    "0:    lwarx  %0, %3, %1\n\t"
                    "      add    %0, %2, %0\n\t"
                    "      stwcx. %0, %3, %1\n\t"
                    "      bne-   0b        \n\t"
                    "      isync                "
                    : "=&r"(ret)
                    :   "r"(&_value),   "r"(-n), "r"(zero)
                    : "cr0", "memory", "r0"
               );
            }

            inline void operator=(atomic_t n)
            {
                volatile register atomic_t ret = 0;

                asm volatile (
                    "0:    lwarx  %0, 0, %1\n\t"
                    "      stwcx. %2, 0, %1\n\t"
                    "      bne-   0b       \n\t"
                    "      isync               "
                    : "=&r"(ret)
                    :   "r"(&_value),  "r"(n)
                    : "cr0","memory", "r0"
               );
            }

            inline bool compareExchange(atomic_t oldval, atomic_t newval)
            {
                volatile register atomic_t ret = 0;

                asm volatile (
                    "sync                    \n\t"
                    "1:    lwarx   %0,  0, %1\n\t"
                    "      subf.   %0, %2, %0\n\t"
                    "      bne     2f        \n\t"
                    "      stwcx.  %3,  0, %1\n\t"
                    "      bne-    1b        \n\t"
                    "2:    isync                 "
                    : "=&r"(ret)
                    :   "b"(&_value), "r"(oldval), "r"(newval)
                    : "cr0", "memory"
                );

                return ret == 0;
            }

        private:
            volatile atomic_t _value;
    };

} // namespace Pt

#endif
