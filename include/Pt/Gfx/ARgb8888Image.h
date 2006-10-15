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
#ifndef Pt_ARgb8888Image_h
#define Pt_ARgb8888Image_h

#include <Pt/Gfx/ARgb8888Color.h>
#include <Pt/Gfx/SubImage.h>


namespace Pt {
	namespace Gfx {

		//!
		//! \brief XRGB8888 image class
		typedef BasicImage<ARgb8888> ARgb8888Image;

		//!
		//! \brief XRGB8888 sub image class
		typedef SubImage<ARgb8888> ARgb8888SubImage;
	}
}

#endif
