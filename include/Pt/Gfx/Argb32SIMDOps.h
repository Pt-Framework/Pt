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
namespace Argb32 {


#if defined(USE_SSE2)

// SSE mask
static const __m128i maskA000 = _mm_set_epi32(0xFF000000U, 0xFF000000U, 0xFF000000U, 0xFF000000U);
static const __m128i maskA0G0 = _mm_set_epi32(0xFF00FF00U, 0xFF00FF00U, 0xFF00FF00U, 0xFF00FF00U);
static const __m128i mask0B0R = _mm_set_epi32(0x00FF00FFU, 0x00FF00FFU, 0x00FF00FFU, 0x00FF00FFU);

#elif defined(USE_NEON)

// NEON macros
#define SET_16X8(A, B, C, D, E, F, G, H) { (int16_t) H, (int16_t) G, (int16_t) F, (int16_t) E, (int16_t) D, (int16_t) C, (int16_t) B, (int16_t) A }
#define SET_32X4(A, B, C, D)             { (int32_t) D, (int32_t) C, (int32_t) B, (int32_t) A }

// NEON mask
static const int32x4_t maskA000 = SET_32X4(0xFF000000U, 0xFF000000U, 0xFF000000U, 0xFF000000U);
static const int32x4_t maskA0G0 = SET_32X4(0xFF00FF00U, 0xFF00FF00U, 0xFF00FF00U, 0xFF00FF00U);
static const int32x4_t mask0B0R = SET_32X4(0x00FF00FFU, 0x00FF00FFU, 0x00FF00FFU, 0x00FF00FFU);

#endif


// Copy a constant color to destination pixels
inline void fastCopyPixels(Pt::uint8_t* toBuffer, Pt::uint32_t fromARGB, size_t length)
{
#if defined(USE_SSE2)

    const size_t   len4     = length / 4;
    const __m128i  srcvARGB = _mm_set1_epi32(fromARGB);
          __m128i* dstvARGB = reinterpret_cast<__m128i*>(toBuffer);

    for(size_t i = 0; i < len4; ++i) {
        _mm_storeu_si128(dstvARGB, srcvARGB);
        ++dstvARGB;
    }

    length %= 4;
    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(dstvARGB);

#elif defined(USE_NEON)

    const size_t     len4     = length / 4;
    const int32x4_t  srcvARGB = vdupq_n_s32(fromARGB);
          int32x4_t* dstvARGB = reinterpret_cast<int32x4_t*>(toBuffer);

    for(size_t i = 0; i < len4; ++i) {
        vst1q_s32((int32_t*) dstvARGB, srcvARGB);
        ++dstvARGB;
    }

    length %= 4;
    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(dstvARGB);

#else

    Pt::uint32_t* dst = reinterpret_cast<Pt::uint32_t*>(toBuffer);

#endif

    for(size_t i = 0; i < length; ++i) *dst++ = fromARGB;
}

// Blend a constant color to destination pixels
// (the source color must have been premultiplied with the non-inverted blending factor)
inline void fastBlendPixels(Pt::uint8_t* toBuffer, Pt::uint32_t srcA, Pt::uint32_t srcR, Pt::uint32_t srcG, Pt::uint32_t srcB, Pt::uint32_t bfcI, size_t length)
{
#if defined(USE_SSE2)

    const size_t   len4     = length / 4;
    const __m128i  srcvAGAG = _mm_set_epi16(srcA, srcG, srcA, srcG, srcA, srcG, srcA, srcG); // [ AAGG AAGG AAGG AAGG ]
    const __m128i  srcvRBRB = _mm_set_epi16(srcR, srcB, srcR, srcB, srcR, srcB, srcR, srcB); // [ RRBB RRBB RRBB RRBB ]
    const __m128i  srci0A0A = _mm_set_epi16(bfcI, bfcI, bfcI, bfcI, bfcI, bfcI, bfcI, bfcI); // [ 0I0I 0I0I 0I0I 0I0I ]
          __m128i* dstvARGB = reinterpret_cast<__m128i*>(toBuffer);
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

    length %= 4;
    Pt::uint8_t* dst = reinterpret_cast<Pt::uint8_t*>(dstvARGB);

#elif defined(USE_NEON)

    const size_t     len4     = length / 4;
    const int16x8_t  srcvAGAG = SET_16X8(srcA, srcG, srcA, srcG, srcA, srcG, srcA, srcG); // [ AAGG AAGG AAGG AAGG ]
    const int16x8_t  srcvRBRB = SET_16X8(srcR, srcB, srcR, srcB, srcR, srcB, srcR, srcB); // [ RRBB RRBB RRBB RRBB ]
    const int16x8_t  srci0A0A = SET_16X8(bfcI, bfcI, bfcI, bfcI, bfcI, bfcI, bfcI, bfcI);         // [ 0I0I 0I0I 0I0I 0I0I ]
          int32x4_t* dstvARGB = reinterpret_cast<int32x4_t*>(toBuffer);
          int32x4_t  dstv4PIX;
          int32x4_t  dstvAGAG;
          int32x4_t  dstvRBRB;

    for(size_t i = 0; i < len4; ++i) {
        // Load 4 pixels
        dstv4PIX =             vld1q_s32  ((int32_t*  ) dstvARGB          ); // [ ARGB ARGB ARGB ARGB ]
        // Process A and G
        dstvAGAG =             vandq_s32  (             dstv4PIX, maskA0G0); // [ A0G0 A0G0 A0G0 A0G0 ]
        dstvAGAG = (int32x4_t) vshrq_n_u32((uint32x4_t) dstvAGAG, 8       ); // [ 0A0G 0A0G 0A0G 0A0G ]
        dstvAGAG = (int32x4_t) vmulq_s16  ((int16x8_t ) dstvAGAG, srci0A0A); // [ AAGG AAGG AAGG AAGG ]
        dstvAGAG = (int32x4_t) vaddq_s16  ((int16x8_t ) dstvAGAG, srcvAGAG); // [ AAGG AAGG AAGG AAGG ]
        dstvAGAG =             vandq_s32  (             dstvAGAG, maskA0G0); // [ A0G0 A0G0 A0G0 AAG0 ]
        // Process R and B
        dstvRBRB =             vandq_s32  (             dstv4PIX, mask0B0R); // [ 0R0B 0R0B 0R0B 0R0B ]
        dstvRBRB = (int32x4_t) vmulq_s16  ((int16x8_t ) dstvRBRB, srci0A0A); // [ RRBB RRBB RRBB RRBB ]
        dstvRBRB = (int32x4_t) vaddq_s16  ((int16x8_t ) dstvRBRB, srcvRBRB); // [ RRBB RRBB RRBB RRBB ]
        dstvRBRB = (int32x4_t) vshrq_n_u32((uint32x4_t) dstvRBRB, 8       ); // [ .R.B .R.B .R.B .R.B ]
        dstvRBRB =             vandq_s32  (             dstvRBRB, mask0B0R); // [ 0R0B 0R0B 0R0B 0R0B ]
        // Store 4 pixels
        dstv4PIX =             vorrq_s32  (             dstvAGAG, dstvRBRB); // [ ARGB ARGB ARGB ARGB ]
                               vst1q_s32  ((int32_t*  ) dstvARGB, dstv4PIX);
        // Increment the destination pointer
        ++dstvARGB;
    }

    length %= 4;
    Pt::uint8_t* dst = reinterpret_cast<Pt::uint8_t*>(dstvARGB);

#else

    Pt::uint8_t* dst = toBuffer;

#endif

    for(size_t i = 0; i < length; ++i) {
        dst[0] = (srcB + bfcI * dst[0]) >> 8;
        dst[1] = (srcG + bfcI * dst[1]) >> 8;
        dst[2] = (srcR + bfcI * dst[2]) >> 8;
        dst[3] = (srcA + bfcI * dst[3]) >> 8;
        dst += 4;
    }
}

// Copy source pixels to destination pixels
inline void fastCopyPixels(Pt::uint8_t* toBuffer, const Pt::uint8_t* fromBuffer, size_t length)
{
#if defined(USE_SSE2)

    const size_t   len4     = length / 4;
    const __m128i* srcvARGB = reinterpret_cast<const __m128i*>(fromBuffer);
          __m128i* dstvARGB = reinterpret_cast<      __m128i*>(toBuffer  );

    for(size_t i = 0; i < len4; ++i) {
        _mm_prefetch(srcvARGB + 1, _MM_HINT_T0);
        _mm_storeu_si128(dstvARGB, _mm_loadu_si128(srcvARGB));
        ++srcvARGB;
        ++dstvARGB;
    }

    const Pt::uint32_t* src = reinterpret_cast<const Pt::uint32_t*>(srcvARGB);
          Pt::uint32_t* dst = reinterpret_cast<      Pt::uint32_t*>(dstvARGB);
          Pt::uint32_t* dsm = dst + length % 4;
    while(dst < dsm) *dst++ = *src++;

#elif defined(USE_NEON)

    const size_t     len4     = length / 4;
    const int32x4_t* srcvARGB = reinterpret_cast<const int32x4_t*>(fromBuffer);
          int32x4_t* dstvARGB = reinterpret_cast<      int32x4_t*>(toBuffer  );

    for(size_t i = 0; i < len4; ++i) {
        vst1q_s32((int32_t*) dstvARGB, vld1q_s32((int32_t*) srcvARGB));
        ++srcvARGB;
        ++dstvARGB;
    }

    const Pt::uint32_t* src = reinterpret_cast<const Pt::uint32_t*>(srcvARGB);
          Pt::uint32_t* dst = reinterpret_cast<      Pt::uint32_t*>(dstvARGB);
          Pt::uint32_t* dsm = dst + length % 4;
    while(dst < dsm) *dst++ = *src++;

#else

    memcpy(toBuffer, fromBuffer, length * 4);

#endif
}

// Blend source pixels to destination pixels
inline void fastBlendPixels(Pt::uint8_t* toBuffer, const Pt::uint8_t* fromBuffer, size_t length)
{
#if defined(USE_SSE2)

    const size_t   len4     = length / 4;
    const __m128i* srcvARGB = reinterpret_cast<const __m128i*>(fromBuffer);
          __m128i* dstvARGB = reinterpret_cast<      __m128i*>(toBuffer  );
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
        srci0A0A = _mm_sub_epi32   (maskA000, srcv0A0A); // [ I000 I000 I000 I000 ]
        srcv0A0A = _mm_or_si128    (                     // [ 0A0A 0A0A 0A0A 0A0A ]
                       _mm_srli_epi32(srcv0A0A,  8),
                       _mm_srli_epi32(srcv0A0A, 24)
                   );
        srci0A0A = _mm_or_si128    (                     // [ 0I0I 0I0I 0I0I 0I0I ]
                       _mm_srli_epi32(srci0A0A,  8),
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

    length %= 4;
    const Pt::uint8_t* src = reinterpret_cast<const Pt::uint8_t*>(srcvARGB);
          Pt::uint8_t* dst = reinterpret_cast<      Pt::uint8_t*>(dstvARGB);

#elif defined(USE_NEON)

    const size_t     len4     = length / 4;
    const int32x4_t* srcvARGB = reinterpret_cast<const int32x4_t*>(fromBuffer);
          int32x4_t* dstvARGB = reinterpret_cast<      int32x4_t*>(toBuffer  );
          int32x4_t  srcv4PIX;
          int32x4_t  srcv0A0A;
          int32x4_t  srci0A0A;
          int32x4_t  srcvAGAG;
          int32x4_t  srcvRBRB;
          int32x4_t  dstv4PIX;
          int32x4_t  dstvAGAG;
          int32x4_t  dstvRBRB;

    for(size_t i = 0; i < len4; ++i) {
        // Load 4 pixels
        srcv4PIX =             vld1q_s32  ((int32_t*  ) srcvARGB                      ); // [ ARGB ARGB ARGB ARGB ]
        dstv4PIX =             vld1q_s32  ((int32_t*  ) dstvARGB                      ); // [ ARGB ARGB ARGB ARGB ]
        // Get the source alpha
        srcv0A0A =             vandq_s32  (             srcv4PIX,             maskA000); // [ A000 A000 A000 A000 ]
        srci0A0A =             vsubq_s32  (             maskA000,             srcv0A0A); // [ I000 I000 I000 I000 ]
        srcv0A0A =             vorrq_s32  (                                              // [ 0A0A 0A0A 0A0A 0A0A ]
                                   (int32x4_t) vshrq_n_u32((uint32x4_t) srcv0A0A,  8),
                                   (int32x4_t) vshrq_n_u32((uint32x4_t) srcv0A0A, 24)
                               );
        srci0A0A =             vorrq_s32  (                                              // [ 0I0I 0I0I 0I0I 0I0I ]
                                   (int32x4_t) vshrq_n_u32((uint32x4_t) srci0A0A,  8),
                                   (int32x4_t) vshrq_n_u32((uint32x4_t) srci0A0A, 24)
                               );
        // Process A and G
        srcvAGAG =             vandq_s32  (             srcv4PIX,             maskA0G0); // [ A0G0 A0G0 A0G0 A0G0 ]
        srcvAGAG = (int32x4_t) vshrq_n_u32((uint32x4_t) srcvAGAG, 8                   ); // [ A0G0 A0G0 A0G0 A0G0 ]
        srcvAGAG = (int32x4_t) vmulq_s16  ((int16x8_t ) srcvAGAG, (int16x8_t) srcv0A0A); // [ AAGG AAGG AAGG AAGG ]
        dstvAGAG =             vandq_s32  (             dstv4PIX,             maskA0G0); // [ A0G0 A0G0 A0G0 A0G0 ]
        dstvAGAG = (int32x4_t) vshrq_n_u32((uint32x4_t) dstvAGAG, 8                   ); // [ 0A0G 0A0G 0A0G 0A0G ]
        dstvAGAG = (int32x4_t) vmulq_s16  ((int16x8_t ) dstvAGAG, (int16x8_t) srci0A0A); // [ AAGG AAGG AAGG AAGG ]
        dstvAGAG = (int32x4_t) vaddq_s16  ((int16x8_t ) dstvAGAG, (int16x8_t) srcvAGAG); // [ AAGG AAGG AAGG AAGG ]
        dstvAGAG =             vandq_s32  (             dstvAGAG,             maskA0G0); // [ A0G0 A0G0 A0G0 AAG0 ]
        // Process R and B
        srcvRBRB =             vandq_s32  (             srcv4PIX,             mask0B0R); // [ 0R0B 0R0B 0R0B 0R0B ]
        srcvRBRB = (int32x4_t) vmulq_s16  ((int16x8_t ) srcvRBRB, (int16x8_t) srcv0A0A); // [ RRBB RRBB RRBB RRBB ]
        dstvRBRB =             vandq_s32  (             dstv4PIX,             mask0B0R); // [ 0R0B 0R0B 0R0B 0R0B ]
        dstvRBRB = (int32x4_t) vmulq_s16  ((int16x8_t ) dstvRBRB, (int16x8_t) srci0A0A); // [ RRBB RRBB RRBB RRBB ]
        dstvRBRB = (int32x4_t) vaddq_s16  ((int16x8_t ) dstvRBRB, (int16x8_t) srcvRBRB); // [ RRBB RRBB RRBB RRBB ]
        dstvRBRB = (int32x4_t) vshrq_n_u32((uint32x4_t) dstvRBRB, 8                   ); // [ .R.B .R.B .R.B .R.B ]
        dstvRBRB =             vandq_s32  (             dstvRBRB,             mask0B0R); // [ 0R0B 0R0B 0R0B 0R0B ]
        // Store 4 pixels
        dstv4PIX =             vorrq_s32  (             dstvAGAG,             dstvRBRB); // [ ARGB ARGB ARGB ARGB ]
                               vst1q_s32  ((int32_t*  ) dstvARGB,             dstv4PIX);
        // Increment the pointers
        ++srcvARGB;
        ++dstvARGB;
    }

    length %= 4;
    const Pt::uint8_t* src = reinterpret_cast<const Pt::uint8_t*>(srcvARGB);
          Pt::uint8_t* dst = reinterpret_cast<      Pt::uint8_t*>(dstvARGB);

#else

    const Pt::uint8_t* src = fromBuffer;
          Pt::uint8_t* dst = toBuffer;

#endif

    for(size_t i = 0; i < length; ++i) {
        const Pt::uint32_t alphaSrc = src[3];
        const Pt::uint32_t alphaInv = 255 - alphaSrc;
        dst[0] = (Pt::uint8_t) ( (alphaSrc * src[0]   + alphaInv * dst[0]) >> 8 );
        dst[1] = (Pt::uint8_t) ( (alphaSrc * src[1]   + alphaInv * dst[1]) >> 8 );
        dst[2] = (Pt::uint8_t) ( (alphaSrc * src[2]   + alphaInv * dst[2]) >> 8 );
        dst[3] = (Pt::uint8_t) ( (alphaSrc * alphaSrc + alphaInv * dst[3]) >> 8 );
        src += 4;
        dst += 4;
    }
}


} // namespace
} // namespace
} // namespace

