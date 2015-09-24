/*
 * Copyright (C) 2006-2007 by Aloysius Indrayanto
 * Copyright (C) 2006-2007 by Marc Boris Duerner
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Pt/Gfx/ARgbImage.h>
#include <Pt/Gfx/ARgbFImage.h>
#include <Pt/Gfx/ARgb8888Image.h>
#include <Pt/Gfx/Rgb888Image.h>
#include <Pt/Gfx/Rgb565Image.h>
#include <Pt/Gfx/Rgb555Image.h>
#include <Pt/Gfx/PlanarImage.tpp>
//#include <Pt/Gfx/PlanarSubImage.h>
//#include <Pt/Gfx/PlanarSubImage.tpp>

namespace Pt {

    namespace Gfx {

// With GCC we should be able to use explicit template instantiation correctly
// and thus we instantiate all the interleaved image and subimage classes here
#ifdef __GNUC__

        // Explicit instantiation of the standard planar image model classes
        //template class PT_EXPORT PlanarImageModel<ARgbColorProxy, 1, 1>;

        // Explicit instantiation of the standard planar image classes
        //template class PT_EXPORT PlanarImage< PlanarImageModel<ARgbColorProxy, 1, 1> >;

        // Explicit instantiation of the standard subimage classes of the above image classes
        //template class PlanarSubImage<ARgbPlanarImage>;

#endif

    } // namespace Gfx

} // namespace Pt

