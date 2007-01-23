/***************************************************************************
 *   Copyright (C) 2005 by Aloysius Indrayanto                             *
 *   Copyright (C) 2005 by Marc Boris Dürner                               *
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
#include <Pt/Gfx/ARgbColor.h>
#include <Pt/Gfx/Rgb565Color.h>
#include <Pt/Gfx/XRgb1555Color.h>
#include <Pt/Gfx/ARgb8888Color.h>

#include <Pt/Gfx/BasicImage.h>
#include <Pt/Gfx/SubImage.h>


namespace Pt {

	namespace Gfx {

		// Explicit instantiation of the image classes
		template class BasicImage<ARgb>;
		template class BasicImage<Rgb565>;
		template class BasicImage<XRgb1555>;
		template class BasicImage<ARgb8888>;

		// Explicit instantiation of the subimage classes
		template class SubImage<ARgb>;
		template class SubImage<Rgb565>;
		template class SubImage<XRgb1555>;
		template class SubImage<ARgb8888>;

	}
}
