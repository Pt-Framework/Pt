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
		return false; // TODO
	}

} // namespace gfx

} // namespace ptv

#endif
