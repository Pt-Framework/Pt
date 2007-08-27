/***************************************************************************
 *   Copyright (C) 2007 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_MATH_LINE_SEGMENT_H
#define PT_MATH_LINE_SEGMENT_H

#include <Pt/Math/Api.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/Math.h>
#include <math.h>
#include <stdexcept>
#include <algorithm>
#include <limits>

namespace Pt {
namespace Math {

    //! \brief A generic line segment class.
    template<typename PointT>
    class BasicLineSegment
    {
        public:
            //! Construct a BasicLineSegment specified by two points of type BasicPoint<PointT>.
            BasicLineSegment(const BasicPoint<PointT>& p1, const BasicPoint<PointT>& p2)
            {
                setPoints(p1, p2);
            }

            // No default constructor available as it would most likely set the points
            // to (0, 0) and (0, 0) which is invalid.

            double length() const
            {
                return _p1.calcDistance(_p2);
            }

            const BasicPoint<PointT>& p1() const
            {
                return _p1;
            }

            const BasicPoint<PointT>& p2() const
            {
                return _p2;
            }

            /**
             * @brief Sets both points of this line segments.
             *
             * The points provided must not be identical as this is invalid for
             * a line segments which must have a length greater than 0. If both
             * points are identical an std::invalid_argument exception is thrown.
             *
             * @param p1 The point to which the first point of this line segment is set.
             * @param p1 The point to which the second point of this line segment is set.
             * @throws std::invalid_argument if both points are identical.
             */
            void setPoints(const BasicPoint<PointT>& p1, const BasicPoint<PointT>& p2)
            {
                if (p1 == p2)
                {
                    throw std::invalid_argument("The points to construct a line from must not be identical." + PT_SOURCEINFO);
                }
                
                _p1 = p1;
                _p2 = p2;
            }
            
            bool operator==(const BasicLineSegment& other) const
            {
                return (other._p1 == this->_p1 && other._p2 == this->_p2)
                    || (other._p2 == this->_p1 && other._p1 == this->_p2);
            }

            bool operator!=(const BasicLineSegment& other) const
            {
                return !(other == *this);
            }
            
            bool isVertical() const
            {
                return _p1.x() == _p2.x();
            }
            
            /**
             * @brief Returns the slope of the line or numeric_limits<double>::infinity()
             * if it is a vertical line.
             *
             * In case this line is a vertical line, the slope is inifinite. Because of
             * this the method returns the value numeric_limits<double>::infinity() which
             * is defined as being the infinite value of double in Standard C++.
             *
             * @return The slope of this line or numeric_limits<double>::infinity( )
             * if it is a vertical line.
             */
            double slope() const
            {
                if (isVertical())
                {
                    return std::numeric_limits<double>::infinity();
                }
                
                return (_p2.y() - _p1.y()) / double(_p2.x() - _p1.x());
            }
            
            template <typename T>
            inline PointF calcNearestPointOnLine(const BasicPoint<T>& point) const
            {
                PointF perp = calcPerpendicular(point);
                
                if (inBounds(perp))
                {
                    return perp;
                }
                
                // Perpendicular is not in line bounds. Thus we have to calculate
                // the nearest of the two line's points and return this.
                const double dx1 = perp.x() - _p1.x();
                const double dy1 = perp.y() - _p1.y();
                const double dx2 = perp.x() - _p2.x();
                const double dy2 = perp.y() - _p2.y();
                
                if (dx1 * dx1 + dy1 * dy1 < dx2 * dx2 + dy2 * dy2)
                {
                    return _p1;
                }
                else
                {
                    return _p2;
                }
            }

            inline double calcDistance(const PointF& point) const
            {
                const PointF nearestPoint = calcNearestPointOnLine(point);
                return nearestPoint.calcDistance(point);
            }
            
            template <typename T>
            inline bool inBounds(const BasicPoint<T>& point) const
            {
                return point.x() <= std::max(_p1.x(), _p2.x())
                    && point.x() >= std::min(_p1.x(), _p2.x())
                    && point.y() <= std::max(_p1.y(), _p2.y())
                    && point.y() >= std::min(_p1.y(), _p2.y());
            }

        
        protected:

            // You can not calculate a perpendiclar on a line segment, but only
            // on (straight) lines. Use calcNearestPointOnLine() instead. This
            // method is only used internally by said method.
            template <typename T>
            inline PointF calcPerpendicular(const BasicPoint<T>& point) const
            {
                if (point == _p1 || point == _p2)
                {
                    return point;
                }

                if (_p1.x() == _p2.x())
                {
                    // Special case: x1 and x2 are the same. Perendicular is P(x, y) where
                    // x is the x-value of 'line.p1' or 'line.p2' and y is the y-value of 'point'.
                    return PointF(_p1.x(), point.y());
                }
                
                if (_p1.y() == _p2.y())
                {
                    // Special case: y1 and y2 are the same. Perendicular is P(x, y) where
                    // x is the x-value of 'point' and y is the y-value of 'line.p1' or 'line.p2'.
                    return PointF(point.x(), _p1.y());
                }

                const double slope = (_p2.y() - _p1.y()) / double(_p2.x() - _p1.x());  // Slope of 'line'.
                
                assert(slope != 0);
                
                const double reverseSlope = -1 / slope;
                
                assert(slope - reverseSlope);

                const double b  = _p1.y()   - slope        * _p1.x();
                const double bt = point.y() - reverseSlope * point.x();

                const double perpX = (bt - b) / (slope - reverseSlope);
                const double perpY = slope * perpX + b;

                return PointF(perpX, perpY);
            }
            

        protected:
            BasicPoint<PointT> _p1;
            BasicPoint<PointT> _p2;

    };

} // namespace Math
} // namespace Pt

#endif
