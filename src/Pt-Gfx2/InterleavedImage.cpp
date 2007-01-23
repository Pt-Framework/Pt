/***************************************************************************
 *   Copyright (C) 2006-2007 by Aloysius Indrayanto                        *
 *   Copyright (C) 2006-2007 by Marc Boris Dürner                          *
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

#include <Pt/Gfx2/ARgbColor.h>
#include <Pt/Gfx2/ARgbFColor.h>
#include <Pt/Gfx2/ARgb8888Color.h>
#include <Pt/Gfx2/Rgb888Color.h>
#include <Pt/Gfx2/Rgb565Color.h>
#include <Pt/Gfx2/Rgb555Color.h>
#include <Pt/Gfx2/InterleavedImage.h>
#include <Pt/Gfx2/InterleavedImage.tpp>
#include <Pt/Gfx2/SubImage.h>
#include <Pt/Gfx2/SubImage.tpp>

namespace Pt {

	namespace Gfx {

		// Explicit instantiation of the interleaved image classes
		template class InterleavedImage<ARgb>;
		template class InterleavedImage<ARgbF>;
		template class InterleavedImage<ARgb8888>;
		template class InterleavedImage<Rgb888>;
		template class InterleavedImage<Rgb565>;
		template class InterleavedImage<Rgb555>;

		// Explicit instantiation of the sub image classes of the above image classes
		template class SubImage< InterleavedImage<ARgb> >;
		template class SubImage< InterleavedImage<ARgbF> >;
		template class SubImage< InterleavedImage<ARgb8888> >;
		template class SubImage< InterleavedImage<Rgb888> >;
		template class SubImage< InterleavedImage<Rgb565> >;
		template class SubImage< InterleavedImage<Rgb555> >;

	} // namespace Gfx

} // namespace Pt

