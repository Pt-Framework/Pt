/*
 * Copyright (C) 2006-2007 by Marc Boris Duerner
 * Copyright (C) 2010-2010 by Aloysius Indrayanto
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef PT_ATOMICITY_H
#define PT_ATOMICITY_H

#include <Pt/Api.h>

#if defined(PT_ATOMICITY_GCC_ARM)
    #include <Pt/Atomicity.gcc.arm.h>

#elif defined(PT_ATOMICITY_GCC_MIPS)
    #include <Pt/Atomicity.gcc.mips.h>

#elif defined(PT_ATOMICITY_GCC_SPARC)
    #include <Pt/Atomicity.gcc.sparc.h>

#elif defined(PT_ATOMICITY_GCC_X86_64)
    #include <Pt/Atomicity.gcc.x86_64.h>

#elif defined(PT_ATOMICITY_GCC_X86)
    #include <Pt/Atomicity.gcc.x86.h>

#elif defined(PT_ATOMICITY_GCC_AVR32)
    #include <Pt/Atomicity.gcc.avr32.h>

#elif defined(PT_ATOMICITY_GCC_PPC)
    #include <Pt/Atomicity.gcc.ppc.h>

#elif defined(PT_ATOMICITY_WINDOWS)
    #include <Pt/Atomicity.windows.h>

#elif defined(PT_ATOMICITY_SUN)
    #include <Pt/Atomicity.sun.h>

#elif defined(PT_ATOMICITY_PTHREAD)
    #include <Pt/Atomicity.pthread.h>

#elif defined(_WIN32) || defined(WIN32) || defined(_WIN32_WCE)
    #define PT_ATOMICITY_WINDOWS
    #include <Pt/Atomicity.windows.h>

#elif defined(__sun)
    #define PT_ATOMICITY_SUN
    #include <Pt/Atomicity.sun.h>

#elif defined(__GNUC__) || defined(__xlC__) || \
      defined(__SUNPRO_CC) || defined(__SUNPRO_C)

    #if defined (i386) || defined(__i386) || defined (__i386__) || \
        defined(_X86_) || defined(sun386) || defined (_M_IX86)
        #define PT_ATOMICITY_GCC_X86
        #include <Pt/Atomicity.gcc.x86.h>

    #elif defined(__x86_64__) || defined(__amd64__)
        #define PT_ATOMICITY_GCC_X86_64
        #include <Pt/Atomicity.gcc.x86_64.h>

    #elif defined (ARM) || defined(__arm__) || defined(_M_ARM) || defined(_M_ARMT)
        #define PT_ATOMICITY_GCC_ARM
        #include <Pt/Atomicity.gcc.arm.h>

    #elif defined (AVR) || defined(__AVR__)
        #define PT_ATOMICITY_GCC_AVR32
        #include <Pt/Atomicity.avr32.h>

    #elif defined( _M_PPC  ) || defined( PPC         ) || \
          defined( ppc     ) || defined( __powerpc__ ) || \
          defined( __ppc__ )
        #define PT_ATOMICITY_GCC_PPC
        #include <Pt/Atomicity.gcc.ppc.h>

    #elif defined(__mips__) || defined(MIPSEB) || defined(_MIPSEB) || \
          defined(MIPSEL) || defined(_MIPSEL)
        #define PT_ATOMICITY_GCC_MIPS
        #include <Pt/Atomicity.gcc.mips.h>

    #elif defined(__sparc__) || defined(sparc) || defined(__sparc) || \
          defined(__sparcv8) || defined(__sparcv9)
        #define PT_ATOMICITY_GCC_SPARC
        #include <Pt/Atomicity.gcc.sparc.h>

    #else
        #define PT_ATOMICITY_PTHREAD
        #include <Pt/Atomicity.pthread.h>

    #endif

#elif defined(__SYMBIAN32__)
    #define PT_ATOMICITY_SYMBIAN
    #include <Pt/Atomicity.cw.x86.h>

#else
    #define PT_ATOMICITY_PTHREAD
    #include <Pt/Atomicity.pthread.h>

#endif

namespace Pt {

/** @brief Atomically get a value

    Returns the value after employing a memory fence.
*/
PT_API atomic_t atomicGet(volatile atomic_t& val);

/** @brief Atomically set a value

    Sets the value and employs a memory fence.
*/
PT_API void atomicSet(volatile atomic_t& val, atomic_t n);

/** @brief Increases a value by one as an atomic operation

    Returns the resulting incremented value.
*/
PT_API atomic_t atomicIncrement(volatile atomic_t& val);

/** @brief Decreases a value by one as an atomic operation

    Returns the resulting decremented value.
*/
PT_API atomic_t atomicDecrement(volatile atomic_t& val);

/** @brief Performs atomic addition of two values

    Returns the initial value of the addend.
*/
PT_API atomic_t atomicExchangeAdd(volatile atomic_t& val, atomic_t add);

/** @brief Performs an atomic compare-and-exchange operation

    If \a val is equal to \a comp, \a val is replaced by \a exch. The initial
    value of of \a val is returned.
*/
PT_API atomic_t atomicCompareExchange(volatile atomic_t& val, atomic_t exch, atomic_t comp);

/** @brief Performs an atomic compare-and-exchange operation

    If \a ptr is equal to \a comp, \a ptr is replaced by \a exch. The initial
    value of \a ptr is returned.
*/
PT_API void* atomicCompareExchange(void* volatile& ptr, void* exch, void* comp);

/** @brief Performs an atomic exchange operation

    Sets \a val to \a exch and returns the initial value of \a val.
*/
PT_API atomic_t atomicExchange(volatile atomic_t& val, atomic_t exch);

/** @brief Performs an atomic exchange operation

    Sets \a dest to \a exch and returns the initial value of \a dest.
*/
PT_API void* atomicExchange(void* volatile& dest, void* exch);




////////////////////////////////////////// BELOW ARE FOR TEMPORARY TESTING ///////////////////////////////////////////

union new_atomic_t
{
    int   i; // 32 bit in both 32 bit and 64 bit platforms
    long  l; // 32 bit in 32 bit platform and 64 bit in 64 bit platform
    void* p; // should follow the system word size (and hence will be 128 bit in a hypothetical 128 bit platform ;)

#ifdef __cplusplus
    // To prevent link error
    inline new_atomic_t()
    {}

    // User must not set the union directly.
    // Hence using any of these functions will cause undefined reference error in link-time
    inline explicit new_atomic_t(int);
    inline explicit new_atomic_t(long);
    inline explicit new_atomic_t(void*);
#endif
};

PT_API int   new_atomicGet            (volatile new_atomic_t& val);
PT_API void  new_atomicSet            (volatile new_atomic_t& val, int n);
PT_API int   new_atomicIncrement      (volatile new_atomic_t& val);
PT_API int   new_atomicDecrement      (volatile new_atomic_t& val);
PT_API int   new_atomicExchange       (volatile new_atomic_t& val, new_atomic_t exch);
PT_API int   new_atomicCompareExchange(volatile new_atomic_t& val, new_atomic_t exch, new_atomic_t comp);
PT_API int   new_atomicExchangeAdd    (volatile new_atomic_t& val, new_atomic_t add);
PT_API void* new_atomicCompareExchange(void* volatile& ptr, void* exch, void* comp);
PT_API void* new_atomicExchange       (void* volatile& dest, void* exch);
}

#endif
