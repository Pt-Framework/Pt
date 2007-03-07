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
#ifndef Pt_Gfx_ARgbImage_h
#define Pt_Gfx_ARgbImage_h

#include <Pt/Gfx/ARgbFColor.h>
#include <Pt/Gfx/ARgbFColorRef.h>
#include <Pt/Gfx/InterleavedImage.h>
#include <Pt/Gfx/InterleavedSubImage.h>
#include <Pt/Gfx/PlanarImage.h>


namespace Pt {
    namespace Gfx {

        /** @brief Standard interleaved ARgb image class.
         *
         *  <B>This is the master interleaved image model</B>
         */
        typedef InterleavedImage<ARgbColor> ARgbInterleavedImage;


        /** @brief Standard interleaved ARgb subimage class.
         *
         *  <B>This is the master interleaved subimage model</B>
         */
        typedef InterleavedSubImage<ARgbInterleavedImage> ARgbInterleavedSubImage;


        /** @brief Standard planar ARgb image class.
         *
         *  <B>This is the master planar image model</B>
         */
        typedef PlanarImage<ARgbColorProxy, ColorTraits<ARgbColorProxy> > ARgbPlanarImage;


        /** @brief Standard planar ARgb subimage class.
         *
         *  <B>This is the master planar subimage model</B>
         */
        //typedef PlanarSubImage<ARgbPlanarImage> ARgbPlanarSubImage;


        //
        // Interleaved images and subimages will be the most used types
        // and so typedefs them for convenience
        //
        typedef ARgbInterleavedImage    ARgbImage;
        typedef ARgbInterleavedSubImage ARgbSubImage;

    }
}

#endif
