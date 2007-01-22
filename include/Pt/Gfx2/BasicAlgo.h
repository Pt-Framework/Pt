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
#ifndef Pt_Gfx2_BasicAlgo_h
#define Pt_Gfx2_BasicAlgo_h

namespace Pt {

	namespace Gfx {

		/** @brief Transform one sequence into another.
		 */
		template <typename In, typename Out, typename Op> inline
		Op transform(In begin, In end, Out dest, Op op)
		{
			for(; begin != end; ++begin, ++dest) op(*dest, *begin);
			return op;
		}

		/** @brief Transform one sequence into another.
		 */
		template <typename Iter, typename Op> inline
		Op transform(Iter begin, Iter end, Op op)
		{
			for(; begin != end; ++begin) op(*begin);
			return op;
		}


		/** @brief Assign one iterator range to another
		 */
		template <typename InputIteratorT, typename OutputIteratorT> inline
		OutputIteratorT assign(InputIteratorT begin, InputIteratorT end, OutputIteratorT to)
		{
			for(; begin != end; ++to, ++begin) assign(*to, *begin);
			return to;
		}

	} // namespace Gfx

} // namespace Pt

#endif
