/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_MATHUTILS_H
#define PT_MATHUTILS_H

#include <Pt/Types.h>
#include <Pt/Math/Api.h>
#include <Pt/Math/Math.h>
#include <Pt/Math/Point.h>
#include <cmath>
#include <cassert>
#ifdef __QNX__
#include <math.h>
#endif

namespace Pt {

    namespace Math {

        //! Natural const
        static const double PI      = 3.14159265358979323846; // pi
        static const double PI_2    = 6.28318530717958647692; // 2*pi
        static const double PI_HALF = 1.57079632679489661923; // pi/2
        static const double PI_QUAT = 0.78539816339744830961; // pi/4
        static const double PI_180  = 0.01745329251994329576; // pi/180
        static const double PI_SQR  = 9.86960440108935861883449099987615114f; //pi^2


        //! Converts angle from degree [0, 360] to radiant [0, 2*pi]
        inline double degToRad(double angle)
        {
            return (angle * PI) / 180.0;
        }

        //! Converts radiant [0, 2*pi] to degree [0, 360]
        inline double radToDeg(double angle)
        {
            return (angle * 180.0) / PI;
        }

        // accuracy for equal comparison
        static const double Eps9    = 10e-9;
        static const double Eps6    = 10e-6;
        static const double Eps3    = 10e-3;
        static const double Eps1    = 10e-1;

        template<typename T>
        inline bool equal(const T& val, const T& val2, const double& eps)
        {
            double diff = fabs(val - val2);
            return diff < eps;
        }

        /**
         * @brief Fast sine calculation, not as precise as sin(theta)
         *
         * theta is required in rad [0, 2*PI]
         *
         * In range [0, 2*PI] max. abs error in fast accurate mode is 0.0015
         *
         * In range [0, 2*PI] max. abs error in fast mode is 0.06
         */
        template <typename T, bool accurate>
        T fastSin(const T& theta)
        {
            assert(theta <= PI_2);
            assert(theta >= 0);
            T localTheta = theta;

            if (localTheta > PI)
            {
                localTheta -= PI_2;
            }

            const T B = 4 / PI;
            const T C = -4 / PI_SQR;

            T y = B * localTheta + C * localTheta * fabs(localTheta);

            if (accurate)
            {
             //  const float Q = 0.775;
                 const T P = 0.225;

                 y = P * (y * fabs(y) - y) + y;   // Q * y + P * y * abs(y)
            }

            return y;
        }

        /**
         * @brief Fast cosine calculation, not as precise as cos(theta)
         *
         * theta is required in rad [0, 2*PI]
         *
         * In range [0, 2*PI] max. abs error in fast accurate mode is 0.0015
         *
         * In range [0, 2*PI] max. abs error in fast mode is 0.06
         */
        template <typename T, bool accurate>
        T fastCos(const T& theta)
        {
            assert(theta <= PI_2);
            assert(theta >= 0);

            T sinTheta = theta + PI_HALF;

            if (sinTheta > PI_2)     // Original x > pi/2
            {
                sinTheta -= PI_2;   // Wrap: cos(x) = cos(x - 2 pi)
            }

            return fastSin<T, accurate>(sinTheta);
        }


        inline double hypot(double x, double y)
        {
            #if defined(_MSC_VER) || defined(_WIN32_WCE) || defined(_WIN32)
                return _hypot(x, y);
            #else
                return ::hypot(x, y);
            #endif
        }

        inline int round(double d)
        {
            return static_cast<int>(d<0 ? d-.5 : d+.5);
        }

        template <typename T>
        inline BasicPoint<T> getCenterOfLine( const BasicPoint<T>& begin, const BasicPoint<T>& end )
        {
            return (begin + ( (end - begin) * 0.5) );
        }


    } // namespace Math

} // namespace Pt

#endif // PT_MATHUTILS_H