#endif


/*
---------------------------------------
Result on x86_64 (i5-4460; 64-Bit Mode)
---------------------------------------
Pt::Gfx - CompositionMode::SourceCopy                  Without SSE        With SSE
                                                       ------ --------    ------ -------
                                                       (Time) (Factor)    (Time) (Factor)
                                                       ------ --------    ------ --------
    Solid-filled    polygon          @ ImagePainter  =     62                 57
    Solid-filled    polygon NOAA     @ ImagePainter2 =     51 ( 0.823)        35 ( 0.614)
    Solid-filled    polygon XWAA     @ ImagePainter2 =     99 ( 1.597)        82 ( 1.439)
    Solid-filled    polygon FSAA 2x2 @ ImagePainter2 =    119 ( 1.919)        96 ( 1.684)

    Gradient-filled polygon          @ ImagePainter  =   1085               1019
    Gradient-filled polygon NOAA     @ ImagePainter2 =     62 ( 0.057)        55 ( 0.054)
    Gradient-filled polygon XWAA     @ ImagePainter2 =    128 ( 0.118)       123 ( 0.121)
    Gradient-filled polygon FSAA 2x2 @ ImagePainter2 =    188 ( 0.173)       179 ( 0.176)

    Texture-filled  polygon          @ ImagePainter  =     73                 71
    Texture-filled  polygon NOAA     @ ImagePainter2 =     59 ( 0.808)        62 ( 0.873)
    Texture-filled  polygon XWAA     @ ImagePainter2 =    128 ( 1.753)       141 ( 1.986)
    Texture-filled  polygon FSAA 2x2 @ ImagePainter2 =    186 ( 2.548)       186 ( 2.620)

Pt::Gfx - CompositionMode::SourceOver                  Without SSE        With SSE
                                                       ------ --------    ------ -------
                                                       (Time) (Factor)    (Time) (Factor)
                                                       ------ --------    ------ --------
    Solid-filled    polygon          @ ImagePainter  =    255                117
    Solid-filled    polygon NOAA     @ ImagePainter2 =    174 ( 0.682)        70 ( 0.598)
    Solid-filled    polygon XWAA     @ ImagePainter2 =    251 ( 0.984)       147 ( 1.256)
    Solid-filled    polygon FSAA 2x2 @ ImagePainter2 =    229 ( 0.898)       130 ( 1.111)

    Gradient-filled polygon          @ ImagePainter  =   1200               1129
    Gradient-filled polygon NOAA     @ ImagePainter2 =    218 ( 0.182)       104 ( 0.092)
    Gradient-filled polygon XWAA     @ ImagePainter2 =    301 ( 0.251)       195 ( 0.173)
    Gradient-filled polygon FSAA 2x2 @ ImagePainter2 =    334 ( 0.278)       227 ( 0.201)

    Texture-filled  polygon          @ ImagePainter  =    265                138
    Texture-filled  polygon NOAA     @ ImagePainter2 =    259 ( 0.977)       130 ( 0.942)
    Texture-filled  polygon XWAA     @ ImagePainter2 =    338 ( 1.275)       228 ( 1.652)
    Texture-filled  polygon FSAA 2x2 @ ImagePainter2 =    375 ( 1.415)       251 ( 1.819)


--------------------------------------------------------
Result on v7l (A53; BCM2709; RaspberryPi 3; 32-bit Mode)
--------------------------------------------------------
Pt::Gfx - CompositionMode::SourceCopy                  Without NEON       With NEON
                                                       ------ --------    ------ -------
                                                       (Time) (Factor)    (Time) (Factor)
                                                       ------ --------    ------ --------
    Solid-filled    polygon          @ ImagePainter  =    520                436
    Solid-filled    polygon NOAA     @ ImagePainter2 =    408 ( 0.785)       348 ( 0.798)
    Solid-filled    polygon XWAA     @ ImagePainter2 =    966 ( 1.858)       908 ( 2.083)
    Solid-filled    polygon FSAA 2x2 @ ImagePainter2 =   1151 ( 2.213)      1058 ( 2.427)

    Gradient-filled polygon          @ ImagePainter  =   7057               4344
    Gradient-filled polygon NOAA     @ ImagePainter2 =    552 ( 0.078)       484 ( 0.111)
    Gradient-filled polygon XWAA     @ ImagePainter2 =   1233 ( 0.175)      1168 ( 0.269)
    Gradient-filled polygon FSAA 2x2 @ ImagePainter2 =   1405 ( 0.199)      1321 ( 0.304)

    Texture-filled  polygon          @ ImagePainter  =    637                638
    Texture-filled  polygon NOAA     @ ImagePainter2 =    612 ( 0.961)       655 ( 1.027)
    Texture-filled  polygon XWAA     @ ImagePainter2 =   1362 ( 2.138)      1404 ( 2.201)
    Texture-filled  polygon FSAA 2x2 @ ImagePainter2 =   1682 ( 2.641)      1693 ( 2.654)

Pt::Gfx - CompositionMode::SourceOver                  Without NEON       With NEON
                                                       ------ --------    ------ -------
                                                       (Time) (Factor)    (Time) (Factor)
                                                       ------ --------    ------ -------
    Solid-filled    polygon          @ ImagePainter  =   2002              1072
    Solid-filled    polygon NOAA     @ ImagePainter2 =   1343 ( 0.671)      781 ( 0.729)
    Solid-filled    polygon XWAA     @ ImagePainter2 =   2141 ( 1.069)     1598 ( 1.491)
    Solid-filled    polygon FSAA 2x2 @ ImagePainter2 =   2050 ( 1.024)     1524 ( 1.422)

    Gradient-filled polygon          @ ImagePainter  =   5790              5414
    Gradient-filled polygon NOAA     @ ImagePainter2 =   1721 ( 0.297)     1016 ( 0.188)
    Gradient-filled polygon XWAA     @ ImagePainter2 =   2612 ( 0.451)     1915 ( 0.354)
    Gradient-filled polygon FSAA 2x2 @ ImagePainter2 =   2559 ( 0.442)     1886 ( 0.348)

    Texture-filled  polygon          @ ImagePainter  =   2349              1406
    Texture-filled  polygon NOAA     @ ImagePainter2 =   2366 ( 1.007)     1421 ( 1.011)
    Texture-filled  polygon XWAA     @ ImagePainter2 =   3290 ( 1.401)     2379 ( 1.692)
    Texture-filled  polygon FSAA 2x2 @ ImagePainter2 =   3312 ( 1.410)     2418 ( 1.720)
*/
