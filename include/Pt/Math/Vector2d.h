/*
 * Copyright (C) 2006 PTV AG
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef PTV_GFX_VECTOR2D_H
#define PTV_GFX_VECTOR2D_H

#include <Pt/Types.h>
#include <Pt/Math/Api.h>
#include <Pt/Math/Math.h>
#include <Pt/Math/MathUtils.h>
#include <math.h>


namespace Pt {

namespace Math {

    /** \brief BasicVector2d class
    */
    template <typename T>
    class BasicVector2d {

        public:

            BasicVector2d():
                m_x(0),
                m_y(0)
            {
            }


            BasicVector2d(const BasicVector2d<T>& vector)
            {
                this->m_x = vector.m_x;
                this->m_y = vector.m_y;
            }


            BasicVector2d(const T& x, const T& y):
                 m_x(x),
                 m_y(y)
            {
            }

            template<typename PointType>
            BasicVector2d(const BasicPoint<PointType>& startPt, const BasicPoint<PointType>& endPt)
            : m_x( (T)(endPt.x() - startPt.x()) )
            , m_y( (T)(endPt.y() - startPt.y()) )
            {
            }

            template<typename PointType>
            inline void set(const BasicPoint<PointType>& startPt, const BasicPoint<PointType>& endPt)
            {
                m_x = (T)(endPt.x() - startPt.x());
                m_y = (T)(endPt.y() - startPt.y());
            }

            void normalize()
            {
                double val = (double)(this->m_x * this->m_x + this->m_y * this->m_y);
                double length = sqrt(val);
                if( (length != 0.0) && (length != 1.0) )
                {
                    this->m_x = (T)(this->m_x / length);
                    this->m_y = (T)(this->m_y / length);
                }
            }


            inline const BasicVector2d<T>& operator=(const BasicVector2d<T>& vector)
            {
                this->m_x = vector.m_x;
                this->m_y = vector.m_y;
                return *this;
            }


            inline bool operator==(const BasicVector2d<T>& vector) const
            {
                return (this->m_x == vector.m_x) && (this->m_y == vector.m_y);
            }


            inline bool operator!=(const BasicVector2d<T>& vector) const
            {
                return (this->m_x != vector.m_x) || (this->m_y != vector.m_y);
            }


            inline const BasicVector2d<T>& operator+=(const BasicVector2d<T>& vector)
            {
                this->m_x += vector.m_x;
                this->m_y += vector.m_y;
                return *this;
            }


            inline BasicVector2d<T> operator+(const BasicVector2d<T>& vector)
            {
                BasicVector2d<T> resVector(*this);
                resVector += vector;
                return resVector;
            }

            inline const BasicVector2d<T>& operator-=(const BasicVector2d<T>& vector)
            {
                this->m_x -= vector.m_x;
                this->m_y -= vector.m_y;
                return *this;
            }


            inline BasicVector2d<T> operator-(const BasicVector2d<T>& vector)
            {
                BasicVector2d<T> resVector(*this);
                resVector -= vector;
                return resVector;
            }


            template <typename valT>
            const BasicVector2d<T>& operator*=(const valT& factor)
            {
                this->m_x = (T)(this->m_x * factor);
                this->m_y = (T)(this->m_y * factor);
                return *this;
            }


            template <typename valT>
            inline BasicVector2d<T> operator*(const valT& factor)
            {
                BasicVector2d<T> resVector(*this);
                resVector *= factor;
                return resVector;
            }


            /** \brief Calculate the scalar product of two vectors.
                \param vector a vector
                \return the result
            */
            inline double operator*(const BasicVector2d<T>& vector)
            {
                return this->m_x * vector.m_x + this->m_y * vector.m_y;
            }


            inline const BasicVector2d<T> operator+=(const BasicPoint<T>& point)
            {
                m_x += point.x();
                m_y += point.y();
                return *this;
            }


            inline BasicVector2d<T> operator+(const BasicPoint<T>& point) const
            {
                BasicVector2d<T> newVector( (m_x+point.x()), (m_y+point.y()) );
                return newVector;
            }


            inline const BasicVector2d<T> operator-=(const BasicPoint<T>& point)
            {
                m_x -= point.x();
                m_y -= point.y();
                return *this;
            }


            inline BasicVector2d<T> operator-(const BasicPoint<T>& point) const
            {
                BasicVector2d<T> newVector( (m_x-point.x()), (m_y-point.y()) );
                return newVector;
            }


            inline double length() const
            {
                double val = (double)(this->m_x * this->m_x + this->m_y * this->m_y);
                return sqrt(val);
            }


            inline double sqrtLength() const
            {
                return (double)(this->m_x * this->m_x + this->m_y * this->m_y);
            }


            /**
             * @brief Calculates and returns the angle of this vector towards north.
             */
            inline double angleTowardsNorth()
            {
                const double angleInDegrees = Pt::Math::radToDeg(atan2((double)m_y, (double)m_x));
                const double normalizedAngle = 360 + 90 - angleInDegrees;

                return normalizedAngle >= 360 ? normalizedAngle - 360 : normalizedAngle;
            }


            inline T x() const
            {
                return this->m_x;
            }


            inline T y() const
            {
                return this->m_y;
            }


            inline void setX(const T& x)
            {
                this->m_x = x;
            }


            inline void setY(const T& y)
            {
                this->m_y = y;
            }

            inline void set(const T& x, const T& y)
            {
                this->m_x = x;
                this->m_y = y;
            }

        protected:
            T m_x;
            T m_y;

};

} // namespace Math

} // namespace Pt

#endif // PTV_GFX_VECTOR2D_H
