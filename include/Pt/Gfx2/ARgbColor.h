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

#include <Pt/Gfx2/BasicColorAlgorithm.h>


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
				/** @brief The default constructor, will generate the default color (black).
				 */
				inline ARgbColor()
				: _a(0xFFFF), _r(0), _g(0), _b(0)
				{}

				/** @brief Copy constructor.
				 */
				inline ARgbColor(const ARgbColor& c)
				: _a(c._a), _r(c._r), _g(c._g), _b(c._b)
				{}

				/** @brief Construct color using the given components.
				 */
				inline ARgbColor(uint16_t a, uint16_t r, uint16_t g, uint16_t b)
				: _a(a), _r(r), _g(g), _b(b)
				{}

				/** @brief Construct color using the given components.
				 */
				inline ARgbColor(uint16_t r, uint16_t g, uint16_t b)
				: _a(0xFFFF), _r(r), _g(g), _b(b)
				{}


				/** @brief Assignment operator.
				 */
				inline const ARgbColor& operator=(const ARgbColor& c)
				{ _a = c._a; _r = c._r; _g = c._g; _b = c._b; return *this; }

				/** @brief Assignment-addition operator (beware of overflow).
				 */
				inline const ARgbColor& operator+=(const ARgbColor& c)
				{ _a += c._a; _r += c._r; _g += c._g; _b += c._b; return *this; }

				/** @brief Assignment-substraction operator (beware of underflow).
				 */
				inline const ARgbColor& operator-=(const ARgbColor& c)
				{ _a -= c._a; _r -= c._r; _g -= c._g; _b -= c._b; return *this; }


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

			public:
				friend void toARgb<ARgbColor>(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const ARgbColor& from);
				friend void fromARgb<ARgbColor>(ARgbColor& to, const uint16_t a, const uint16_t r, const uint16_t g, const uint16_t b);
				friend bool operator==<ARgbColor>(const ARgbColor& c1, const ARgbColor& c2);
				friend bool operator< <ARgbColor>(const ARgbColor& c1, const ARgbColor& c2);
				friend bool operator> <ARgbColor>(const ARgbColor& c1, const ARgbColor& c2);

			protected:
				uint16_t _a, _r, _g, _b;
		};


		/** @brief Convert an ARgbColor to ARgbColor's components.
		 *
		 *  Valid range of the individual color components (a, r, g, and b) are
		 *  from 0 to 65535 (0xFFFF).
		 */
		template <> inline
		void toARgb<ARgbColor>(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const ARgbColor& from)
		{ a = from._a; r = from._r; g = from._g; b = from._b; }

		/** @brief Convert ARgbColor's components to an ARgbColor.
		 *
		 *  Valid range of the individual color components (a, r, g, and b) are
		 *  from 0 to 65535 (0xFFFF).
		 */
		template <> inline
		void fromARgb<ARgbColor>(ARgbColor& to, const uint16_t a, const uint16_t r, const uint16_t g, const uint16_t b)
		{ to._a = a; to._r = r; to._g = g; to._b = b; }


		/** @brief Equality operator for ARgbColor comparison.
		 */
		template <> inline
		bool operator==<ARgbColor>(const ARgbColor& c1, const ARgbColor& c2)
		{ return c1._a==c2._a && c1._r==c2._r && c1._g==c2._g && c1._b==c2._b; }

		/** @brief Less-than operator for ARgbColor comparison.
		 */
		template <> inline
		bool operator< <ARgbColor>(const ARgbColor& c1, const ARgbColor& c2)
		{ return c1._a<c2._a || c1._r<c2._r || c1._g<c2._g || c1._b<c2._b; }

		/** @brief Greater-than operator for ARgbColor comparison.
		 */
		template <> inline
		bool operator> <ARgbColor>(const ARgbColor& c1, const ARgbColor& c2)
		{ return c1._a>c2._a || c1._r>c2._r || c1._g>c2._g || c1._b>c2._b; }

	} // namespace Gfx

} // namespace Pt

#endif

