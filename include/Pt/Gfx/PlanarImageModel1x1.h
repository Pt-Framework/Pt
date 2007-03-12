/***************************************************************************
 *   Copyright (C) 2007 by Aloysius Indrayanto                             *
 *   Copyright (C) 2007 by Marc Boris Dürner                               *
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
#ifndef Pt_Gfx_PlanarImageModel1x1_h
#define Pt_Gfx_PlanarImageModel1x1_h

#include <Pt/Gfx/PlanarImageModel.h>


namespace Pt {

    namespace Gfx {

        /** @brief Planar image model class for non-subsampled planar images.
         *  @ingroup Gfx
         *
         *  For now it just an empty foward declaration but later it will become
         *  the generic implementation of the planar image model.
         */
        template<typename ColorProxyT_>
        class PlanarImageModel<ColorProxyT_, 1, 1> {
            public:
                // Color proxy type type (value type) of the color model
                typedef ColorProxyT_ ColorProxyT;

                // Non-reference type (value type) of the color
                typedef typename ColorProxyT::ValueT ValueT;

                // Value type of each individual component of the color model
                typedef typename ColorProxyT::ComponentT ComponentT;

                // Number of channels of the color model
                static const size_t NumberOfChannels = ColorProxyT::NumberOfChannels;

                // Color pointer class for the color model
                class ColorPtrT;

                // Constant color pointer class for the color model
                class ConstColorPtrT;

                // Scanline class for the color model
                class ScanlineT;

                // Constant scanline class for the color model
                class ConstScanlineT;
        };

    } // namespace Gfx

} // namespace Pt


//
// Include the template implementation header
//
#include "PlanarImageModel1x1.tpp"

#endif

