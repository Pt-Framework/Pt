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
#ifndef Pt_Gfx_Rect_h
#define Pt_Gfx_Rect_h

#include <Pt/Gfx/Point.h>
#include <Pt/Gfx/Size.h>


namespace Pt {

	namespace Gfx {

		//! \brief A generic Rect class
		class PT_EXPORT Rect {
			public:
				//! Construct a Rect at a given position and size
				Rect(const Point& p = Point(0, 0), const Size& s = Size(0, 0))
				: _p(p), _s(s)
				{}

				void setOrigin(const Point& p)
				{ _p = p; }

				//! Return the top left coordinates as a const Point
				const Point& origin() const
				{ return _p; }

				//! Return the size as a Size
				void setSize(const Size& s)
				{ _s = s; }

				//! Return the size as a const Size
				const Size& size() const
				{ return _s; }

				//! Return the leftmost x coordinate
				size_t x1() const
				{ return _p.x(); }

				//! Return the topmost y coordinate
				size_t y1() const
				{ return _p.y(); }

				//! Return the rightmost x coordinate
				size_t x2() const
				{ return _p.x() + _s.width() - 1; }

				//! Return the bottommost y coordinate
				size_t y2() const
				{ return _p.y() + _s.height() - 1; }

				//! Set the leftmost x coordinate
				void setX1(size_t x1_)
				{ _p.setX(x1_); }

				//! Set the topmost y coordinate
				void setY1(size_t y1_)
				{ _p.setY(y1_); }

				//! Set the rightmost x coordinate
				void setX2(size_t x2_)
				{ _s.setWidth(x2_ - x1() + 1); }
				//!
				//! Set the bottommost y coordinate
				void setY2(size_t y2_)
				{ _s.setHeight(y2_ - y1() + 1); }

				//! Return the width
				size_t width() const
				{ return _s.width(); }

				//! Return the height
				size_t height() const
				{ return _s.height(); }

				//! Set the width
				void setWidth(size_t w)
				{ _s.setWidth(w); }

				//! Set the height
				void setHeight(size_t h)
				{ _s.setHeight(h); }

				//! Set the starting coordinates and the size
				void setGeometry(const Point& p, const Size& s)
				{
					_p = p;
					_s = s;
				}

				//! Set the starting coordinates and the ending coordinates
				void setGeometry(const Point& p1, const Point& p2)
				{
					_p = p1;
					_s.setWidth(p2.x() - p1.x() + 1);
					_s.setHeight(p2.y() - p1.y() + 1);
				}

				//! Return the top left coordinates as a const Point
				const Point& topLeft() const
				{ return _p; }

				//! Return the top right coordinates as a const Point
				const Point topRight() const
				{ return Point(x2(), y1()); }

				//! Return the bottom left coordinates as a const Point
				const Point bottomLeft() const
				{ return Point(x1(), y2()); }

				//! Return the bottom right coordinates as a const Point
				const Point bottomRight() const
				{ return Point(x2(), y2()); }

				bool operator==(const Rect& other) const
				{ return _p == other._p && _s == other._s; }

				bool operator!=(const Rect& other) const
				{ return _p != other._p || _s != other._s; }

			protected:
				Point _p;
				Size  _s;
		};

	} // namespace Gfx

} // namespace Pt

#endif

