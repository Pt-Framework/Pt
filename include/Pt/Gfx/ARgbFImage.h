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
#ifndef Pt_Gfx_ARgbFImage_h
#define Pt_Gfx_ARgbFImage_h

#include <Pt/Gfx/ARgbFColor.h>
#include <Pt/Gfx/InterleavedImage.h>
#include <Pt/Gfx/SubImage.h>


namespace Pt {
	namespace Gfx {

		/** @brief Standard interleaved ARgbF image class.
		  */
		typedef InterleavedImage<ARgbFColor, ColorTraits<ARgbFColor> > ARgbFInterleavedImage;


		/** @brief Standard interleaved ARgbF subimage class.
		 */
		typedef SubImage<ARgbFInterleavedImage> ARgbFInterleavedSubImage;


		//
		// Interleaved images and subimages will be the most used types
		// and so typedefs them for convenience
		//
		typedef ARgbFInterleavedImage    ARgbFImage;
		typedef ARgbFInterleavedSubImage ARgbFSubImage;

	}
}

#endif
