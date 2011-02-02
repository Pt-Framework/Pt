/*
 * Copyright (C) 2006 by Dr. Marc Boris Duerner
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
#include "Pt/Atomicity.h"

#if defined(PT_ATOMICITY_GCC_ARM)
    #include "Atomicity.gcc.arm.cpp"

#elif defined(PT_ATOMICITY_GCC_MIPS)
    #include "Atomicity.gcc.mips.cpp"

#elif defined(PT_ATOMICITY_GCC_SPARC)
    #include "Atomicity.gcc.sparc.cpp"

#elif defined(PT_ATOMICITY_GCC_X86_64)
    #include "Atomicity.gcc.x86_64.cpp"

#elif defined(PT_ATOMICITY_GCC_X86)
    #include "Atomicity.gcc.x86.cpp"

#elif defined(PT_ATOMICITY_GCC_AVR32)
    #include "Atomicity.gcc.avr32.cpp"

#elif defined(PT_ATOMICITY_GCC_PPC)
    #include "Atomicity.gcc.ppc.cpp"

#elif defined(PT_ATOMICITY_WINDOWS)
    #include "Atomicity.windows.cpp"

#elif defined(PT_ATOMICITY_SUN)
    #include "Atomicity.sun.cpp"

#elif defined(PT_ATOMICITY_SYMBIAN)
    #include "Atomicity.cw.x86.cpp"

#elif defined(PT_ATOMICITY_PTHREAD)
    #include "Atomicity.pthread.cpp"

#elif defined(_WIN32) || defined(WIN32) || defined(_WIN32_WCE)
    #include "Atomicity.windows.cpp"

#elif defined(__sun)
    #include "Atomicity.sun.cpp"

#elif defined(__GNUC__) || defined(__xlC__) || \
      defined(__SUNPRO_CC) || defined(__SUNPRO_C)

    #if defined (i386) || defined(__i386) || defined (__i386__) || \
        defined(_X86_) || defined(sun386) || defined (_M_IX86)
        #include "Atomicity.gcc.x86.cpp"

    #elif defined(__x86_64__) || defined(__amd64__)
        #include "Atomicity.gcc.x86_64.cpp"

    #elif defined (ARM) || defined(__arm__) || defined(_M_ARM) || defined(_M_ARMT)
        #include "Atomicity.gcc.arm.cpp"

    #elif defined (AVR) || defined(__AVR__)
        #include "Atomicity.gcc.avr32.cpp"

    #elif defined( _M_PPC  ) || defined( PPC         ) || \
          defined( ppc     ) || defined( __powerpc__ ) || \
          defined( __ppc__ )
        #include "Atomicity.gcc.ppc.cpp"

    #elif defined(__mips__) || defined(MIPSEB) || defined(_MIPSEB) || \
          defined(MIPSEL) || defined(_MIPSEL)
        #include "Atomicity.gcc.mips.cpp"

    #elif defined(__sparc__) || defined(sparc) || defined(__sparc) || \
          defined(__sparcv8) || defined(__sparcv9)
        #include "Atomicity.gcc.sparc.cpp"

    #else
        #include "Atomicity.pthread.cpp"

    #endif

#elif defined(__SYMBIAN32__)
    #include "Atomicity.cw.x86.cpp"

#else
    #define #include "Atomicity.pthread.cpp"

#endif
