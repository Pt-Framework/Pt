/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PTm_MATHm_POINT3Dm_H
#define PTm_MATHm_POINT3Dm_H

#include <Pt/Types.h>
#include <Pt/Math/Api.h>
#include <Pt/Math/Math.h>
#include <Pt/SourceInfo.h>

namespace Pt {

    namespace Math {

        /** \brief BasicPoint3D class
        */
        template<typename T>
        class BasicPoint3D {
            public:
                BasicPoint3D()
                : m_x(0)
                , m_y(0)
                , m_z(0)
                {}

                BasicPoint3D(T x, T y, T z)
                : m_x(x)
                , m_y(y)
                , m_z(z)
                {}

                BasicPoint3D(const BasicPoint3D& pt)
                : m_x(pt.m_x)
                , m_y(pt.m_y)
                , m_z(pt.m_z)
                { }

                T x() const
                {
                    return m_x;
                }

                T y() const
                {
                    return m_y;
                }

                T z() const
                {
                    return m_z;
                }

                void setX(T x)
                {
                    m_x = x;
                }

                void setY(T y)
                {
                    m_y = y;
                }

                void setZ(T z)
                {
                    m_z = z;
                }

                void set(T x, T y, T z)
                {
                    m_x = x;
                    m_y = y;
                    m_z = z;
                }

                const BasicPoint3D& move(T dx, T dy, T dz)
                {
                    m_x += dy;
                    m_y += dy;
                    m_z += dz;
                    return *this;
                }

                const BasicPoint3D& operator=(const BasicPoint3D& pt)
                {
                    m_x = pt.m_x;
                    m_y = pt.m_y;
                    m_z = pt.m_z;
                    return *this;
                }

                bool operator==(const BasicPoint3D& pt) const
                {
                    return (m_x == pt.m_x && m_y == pt.m_y && m_z == pt.m_z);
                }

                bool operator!=(const BasicPoint3D& pt) const
                {
                    return (m_x != pt.m_x || m_y != pt.m_y || m_z != pt.m_z);
                }

                bool operator>(const BasicPoint3D& pt) const
                {
                    if ( m_x < pt.m_x || m_y < pt.m_y || m_z < pt.m_z )
                    {
                        return false;
                    }

                    return ( (*this) != pt );
                }

                bool operator<(const BasicPoint3D& pt) const
                {
                    if ( m_x > pt.m_x || m_y > pt.m_y || m_y > pt.m_y )
                    {
                        return false;
                    }

                    return ( pt != (*this) );
                }

                inline const BasicPoint3D operator+=(const BasicVector3d<T>& vec)
                {
                    m_x += vec.x();
                    m_y += vec.y();
                    m_z += vec.z();
                    return *this;
                }

                inline BasicPoint3D operator+(const BasicVector3d<T>& vec) const
                {
                    return BasicPoint3D( (m_x+vec.x()), (m_y+vec.y()), (m_z+vec.z()) );
                }

                inline const BasicPoint3D operator-=(const BasicVector3d<T>& vec)
                {
                    m_x -= vec.x();
                    m_y -= vec.y();
                    m_z -= vec.z();
                    return *this;
                }

                inline BasicPoint3D operator-(const BasicVector3d<T>& vec) const
                {
                    return BasicPoint3D( (m_x-vec.x()), (m_y-vec.y()), (m_z-vec.z()) );
                }

                inline const BasicPoint3D operator+=(const BasicPoint3D<T>& pt)
                {
                    m_x += pt.x();
                    m_y += pt.y();
                    m_y += pt.z();
                    return *this;
                }

                inline BasicPoint3D operator+(const BasicPoint3D<T>& pt) const
                {
                    return BasicPoint3D( (m_x+pt.x()), (m_y+pt.y()), (m_z+pt.z()) );
                }

                inline const BasicPoint3D operator-=(const BasicPoint3D<T>& pt)
                {
                    m_x -= pt.x();
                    m_y -= pt.y();
                    m_z -= pt.z();
                    return *this;
                }

                inline BasicPoint3D operator-(const BasicPoint3D<T>& pt) const
                {
                    return BasicPoint3D( (m_x-pt.x()), (m_y-pt.y()), (m_z-pt.z()) );
                }

                inline BasicPoint3D operator*(const double factor) const
                {
                    return BasicPoint3D( (T)(m_x * factor), (T)(m_y * factor), (T)(m_z * factor) );
                }

            protected:
                T m_x;
                T m_y;
                T m_z;
        };


        /** @brief functor to compare to points.

            First point is smaller as second if the x value
            is smaller or the x values are equal and the
            y value of first point is smaller.
        */
        class PointCompareFunctorXYZ
        {
        public:
            bool operator()(const Pt::Math::Point3D& pt1, const Pt::Math::Point3D& pt2) const
            {
                if( (pt1.x() < pt2.x()) ||
                    ( (pt1.x() == pt2.x()) && (pt1.y() < pt2.y()) ) ||
                    ( (pt1.x() == pt2.x()) && (pt1.y() == pt2.y()) && (pt1.z() < pt2.z()) ) )
                {
                        return true;
                }
                return false;
            }
        };

    } // namespace Math

} // namespace Pt

#endif

