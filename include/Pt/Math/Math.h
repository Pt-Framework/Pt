/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_MATH_H
#define PT_MATH_H

#include <Pt/Types.h>

namespace Pt {

	namespace Math {

		template <typename T>
		class BasicPoint;

		template <typename T>
		class BasicSize;

		template <typename T>
		class BasicRect;

		typedef BasicPoint<Pt::ssize_t>    Point;
		typedef BasicPoint<double>         PointF;

		typedef BasicSize<Pt::ssize_t>     Size;
		typedef BasicSize<double>          SizeF;

		typedef BasicRect<Pt::ssize_t>     Rect;
		typedef BasicRect<double>          RectF;

	} // namespace Math

} // namespace Pt

#endif

