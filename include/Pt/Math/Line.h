/***************************************************************************
 *   Copyright (C) 2007 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_MATH_LINE_H
#define PT_MATH_LINE_H

#include <Pt/Math/Api.h>
#include <Pt/Math/Point.h>
#include <Pt/Math/Math.h>
#include <math.h>
#include <stdexcept>
#include <algorithm>
#include <limits>

namespace Pt {
namespace Math {

    //! \brief A generic straight line class
    template<typename PointT>
    class BasicLine {
        public:
            //! Construct a BasicLine specified by two points of type BasicPoint<PointT>.
            BasicLine(const BasicPoint<PointT>& p1, const BasicPoint<PointT>& p2)
            : _p1(p1)
            , _p2(p2)
            {
                if (p1 == p2)
                {
                    throw std::invalid_argument("The points to construct a line from must not be identical.");
                }
            }

            // No default constructor available as it would most likely set the points
            // to (0, 0) and (0, 0) which is invalid.

            const BasicPoint<PointT>& p1() const
            {
                return _p1;
            }

            void setP1(const BasicPoint<PointT>& p1)
            {
                _p1 = p1;
            }
            
            const BasicPoint<PointT>& p2() const
            {
                return _p2;
            }

            void setP2(const BasicPoint<PointT>& p2)
            {
                _p2 = p2;
            }
            
            template <typename T>
            bool operator==(const BasicLine<T>& other) const
            {
                // Special case: One of the lines is a vertical line.
                if (other.isVertical() || this->isVertical())
                {
                    return other.isVertical() && this->isVertical() && other.p1().x() == this->_p1.x();
                }
            
                return other.slope()         == this->slope()
                    && other.axisIntercept() == this->axisIntercept();
            }
            
            template <typename T>
            bool operator!=(const BasicLine<T>& other) const
            {
                return !(other == *this);
            }
            
            double axisIntercept() const
            {
                const PointT x0 = _p1.x();
                const PointT y0 = _p1.y();
                const PointT x1 = _p2.x();
                const PointT y1 = _p2.y();
                
                return y0 - x0 * (y1 - y0) / double(x1 - x0);
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
             * @return The slope of this line or numeric_limits<double>::infinity()
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

                const double slope = this->slope();  // Slope of 'line'.
                
                assert(slope != 0);
                
                const double reverseSlope = -1 / slope;
                
                assert(slope - reverseSlope);

                const double b  = _p1.y()   - slope        * _p1.x();
                const double bt = point.y() - reverseSlope * point.x();

                const double perpX = (bt - b) / (slope - reverseSlope);
                const double perpY = slope * perpX + b;

                return PointF(perpX, perpY);
            }
            
            inline double calcDistance(const PointF& point) const
            {
                const PointF perpendicular = calcPerpendicular(point);
                return perpendicular.calcDistance(point);
            }
            
        protected:
            BasicPoint<PointT> _p1;
            BasicPoint<PointT> _p2;

    };

} // namespace Math
} // namespace Pt

#endif
