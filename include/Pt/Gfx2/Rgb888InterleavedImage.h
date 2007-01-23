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
#ifndef Pt_Gfx2_Rgb888InterleavedImage_h
#define Pt_Gfx2_Rgb888InterleavedImage_h

#include <Pt/Gfx2/Rgb888Color.h>
#include <Pt/Gfx2/InterleavedImage.h>
#include <Pt/Gfx2/SubImage.h>


namespace Pt {
	namespace Gfx {

		/** @brief Interleaved Rgb888 image class.
		 */
		typedef InterleavedImage<Rgb888> Rgb888InterleavedImage;


		/** @brief Interleaved Rgb888 subimage class.
		 */
		typedef SubImage< InterleavedImage<Rgb888> > Rgb888InterleavedSubImage;

	}
}

#endif
