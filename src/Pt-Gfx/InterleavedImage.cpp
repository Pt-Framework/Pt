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

#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/ARgbFImage.h>
#include <Pt/Gfx/ARgb8888Image.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Gfx/Rgb565Image.h>
#include <Pt/Gfx/Rgb555Image.h>
#include <Pt/Gfx/InterleavedImage.tpp>
#include <Pt/Gfx/SubImage.h>
#include <Pt/Gfx/SubImage.tpp>

namespace Pt {

    namespace Gfx {

// With GCC we should be able to use explicit template instantiation correctly
// and thus we instantiate all the interleaved image and subimage classes here
#ifdef __GNUC__

        // Explicit instantiation of the standard interleaved image classes
        template class InterleavedImage<ARgbColor,     ColorTraits<ARgbColor> >;
        template class InterleavedImage<ARgbFColor,    ColorTraits<ARgbFColor> >;
        template class InterleavedImage<ARgb8888Color, ColorTraits<ARgb8888Color> >;
        template class InterleavedImage<Rgb888Color,   ColorTraits<Rgb888Color> >;
        template class InterleavedImage<Rgb565Color,   ColorTraits<Rgb565Color> >;
        template class InterleavedImage<Rgb555Color,   ColorTraits<Rgb555Color> >;

        // Explicit instantiation of the standard subimage classes of the above image classes
        template class SubImage<ARgbInterleavedImage>;
        template class SubImage<ARgbFInterleavedImage>;
        template class SubImage<ARgb8888InterleavedImage>;
        template class SubImage<Rgb888InterleavedImage>;
        template class SubImage<Rgb565InterleavedImage>;
        template class SubImage<Rgb555InterleavedImage>;

#endif

    } // namespace Gfx

} // namespace Pt

