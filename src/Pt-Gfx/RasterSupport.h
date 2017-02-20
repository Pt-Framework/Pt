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

#include <Pt/Math.h>

#include <Pt/Gfx/Algorithm.h>

#ifndef PT_GFX_RASTERSUPPORT_H
#define PT_GFX_RASTERSUPPORT_H


// ======================================================================================
// ===== Configurations and Macros ======================================================
// ======================================================================================

// Fixed-Point 16.16 Settings
#define FIXED_POINT_SHIFT_FACTOR     16         // Shift factor
#define FIXED_POINT_FRACT_BITMASK    0x0000FFFF // Bit mask for the fractional value; must be (2 ^ FIXED_POINT_SHIFT_FACTOR - 1)
#define FIXED_POINT_CONSTANT_ONE     65536      // The value 1.0  in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR    )
#define FIXED_POINT_CONSTANT_HALF    32768      // The value 0.5  in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / 2)
#define FIXED_POINT_CONSTANT_QUARTER 16384      // The value 0.25 in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / 4)

// Fixed-Point 16.16 Helper Macros
#define FIXED_POINT_IPART(V)        ( (V) & ~FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_FPART(V)        ( (V) &  FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_RFPART(V)       ( FIXED_POINT_FRACT_BITMASK - FIXED_POINT_FPART(V) )
#define FIXED_POINT_ROUND(V)        ( FIXED_POINT_IPART( (V) + FIXED_POINT_CONSTANT_HALF ) )
#define FIXED_POINT_FPART_TO_A8(V)  ( FIXED_POINT_FPART (V) >> 8 )
#define FIXED_POINT_RFPART_TO_A8(V) ( FIXED_POINT_RFPART(V) >> 8 )
#define FIXED_POINT_MUL_TO_A8(A, B) ( ( ( (Pt::uint32_t)(A) * (Pt::uint32_t)(B) + FIXED_POINT_FRACT_BITMASK ) >> FIXED_POINT_SHIFT_FACTOR ) )
#define FIXED_POINT_FROM_INT(V)     ( (V) << FIXED_POINT_SHIFT_FACTOR )
#define FIXED_POINT_TO_INT(V)       ( (V) >> FIXED_POINT_SHIFT_FACTOR )

// Coordinate limit
#define COORDINATE_LIMIT Painter::MaximumCoordinate

// Just for easy and faster debugging ;)
#include <stdio.h>
#define lprintf(...) fprintf (stderr, __VA_ARGS__)


// ======================================================================================
// ===== Support Structures =============================================================
// ======================================================================================

// Specify four pixels with alpha
struct AA4Pixels {
    Pt::int32_t centerX, centerY;
    Pt::int32_t deltaX, deltaY;
    Pt::uint8_t alpha;

    AA4Pixels(Pt::int32_t centerX_, Pt::int32_t centerY_, Pt::int32_t deltaX_, Pt::int32_t deltaY_, Pt::uint8_t alpha_)
    : centerX(centerX_), centerY(centerY_), deltaX(deltaX_), deltaY(deltaY_), alpha(alpha_)
    {}
};

// Specify a span (scanline)
struct AASpan {
    Pt::int32_t from;
    Pt::int32_t to;
    Pt::int32_t pixelY;

    AASpan(Pt::int32_t from_, Pt::int32_t to_, Pt::int32_t pixelY_)
    : from(from_), to(to_), pixelY(pixelY_)
    {}

    bool operator < (const AASpan& ref) const
    {
        if(pixelY < ref.pixelY) return true;
        if(pixelY > ref.pixelY) return false;
        return from < ref.to;
    }
};


// ======================================================================================
// ===== Support Functions ==============================================================
// ======================================================================================

#if defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || defined(_M_ARMT) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(__arm)

// https://en.wikipedia.org/wiki/Fast_inverse_square_root
static inline float fastInvSqrt(float x)
{
    // ### TODO: Check if this function is actually faster in ARM CPUs !!! ###

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

// https://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Babylonian_method
static inline float fastSqrt(float x)
{
    // ### TODO: Check if this function is actually faster and produce the correct result in ARM CPUs !!! ###

    float y = x;

    y = (y + x / y) * 0.5f;
    y = (y + x / y) * 0.5f;

    return y;
}

#else

#define fastInvSqrt(X) (1.0f / sqrtf(X))
#define fastSqrt(X)    sqrtf(X)

#endif

// From https://gist.github.com/volkansalma/2972237
// Original code by Volkan SALMA, 2012
static inline float fastAtan2(float y, float x)
{
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

static inline float convertCartesianToPolar(float x, float y)
{
    // Quadrant I & II
    if(y >= 0)
        return fastAtan2(y, x) * 180 / Pt::Pi;

    // Quadrant III && IV
    return fastAtan2(y, x) * 180 / Pt::Pi + 360;
}

static inline bool insideDegRange(Pt::int32_t x, Pt::int32_t y, Pt::int32_t ctrX, Pt::int32_t ctrY, float degBeg, float degEnd)
{
    const float angle = convertCartesianToPolar(x - ctrX, -(y - ctrY));

    if(degEnd < degBeg) {
        if(angle >= degBeg && angle <= 360   ) return true;
        if(angle >= 0      && angle <= degEnd) return true;
        return false;
    }

    return angle >= degBeg && angle <= degEnd;
}


#endif
