/***************************************************************************
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

