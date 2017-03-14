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

#ifndef PT_GFX_ARGB32IMAGE_SIMDOPERATIONS_H
#define PT_GFX_ARGB32IMAGE_SIMDOPERATIONS_H


#ifdef RASTERIZER2

#if defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || defined(_M_ARMT) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(__arm)

    #include <arm_neon.h>
    #define USE_NEON

    // ### TODO ###

#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(_X86_) || defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(__amd64__)

    #include <x86intrin.h>
    #define USE_SSE2

#elif defined(_M_IX86) || defined(_M_AMD64) || defined(_M_X64)

    #include <intrin.h>
    #define USE_SSE2

#endif

#endif


namespace Pt {
namespace Gfx {


#ifdef USE_SSE2

// SSE mask
static const __m128i maskA000 = _mm_set_epi32(0xFF000000U, 0xFF000000U, 0xFF000000U, 0xFF000000U);
static const __m128i maskA0G0 = _mm_set_epi32(0xFF00FF00U, 0xFF00FF00U, 0xFF00FF00U, 0xFF00FF00U);
static const __m128i mask0B0R = _mm_set_epi32(0x00FF00FFU, 0x00FF00FFU, 0x00FF00FFU, 0x00FF00FFU);

#endif


// Copy a constant color to destination pixels
inline void Argb32Model::fastCopyPixels(Pt::uint8_t* dst_, Pt::uint32_t srcARGB, size_t length)
{
#ifdef USE_SSE2

    const size_t   len4     = length / 4;

    const __m128i  srcvARGB = _mm_set1_epi32(srcARGB);
          __m128i* dstvARGB = reinterpret_cast<__m128i*>(dst_);

    for(size_t i = 0; i < len4; ++i) {
        _mm_storeu_si128(dstvARGB, srcvARGB);
        ++dstvARGB;
    }

    length -= (len4 * 4);

    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(dstvARGB);

#else

    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(dst_);

#endif

    for(size_t i = 0; i < length; ++i) *dst++ = srcARGB;
}

// Blend a constant color to destination pixels
inline void Argb32Model::fastBlendPixels(Pt::uint8_t* dst_, Pt::uint32_t srcA, Pt::uint32_t srcR, Pt::uint32_t srcG, Pt::uint32_t srcB, Pt::uint32_t blendInv, size_t length)
{
#ifdef USE_SSE2

    const size_t   len4     = length / 4;

    const __m128i  srcvAGAG = _mm_set_epi16(srcA, srcG, srcA, srcG, srcA, srcG, srcA, srcG); // [ AAGG AAGG AAGG AAGG ]
    const __m128i  srcvRBRB = _mm_set_epi16(srcR, srcB, srcR, srcB, srcR, srcB, srcR, srcB); // [ RRBB RRBB RRBB RRBB ]
    const __m128i  srci0A0A = _mm_set_epi32(blendInv, blendInv, blendInv, blendInv);         // [ 0I0I 0I0I 0I0I 0I0I ]
          __m128i* dstvARGB = reinterpret_cast<__m128i*>(dst_);
          __m128i  dstv4PIX;
          __m128i  dstvAGAG;
          __m128i  dstvRBRB;

    for(size_t i = 0; i < len4; ++i) {
        // Load 4 pixels
        dstv4PIX = _mm_loadu_si128 (dstvARGB          ); // [ ARGB ARGB ARGB ARGB ]
        // Process A and G
        dstvAGAG = _mm_and_si128   (dstv4PIX, maskA0G0); // [ A0G0 A0G0 A0G0 A0G0 ]
        dstvAGAG = _mm_srli_epi16  (dstvAGAG, 8       ); // [ 0A0G 0A0G 0A0G 0A0G ]
        dstvAGAG = _mm_mullo_epi16 (dstvAGAG, srci0A0A); // [ AAGG AAGG AAGG AAGG ]
        dstvAGAG = _mm_add_epi16   (dstvAGAG, srcvAGAG); // [ AAGG AAGG AAGG AAGG ]
        dstvAGAG = _mm_and_si128   (dstvAGAG, maskA0G0); // [ A0G0 A0G0 A0G0 AAG0 ]
        // Prefetch the next 4 pixels
        _mm_prefetch(dstvARGB + 1, _MM_HINT_T0);
        // Process R and B
        dstvRBRB = _mm_and_si128   (dstv4PIX, mask0B0R); // [ 0R0B 0R0B 0R0B 0R0B ]
        dstvRBRB = _mm_mullo_epi16 (dstvRBRB, srci0A0A); // [ RRBB RRBB RRBB RRBB ]
        dstvRBRB = _mm_add_epi16   (dstvRBRB, srcvRBRB); // [ RRBB RRBB RRBB RRBB ]
        dstvRBRB = _mm_srli_epi16  (dstvRBRB, 8       ); // [ .R.B .R.B .R.B .R.B ]
        dstvRBRB = _mm_and_si128   (dstvRBRB, mask0B0R); // [ 0R0B 0R0B 0R0B 0R0B ]
        // Store 4 pixels
        dstv4PIX = _mm_or_si128    (dstvAGAG, dstvRBRB); // [ ARGB ARGB ARGB ARGB ]
                   _mm_storeu_si128(dstvARGB, dstv4PIX);
        // Increment the destination pointer
        ++dstvARGB;
    }

    length -= (len4 * 4);

    Pt::uint8_t* dst = reinterpret_cast<Pt::uint8_t*>(dstvARGB);

#else

    Pt::uint8_t* dst = dst_;

#endif

    for(size_t i = 0; i < length; ++i) {
        dst[0] = (srcB + blendInv * dst[0]) >> 8;
        dst[1] = (srcG + blendInv * dst[1]) >> 8;
        dst[2] = (srcR + blendInv * dst[2]) >> 8;
        dst[3] = (srcA + blendInv * dst[3]) >> 8;
        dst += 4;
    }
}

// Copy source pixels to destination pixels
inline void Argb32Model::fastCopyPixels(Pt::uint8_t* dst, const Pt::uint8_t* src, size_t length)
{
/*
#ifdef USE_SSE2

    // ### NOTE: This one is actually a bit slower than a simple memcpy() on an x86_64 ###

    const size_t   len4     = length / 4;

    const __m128i* srcvARGB = reinterpret_cast<const __m128i*>(src);
          __m128i* dstvARGB = reinterpret_cast<      __m128i*>(dst);

    for(size_t i = 0; i < len4; ++i) {
        _mm_prefetch(srcvARGB + 1, _MM_HINT_T0);
        _mm_storeu_si128(dstvARGB, _mm_loadu_si128(srcvARGB));
        ++srcvARGB;
        ++dstvARGB;
    }

    length -= (len4 * 4);

    src = reinterpret_cast<const Pt::uint8_t*>(srcvARGB);
    dst = reinterpret_cast<      Pt::uint8_t*>(dstvARGB);

#endif
*/

    memcpy(dst, src, length * 4);
}

// Blend source pixels to destination pixels
inline void Argb32Model::fastBlendPixels(Pt::uint8_t* dst, const Pt::uint8_t* src, size_t length)
{

#ifdef USE_SSE2

    const size_t   len4     = length / 4;

    const __m128i* srcvARGB = reinterpret_cast<const __m128i*>(src);
          __m128i* dstvARGB = reinterpret_cast<      __m128i*>(dst);
          __m128i  srcv4PIX;
          __m128i  srcv0A0A;
          __m128i  srci0A0A;
          __m128i  srcvAGAG;
          __m128i  srcvRBRB;
          __m128i  dstv4PIX;
          __m128i  dstvAGAG;
          __m128i  dstvRBRB;

    for(size_t i = 0; i < len4; ++i) {
        // Load 4 pixels
        srcv4PIX = _mm_loadu_si128 (srcvARGB          ); // [ ARGB ARGB ARGB ARGB ]
        dstv4PIX = _mm_loadu_si128 (dstvARGB          ); // [ ARGB ARGB ARGB ARGB ]
        // Get the source alpha
        srcv0A0A = _mm_and_si128   (srcv4PIX, maskA000); // [ A000 A000 A000 A000 ]
        srci0A0A = _mm_sub_epi16   (maskA000, srcv0A0A); // [ I000 I000 I000 I000 ]
        srcv0A0A = _mm_or_si128    (                     // [ 0A0A 0A0A 0A0A 0A0A ]
                       _mm_srli_epi16(srcv0A0A,  8),
                       _mm_srli_epi32(srcv0A0A, 24)
                   );
        srci0A0A = _mm_or_si128    (                     // [ 0I0I 0I0I 0I0I 0A0I ]
                       _mm_srli_epi16(srci0A0A,  8),
                       _mm_srli_epi32(srci0A0A, 24)
                   );
        // Process A and G
        srcvAGAG = _mm_and_si128   (srcv4PIX, maskA0G0); // [ A0G0 A0G0 A0G0 A0G0 ]
        srcvAGAG = _mm_srli_epi16  (srcvAGAG, 8       ); // [ A0G0 A0G0 A0G0 A0G0 ]
        srcvAGAG = _mm_mullo_epi16 (srcvAGAG, srcv0A0A); // [ AAGG AAGG AAGG AAGG ]
        dstvAGAG = _mm_and_si128   (dstv4PIX, maskA0G0); // [ A0G0 A0G0 A0G0 A0G0 ]
        dstvAGAG = _mm_srli_epi16  (dstvAGAG, 8       ); // [ 0A0G 0A0G 0A0G 0A0G ]
        dstvAGAG = _mm_mullo_epi16 (dstvAGAG, srci0A0A); // [ AAGG AAGG AAGG AAGG ]
        dstvAGAG = _mm_add_epi16   (dstvAGAG, srcvAGAG); // [ AAGG AAGG AAGG AAGG ]
        dstvAGAG = _mm_and_si128   (dstvAGAG, maskA0G0); // [ A0G0 A0G0 A0G0 AAG0 ]
        // Prefetch the next 4 pixels
        _mm_prefetch(srcvARGB + 1, _MM_HINT_T0);
        _mm_prefetch(dstvARGB + 1, _MM_HINT_T0);
        // Process R and B
        srcvRBRB = _mm_and_si128   (srcv4PIX, mask0B0R); // [ 0R0B 0R0B 0R0B 0R0B ]
        srcvRBRB = _mm_mullo_epi16 (srcvRBRB, srcv0A0A); // [ RRBB RRBB RRBB RRBB ]
        dstvRBRB = _mm_and_si128   (dstv4PIX, mask0B0R); // [ 0R0B 0R0B 0R0B 0R0B ]
        dstvRBRB = _mm_mullo_epi16 (dstvRBRB, srci0A0A); // [ RRBB RRBB RRBB RRBB ]
        dstvRBRB = _mm_add_epi16   (dstvRBRB, srcvRBRB); // [ RRBB RRBB RRBB RRBB ]
        dstvRBRB = _mm_srli_epi16  (dstvRBRB, 8       ); // [ .R.B .R.B .R.B .R.B ]
        dstvRBRB = _mm_and_si128   (dstvRBRB, mask0B0R); // [ 0R0B 0R0B 0R0B 0R0B ]
        // Store 4 pixels
        dstv4PIX = _mm_or_si128    (dstvAGAG, dstvRBRB); // [ ARGB ARGB ARGB ARGB ]
                   _mm_storeu_si128(dstvARGB, dstv4PIX);
        // Increment the pointers
        ++srcvARGB;
        ++dstvARGB;
    }

    length -= (len4 * 4);

    src = reinterpret_cast<const Pt::uint8_t*>(srcvARGB);
    dst = reinterpret_cast<      Pt::uint8_t*>(dstvARGB);

#endif

    for(size_t i = 0; i < length; ++i) {
        Argb32Model::sourceOver(dst, src);
        src += 4;
        dst += 4;
    }
}


} // namespace
} // namespace

#endif
