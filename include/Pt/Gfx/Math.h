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

namespace Pt {
namespace Gfx {
namespace Math {


// The real implementation
// NOTE: They are separated from the real public API so that we can benchmark them easily

inline float fastSqrt_impl(float x)
{
    // NOTE: This function is NOT actually faster on any CPU

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

inline float fastInvSqrt_impl(float x)
{
    // NOTE: This function is only slightly faster on an ARM CPU

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

inline float fastSin_impl(float x)
{
    if (x > Pt::Pi) x -= Pt::PiDouble;

    const float b =  4 / Pt::Pi;
    const float c = -4 / Pt::PiSqr;
    const float p = 0.225;
    const float y = b * x + c * x * ::fabs(x);

    return p * (y * ::fabs(y) - y) + y;
}

inline float fastCos_impl(float x)
{
    x += Pt::PiHalf;
    if(x > Pt::PiDouble) x -= Pt::PiDouble;

    return Gfx::Math::fastSin_impl(x);
}

inline float fastAtan2_impl(float y, float x)
{

    // Based on: atan2_approximation.c
    //           https://gist.github.com/volkansalma/2972237
    //           Original code by Volkan SALMA, 2012

    if(x == 0.0f) {
        if(y >  0.0f) return Pt::PiHalf;
        if(y == 0.0f) return 0.0f;
        return -Pt::PiHalf;
    }

    const float z = y / x;
          float atan;

    if(fabs(z) < 1.0f) {
        atan = z / (1.0f + 0.28f * z * z);
        if(x < 0.0f) {
            if(y < 0.0f) return atan - Pt::Pi;
            return atan + Pt::Pi;
        }
    }

    else {
        atan = Pt::PiHalf - z / (z * z + 0.28f);
        if(y < 0.0f) return atan - Pt::Pi;
    }

    return atan;
}


// The public API
// NOTE: Defined using the benchmark result of the above functions versus the native library implementations

inline float fastSqrt(float x)
{ return ::sqrtf(x); }

#if defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || defined(_M_ARMT) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(__arm)

inline float fastInvSqrt(float x)
{ return Gfx::Math::fastInvSqrt_impl(x); }

#else

inline float fastInvSqrt(float x)
{ return 1.0f / ::sqrtf(x); }

#endif

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
        return Gfx::Math::fastAtan2(y, x) * 180 / Pt::Pi;

    // Quadrant III && IV
    return Gfx::Math::fastAtan2(y, x) * 180 / Pt::Pi + 360;
}


} // namespace
} // namespace
} // namespace

#endif
