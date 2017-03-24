/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2015 Marc Boris Duerner

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

#ifndef PT_GFX_MATH_H
#define PT_GFX_MATH_H


#include <Pt/Math.h>

#include <Pt/Gfx/Api.h>

#include <Pt/Gfx/SIMDConfig.h>


namespace Pt {
namespace Gfx {
namespace Math {


//
// The constant Pi and its variations in 32-bit floating-point
//

static const float Pi       = 3.14159265f;
static const float PiMul2   = 6.28318531f;
static const float PiDiv2   = 1.57079633f;
static const float PiDiv4   = 0.78539816f;
static const float PiDiv180 = 0.01745329f;
static const float PiSqr    = 9.86960440f;


//
// The real implementation
//

// NOTE: They are separated from the real public API so that we can benchmark them easily

inline float fastSqrt_impl(float x) // X86_64 => MUCH SLOWER | ARM => SLOWER
{
    // Using algorithm from: Methods of Computing Square Roots
    //                       https://en.wikipedia.org/wiki/Methods_of_computing_square_roots
    //                       Last modified on February 25, 2017

    union {
        float       f;
        Pt::int32_t i;
    } u;

    u.f = x;
    u.i = (1 << 29) + (u.i >> 1) - (1 << 22) - 0x0004C000;
    u.f = (u.f + x / u.f) * 0.5;

    return u.f;
}

//__m128 nr = _mm_rsqrt_ps( x );
//__m128 muls = _mm_mul_ps( _mm_mul_ps( x, nr ), nr );
//result = _mm_mul_ps( _mm_mul_ps( half, nr ), _mm_sub_ps( three, muls ) );

inline float fastSqrt_impl_SIMD(float x) // X86_64 => SLIGHTLY SLOWER | ARM => SLIGHTLY FASTER
{
#if defined(PT_GFX_USE_SSE1)
    return _mm_cvtss_f32( _mm_rcp_ss( _mm_rsqrt_ss( _mm_load_ss( &x ) ) ) );
#elif defined(PT_GFX_USE_NEON)
    return vgetq_lane_f32( vrecpeq_f32( vrsqrteq_f32( vld1q_dup_f32( &x ) ) ), 0 );
#else
    return ::sqrtf(x);
#endif

}

inline float fastInvSqrt_impl(float x) // X86_64 => SLOWER | ARM => SLIGHTLY FASTER
{
    // Using algorithm from: Fast Inverse Square Root
    //                       https://en.wikipedia.org/wiki/Fast_inverse_square_root
    //                       Last modified on February 17, 2017

    const float x2 = x * 0.5f;

    union {
        float       f;
        Pt::int32_t i;
    } u;

    u.f = x;
    u.i = 0x5F3759DF - ( u.i >> 1 );
    u.f = u.f * ( 1.5f - ( x2 * u.f * u.f ) );

    return u.f;
}

inline float fastInvSqrt_impl_SIMD(float x) // X86_64 => FASTER | ARM => FASTER
{
#if defined(PT_GFX_USE_SSE1)
    return _mm_cvtss_f32( _mm_rsqrt_ss( _mm_load_ss( &x ) ) );
#elif defined(PT_GFX_USE_NEON)
    return vgetq_lane_f32( vrsqrteq_f32( vld1q_dup_f32( &x ) ), 0 );
#else
    return 1.0f / ::sqrtf(x);
#endif
}

inline float fastSin_impl(float x) // X86_64 => FASTER | ARM => MUCH FASTER
{
    if (x > Gfx::Math::Pi) x -= Gfx::Math::PiMul2;

    const float b =  4 / Gfx::Math::Pi;
    const float c = -4 / Gfx::Math::PiSqr;
    const float p = 0.225;
    const float y = b * x + c * x * ::fabs(x);

    return p * (y * ::fabs(y) - y) + y;
}

inline float fastCos_impl(float x) // X86_64 => FASTER | ARM => MUCH FASTER
{
    x += Gfx::Math::PiDiv2;
    if(x > Gfx::Math::PiMul2) x -= Gfx::Math::PiMul2;

    return Gfx::Math::fastSin_impl(x);
}

inline float fastAtan2_impl(float y, float x) // X86_64 => FASTER | ARM => FASTER
{

    // Based on: atan2_approximation.c
    //           https://gist.github.com/volkansalma/2972237
    //           Original code by Volkan SALMA, 2012

    if(x == 0.0f) {
        if(y >  0.0f) return Gfx::Math::PiDiv2;
        if(y == 0.0f) return 0.0f;
        return -Gfx::Math::PiDiv2;
    }

    const float z = y / x;
          float atan;

    if(fabs(z) < 1.0f) {
        atan = z / (1.0f + 0.28f * z * z);
        if(x < 0.0f) {
            if(y < 0.0f) return atan - Gfx::Math::Pi;
            return atan + Gfx::Math::Pi;
        }
    }

    else {
        atan = Gfx::Math::PiDiv2 - z / (z * z + 0.28f);
        if(y < 0.0f) return atan - Gfx::Math::Pi;
    }

    return atan;
}


//
// The public API
//

// NOTE: Defined using the benchmark result of the above functions versus the native library implementations

inline float fastSqrt(float x)
{
#if defined(PT_GFX_USE_NEON)
    return fastSqrt_impl_SIMD(x);
#else
    return ::sqrtf(x);
#endif
}

inline float fastInvSqrt(float x)
{
#if defined(PT_GFX_USE_SSE1) || defined(PT_GFX_USE_NEON)
    return fastInvSqrt_impl_SIMD(x);
#elif defined(PT_GFX_USE_ARM_CPU)
    return fastInvSqrt_impl(x);
#else
    return 1.0f / ::sqrtf(x);
#endif
}

inline float fastSin(float x)
{ return Gfx::Math::fastSin_impl(x); }

inline float fastCos(float x)
{ return Gfx::Math::fastCos_impl(x); }

inline float fastAtan2(float y, float x)
{ return Gfx::Math::fastAtan2_impl(y, x); }

inline float convertCartesianToPolarCoordinate(float x, float y)
{
    // Quadrant I & II
    if(y >= 0)
        return Gfx::Math::fastAtan2(y, x) * 180.0f / Gfx::Math::Pi;

    // Quadrant III && IV
    return Gfx::Math::fastAtan2(y, x) * 180.0f / Gfx::Math::Pi + 360.0f;
}


} // namespace
} // namespace
} // namespace

#endif
