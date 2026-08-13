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

class PointI;

/** @brief %Point with floating-point X and Y coordinates.
*/
class Point
{
    public:
        Point()
        : _x(0), _y(0)
        {}

        Point(Float x, Float y)
        : _x(x), _y(y)
        {}

        Point(const Point& pt)
        : _x(pt._x), _y(pt._y)
        {}

        void clear()
        {
            _x = 0;
            _y = 0;
        }

        void set(Float xpos, Float ypos)
        {
            _x = xpos;
            _y = ypos;
        }

        void setX(Float xpos)
        { _x = xpos; }

        void setY(Float ypos)
        { _y = ypos; }

        Float x() const
        { return _x; }

        Float y() const
        { return _y; }

        const Point& addX(Float x)
        {
            _x += x;
            return *this;
        }

        const Point& subX(Float x)
        {
            _x -= x;
            return *this;
        }

        const Point& addY(Float y)
        {
            _y += y;
            return *this;
        }

        const Point& subY(Float y)
        {
            _y -= y;
            return *this;
        }

        const Point& move(Float dx, Float dy)
        {
            _x += dx;
            _y += dy;
            return *this;
        }

        Float distanceTo(const Point& other) const
        {
            if(isEqual(other))
                return 0;

            return hypot(_x - other._x, _y - other._y);
        }

        Point& operator=(const Point& pt)
        {
            _x = pt._x;
            _y = pt._y;
            return *this;
        }

        bool isEqual(const Point& other, Float eps = FloatNearlyZero) const
        {
            return std::abs(_x - other._x) <= eps &&
                   std::abs(_y - other._y) <= eps;
        }

        Point& operator+=(const Point& pt)
        {
            _x += pt._x;
            _y += pt._y;
            return *this;
        }

        Point operator+(const Point& pt) const
        {
            return Point(_x + pt._x, _y + pt._y);
        }

        Point& operator-=(const Point& pt)
        {
            _x -= pt._x;
            _y -= pt._y;
            return *this;
        }

        Point operator-(const Point& pt) const
        {
            return Point(_x - pt._x, _y - pt._y);
        }

        Point operator*(Float factor) const
        {
            return Point(_x * factor, _y * factor);
        }

        Point operator/(Float factor) const
        {
            return Point(_x / factor, _y / factor);
        }

        Point operator+(Float factor) const
        {
            return Point(_x + factor, _y + factor);
        }

        Point operator-(Float factor) const
        {
            return Point(_x - factor, _y - factor);
        }

        Point& operator*=(Float factor)
        {
            _x *= factor;
            _y *= factor;
            return *this;
        }

        Point& operator/=(Float factor)
        {
            _x /= factor;
            _y /= factor;
            return *this;
        }

        /** @brief Returns true if both X and Y are zero.
        */
        bool isOrigin() const
        { return _x == 0 && _y == 0; }

        /** @brief Returns the negation of this point.
        */
        Point operator-() const
        { return Point(-_x, -_y); }

        /** @brief Returns the Euclidean length of the vector from the origin.
        */
        Float length() const
        { return hypot(_x, _y); }

        /** @brief Returns the squared Euclidean length; cheaper than length().
        */
        Float lengthSquared() const
        { return _x * _x + _y * _y; }

        /** @brief Returns a unit-length copy of this point, or a zero point if the length is zero.
        */
        Point toNormalized() const
        {
            const Float len = this->length();
            if(len == 0)
                return Point();
            return Point(_x / len, _y / len);
        }

        /** @brief Constructs from a %PointI by widening the coordinates.
        */
        explicit Point(const PointI& pt);

        /** @brief Assigns from a %PointI by widening the coordinates.
        */
        Point& operator=(const PointI& pt);

        /** @brief Rounds each coordinate to the nearest integer and returns a %PointI.
        */
        PointI round() const;

        /** @brief Floors each coordinate and returns a %PointI.
        */
        PointI floor() const;

        /** @brief Ceils each coordinate and returns a %PointI.
        */
        PointI ceil() const;

    private:
        Float _x;
        Float _y;
};

typedef Point PointF;

/** @brief %Point with integer X and Y coordinates.
*/
class PointI
{
    public:
        PointI()
        : _x(0), _y(0)
        {}

        PointI(Int x, Int y)
        : _x(x), _y(y)
        {}

        PointI(const PointI& pt)
        : _x(pt._x), _y(pt._y)
        {}

        void clear()
        {
            _x = 0;
            _y = 0;
        }

        void set(Int xpos, Int ypos)
        {
            _x = xpos;
            _y = ypos;
        }

        void setX(Int xpos)
        { _x = xpos; }

        void setY(Int ypos)
        { _y = ypos; }

        Int x() const
        { return _x; }

        Int y() const
        { return _y; }

        const PointI& addX(Int x)
        {
            _x += x;
            return *this;
        }

        const PointI& subX(Int x)
        {
            _x -= x;
            return *this;
        }

        const PointI& addY(Int y)
        {
            _y += y;
            return *this;
        }

        const PointI& subY(Int y)
        {
            _y -= y;
            return *this;
        }

        const PointI& move(Int dx, Int dy)
        {
            _x += dx;
            _y += dy;
            return *this;
        }

        PointI& operator=(const PointI& pt)
        {
            _x = pt._x;
            _y = pt._y;
            return *this;
        }

        bool operator==(const PointI& pt) const
        { return (_x == pt._x && _y == pt._y); }

        bool operator!=(const PointI& pt) const
        { return (_x != pt._x || _y != pt._y); }

        PointI& operator+=(const PointI& pt)
        {
            _x += pt._x;
            _y += pt._y;
            return *this;
        }

        PointI operator+(const PointI& pt) const
        {
            return PointI(_x + pt._x, _y + pt._y);
        }

        PointI& operator-=(const PointI& pt)
        {
            _x -= pt._x;
            _y -= pt._y;
            return *this;
        }

        PointI operator-(const PointI& pt) const
        {
            return PointI(_x - pt._x, _y - pt._y);
        }

        PointI operator*(Int factor) const
        {
            return PointI(_x * factor, _y * factor);
        }

        PointI operator+(Int offset) const
        {
            return PointI(_x + offset, _y + offset);
        }

        PointI operator-(Int offset) const
        {
            return PointI(_x - offset, _y - offset);
        }

        PointI& operator*=(Int factor)
        {
            _x *= factor;
            _y *= factor;
            return *this;
        }

        /** @brief Returns true if both X and Y are zero.
        */
        bool isOrigin() const
        { return _x == 0 && _y == 0; }

        /** @brief Returns the negation of this point.
        */
        PointI operator-() const
        { return PointI(-_x, -_y); }

    private:
        Int _x;
        Int _y;
};

inline Point::Point(const PointI& pt)
: _x(static_cast<Float>(pt.x())), _y(static_cast<Float>(pt.y()))
{}

inline Point& Point::operator=(const PointI& pt)
{
    _x = static_cast<Float>(pt.x());
    _y = static_cast<Float>(pt.y());
    return *this;
}

inline PointI Point::round() const
{
    return PointI(static_cast<Int>(std::lround(_x)),
                  static_cast<Int>(std::lround(_y)));
}

inline PointI Point::floor() const
{
    return PointI(static_cast<Int>(std::floor(_x)),
                  static_cast<Int>(std::floor(_y)));
}

inline PointI Point::ceil() const
{
    return PointI(static_cast<Int>(std::ceil(_x)),
                  static_cast<Int>(std::ceil(_y)));
}

} // namespace

} // namespace

#endif

