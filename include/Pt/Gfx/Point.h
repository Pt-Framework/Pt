/***************************************************************************
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef Pt_Gfx_Point_h
#define Pt_Gfx_Point_h

#include <Pt/Api.h>
#include <Pt/Types.h>


namespace Pt {

	namespace Gfx {

		/** \brief Point class
		*/
		class PT_EXPORT Point {
			public:
				Point(size_t x = 0, size_t y = 0)
				: _x(x), _y(y)
				{}
	
				Point(const Point& pt)
				: _x(pt._x), _y(pt._y)
				{ }

				size_t x() const
				{ return _x; }

				size_t y() const
				{ return _y; }

				void setX(int x_)
				{_x = x_; }

				void setY(int y_)
				{_y = y_; }

				const Point& move(size_t dx, size_t dy)
				{ _x += dy; _y += dy; return *this; }

				const Point& operator=(const Point& pt)
				{
					_x = pt._x; _y = pt._y;
					return *this;
				}

				bool operator==(const Point& pt) const
				{ return (_x == pt._x && _y == pt._y); }

				bool operator!=(const Point& pt) const
				{ return (_x != pt._x || _y != pt._y); }

				bool operator>(const Point& pt) const
				{
					if ( _x < pt._x || _y < pt._y) 
						return false;

					return ( (*this) != pt );
				}

				bool operator<(const Point& pt) const
				{
					if ( _x > pt._x || _y > pt._y ) 
						return false;

					return ( pt != (*this) );
				}

			protected:
				size_t _x;
				size_t _y;
		};

	} // namespace Gfx

} // namespace Pt

#endif

