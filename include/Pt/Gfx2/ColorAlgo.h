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
#ifndef Pt_Gfx2_ColorAlgo_h
#define Pt_Gfx2_ColorAlgo_h

#include <Pt/Gfx2/ARgbColor.h>


namespace Pt {

	namespace Gfx {

		/** @brief Assign a color model to another color model.
		 *
		 *  A color model implementor should specialize this function as needed if
		 *  faster implementation for the two colors is exist.
		 */
		template <typename DstColorT, typename SrcColorT> inline
		void assign(Color<DstColorT>& to, const Color<SrcColorT>& from)
		{ fromARgb( to, toARgb(from) ); }

		/** @brief Partial specialization of assign() if both the color models are the same.
		 *
		 *  This function will just copy the value from the source to the destiantion.
		 */
		template <typename ColorT> inline
		void assign(Color<ColorT>& to, const Color<ColorT>& from)
		{ to = from; }


		/** @brief Make the greyscale version of the source color.
		 *
		 *  This is a fallback version in case the color model implementor does not
		 *  implement the specific version of greyscale() for the color model.
		 *  \n\n
		 *  A color implementor must not rely on this function since this function
		 *  will cause some overhead because of the conversion to and from ARgbColor.
		 */
		template <typename ColorT> inline
		Color<ColorT>& greyscale(Color<ColorT>& to, const Color<ColorT>& from)
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
		Color<ColorT>& greyscale(Color<ColorT>& c)
		{ return greyscale(c, c); }

	} // namespace Gfx

} // namespace Pt

#endif

