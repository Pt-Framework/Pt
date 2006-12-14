/***************************************************************************
 *   Copyright (C) 2006 by Aloysius Indrayanto                             *
 *   Copyright (C) 2006 by Marc Boris Dürner                               *
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
#ifndef Pt_Gfx2_ColorAlgorithm_h
#define Pt_Gfx2_ColorAlgorithm_h

#include <Pt/Gfx2/ARgbColor.h>


namespace Pt {

	namespace Gfx {

		/** @brief Make the greyscale version of the source color.
		 *
		 *  This is a fallback version in case the color model implementor does not
		 *  implement the specific version of greyscale() for the color model.
		 *  \n\n
		 *  A color implementor must not rely on this function since this function
		 *  will cause some overhead because of the conversion to and from ARgbColor.
		 */
		template <typename ColorT> inline
		const ColorT& greyscale(ColorT& to, const ColorT& from)
		{
			ARgbColor tmp;

			assign(tmp, from);
			greyscale(tmp, tmp);
			assign(to, tmp);

			return to;
		}

		/** @brief Make the given ColorT become greyscale.
		 */
		template <typename ColorT> inline
		const ColorT& greyscale(ColorT& c)
		{ return greyscale(c, c); }

	} // namespace Gfx

} // namespace Pt

#endif

