/***************************************************************************
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
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
#ifndef Pt_Gfx_h
#define Pt_Gfx_h


namespace Pt {

	namespace Gfx {

		class Brush;
		class Font;
		class Point;
		class Size;
		class Rect;
		class Pen;

		struct ARgb;
		struct Cmyk;
		struct Rgb565;
		struct XRgb1555;
		struct XRgb8888;

		template <typename ColorSpaceT>
		class BasicColor;

		typedef BasicColor<ARgb>     ARgbColor;
		typedef BasicColor<Cmyk>     CmykColor;
		typedef BasicColor<Rgb565>   Rgb565Color;
		typedef BasicColor<XRgb1555> XRgb1555Color;
		typedef BasicColor<XRgb8888> XRgb8888Color;

		template <typename ColorSpaceT>
		class BasicImage;

		typedef BasicImage<ARgb>     ARgbImage;
		typedef BasicImage<Cmyk>     CmykImage;
		typedef BasicImage<Rgb565>   Rgb565Image;
		typedef BasicImage<XRgb1555> XRgb1555Image;
		typedef BasicImage<XRgb8888> XRgb8888Image;

		template <typename ColorSpaceT>
		class SubImage;

		typedef SubImage<ARgb>     ARgbSubImage;
		typedef SubImage<Cmyk>     CmykSubImage;
		typedef SubImage<Rgb565>   Rgb565SubImage;
		typedef SubImage<XRgb1555> XRgb1555SubImage;
		typedef SubImage<XRgb8888> XRgb8888SubImage;

	} // namespace Gfx

} // namespace Pt

#endif

