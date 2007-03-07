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
#include <Pt/Gfx/InterleavedSubImage.h>
#include <Pt/Gfx/InterleavedSubImage.tpp>

namespace Pt {

    namespace Gfx {

// With GCC we should be able to use explicit template instantiation correctly
// and thus we instantiate all the interleaved image and subimage classes here
#ifdef __GNUC__

        // Explicit instantiation of the standard interleaved image classes
        template class PT_EXPORT InterleavedImage<ARgbColor>;
        template class PT_EXPORT InterleavedImage<ARgbFColor>;
        template class PT_EXPORT InterleavedImage<ARgb8888Color>;
        template class PT_EXPORT InterleavedImage<Rgb888Color>;
        template class PT_EXPORT InterleavedImage<Rgb565Color>;
        template class PT_EXPORT InterleavedImage<Rgb555Color>;

        // Explicit instantiation of the standard subimage classes of the above image classes
        template class PT_EXPORT InterleavedSubImage<ARgbInterleavedImage>;
        template class PT_EXPORT InterleavedSubImage<ARgbFInterleavedImage>;
        template class PT_EXPORT InterleavedSubImage<ARgb8888InterleavedImage>;
        template class PT_EXPORT InterleavedSubImage<Rgb888InterleavedImage>;
        template class PT_EXPORT InterleavedSubImage<Rgb565InterleavedImage>;
        template class PT_EXPORT InterleavedSubImage<Rgb555InterleavedImage>;

#endif

    } // namespace Gfx

} // namespace Pt

