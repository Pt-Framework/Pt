#ifndef PT_MATH_POINT_H
#define PT_MATH_POINT_H

#include <Pt/Math/Api.h>
#include <Pt/Math/Math.h>
#include <Pt/Math/MathUtils.h>

#include <Pt/Types.h>
#include <Pt/SourceInfo.h>
#include <Pt/SerializationInfo.h>

#include <vector>


namespace Pt {

    namespace Math {

        /** @brief BasicPoint class
        */
        template<typename T>
        class BasicPoint {
            public:
                BasicPoint()
                : _x(0)
                , _y(0)
                {}

                BasicPoint(T x, T y)
                : _x(x)
                , _y(y)
                {}

                BasicPoint(const BasicPoint& pt)
                : _x(pt._x)
                , _y(pt._y)
                { }

                T x() const
                { return _x; }

                T y() const
                { return _y; }

                void setX(T x_)
                {_x = x_; }

                void setY(T y_)
                {_y = y_; }

                void set(T x_, T y_)
                {
                    _x = x_;
                    _y = y_;
                }

                const BasicPoint& addX(T x)
                {
                  _x +=  x;
                  return *this;
                }

                const BasicPoint& subX(T x)
                {
                  _x -=  x;
                  return *this;
                }
                const BasicPoint& addY(T y)
                {
                  _y +=  y;
                  return *this;
                }

                const BasicPoint& subY(T y)
                {
                  _y -=  y;
                  return *this;
                }

                const BasicPoint& move(T dx, T dy)
                {
                    _x += dy; _y += dy; return *this;
                }


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

                inline const BasicPoint operator+=(const BasicVector2d<T>& vec)
                {
                    _x += vec.x();
                    _y += vec.y();
                    return *this;
                }

                inline BasicPoint operator+(const BasicVector2d<T>& vec) const
                {
                    return BasicPoint( (_x+vec.x()), (_y+vec.y()) );
                }

                inline const BasicPoint operator-=(const BasicVector2d<T>& vec)
                {
                    _x -= vec.x();
                    _y -= vec.y();
                    return *this;
                }

                inline BasicPoint operator-(const BasicVector2d<T>& vec) const
                {
                    return BasicPoint( (_x-vec.x()), (_y-vec.y()) );
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


        /** @brief functor to compare to points.

            First point is smaller as second if the x value
            is smaller or the x values are equal and the
            y value of first point is smaller.
        */
        class PointCompareFunctorXY
        {
        public:
            bool operator()(const Pt::Math::Point& pt1, const Pt::Math::Point& pt2) const
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
            si.addValue("x", Pt::Variant(static_cast<Pt::uint16_t>(point.x())));
            si.addValue("y", Pt::Variant(static_cast<Pt::uint16_t>(point.y())));
            si.setTypeName("Point");
        }

        /** @brief serialization BasicPoint
         */
        template <typename T>
        inline void operator <<=(Pt::SerializationInfo& si, const BasicPoint<T>& point)
        {
            si.addValue("x", Pt::Variant(point.x()));
            si.addValue("y", Pt::Variant(point.y()));
            si.setTypeName("Point");
        }

        /** @brief deserialization BasicPoint<Pt::uint8_t>
         */
        inline void operator >>=(const Pt::SerializationInfo& si, BasicPoint<Pt::uint8_t>& point)
        {
            Pt::uint16_t x = si.getValue<Pt::uint16_t>("x");
            Pt::uint16_t y = si.getValue<Pt::uint16_t>("y");

            point.setX( static_cast<Pt::uint8_t>(x) );
            point.setY( static_cast<Pt::uint8_t>(y)) ;
        }

        /** @brief deserialization BasicPoint
         */
        template <typename T>
        inline void operator >>=(const Pt::SerializationInfo& si, BasicPoint<T>& point)
        {
            T x = si.toValue<T>("x");
            T y = si.toValue<T>("y");

            point.setX(x);
            point.setY(y);
        }

    } // namespace Math

} // namespace Pt

#endif

