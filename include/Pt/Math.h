/*
 * Copyright (C) 2006-2017 Marc Duerner
 * Copyright (C) 2010-2017 Aloysius Indrayanto
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
#include <iostream>
#include <cmath>
#include <cassert>
#include <math.h> // hypot

//#include <x86intrin.h>
//#include <intrin.h>

namespace Pt {

/** @brief The constant pi.
*/
template<typename T>
T pi();

/** @brief The constant pi*2.
*/
template<typename T>
T piDouble();

/** @brief The constant pi/2.
*/
template<typename T>
T piHalf();

/** @brief The constant pi/4.
*/
template<typename T>
T piQuart();

/** @brief The constant pi^2.
*/
template<typename T>
T piSquare();


template<>
inline float pi<float>()
{
  return 3.14159265f;
}

template<>
inline float piDouble<float>()
{
  return 6.28318531f;
}

template<>
inline float piHalf<float>()
{
  return 1.57079633f;
}

template<>
inline float piQuart<float>()
{
  return 0.78539816f;
}

template<>
inline float piSquare<float>()
{
  return 9.86960440f;
}


template<>
inline double pi<double>()
{
  return 3.14159265358979323846;
}

template<>
inline double piDouble<double>()
{
  return 6.28318530717958647692;
}

template<>
inline double piHalf<double>()
{
  return 1.57079632679489661923;
}

template<>
inline double piQuart<double>()
{
  return 0.78539816339744830961;
}

template<>
inline double piSquare<double>()
{
  return 9.86960440108935861883449099987615114;
}


static const float DegToRadF = 0.01745329f;

static const float RadToDegF = 57.2957795f;

static const double DegToRad = 0.0174532925199432957692;

static const double RadToDeg = 57.295779513082320876846364344191;


inline float degToRad(float deg)
{
    return deg * DegToRadF;
}

inline double degToRad(double deg)
{
    return deg * DegToRad;
}


inline float radToDeg(float rad)
{
    return rad * RadToDegF;
}

inline double radToDeg(double rad)
{
    return rad * RadToDeg;
}


/** @brief Fast, but less precise sine calculation.

    The @a theta is required in rad [0, 2*Pi]. In the range [0, 2*Pi], the
    max. abs error is 0.0015.
*/
template <typename T>
T fastSin(const T& theta)
{
    //return sin(theta);

    assert(theta <= piDouble<T>());
    assert(theta >= 0);

    T localTheta = theta;

    if(localTheta > pi<T>())
    {
        localTheta -= piDouble<T>();
    }

    const T B = 4 / pi<T>();
    const T C = -4 / piSquare<T>();
    //const float Q = 0.775;
    const T P = static_cast<T>(0.225);

    T y = B * localTheta + C * localTheta * ::fabs(localTheta);
    y = P * (y * std::fabs(y) - y) + y;   // Q * y + P * y * abs(y)
    return y;
}

/** @brief Fast, less precise cosine calculation.

    The @a theta is required in rad [0, 2*Pi]. In the range [0, 2*Pi], the
    max. abs error is 0.0015.
*/
template <typename T>
T fastCos(const T& theta)
{
    //return cos(theta);

    assert(theta <= piDouble<T>());
    assert(theta >= 0);

    T sinTheta = theta + piHalf<T>();

    if(sinTheta > piDouble<T>())     // Original x > pi/2
    {
        sinTheta -= piDouble<T>();   // Wrap: cos(x) = cos(x - 2 pi)
    }

    return fastSin(sinTheta);
}

/** @brief Fast, but less precise atan2 calculation.
*/
template <typename T>
T fastAtan2(T y, T x)
{ 
    if(x == 0.0)
    {
        if(y >  0) return piHalf<T>();
        if(y == 0) return 0;
        return -piHalf<T>();
    }

    const T z = y / x;
    T atan = 0.0;

    if(std::fabs(z) < 1.0)
    {
        atan = z / (static_cast<T>(1.0) + static_cast<T>(0.28) * z * z);

        if(x < 0.0)
        {
            return y < 0 ? atan - pi<T>()
                         : atan + pi<T>();
        }
    }
    else
    {
        atan = piHalf<T>() - z / (z * z + static_cast<T>(0.28));
        if(y < 0.0)
            return atan - pi<T>();
    }

    return atan;
}

