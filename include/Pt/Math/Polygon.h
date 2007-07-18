/***************************************************************************
 *   Copyright (C) 2007 PTV AG                                             *
 ***************************************************************************/

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

    void addPoint(BasicPoint<PointT> point)
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


    template <typename T>
    inline bool contains(const BasicPoint<T>& point) const
    {
        if (this->size() < 3)
        {
            return false;
        }

        bool result = false;

        BasicPoint<PointT> last = _points.back();
        typename std::vector< BasicPoint<PointT> >::const_iterator it;

        for (it = _points.begin(); it != _points.end(); ++it)
        {
            BasicPoint<PointT> current = *it;

            if ((current.y() <= point.y() && point.y() < last.y()) ||
                (last.y()    <= point.y() && point.y() < current.y()))
            {
                // y-value of point lies between y-value of last and y-value of current point.
                // Now check if the x-value of the point is left of the line (between last point
                // and current point). If so, we passed a line and thus have to invert our result.
                double xPositionOnLine = (last.x() - current.x()) * (point.y() - current.y())
                                       / double(last.y() - current.y()) + current.x();

                if (double(point.x()) <= xPositionOnLine)
                {
                    result = !result;
                }
            }

            last = current;
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
