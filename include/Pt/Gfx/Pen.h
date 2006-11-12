/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_GFX_PEN_H
#define PT_GFX_PEN_H

#include <Pt/Api.h>
#include <Pt/Gfx/ARgbColor.h>


namespace Pt {

namespace Gfx {

	class PT_API Pen
	{
		friend bool operator==(const Pen& a, const Pen& b);

		public:
			Pen(size_t size = 1, const ARgbColor& color = ARgbColor(0, 0, 0));

			size_t size() const;

			const ARgbColor& color() const;

		private:
			size_t    _size;
			ARgbColor _color;
	};

	inline bool operator==(const Pen& a, const Pen& b)
	{
		return a._size == b._size && a._color == b._color;
	}

} // namespace Gfx

} // namespace Pt

#endif
