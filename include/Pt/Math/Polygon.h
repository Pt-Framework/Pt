/*
 * Copyright (C) 2007 PTV AG
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

#ifndef PT_MATH_POLYGON_H
#define PT_MATH_POLYGON_H

#include <Pt/Math/Api.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/Math.h>

#include <vector>

namespace Pt {
namespace Math {

//! \brief A generic line segment class.
template<typename PointT>
class BasicPolygon
{
public:
    //! A basic class for polygons
    BasicPolygon(const std::vector< BasicPoint<PointT> >& pointList)
    : _points(pointList)
    {
    }

    BasicPolygon(Pt::size_t initialCapacity)
    {
        _points.reserve(initialCapacity);
    }

    BasicPolygon()
    {
        _points.reserve(2);
    }

    typename std::vector< BasicPoint<PointT> >::const_iterator begin() const
    {
        return _points.begin();
    }

    typename std::vector< BasicPoint<PointT> >::const_iterator end() const
    {
        return _points.end();
    }

    void addPoint(const BasicPoint<PointT>& point)
    {
        _points.push_back(point);
    }

    template <typename T>
    bool operator==(const BasicPolygon<T>& other) const
    {
        if (other.size() != this->size())
        {
            return false;
        }

        typename std::vector< BasicPoint<T> >::const_iterator      otherIt = other.points().begin();
        typename std::vector< BasicPoint<PointT> >::const_iterator thisIt  = this->_points.begin();

        while (thisIt != this->_points.end())
        {
            if (otherIt->x() != thisIt->x() || otherIt->y() != thisIt->y())
            {
                return false;
            }

            ++otherIt;
            ++thisIt;
        }

        return true;
    }


    template <typename T>
    bool operator!=(const BasicPolygon<T>& other) const
    {
        return !(*this == other);
    }


    Pt::size_t size() const
    {
        return _points.size();
    }

    void clear()
    {
        _points.clear();
    }

    const std::vector< BasicPoint<PointT> >& points() const
    {
        return _points;
    }


    /**
     * @brief Tests if a point lies in a the polygon.
     *
     * This algorithm of the point in polygon test return true if the point lies
     * within the polygon and if the point lies accurate on one border of the
     * polygon.
     *
     * @param point The point to test.
     * @return \c true if the point lies within the polygon and false otherwise.
     */
    template <typename T>
    inline bool contains(const BasicPoint<T>& point) const
    {
        // polygon has not three points at least -> no test necessary
        if (this->size() < 3)
        {
            return false;
        }

        bool result = false;

        // this is a implementation of the "crossing number algorithm" / "even-odd rule algorithm"
        // it based on the Jordan curve theorem
        BasicPoint<PointT> previous = _points.back();
        typename std::vector< BasicPoint<PointT> >::const_iterator it;

        for (it = _points.begin(); it != _points.end(); ++it)
        {
            const BasicPoint<PointT>& current = *it;

            // First check if y-value of point lies between y-value of previous and y-value of current point.
            // After this check if the x-value of the point is left of the line (between previous point
            // and current point). If so, we passed a line and thus have to invert our result.
            if ((current.y() <= point.y() && point.y() < previous.y()) ||
                (previous.y() <= point.y() && point.y() < current.y()))
            {
                // "two point equation" for a line: 
                // (y - y0) = (y1 - y0) / (x1 - x0) * (x - x0)
                // convert the equation that x is on one side:
                // x =  (x1 - x0) * (y - y0) / (y1 - y0) + x0
                double xPositionOnLine = (double(previous.x()) - current.x()) * (point.y() - current.y())
                                         / (previous.y() - current.y()) + current.x();

                
                // check if point lies on the left side of the current line
                if (double(point.x()) < xPositionOnLine)
                {
                    result = !result;
                }
            }

            previous = current;
        }

        return result;
    }


    const BasicPoint<PointT>& operator[](const Pt::size_t index) const
    {
        return _points[index];
    }

protected:
    std::vector< BasicPoint<PointT> > _points;
};

} // namespace Math
} // namespace Pt

#endif
