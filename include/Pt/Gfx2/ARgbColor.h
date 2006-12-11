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
#ifndef Pt_Gfx2_ARgbColor_h
#define Pt_Gfx2_ARgbColor_h

#include <Pt/Api.h>
#include <Pt/Types.h>


namespace Pt {

	namespace Gfx {

		/** @brief 64-Bit ARGB Color.
		 *  @ingroup Gfx
		 *
		 *  This is the master color model for Pt::Gfx.
		 *  \n\n
		 *  Valid range of the color components for this color model:
		 *  <TABLE>
		 *    <TR> <TD>Alpha</TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
		 *    <TR> <TD>Red  </TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
		 *    <TR> <TD>Green</TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
		 *    <TR> <TD>Blue </TD> <TD>0</TD> <TD>to</TD> <TD>65535 (0xFFFF)</TD> </TR>
		 *  </TABLE>
		 */
		class PT_EXPORT PT_PACKED ARgbColor {
			public:
				/** @brief The default constructor, will generate default color (black).
				 */
				inline ARgbColor()
				: _a(0), _r(0), _g(0), _b(0)
				{}

				/** @brief Copy constructor.
				 */
				inline ARgbColor(const ARgbColor& c)
				: _a(c._a), _r(c._r), _g(c._g), _b(c._b)
				{}

				/** @brief Construct color using the given components.
				 */
				inline ARgbColor(uint16_t r, uint16_t g, uint16_t b)
				: _a(0xFFFF), _r(0), _g(0), _b(0)
				{
					setRed(r);
					setGreen(g);
					setBlue(b);
				}

				/** @brief Construct color using the given components.
				 */
				inline ARgbColor(uint16_t a, uint16_t r, uint16_t g, uint16_t b)
				: _a(0), _r(0), _g(0), _b(0)
				{
					setAlpha(a);
					setRed(r);
					setGreen(g);
					setBlue(b);
				}


				/** @brief Assignment operator.
				 */
				inline ARgbColor& operator=(const ARgbColor& c)
				{ _a = c._a; _r = c._r; _g = c._g; _b = c._b; return *this; }


				/** @brief Return the alpha component of this color.
				 */
				inline uint16_t alpha() const
				{ return _a; }

				/** @brief Return the red component of this color.
				 */
				inline uint16_t red() const
				{ return _r; }

				/** @brief Return the green component of this color.
				 */
				inline uint16_t green() const
				{ return _g; }

				/** @brief Return the blue component of this color.
				 */
				inline uint16_t blue() const
				{ return _b; }


				/** @brief Set the alpha component of this color.
				 */
				inline void setAlpha(uint16_t a)
				{ _a = a; }

				/** @brief Set the red component of this color.
				 */
				inline void setRed(uint16_t r)
				{ _r = r; }

				/** @brief Set the green component of this color.
				 */
				inline void setGreen(uint16_t g)
				{ _g = g; }

				/** @brief Set the blue component of this color.
				 */
				inline void setBlue(uint16_t b)
				{ _b = b; }

			protected:
				uint16_t _a, _r, _g, _b;
		};


		/** @brief Dummy function which will just copy the value.
		 *
		 *  Just for the sake of completeness.
		 */
		inline void toARgb(ARgbColor& dst, const ARgbColor& src)
		{ dst = src; }

		/** @brief Dummy function which will just copy the value.
		 *
		 *  Just for the sake of completeness.
		 */
		inline void fromARgb(ARgbColor& dst, const ARgbColor& src)
		{ dst = src; }


		/** @brief Dummy function which will just copy the value.
		 *
		 *  Just for the sake of completeness.
		 */
		inline void toARgb_fast(ARgbColor& dst, const ARgbColor& src)
		{ dst = src; }

		/** @brief Dummy function which will just copy the value.
		 *
		 *  Just for the sake of completeness.
		 */
		inline void fromARgb_fast(ARgbColor& dst, const ARgbColor& src)
		{ dst = src; }

	} // namespace Gfx

} // namespace Pt

#endif

