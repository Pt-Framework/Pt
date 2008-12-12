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

#ifndef PTV_GFX_VECTOR3D_H
#define PTV_GFX_VECTOR3D_H

#include <Pt/Types.h>
#include <Pt/Math/Api.h>
#include <Pt/Math/Math.h>
#include <math.h>


namespace Pt {

namespace Math {

    /** \brief BasicVector3d class
    */
    template <typename T>
    class BasicVector3d {

        public:

            BasicVector3d():
                m_x(0),
                m_y(0),
                m_z(0)
            {
            }


            BasicVector3d(const BasicVector3d<T>& vector)
            {
                this->m_x = vector.m_x;
                this->m_y = vector.m_y;
                this->m_z = vector.m_z;
            }


            BasicVector3d(const T& x, const T& y, const T& z)
            {
                this->m_x = x;
                this->m_y = y;
                this->m_z = z;
            }

/*
            BasicVector3d(const BasicPoint3d& startPt, const BasicPoint3d& endPt)
            {
                m_x = entPt.x - startPt.x;
                m_x = entPt.y - startPt.y;
                m_x = entPt.z - startPt.z;
            }

            void set(const BasicPoint3d& startPt, const BasicPoint3d& endPt)
            {
                m_x = entPt.x - startPt.x;
                m_x = entPt.y - startPt.y;
                m_x = entPt.z - startPt.z;
            }

*/

            void normalize()
            {
                double val = (double)(this->m_x * this->m_x + this->m_y * this->m_y + this->m_z * this->m_z);
                double length = sqrt(val);
                if( (length != 0.0) && (length != 1.0) )
                {
                    this->m_x = (T)(this->m_x / length);
                    this->m_y = (T)(this->m_y / length);
                    this->m_z = (T)(this->m_z / length);
                }
            }


            BasicVector3d<T> crossProduct(const BasicVector3d<T>& vector)
            {
                BasicVector3d<T> resVector;

                resVector.m_x = this->m_y * vector.z - this->m_z * vector.y;
                resVector.m_y = this->m_z * vector.x - this->m_x * vector.z;
                resVector.m_z = this->m_x * vector.y - this->m_y * vector.x;

                return resVector;
            }


            const BasicVector3d<T>& operator=(const BasicVector3d<T>& vector)
            {
                this->m_x = vector.m_x;
                this->m_y = vector.m_y;
                this->m_z = vector.m_z;
                return *this;
            }


            bool operator==(const BasicVector3d<T>& vector) const
            {
                return (this->m_x == vector.m_x) && (this->m_y == vector.m_y) && (this->m_z == vector.m_z);
            }


            bool operator!=(const BasicVector3d<T>& vector) const
            {
                return (this->m_x != vector.m_x) || (this->m_y != vector.m_y) || (this->m_z != vector.m_z);
            }


            const BasicVector3d<T>& operator+=(const BasicVector3d<T>& vector)
            {
                this->m_x += vector.m_x;
                this->m_y += vector.m_y;
                this->m_z += vector.m_z;
                return *this;
            }


            BasicVector3d<T> operator+(const BasicVector3d<T>& vector)
            {
                BasicVector3d<T> resVector(*this);
                resVector += vector;
                return resVector;
            }

            const BasicVector3d<T>& operator-=(const BasicVector3d<T>& vector)
            {
                this->m_x -= vector.m_x;
                this->m_y -= vector.m_y;
                this->m_z -= vector.m_z;
                return *this;
            }


            BasicVector3d<T> operator-(const BasicVector3d<T>& vector)
            {
                BasicVector3d<T> resVector(*this);
                resVector -= vector;
                return resVector;
            }


            const BasicVector3d<T>& operator*=(const double& factor)
            {
                this->m_x = (T)(this->m_x * factor);
                this->m_y = (T)(this->m_y * factor);
                this->m_z = (T)(this->m_z * factor);
                return *this;
            }


            BasicVector3d<T> operator*(const double& factor)
            {
                BasicVector3d<T> resVector(*this);
                resVector *= factor;
                return resVector;
            }


            /** \brief Calculate the scalar product of two vectors.
                \param
                \return
            */
            double operator*(const BasicVector3d<T>& vector)
            {
                return this->m_x * vector.m_x + this->m_y * vector.m_y + this->m_z * vector.m_z;
            }


            double length() const
            {
                double val = (double)(this->m_x * this->m_x + this->m_y * this->m_y + this->m_z * this->m_z);
                return sqrt(val);
            }


            double sqrtLength() const
            {
                return (double)(this->m_x * this->m_x + this->m_y * this->m_y + this->m_z * this->m_z);
            }


            T x() const
            {
                return this->m_x;
            }


            T y() const
            {
                return this->m_y;
            }


            T z() const
            {
                return this->m_z;
            }


            void setX(const T& x)
            {
                this->m_x = x;
            }


            void setY(const T& y)
            {
                this->m_y = y;
            }


            void setZ(const T& z)
            {
                this->m_z = z;
            }

            void set(const T& x, const T& y, const T& z)
            {
                this->m_x = x;
                this->m_y = y;
                this->m_z = z;
            }

        protected:
            T m_x;
            T m_y;
            T m_z;

};

} // namespace Math

} // namespace Pt

#endif // PTV_GFX_VECTOR3D_H
