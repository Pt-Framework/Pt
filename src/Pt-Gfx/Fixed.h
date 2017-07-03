/* Copyright (C) 2017-2017 Aloysius Indrayanto
   Copyright (C) 2006-2017 Marc Boris Duerner
   Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan

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
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301 USA
*/

#ifndef PT_GFX_FIXED_H
#define PT_GFX_FIXED_H

// Fixed-Point 16.16 Settings
#define FIXED_POINT_SHIFT_FACTOR     16         // Shift factor
#define FIXED_POINT_FRACT_BITMASK    0x0000FFFF // Bit mask for the fractional value; must be (2 ^ FIXED_POINT_SHIFT_FACTOR - 1)
#define FIXED_POINT_CONSTANT_ONE     65536      // The value 1.0       in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR           )
#define FIXED_POINT_CONSTANT_HALF    32768      // The value 0.5       in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / 2       )
#define FIXED_POINT_CONSTANT_QUARTER 16384      // The value 0.25      in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / 4       )
#define FIXED_POINT_CONSTANT_ISQRT2  46341      // The value 1/sqrt(2) in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR / sqrt(2) )
#define FIXED_POINT_CONSTANT_SQRT2   92682      // The value sqrt(2)   in fixed-point ( 2 ^ FIXED_POINT_SHIFT_FACTOR * sqrt(2) )

// Fixed-Point 16.16 Helper Macros
#define FIXED_POINT_IPART(V)        ( (V) & ~FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_FPART(V)        ( (V) & FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_RFPART(V)       ( FIXED_POINT_FRACT_BITMASK - FIXED_POINT_FPART(V) )
#define FIXED_POINT_ROUND(V)        ( FIXED_POINT_IPART( (V) + FIXED_POINT_CONSTANT_HALF ) )
#define FIXED_POINT_FLOOR(V)        ( (V) & ~FIXED_POINT_FRACT_BITMASK )
#define FIXED_POINT_CEIL(V)         ( ((V) | FIXED_POINT_FRACT_BITMASK) + 1)
#define FIXED_POINT_FPART_TO_A8(V)  ( FIXED_POINT_FPART (V) >> 8 )
#define FIXED_POINT_RFPART_TO_A8(V) ( FIXED_POINT_RFPART(V) >> 8 )
#define FIXED_POINT_MUL_TO_A8(A, B) ( ( ( (Pt::uint32_t)(A) * (Pt::uint32_t)(B) + FIXED_POINT_FRACT_BITMASK ) >> FIXED_POINT_SHIFT_FACTOR ) )
#define FIXED_POINT_FROM_FLT(V)     ( Pt::lround( ( (V) * ( (float) FIXED_POINT_CONSTANT_ONE ) ) ) )
#define FIXED_POINT_FROM_INT(V)     ( (V) << FIXED_POINT_SHIFT_FACTOR )
#define FIXED_POINT_TO_INT(V)       ( (V) >> FIXED_POINT_SHIFT_FACTOR )

#endif
