/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_GFX_FONT_H
#define PT_GFX_FONT_H

#include <Pt/Api.h>
#include <Pt/Types.h>

#include <string>

namespace Pt {

namespace Gfx {

	class PT_API Font
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
				size_t      size = 12,
				FontStyle   fontStyle = NormalStyle,
				ssize_t     angle     = 0,
				Direction   direction = LeftToRightDirection
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
