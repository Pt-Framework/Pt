/***************************************************************************
 *   Copyright (C) 2006-2007 by Marc Boris Duerner                         *
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
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
#ifndef PT_ATOMICITY_H

// build systems can specify asm-style by defining PT_ASM_ATT or PT_ASM_INTEL
// as well as the CPU type by defining PT_X86, PT_ARM, PT_PPC etc...
// If these are not defined it is still possible to detect the asm-style
// and CPU correctly for many common cases.

// always use Interlocked-functions when compiling for win32
#if defined(_WIN32) || defined(WIN32) || defined(_WIN32_WCE)

    #define PT_ATOMICITY_H "Atomicity.windows.h"

// use AT&T-style inline asm
#elif defined(PT_ASM_ATT) || \
      defined(__GNUC__) || defined(__xlC__) || \
      defined(__SUNPRO_CC) || defined(__SUNPRO_C)

    #if defined(PT_X86) || \
        defined (i386) || defined(__i386) || defined (__i386__) || \
        defined(_X86_) || defined(sun386) || defined (_M_IX86) || \
        defined(__x86_64__) || defined(__amd64__)

        #define PT_ATOMICITY_H "Atomicity.gcc.x86.h"

    #elif defined(PT_X86_64) || \
        defined(__x86_64__) || defined(__amd64__)

        #define PT_ATOMICITY_H "Atomicity.gcc.x86_64.h"

    #elif defined(PT_ARM) || \
          defined (ARM) || defined(__arm__) || defined(_M_ARM) || defined(_M_ARMT)

        #define PT_ATOMICITY_H "Atomicity.gcc.arm.h"

    #elif defined(PT_PPC) || \
          defined( _M_PPC  ) || defined( PPC         ) || \
          defined( ppc     ) || defined( __powerpc__ ) || \
          defined( __ppc__ )

        #define PT_ATOMICITY_H "Atomicity.gcc.ppc.h"

    #elif defined(PT_MIPS) || \
          defined(__mips__) || defined(MIPSEB) || defined(_MIPSEB) || \
          defined(MIPSEL) || defined(_MIPSEL)

        #define PT_ATOMICITY_H "Atomicity.gcc.mips.h"

    #elif defined(PT_SPARC) || \
          defined(__sparc__) || defined(sparc) || defined(__sparc)

        #define PT_ATOMICITY_H "Atomicity.gcc.sparc.h"

    #else

        #define PT_ATOMICITY_H "Atomicity.generic.h"

    #endif

#else
    #define PT_ATOMICITY_WITH_PTHREAD
    #define PT_ATOMICITY_H "Atomicity.pthread.h"

#endif

#include PT_ATOMICITY_H

namespace Pt {

/** @brief Increases a value by one as an atomic operation

    Returns the resulting incremented value.
*/
atomic_t atomicIncrement(volatile atomic_t& val);

/** @brief Decreases a value by one as an atomic operation

    Returns the resulting decremented value.
*/
atomic_t atomicDecrement(volatile atomic_t& val);

/** @brief Performs atomic addition of two values

    Returns the initial value of the addend.
*/
atomic_t atomicExchangeAdd(volatile atomic_t& val, atomic_t add);

/** @brief Performs an atomic compare-and-exchange operation

    If \a val is equal to \a comp, \a val is replaced by \a exch. The initial
    value of of \a val is returned.
*/
atomic_t atomicCompareExchange(volatile atomic_t& val, atomic_t exch, atomic_t comp);

/** @brief Performs an atomic compare-and-exchange operation

    If \a ptr is equal to \a comp, \a ptr is replaced by \a exch. The initial
    value of \a ptr is returned.
*/
void* atomicCompareExchange(void* volatile& ptr, void* exch, void* comp);

/** @brief Performs an atomic exchange operation

    Sets \a val to \a exch and returns the initial value of \a val.
*/
atomic_t atomicExchange(volatile atomic_t& val, atomic_t exch);

/** @brief Performs an atomic exchange operation

    Sets \a dest to \a exch and returns the initial value of \a dest.
*/
void* atomicExchange(void* volatile& dest, void* exch);

}

#endif
