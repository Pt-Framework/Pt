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


#ifdef RASTERIZER2

#if defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || defined(_M_ARMT) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(__arm)

    #include <arm_neon.h>
    #define PT_GFX_USE_NEON

#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(_X86_) || defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(__amd64__)

    #include <x86intrin.h>
    #define PT_GFX_USE_AVX
    #define PT_GFX_USE_AVX2
    #define PT_GFX_USE_SSE3
    #define PT_GFX_USE_SSE2

#elif defined(_M_IX86) || defined(_M_AMD64) || defined(_M_X64)

    #include <intrin.h>
    #define PT_GFX_USE_AVX
    #define PT_GFX_USE_AVX2
    #define PT_GFX_USE_SSE3
    #define PT_GFX_USE_SSE2

#endif

#endif

#endif
