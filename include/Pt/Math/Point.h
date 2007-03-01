/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_MATH_POINT_H
#define PT_MATH_POINT_H

#include <Pt/Types.h>
#include <Pt/AnyTraits.h>
#include <Pt/SourceInfo.h>
#include <Pt/Char.h>
#include <Pt/Math/Api.h>
#include <Pt/Math/Math.h>

#include <iostream>
#include <stdexcept>


namespace Pt {

    namespace Math {

        /** \brief BasicPoint class
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
                { _x += dy; _y += dy; return *this; }

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

    } // namespace Math



    template <typename T>
    struct AnyTraits<Math::BasicPoint<T> > {
        static void output(std::ostream& os, const Math::BasicPoint<T>& value);
        static void input(std::istream& is, Math::BasicPoint<T>& value);
        static void output(std::basic_ostream<Pt::Char>& os, const Math::BasicPoint<T>& value);
        static void input(std::basic_istream<Pt::Char>& is, Math::BasicPoint<T>& value);
    };


    template <typename CharT, typename T>
    inline void outputGeneric(std::basic_ostream<CharT>& os, const Math::BasicPoint<T>& value)
    {
        os << '(' << value.x() << ' ' << value.y() << ')';
    }

    template <typename T>
    inline void Pt::AnyTraits<Math::BasicPoint<T> >::output(std::ostream& os, const Math::BasicPoint<T>& value)
    {
        outputGeneric(os, value);
    }


    template <typename T>
    inline void Pt::AnyTraits<Math::BasicPoint<T> >::output(std::basic_ostream<Pt::Char>& os, const Math::BasicPoint<T>& value)
    {
        outputGeneric(os, value);
    }


    template <typename CharT, typename T>
    inline void inputGeneric(std::basic_istream<CharT>& is, Math::BasicPoint<T>& value)
    {
        CharT ch;

        is >> ch;
        if (ch != '(')
        {
            throw std::runtime_error("Could not read Point value" + PT_SOURCEINFO);
        }

        T x;
        T y;

        is >> x;
        is >> y;

        is >> ch;
        if (ch != ')')
        {
            throw std::runtime_error("Could not read Point value" + PT_SOURCEINFO);
        }

        value.set(x, y);
    }


    template <typename T>
    inline void Pt::AnyTraits<Math::BasicPoint<T> >::input(std::istream& is, Math::BasicPoint<T>& value)
    {
        inputGeneric(is, value);
    }


    template <typename T>
    inline void Pt::AnyTraits<Math::BasicPoint<T> >::input(std::basic_istream<Pt::Char>& is, Math::BasicPoint<T>& value)
    {
        inputGeneric(is, value);
    }


} // namespace Pt

#endif

