/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_GFX_FONTMETRICS_H
#define PT_GFX_FONTMETRICS_H

#include <Pt/Api.h>
#include <Pt/Types.h>


namespace Pt {

namespace Gfx {

	class PT_API FontMetrics
	{
		public:
			FontMetrics(size_t ascent, size_t descent, size_t width, size_t height);

			size_t ascent() const;

			size_t descent() const;

			size_t width() const;

			size_t height() const;

		private:
			size_t _ascent;
			size_t _descent;
			size_t _width;
			size_t _height;
	};

} // namespace Gfx

} // namespace Pt

#endif
