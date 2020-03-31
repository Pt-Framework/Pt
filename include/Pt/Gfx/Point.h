/* Copyright (C) 2006-2015 Laurentiu-Gheorghe Crisan
   Copyright (C) 2006-2015 Marc Boris Duerner
   Copyright (C) 2010 Aloysius Indrayanto

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

#ifndef PT_GFX_POINT_H
#define PT_GFX_POINT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Math.h>
#include <Pt/Types.h>

namespace Pt {

namespace Gfx {


/** @brief %Point with X ynd X coordinates.
*/
template<typename T>
class BasicPoint
{
    public:
        typedef T ValueT;

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
            _x += dx;
            _y += dy;
            return *this;
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

        BasicPoint operator-(const BasicPoint<T>& pt) const
        {
            return BasicPoint( (_x-pt.x()), (_y-pt.y()) );
        }


        BasicPoint operator*(T factor) const
        {
            return BasicPoint(_x * factor, _y * factor);
        }

        BasicPoint operator/(T factor) const
        {
            return BasicPoint(_x / factor, _y / factor);
        }

        BasicPoint operator+(T factor) const
        {
            return BasicPoint(_x + factor, _y + factor);
        }

        BasicPoint operator-(T factor) const
        {
            return BasicPoint(_x - factor, _y - factor);
        }

        BasicPoint& operator*=(T factor)
        {
            _x *= factor;
            _y *= factor;
            return *this;
        }

        BasicPoint& operator/=(T factor)
        {
            _x /= factor;
            _y /= factor;
            return *this;
        }

        BasicPoint& operator+(T factor)
        {
            _x += factor;
            _y += factor;
            return *this;
        }

        BasicPoint& operator-(T factor)
        {
            _x -= factor;
            _y -= factor;
            return *this;
        }

    protected:
        T _x;
        T _y;
};


typedef BasicPoint<Pt::ssize_t> Point;
typedef BasicPoint<double> PointF;
//typedef BasicPoint<float> PointF;


inline Point round(const PointF& r)
{
  return Point( lround(r.x()),
                lround(r.y()) );
}


} // namespace

} // namespace

#endif

