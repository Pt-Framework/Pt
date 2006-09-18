/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_GFX_PEN_H
#define PT_GFX_PEN_H

#include <Pt/Api.h>
#include <Pt/Gfx/ARgbColor.h>


namespace Pt {

namespace Gfx {

	class PT_EXPORT Pen
	{
		friend bool operator==(const Pen& a, const Pen& b);

		public:
			enum LineStyle {
				SolidLine, DottedLine, DashedLine
			};

			enum CapStyle{
				RoundCap, SquareCap, FlatCap
			};

			enum JoinStyle {
				MiterJoin, RoundJoin, BevelJoin
			};

		public:
			Pen(
				size_t size            = 1,
				const ARgbColor& color = ARgbColor(0, 0, 0),
				LineStyle lineStyle    = SolidLine,
				CapStyle  capStyle     = RoundCap,
				JoinStyle joinStyle    = MiterJoin
			);

			size_t size() const;

			const ARgbColor& color() const;

			LineStyle lineStyle() const;

			CapStyle capStyle() const;

			JoinStyle joinStyle() const;

			void setSize(size_t size);

			void setColor(const ARgbColor& color);

			void setLineStyle(LineStyle lineStyle);

			void setCapStyle(CapStyle capStyle);

			void setJoinStyle(JoinStyle joinStyle);

		private:
			size_t    _size;
			ARgbColor _color;
			LineStyle _lineStyle;
			CapStyle  _capStyle;
			JoinStyle _joinStyle;
	};

	inline bool operator==(const Pen& a, const Pen& b)
	{
		return
			   a._size      == b._size
			&& a._color     == b._color
			&& a._lineStyle == b._lineStyle
			&& a._capStyle  == b._capStyle
			&& a._joinStyle == b._joinStyle;
	}

} // namespace Gfx

} // namespace Pt

#endif
