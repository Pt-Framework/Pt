/*
 * Copyright (C) 2006-2011 Marc Duerner
 * Copyright (C) 2010 Aloysius Indrayanto
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef PT_MATH_H
#define PT_MATH_H

#include <Pt/Types.h>
#include <Pt/Api.h>
#include <math.h> // hypot is not in cmath
#include <cassert>

namespace Pt {

//! Various constants for Pi
static const double Pi      = 3.14159265358979323846;  // pi
static const double PiDouble    = 6.28318530717958647692;  // 2*pi
static const double PiHalf = 1.57079632679489661923;  // pi/2
static const double PiQuart = 0.78539816339744830961;  // pi/4
static const double Pi180  = 0.01745329251994329576;  // pi/180
static const double PiSqr  = 9.86960440108935861883449099987615114f; //pi^2

/** @brief Fast sine calculation, not as precise as sin(theta)
 
    theta is required in rad [0, 2*Pi]
    
    In range [0, 2*Pi] max. abs error in fast accurate mode is 0.0015
    
    In range [0, 2*Pi] max. abs error in fast mode is 0.06
*/
template <typename T, bool accurate>
T fastSin(const T& theta)
{
    assert(theta <= PiDouble);
    assert(theta >= 0);
    T localTheta = theta;

    if (localTheta > Pi)
    {
        localTheta -= PiDouble;
    }

    const T B = 4 / Pi;
    const T C = -4 / PiSqr;

    T y = B * localTheta + C * localTheta * ::fabs(localTheta);

    if (accurate)
    {
        //  const float Q = 0.775;
            const T P = 0.225;

            y = P * (y * ::fabs(y) - y) + y;   // Q * y + P * y * abs(y)
    }

    return y;
}

/** @brief Fast cosine calculation, not as precise as cos(theta)
    
    theta is required in rad [0, 2*Pi]
    
    In range [0, 2*Pi] max. abs error in fast accurate mode is 0.0015
    
    In range [0, 2*Pi] max. abs error in fast mode is 0.06
*/
template <typename T, bool accurate>
T fastCos(const T& theta)
{
    assert(theta <= PiDouble);
    assert(theta >= 0);

    T sinTheta = theta + PiHalf;

    if (sinTheta > PiDouble)     // Original x > pi/2
    {
        sinTheta -= PiDouble;   // Wrap: cos(x) = cos(x - 2 pi)
    }

    return fastSin<T, accurate>(sinTheta);
}

/** @brief Return the Euclidean distance of the given values
*/
inline double hypot(double x, double y)
{
    #if defined(_MSC_VER) || defined(_WIN32_WCE) || defined(_WIN32)
        return _hypot(x, y);
    #else
        return ::hypot(x, y);
    #endif
}

} // namespace Pt

#endif // PT_MATH_H
