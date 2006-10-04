/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
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

#include <Pt/Api.h>

#include <string>


namespace Pt {

	namespace Gfx {

		class PT_EXPORT Font
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
					std::string name,
					size_t      size,
					FontStyle   fontStyle = NormalStyle,
					size_t      angle     = 0,
					Direction   direction = LeftToRightDirection
				);

				std::string name() const;

				FontStyle fontStyle() const;

				size_t size() const;

				size_t angle() const;

				Direction direction() const;

				void setName(std::string name);

				void setFontStyle(FontStyle fontStyle);

				void setSize(size_t size);

				void setAngle(size_t angle);

				void setDirection(Direction direction);

			private:
				std::string _name;
				size_t      _size;
				FontStyle   _fontStyle;
				size_t      _angle;
				Direction   _direction;
		};


		inline bool operator==(const Font& a, const Font& b)
		{
			return
				a._name      == b._name      &&
				a._size      == b._size      &&
				a._fontStyle == b._fontStyle &&
				a._angle     == b._angle     &&
				a._direction == b._direction;
		}

	} // namespace gfx

} // namespace ptv

#endif
