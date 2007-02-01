/***************************************************************************
 *   Copyright (C) 2006-2007 by Tobias Müller                              *
 *   Copyright (C) 2006-2007 by Marc Boris Dürner                          *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
#ifndef PT_GFX_FONT_H
#define PT_GFX_FONT_H

#include <Pt/Gfx/Api.h>
#include <Pt/Types.h>

#include <Pt/Text/String.h>

namespace Pt {

namespace Gfx {

	class PT_GFX_API Font
	{
		friend bool operator==(const Font& a, const Font& b);

		public:
			enum FontStyle {
				NormalStyle, BoldStyle, ItalicStyle, BoldItalicStyle
			};

			enum Direction {
				LeftToRightDirection, RightToLeftDirection
			};

		public:
			Font(
				const std::string& name,
				size_t             size = 12,
				FontStyle          fontStyle = NormalStyle,
				ssize_t            angle     = 0,
				Direction          direction = LeftToRightDirection
				
			);

			std::string name() const;

			size_t size() const;

			FontStyle fontStyle() const;

			ssize_t angle() const;

			Direction direction() const;

		private:
			std::string _name;
			size_t      _size;
			FontStyle   _fontStyle;
			ssize_t     _angle;
			Direction   _direction;
	};

	inline bool operator==(const Font& a, const Font& b)
	{
		return
			   a._name.compare(b._name) == 0
			&& a._fontStyle             == b._fontStyle
			&& a._size                  == b._size
			&& a._angle                 == b._angle
			&& a._direction             == b._direction;
	}

} // namespace Gfx

} // namespace Pt

#endif
