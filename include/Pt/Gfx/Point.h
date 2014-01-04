/*
 * Copyright (C) 2006 PTV AG
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef PT_GFX_POINT_H
#define PT_GFX_POINT_H

#include <Pt/Gfx/Api.h>

#include <Pt/Types.h>
#include <Pt/SourceInfo.h>
#include <Pt/SerializationInfo.h>

#include <vector>


namespace Pt {

    namespace Gfx {

        /** @brief BasicPoint class
        */
        template<typename T>
        class BasicPoint {
            public:
                //! @brief Construct a BasicPoint of (0,0)
                BasicPoint()
                : _x(0), _y(0)
                {}

                //! @brief Construct a BasicPoint of (x,y)
                BasicPoint(T x, T y)
                : _x(x), _y(y)
                {}

                //! @brief Construct a BasicPoint from another BasicPoint
                BasicPoint(const BasicPoint& pt)
                : _x(pt._x), _y(pt._y)
                { }

                //! @brief Set the X and Y components of the BasicPoint
                void set(T x_, T y_)
                {
                    _x = x_;
                    _y = y_;
                }

                //! @brief Set the X component of the BasicPoint
                void setX(T x_)
                {_x = x_; }

                //! @brief Set the Y component of the BasicPoint
                void setY(T y_)
                {_y = y_; }

                //! @brief Return the X component of the BasicPoint
                T x() const
                { return _x; }

                //! @brief Return the Y component of the BasicPoint
                T y() const
                { return _y; }

                //! @brief Increment the X component of the BasicPoint by the given value
                const BasicPoint& addX(T x)
                {
                  _x +=  x;
                  return *this;
                }

                //! @brief Decrement the X component of the BasicPoint by the given value
                const BasicPoint& subX(T x)
                {
                  _x -=  x;
                  return *this;
                }
                //! @brief Increment the Y component of the BasicPoint by the given value
                const BasicPoint& addY(T y)
                {
                  _y +=  y;
                  return *this;
                }

                //! @brief Decrement the Y component of the BasicPoint by the given value
                const BasicPoint& subY(T y)
                {
                  _y -=  y;
                  return *this;
                }

                //! @brief Move the BasicPoint as far as th given the X and Y distances
                const BasicPoint& move(T dx, T dy)
                {
                    _x += dx; _y += dy; return *this;
                }


                //! @brief Calculate distance between this BasicPoint and the given BasicPoint
                template<typename T2>
                T calcDistance(const BasicPoint<T2>& otherPoint) const
                {
                    if (*this == otherPoint)
                    {
                        return 0;
                    }

                    return (T)(hypot(this->x() - otherPoint.x(), this->y() - otherPoint.y()));
                }


                const BasicPoint& operator=(const BasicPoint& pt)
                {
                    _x = pt._x; _y = pt._y;
                    return *this;
                }

                bool operator==(const BasicPoint& pt) const
                { return (_x == pt._x && _y == pt._y); }

                bool operator!=(const BasicPoint& pt) const
                { return (_x != pt._x || _y != pt._y); }

                bool operator>(const BasicPoint& pt) const
                {
                    if ( _x < pt._x || _y < pt._y)
                        return false;

                    return ( (*this) != pt );
                }

                bool operator<(const BasicPoint& pt) const
                {
                    if ( _x > pt._x || _y > pt._y )
                        return false;

                    return ( pt != (*this) );
                }

                inline const BasicPoint operator+=(const BasicPoint<T>& pt)
                {
                    _x += pt.x();
                    _y += pt.y();
                    return *this;
                }

                inline BasicPoint operator+(const BasicPoint<T>& pt) const
                {
                    return BasicPoint( (_x+pt.x()), (_y+pt.y()) );
                }

                inline const BasicPoint operator-=(const BasicPoint<T>& pt)
                {
                    _x -= pt.x();
                    _y -= pt.y();
                    return *this;
                }

                inline BasicPoint operator-(const BasicPoint<T>& pt) const
                {
                    return BasicPoint( (_x-pt.x()), (_y-pt.y()) );
                }

                inline BasicPoint operator*(const double factor) const
                {
                    return BasicPoint( (T)(_x * factor), (T)(_y * factor) );
                }

            protected:
                T _x;
                T _y;
        };

        typedef BasicPoint<Pt::ssize_t>  Point;
        typedef BasicPoint<double>       PointF;



        /** @brief functor to compare to points.
          *
          * The first point is defined as smaller than the second one if the x value
          * is smaller or the x values are equal and the y value of first point is smaller.
          */
        class PointCompareFunctorXY
        {
        public:
            bool operator()(const Pt::Gfx::Point& pt1, const Pt::Gfx::Point& pt2) const
            {
                if( (pt1.x() < pt2.x()) ||
                    ( (pt1.x() == pt2.x()) && (pt1.y() < pt2.y()) ) )
                {
                    return true;
                }
                return false;
            }
        };


        /** @brief serialization BasicPoint<Pt::uint8_t>
         *
         * The type Pt::uint8_t is defined to unsinged char. To make sure the
         * numbers are not interpreted as unsigned char, a cast to Pt::uint16_t
         * is done.
         */
        inline void operator <<=(Pt::SerializationInfo& si, const BasicPoint<Pt::uint8_t>& point)
        {
            si.addMember("x") <<=  static_cast<Pt::uint16_t>(point.x());
            si.addMember("y") <<= static_cast<Pt::uint16_t>(point.y());
            si.setTypeName("Point");
        }

        /** @brief serialization BasicPoint
         */
        template <typename T>
        inline void operator <<=(Pt::SerializationInfo& si, const BasicPoint<T>& point)
        {
            si.addMember("x") <<= point.x();
            si.addMember("y") <<= point.y();
            si.setTypeName("Point");
        }

        /** @brief deserialization BasicPoint<Pt::uint8_t>
         */
        inline void operator >>=(const Pt::SerializationInfo& si, BasicPoint<Pt::uint8_t>& point)
        {
            Pt::uint16_t x, y;
            si.getMember("x") >>= x;
            si.getMember("y") >>= y;

            point.setX( static_cast<Pt::uint8_t>(x) );
            point.setY( static_cast<Pt::uint8_t>(y)) ;
        }

        /** @brief deserialization BasicPoint
         */
        template <typename T>
        inline void operator >>=(const Pt::SerializationInfo& si, BasicPoint<T>& point)
        {
            T x, y;
            si.getMember("x") >>= x;
            si.getMember("y") >>= y;

            point.setX(x);
            point.setY(y);
        }

        /** @brief serialization of a vector of BasicPoint
         */
        template <typename T>
        inline void operator <<=(Pt::SerializationInfo& si, const std::vector<BasicPoint<T> >& points)
        {
            typename std::vector<BasicPoint<T> >::const_iterator it;
            for(it = points.begin(); it != points.end(); ++it)
            {
                si.addMember("Point") <<= *it;
            }
        }

        /** @brief deserialization of a vector of BasicPoint
         */
        template <typename T>
        inline void operator >>=(const Pt::SerializationInfo& si, std::vector< BasicPoint<T> >& points)
        {
            Pt::SerializationInfo::ConstIterator it;
            for (it = si.begin(); it != si.end(); ++it)
            {
                points.resize( points.size() + 1 );
                *it >>= points.back();
            }
        }


    } // namespace Gfx

} // namespace Pt

#endif

