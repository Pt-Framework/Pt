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
#ifndef PT_ATOMICINT_GCC_MIPS_H
#define PT_ATOMICINT_GCC_MIPS_H

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
                register atomic_t expected;
                for (;;) {
                    expected = _value;
                    if(_mipsAtomicTestAndSet(&_value, expected, expected + n)) break;
                }
            }

            inline void operator-=(atomic_t n)
            {
                register atomic_t expected;
                for (;;) {
                    expected = _value;
                    if(_mipsAtomicTestAndSet(&_value, expected, expected - n)) break;
                }
            }

            inline void operator=(atomic_t n)
            {
                register atomic_t expected;
                for (;;) {
                    expected = _value;
                    if(_mipsAtomicTestAndSet(&_value, expected, n)) break;
                }
            }

            inline bool compareExchange(atomic_t oldval, atomic_t newval)
            {
                register atomic_t expected = oldval;
                return _mipsAtomicTestAndSet(&_value, expected, newval);
            }

        private:
            volatile atomic_t _value;

            inline atomic_t _mipsAtomicTestAndSet(volatile atomic_t* dst, atomic_t expected, atomic_t newval)
            {
                asm volatile (
                    "0:                   \n\t"
                    "1:    ll   $8, 0($4) \n\t"
                    "      bne  $8, $5, 2f\n\t"
                    "      move $2, $6    \n\t"
                    "      sc   $2, 0($4) \n\t"
                    "      beqz $2, 1b    \n\t"
                    "      nop            \n\t"
                    "      jr   $31       \n\t"
                    "      nop            \n\t"
                    "2:    jr   $31       \n\t"
                    "      move %2, $0    \n\t"
                    "      .end 0             "
                );
                // Is this correct ???
                // How to return value in MIPS ???
            }

    };

} // namespace Pt

#endif
