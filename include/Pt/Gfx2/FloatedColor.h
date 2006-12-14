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
#ifndef Pt_Gfx2_FloatedColor_h
#define Pt_Gfx2_FloatedColor_h

#include <Pt/Gfx2/ARgbColor.h>


namespace Pt {

	namespace Gfx {

		/** @brief Floated ARGB Color.
		 *  @ingroup Gfx
		 *
		 *  This is the temporary color model for Pt::Gfx.
		 *  \n\n
		 *  Valid range of the color components for this color model:
		 *  <TABLE>
		 *    <TR> <TD>Alpha</TD> <TD>0.0f</TD> <TD>to</TD> <TD>1.0f</TD> </TR>
		 *    <TR> <TD>Red  </TD> <TD>0.0f</TD> <TD>to</TD> <TD>1.0f</TD> </TR>
		 *    <TR> <TD>Green</TD> <TD>0.0f</TD> <TD>to</TD> <TD>1.0f</TD> </TR>
		 *    <TR> <TD>Blue </TD> <TD>0.0f</TD> <TD>to</TD> <TD>1.0f</TD> </TR>
		 *  </TABLE>
		 *  However, values <0.0f and >1.0f are allowed exist for temporal calculation
		 *  results.
		 *  \n\n
		 *  Complex color algorithms such as cubic-scale, dithering, etc. are suggested
		 *  to use this color model to minimize rounding error propagation.
		 */
		class PT_EXPORT PT_PACKED FloatedARgbColor {
			public:
				/** @brief The default constructor, will generate the default color (black).
				 */
				inline FloatedARgbColor()
				: _a(1.0f), _r(0.0f), _g(0.0f), _b(0.0f)
				{}

				/** @brief Copy constructor.
				 */
				inline FloatedARgbColor(const FloatedARgbColor& c)
				: _a(c._a), _r(c._r), _g(c._g), _b(c._b)
				{}

				/** @brief Construct color using the given components.
				 */
				inline FloatedARgbColor(float a, float r, float g, float b)
				: _a(a), _r(r), _g(g), _b(b)
				{}

				/** @brief Construct color using the given components.
				 */
				inline FloatedARgbColor(float r, float g, float b)
				: _a(1.0f), _r(r), _g(g), _b(b)
				{}


				/** @brief Assignment operator.
				 */
				inline const FloatedARgbColor& operator=(const FloatedARgbColor& c)
				{ _a = c._a; _r = c._r; _g = c._g; _b = c._b; return *this; }

				/** @brief Assignment-addition operator.
				 */
				inline const FloatedARgbColor& operator+=(const FloatedARgbColor& c)
				{ _a += c._a; _r += c._r; _g += c._g; _b += c._b; return *this; }

				/** @brief Assignment-substraction operator.
				 */
				inline const FloatedARgbColor& operator-=(const FloatedARgbColor& c)
				{ _a -= c._a; _r -= c._r; _g -= c._g; _b -= c._b; return *this; }


				/** @brief Return the alpha component of this color.
				 */
				inline float alpha() const
				{ return _a; }

				/** @brief Return the red component of this color.
				 */
				inline float red() const
				{ return _r; }

				/** @brief Return the green component of this color.
				 */
				inline float green() const
				{ return _g; }

				/** @brief Return the blue component of this color.
				 */
				inline float blue() const
				{ return _b; }


				/** @brief Set the alpha component of this color.
				 */
				inline void setAlpha(float a)
				{ _a = a; }

				/** @brief Set the red component of this color.
				 */
				inline void setRed(float r)
				{ _r = r; }

				/** @brief Set the green component of this color.
				 */
				inline void setGreen(float g)
				{ _g = g; }

				/** @brief Set the blue component of this color.
				 */
				inline void setBlue(float b)
				{ _b = b; }

			public:
				friend void toARgb<FloatedARgbColor>(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const FloatedARgbColor& from);
				friend void fromARgb<FloatedARgbColor>(FloatedARgbColor& to, const uint16_t a, const uint16_t r, const uint16_t g, const uint16_t b);
				friend bool operator==<FloatedARgbColor>(const FloatedARgbColor& c1, const FloatedARgbColor& c2);
				friend bool operator< <FloatedARgbColor>(const FloatedARgbColor& c1, const FloatedARgbColor& c2);
				friend bool operator> <FloatedARgbColor>(const FloatedARgbColor& c1, const FloatedARgbColor& c2);

			protected:
				float _a, _r, _g, _b;
		};


		/** @brief Convert a FloatedARgbColor to ARgbColor's components.
		 *
		 *  Valid range of the individual color components (a, r, g, and b) are
		 *  from 0 to 65535 (0xFFFF).
		 */
		template <> inline
		void toARgb<FloatedARgbColor>(uint16_t& a, uint16_t& r, uint16_t& g, uint16_t& b, const FloatedARgbColor& from)
		{
			a = from._a<0.0f ? 0 : ( from._a>1.0f ? 0xFFFF : uint16_t(from._a*65535.0f) );
			r = from._r<0.0f ? 0 : ( from._r>1.0f ? 0xFFFF : uint16_t(from._r*65535.0f) );
			g = from._g<0.0f ? 0 : ( from._g>1.0f ? 0xFFFF : uint16_t(from._g*65535.0f) );
			b = from._b<0.0f ? 0 : ( from._b>1.0f ? 0xFFFF : uint16_t(from._b*65535.0f) );
		}

		/** @brief Convert ARgbColor's components to a FloatedARgbColor.
		 *
		 *  Valid range of the individual color components (a, r, g, and b) are
		 *  from 0 to 65535 (0xFFFF).
		 */
		template <> inline
		void fromARgb<FloatedARgbColor>(FloatedARgbColor& to, const uint16_t a, const uint16_t r, const uint16_t g, const uint16_t b)
		{
			to._a = float(a) / 65535.0;
			to._r = float(r) / 65535.0;
			to._g = float(g) / 65535.0;
			to._b = float(b) / 65535.0;
		}


		/** @brief Equality operator for FloatedARgbColor comparison.
		 */
		template <> inline
		bool operator==<FloatedARgbColor>(const FloatedARgbColor& c1, const FloatedARgbColor& c2)
		{ return c1._a==c2._a && c1._r==c2._r && c1._g==c2._g && c1._b==c2._b; }

		/** @brief Less-than operator for FloatedARgbColor comparison.
		 */
		template <> inline
		bool operator< <FloatedARgbColor>(const FloatedARgbColor& c1, const FloatedARgbColor& c2)
		{ return c1._a<c2._a || c1._r<c2._r || c1._g<c2._g || c1._b<c2._b; }

		/** @brief Greater-than operator for FloatedARgbColor comparison.
		 */
		template <> inline
		bool operator> <FloatedARgbColor>(const FloatedARgbColor& c1, const FloatedARgbColor& c2)
		{ return c1._a>c2._a || c1._r>c2._r || c1._g>c2._g || c1._b>c2._b; }

	} // namespace Gfx

} // namespace Pt

#endif

