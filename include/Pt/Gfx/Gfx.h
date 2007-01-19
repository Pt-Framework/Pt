/***************************************************************************
 *   Copyright (C) 2006 PTV AG                                             *
 ***************************************************************************/

#ifndef PT_GFX_H
#define PT_GFX_H

#include <Pt/Types.h>
#include <Pt/Gfx/Api.h>

namespace Pt {

	namespace Gfx {

		class Pen;
		class Brush;
		class Font;
		class FontMetrics;
		class Region;

		struct ARgb;
		struct Rgb565;
		struct XRgb1555;
		struct XRgb8888;

		template <typename ColorSpaceT>
		class BasicColor;

		typedef BasicColor<ARgb>     ARgbColor;
		typedef BasicColor<Rgb565>   Rgb565Color;
		typedef BasicColor<XRgb1555> XRgb1555Color;
		typedef BasicColor<XRgb8888> XRgb8888Color;

		template <typename ColorSpaceT>
		class BasicImage;

		typedef BasicImage<ARgb>     ARgbImage;
		typedef BasicImage<Rgb565>   Rgb565Image;
		typedef BasicImage<XRgb1555> XRgb1555Image;
		typedef BasicImage<XRgb8888> XRgb8888Image;

		template <typename ColorSpaceT>
		class SubImage;

		typedef SubImage<ARgb>     ARgbSubImage;
		typedef SubImage<Rgb565>   Rgb565SubImage;
		typedef SubImage<XRgb1555> XRgb1555SubImage;
		typedef SubImage<XRgb8888> XRgb8888SubImage;


	} // namespace Gfx

} // namespace Pt

#endif

