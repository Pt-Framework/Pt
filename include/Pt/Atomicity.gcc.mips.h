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
#ifndef PT_ATOMICINT_GCC_MIPS_H
#define PT_ATOMICINT_GCC_MIPS_H

#include <csignal>


namespace Pt {

    typedef std::sig_atomic_t atomic_t;
/*
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
              atomic_t result;

              asm volatile (
                  "1:              \n\t"
                  ".set  push      \n\t"
#if _MIPS_SIM == _ABIO32
                  ".set  mips2     \n\t"
#endif
                  " ll   %0, %2    \n\t"
                  " addu %0, %3, %0\n\t"
                  " sc   %0, %1    \n\t"
                  ".set  pop       \n\t"
                  " beqz %0, 1b"
                  : "=&r"(result), "=m"(_value)
                  :   "m"(_value),  "r"(val)
                  : "memory");
            }

            inline void operator-=(atomic_t n)
            {
              atomic_t result;

              asm volatile (
                  "1:               \n\t"
                  ".set  push       \n\t"
#if _MIPS_SIM == _ABIO32
                  ".set  mips2      \n\t"
#endif
                  " ll   %0, %2     \n\t"
                  " subu %0, %3, %0 \n\t" // Check if "subu" is exist just as "addu" ?
                  " sc   %0, %1     \n\t"
                  ".set  pop        \n\t"
                  " beqz %0,1b"
                  : "=&r"(result), "=m"(_value)
                  :   "m"(_value),  "r"(val)  // I don't use "r(-val)" until I'm sure that
                  : "memory");                // "addu" is not actually an "add-unsigned"
            }

            inline void operator=(atomic_t n)
            {
              compareExchange(_value, n);
            }

            inline bool compareExchange(atomic_t oldval, atomic_t newval)
            {
              atomic_t ret, temp;

              asm volatile (
                  "1:              \n\t"
                  ".set  push      \n\t"
#if _MIPS_SIM == _ABIO32
                  ".set  mips2     \n\t"
#endif
#if _MIPS_SIM == _ABI64
                  " lld  %1,%5     \n\t"
#else
                  " ll   %1,%5     \n\t"
#endif
                  " move %0, $0    \n\t"
                  " bne  %1, %3, 2f\n\t"
                  " move %0, %4    \n\t"
#if _MIPS_SIM == _ABI64
                  " scd  %0, %2    \n\t"
#else
                  " sc   %0, %2    \n\t"
#endif
                  ".set  pop       \n\t"
                  " beqz %0, 1b    \n\t"
                  "2: "
                  : "=&r"(ret),  "=&r"(temp),  "=m"(_value)
                  :   "r"(oldval), "r"(newval), "m"(_value)
                  : "memory"
               );

              return ret;
            }

        private:
            volatile atomic_t _value;
    };
*/

/*

static inline gint32 InterlockedIncrement(volatile gint32 *val)
{
       gint32 tmp, result = 0;

       __asm__ __volatile__ ("    .set    mips32\n"
                             "1:  ll      %0, %2\n"
                             "    addu    %1, %0, 1\n"
                              "    sc      %1, %2\n"
                             "    beqz    %1, 1b\n"
                             "    .set    mips0\n"
                             : "=&r" (result), "=&r" (tmp), "=m" (*val)
                             : "m" (*val));
       return result + 1;
}

static inline gint32 InterlockedDecrement(volatile gint32 *val)
{
       gint32 tmp, result = 0;

       __asm__ __volatile__ ("    .set    mips32\n"
                             "1:  ll      %0, %2\n"
                             "    subu    %1, %0, 1\n"
                              "    sc      %1, %2\n"
                             "    beqz    %1, 1b\n"
                             "    .set    mips0\n"
                             : "=&r" (result), "=&r" (tmp), "=m" (*val)
                             : "m" (*val));
       return result - 1;
}

#define InterlockedCompareExchangePointer(dest,exch,comp)
InterlockedCompareExchange((volatile gint32 *)(dest), (gint32)(exch),
gint32)(comp))

static inline gint32 InterlockedCompareExchange(volatile gint32 *dest,
                                               gint32 exch, gint32 comp) {
       gint32 old, tmp;

       __asm__ __volatile__ ("    .set    mips32\n"
                             "1:  ll      %0, %2\n"
                             "    bne     %0, %5, 2f\n"
                             "    move    %1, %4\n"
                              "    sc      %1, %2\n"
                             "    beqz    %1, 1b\n"
                             "2:  .set    mips0\n"
                             : "=&r" (old), "=&r" (tmp), "=m" (*dest)
                             : "m" (*dest), "r" (exch), "r" (comp));
       return(old);
}

static inline gint32 InterlockedExchange(volatile gint32 *dest, gint32 exch)
{
       gint32 result, tmp;

       __asm__ __volatile__ ("    .set    mips32\n"
                             "1:  ll      %0, %2\n"
                             "    move    %1, %4\n"
                              "    sc      %1, %2\n"
                             "    beqz    %1, 1b\n"
                             "    .set    mips0\n"
                             : "=&r" (result), "=&r" (tmp), "=m" (*dest)
                             : "m" (*dest), "r" (exch));
       return(result);
}
#define InterlockedExchangePointer(dest,exch) InterlockedExchange((volatile
gint32 *)(dest), (gint32)(exch))

static inline gint32 InterlockedExchangeAdd(volatile gint32 *dest, gint32 add)
{
        gint32 result, tmp;

       __asm__ __volatile__ ("    .set    mips32\n"
                             "1:  ll      %0, %2\n"
                             "    addu    %1, %0, %4\n"
                              "    sc      %1, %2\n"
                             "    beqz    %1, 1b\n"
                             "    .set    mips0\n"
                             : "=&r" (result), "=&r" (tmp), "=m" (*dest)
                             : "m" (*dest), "r" (add));
        return result;
}

*/
} // namespace Pt

#endif
