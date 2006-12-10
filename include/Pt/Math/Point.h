/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_MATH_POINT_H
#define PT_MATH_POINT_H

#include <Pt/Types.h>
#include <Pt/Api.h>
#include <Pt/Math/Math.h>


namespace Pt {

	namespace Math {

		/** \brief BasicPoint class
		*/
		template<typename T>
		class PT_API BasicPoint {
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

			protected:
				T _x;
				T _y;
		};

	} // namespace Math

} // namespace Pt

#endif