inline float toPolar(float x, float y)
{
    // Quadrant I and II
    if(y >= 0)
        return radToDeg( fastAtan2(y, x) );

    // Quadrant III and IV
    return radToDeg( fastAtan2(y, x) ) + 360.0f;
}

template <typename T>
T invSqrt(T x)
{
    return static_cast<T>(1.0) / std::sqrt(x);
}

inline float invSqrtf(float x)
{
    return 1.0f / std::sqrt(x);
}

/** @brief Return the euclidean distance of the given values.
*/
inline double hypot(double x, double y)
{
#if __cplusplus == 201103L
    return std::hypot(x, y);
#elif defined(_MSC_VER) || defined(_WIN32_WCE) || defined(_WIN32)
    return _hypot(x, y);
#else
    return ::hypot(x, y);
#endif
}


/** @brief Rounds to nearest integer value.
*/
inline Pt::int32_t lround(float x)
{
#if __cplusplus >= 201103L

    return std::lround(x);

#elif (_MSC_VER < 1800)

    Pt::int32_t tmp = static_cast<Pt::int32_t>(x);
    tmp += (x - tmp >= 0.5) - (x - tmp <= -0.5);
    return tmp;

#else

    return ::lround(x);

#endif

//
// asm below uses bankers rounding
//
//#elif defined(_MSC_VER) && defined (_M_IX86)
//
//    Pt::int32_t tmp;
//    __asm fld x
//    __asm fistp tmp
//    return tmp;
//
//#elif ( defined(__GNUC__) || defined(__clang__) ) &&
//      ( defined(__i386) || defined(__x86_64__) )
//
//    Pt::int32_t tmp;
//    __asm__ __volatile__ (
//        "flds   %1\n\t"
//        "fistpl %0    "
//        : "=m"(tmp)
//        :  "m"(x)
//        : "memory"
//    );
//    return tmp;
//
//
//#elif ( defined(__GNUC__) || defined(__clang__) ) &&
//        defined(__arm__)
//
//    float       tmp;
//    Pt::int32_t res;
//    __asm__ __volatile__ ( "ftosis %0, %1" : "=w" (tmp) : "w" (x) );
//    __asm__ __volatile__ ( "fmrs   %0, %1" : "=r" (res) : "w" (tmp) );
//    return res;
    
}

/** @brief Rounds to nearest integer value.
*/
inline Pt::int32_t lround(double x)
{
#if __cplusplus >= 201103L

    return std::lround(x);

#elif (_MSC_VER < 1800)

    Pt::int32_t tmp = static_cast<Pt::int32_t>(x);
    tmp += (x - tmp >= 0.5) - (x - tmp <= -0.5);
    return tmp;

#else

    return ::lround(x);

#endif

//
// asm below uses bankers rounding
//
//#elif defined(_MSC_VER) || defined (_M_IX86)
//
//    Pt::int32_t tmp;
//    __asm fld x
//    __asm fistp tmp
//    return tmp;
//
//#elif ( defined(__GNUC__) || defined(__clang__) ) &&
//      ( defined(__i386) || defined(__x86_64__) )
//
//    Pt::int32_t tmp;
//    __asm__ __volatile__ (
//        "fldl   %1\n\t"
//        "fistpl %0    "
//        : "=m"(tmp)
//        :  "m"(x)
//        : "memory"
//    );
//    return tmp;
//
//#elif ( defined(__GNUC__) || defined(__clang__) ) &&
//        defined(__arm__)
//
//    float       tmp;
//    Pt::int32_t res;
//    __asm__ __volatile__ ( "ftosid %0, %P1" : "=w" (tmp) : "w" (x) );
//    __asm__ __volatile__ ( "fmrs   %0, %1"  : "=r" (res) : "w" (tmp) );
//    return res;
}

} // namespace Pt

#endif // PT_MATH_H
