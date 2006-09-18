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
#ifndef Pt_Gfx_Size_h
#define Pt_Gfx_Size_h

#include <Pt/Api.h>
#include <Pt/Types.h>


namespace Pt {

	namespace Gfx {

		//! \brief The Size class describes the heigth and width of an object
		class PT_EXPORT Size {
			public:
				//! @brief Construct a size of (w,h)
				inline Size(size_t w = 0, size_t h = 0)
				: _w(w), _h(h)
				{}

				//! @brief Returns the width
				inline size_t width() const
				{ return _w; }

				//! @brief Returns the height
				inline size_t height() const
				{ return _h; }

				//! @brief Sets the width
				inline void setWidth(size_t w)
				{ _w = w; }

				//! @brief Sets the height
				inline void setHeight(size_t h)
				{ _h = h; }

				const Size& operator=(const Size& other)
				{
					_w = other._w; _h = other._h;
					return *this;
				}

				bool operator==(const Size& other) const
				{ return (_w == other._w && _h == other._h); }

				bool operator!=(const Size& other) const
				{ return (_w != other._w || _h != other._h); }

				bool operator>(const Size& other) const
				{
					if ( _w < other._w || _h < other._h)
						return false;

					return ( (*this) != other );
				}

				bool operator<(const Size& other) const
				{
					if ( _w > other._w || _h > other._h )
						return false;

					return ( other != (*this) );
				}

			protected:
				size_t _w;
				size_t _h;
		};

	} // namespace Gfx

} // namespace Pt

#endif
