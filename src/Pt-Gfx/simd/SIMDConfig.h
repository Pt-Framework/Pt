/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2016-2016 Marc Boris Duerner

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  As a special exception, you may use this file as part of a free
  software library without restriction. Specifically, if other files
  instantiate templates or use macros or inline functions from this
  file, or you compile this file and link it with other files to
  produce an executable, this file does not by itself cause the
  resulting executable to be covered by the GNU General Public
  License. This exception does not however invalidate any other
  reasons why the executable file might be covered by the GNU Library
  General Public License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#ifndef PT_GFX_SIMDCONFIG_H
#define PT_GFX_SIMDCONFIG_H


//
// SIMD headers and macros
//

// Disable this for now due to SIMD experiment with polygon/vector calculation in Rasterizer2
#undef WITH_EXPERIMENTAL_GFX

// For now only enable SIMD when working with experimental Gfx
#ifdef WITH_EXPERIMENTAL_GFX

// GNU-style compiler
#if defined(__unix__) || defined(__GNUC__)

    // Define the compiler identification macro
    #define PT_GFX_USE_GNU_STYLE_COMPILER
    // Include the appropriate SIMD header x86 and define the appropriate macros
    #if defined(i386) || defined(__i386) || defined(__i386__) || defined(_X86_) || defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(__amd64__)
        #include <x86intrin.h>
        #define PT_GFX_USE_X86_CPU
        #define PT_GFX_USE_FMA3
        #define PT_GFX_USE_AVX2
        #define PT_GFX_USE_AVX1
        #define PT_GFX_USE_SSE4P2
        #define PT_GFX_USE_SSE4P1
        #define PT_GFX_USE_SSSE3
        #define PT_GFX_USE_SSE3
        #define PT_GFX_USE_SSE2
        #define PT_GFX_USE_SSE1
    // Include the appropriate SIMD header Arm and define the appropriate macros
    #elif defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || defined(_M_ARMT) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(__arm)
        #include <arm_neon.h>
        #define PT_GFX_USE_ARM_CPU
        #define PT_GFX_USE_NEON
    #endif

// MSVC-style compiler
#elif defined(_MSC_VER )

    // Define the compiler identification macro
    #define PT_GFX_USE_MSVC_STYLE_COMPILER
    // Include the appropriate SIMD header x86 and define the appropriate macros
    #if defined(_M_IX86) || defined(_M_AMD64) || defined(_M_X64)
        #include <intrin.h>
        #define PT_GFX_USE_X86_CPU
        #define PT_GFX_USE_FMA3
        #define PT_GFX_USE_AVX2
        #define PT_GFX_USE_AVX1
        #define PT_GFX_USE_SSE4P2
        #define PT_GFX_USE_SSE4P1
        #define PT_GFX_USE_SSSE3
        #define PT_GFX_USE_SSE3
        #define PT_GFX_USE_SSE2
        #define PT_GFX_USE_SSE1
    // Include the appropriate SIMD header Arm and define the appropriate macros
    #elif defined(_M_ARM) || defined(_M_ARM64)
        #include <arm_neon.h>
        #define PT_GFX_USE_ARM_CPU
        #define PT_GFX_USE_NEON
    #endif

#endif

#endif


//#undef PT_GFX_USE_FMA3

//#undef PT_GFX_USE_AVX2
//#undef PT_GFX_USE_AVX1

//#undef PT_GFX_USE_SSE4P2
//#undef PT_GFX_USE_SSE4P1
//#undef PT_GFX_USE_SSSE3
//#undef PT_GFX_USE_SSE3
//#undef PT_GFX_USE_SSE2
//#undef PT_GFX_USE_SSE1

//#undef PT_GFX_USE_NEON


//
// When the higher SIMD level is supported, the lower level SIMD(s) should be also supported by the CPU
//

#if defined(PT_GFX_USE_FMA3) && !defined(PT_GFX_USE_AVX2)
#define PT_GFX_USE_AVX2
#endif

#if defined(PT_GFX_USE_AVX2) && !defined(PT_GFX_USE_AVX1)
#define PT_GFX_USE_AVX1
#endif

#if defined(PT_GFX_USE_AVX1) && !defined(PT_GFX_USE_SSE4P2)
#define PT_GFX_USE_SSE4P2
#endif

#if defined(PT_GFX_USE_SSE4P2) && !defined(PT_GFX_USE_SSE4P1)
#define PT_GFX_USE_SSE4P1
#endif

#if defined(PT_GFX_USE_SSE4P1) && !defined(PT_GFX_USE_SSSE3)
#define PT_GFX_USE_SSSE3
#endif

#if defined(PT_GFX_USE_SSSE3) && !defined(PT_GFX_USE_SSE3)
#define PT_GFX_USE_SSE3
#endif

#if defined(PT_GFX_USE_SSE3) && !defined(PT_GFX_USE_SSE2)
#define PT_GFX_USE_SSE2
#endif

#if defined(PT_GFX_USE_SSE2) && !defined(PT_GFX_USE_SSE1)
#define PT_GFX_USE_SSE1
#endif


//
// SIMD Helper functions
//

#if defined(PT_GFX_USE_NEON)

// NOTE: * Unlike x86_64's SSE and AVX, ARM's NEON does not have convenience-set functions such as:
//             _mm_set_epi32
//             _mm_set_ps
//             ... etc.
//         Therefore, they are implemented using C code. This will cause endianness issue.
//
//       * The code below will only work if the ARM CPU is running in little endian mode.
//       * The code below is designed so that the behavior of those functions are the same
//         with the SSE/AVX counterparts.

static inline int16x8_t NEON_SET_INT16X8(int16_t h, int16_t g, int16_t f, int16_t e, int16_t d, int16_t c, int16_t b, int16_t a)
{
    const int16x8_t vec = {
        a, b, c, d, e, f, g, h
    };

    return vec;
}

static inline int32x4_t NEON_SET_INT32X4(int32_t d, int32_t c, int32_t b, int32_t a)
{
    const int32x4_t vec = {
        a, b, c, d
    };

    return vec;
}

static inline float32x4_t NEON_SET_FLT32X4(float d, float c, float b, float a)
{
    const float32x4_t vec = {
        a, b, c, d
    };

    return vec;
}

#endif


#endif
