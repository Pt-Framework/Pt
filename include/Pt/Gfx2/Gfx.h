/***************************************************************************
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
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
#ifndef Pt_Gfx2_Gfx_h
#define Pt_Gfx2_Gfx_h

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
		struct ARgbF;
		struct ARgb8888;
		struct Rgb888;
		struct Rgb565;
		struct Rgb555;

		template <typename TagT>
		class Color;

		typedef Color<ARgb>     ARgbColor;
		typedef Color<ARgbF>    ARgbFColor;
		typedef Color<ARgb8888> ARgb8888Color;
		typedef Color<Rgb888>   Rgb888Color;
		typedef Color<Rgb565>   Rgb565Color;
		typedef Color<Rgb555>   Rgb555Color;

		template <typename ColorTagT>
		class InterleavedImage;

		typedef InterleavedImage<ARgb>     ARgbInterleavedImage;
		typedef InterleavedImage<ARgbF>    ARgbFInterleavedImage;
		typedef InterleavedImage<ARgb8888> ARgb8888InterleavedImage;
		typedef InterleavedImage<Rgb888>   Rgb888InterleavedImage;
		typedef InterleavedImage<Rgb565>   Rgb565InterleavedImage;
		typedef InterleavedImage<Rgb555>   Rgb555InterleavedImage;

		template <typename ImageT_>
		class SubImage;

		typedef SubImage< InterleavedImage<ARgb> >     ARgbInterleavedSubImage;
		typedef SubImage< InterleavedImage<ARgbF> >    ARgbFInterleavedSubImage;
		typedef SubImage< InterleavedImage<ARgb8888> > ARgb8888InterleavedSubImage;
		typedef SubImage< InterleavedImage<Rgb888> >   Rgb888InterleavedSubImage;
		typedef SubImage< InterleavedImage<Rgb565> >   Rgb565InterleavedSubImage;
		typedef SubImage< InterleavedImage<Rgb555> >   Rgb555InterleavedSubImage;

		typedef ARgbInterleavedImage     ARgbImage;
		typedef ARgbFInterleavedImage    ARgbFImage;
		typedef ARgb8888InterleavedImage ARgb8888Image;
		typedef Rgb888InterleavedImage   Rgb888Image;
		typedef Rgb565InterleavedImage   Rgb565Image;
		typedef Rgb555InterleavedImage   Rgb555Image;

		typedef ARgbInterleavedSubImage     ARgbSubImage;
		typedef ARgbFInterleavedSubImage    ARgbFSubImage;
		typedef ARgb8888InterleavedSubImage ARgb8888SubImage;
		typedef Rgb888InterleavedSubImage   Rgb888SubImage;
		typedef Rgb565InterleavedSubImage   Rgb565SubImage;
		typedef Rgb555InterleavedSubImage   Rgb555SubImage;

	} // namespace Gfx

} // namespace Pt

#endif
